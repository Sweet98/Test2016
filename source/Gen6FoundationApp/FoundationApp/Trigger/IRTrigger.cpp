//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Trigger/HWTrigger.cpp $
//======================================================================================
#if defined(IR_TRIGGER)

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "interrupts.h"
#include "iMX25_IOMUX.h"
#include "iMX25_GPIO.h"
#include "mx25spi.h"
#include "PWM_Driver.h"
#include "IRTrigger.h"
#include "OsTimer.h"
#include "ProductConfiguration.h"
#include "Readerconfiguration.h"
#include "menu.h"
#include "MenuFlash.h"
#include "thread.h"
#include "timeout.h"
#include "beep.h"
#include "InterfaceMenuSettings.h"	
#pragma thumb

//#define IR_DEBUGING					//un-comment out for ir trigger debugging
#if defined(IR_DEBUGING)
	#include "ioman.h"
	#define IR_DEBUG(format, ... ) theCurrentInterface.printf(format, ## __VA_ARGS__)
#define DEBUG_PORT3_17(high_low)	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT17_LSH,GPIO_BIT17_WID, high_low);
#define DEBUG_PORT3_18(high_low)	HAZARD_WRITE_FIELD_TO_REG(GPIO3_DR0,GPIO_BIT18_LSH,GPIO_BIT18_WID, high_low);
#else
	#define IR_DEBUG(format, ... ) do {} while(0)
#endif


/* config KPP_ROW0 as feedback of IR */
#define SETUP_IR_FEEDBACK()	do {					\
	  HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_EB1,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5); \
	  HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_EB1,1);		\
	  HAZARD_WRITE_FIELD_TO_REG(GPIO2_GDIR0,GPIO_BIT13_LSH,GPIO_BIT13_WID,GPIO_GDIR_INPUT); \
  } while (0)

 /* config UART2_RTS as IR enable signal */
#define SETUP_IR_ENABLE_SIGNAL() do {					\
	HAZARD_WRITE_FIELD_TO_REG(IOMUXC_SW_MUX_CTL_PAD_UART2_TXD,IOMUXC_SW_MUX_CTL_MUX_MODE_LSH,IOMUXC_SW_MUX_CTL_MUX_MODE_WID,MUX_MODE_5); \
	HAZARD_WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_UART2_TXD,0x00000080);	\
    HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_LOW);	\
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_GDIR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_GDIR_OUTPUT); \
  } while (0)

#define IR_ENABLE_SIGNAL()	do {													\
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_HIGH);	\
  } while (0)

#define IR_DISABLE_SIGNAL()	do {													\
	HAZARD_WRITE_FIELD_TO_REG(GPIO4_DR0,GPIO_BIT27_LSH,GPIO_BIT27_WID,GPIO_LOW);	\
  } while (0)


#define IR_STATUS_IN     0
#define IR_STATUS_OUT    1
#define IR_UNSETTING_STATUS   100
static ObjectDetectionState_t ObjectDetectionState = IR_DETECTION_STATE_BACKGROUND;
static bool ir_enable = 0;
static int ir_pwm_duty_hysteresis = 0;
static int ir_pwm_duty = 0;
static int ir_input = 0;

static int CalibrateIrPwmDuty(int start_pwm_duty, int end_pwm_duty);
static void MsDelay(UINT32);
static BOOL ir_pwm_duty_calc(void);
static BOOL start_ir_detection_task(void);
static BOOL stop_ir_detection_task(void);
static BOOL ir_pwm_init(void);
static BOOL ir_pwm_start(void);
static BOOL ir_pwm_stop(void);
inline static BOOL ir_pwm_set_duty(UINT32);
inline static void UpdateIrState(void);
static void ir_detection(void);

static
void MsDelay(UINT32 milliseconds)
{
	CTimeoutmS theTimeOut(milliseconds);
	while(!theTimeOut.HasExpired());
	return;
}

static BOOL ir_pwm_duty_calc(void)
{
	ir_pwm_duty_hysteresis = GET_CONFIG_SETTING(IrPwmDutyHysteresis);
	switch(GET_SETTING(IrTriggerEnable))
	{
		case IR_TRG_OFF:
		case IR_TRG_ON_STD:
			ir_pwm_duty = GET_CONFIG_SETTING(IrPwmDuty);
			break;
		case IR_TRG_ON_NEAR:
			ir_pwm_duty = GET_CONFIG_SETTING(IrPwmDuty)  / 2;
			break;
		case IR_TRG_ON_FAR:
			ir_pwm_duty = GET_CONFIG_SETTING(IrPwmDuty) + (GET_CONFIG_SETTING(IrPwmDuty) / 2);
			break;
		default:
			ir_pwm_duty = GET_CONFIG_SETTING(IrPwmDuty);
			break;
	}
	if( (ir_pwm_duty + ir_pwm_duty_hysteresis) > IR_MAX_DUTY ) ir_pwm_duty = IR_MAX_DUTY - ir_pwm_duty_hysteresis;
	IR_DEBUG("<IR> duty = %d, hysteresis = %d\r\n", ir_pwm_duty, ir_pwm_duty_hysteresis);

	return true;
}

#define IR_PWM_DIV	240
#define IR_PWM_CLOCK	(240000000/IR_PWM_DIV)
static BOOL ir_pwm_init(void)
{
	bool result = 1;
	UINT32 pwm_frequency = GET_CONFIG_SETTING(IrPwmFreq);
	UINT16 Period = IR_PWM_CLOCK / pwm_frequency;

	result &= ir_pwm_duty_calc();

	// inital and start PWM
	result &= PWM_Init(PWM2, CSPI1_SS0, IR_PWM_DIV-1);
	result &= PWM_Period(PWM2, Period - 2);
	result &= PWM_Sample(PWM2, ir_pwm_duty*Period/IR_MAX_DUTY);
	result &= PWM_Stop(PWM2);

	IR_DEBUG("<IR> set freq: %d, duty: %d \r\n", pwm_frequency, ir_pwm_duty);

	return result;
}

static BOOL ir_pwm_start(void)
{
	return PWM_Start(PWM2);
}

static BOOL ir_pwm_stop(void)
{
	return PWM_Stop(PWM2);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ir_pwm_set_duty
 *  Description:  Set IR emit power by write pwm duty 
 * =====================================================================================
 */
inline static BOOL ir_pwm_set_duty(UINT32 pwm_duty)
{
	static int pwm_sample = 0;
	BOOL result = 0;
	int Period = IR_PWM_CLOCK / GET_CONFIG_SETTING(IrPwmFreq);
	pwm_sample = pwm_duty*Period/IR_MAX_DUTY;

	if( pwm_sample != Read_PWM_Count(PWM2) )
	{
		result = PWM_Sample(PWM2, pwm_sample);
		IR_DEBUG("<IR> set freq: %d, duty: %d \r\n", GET_CONFIG_SETTING(IrPwmFreq), pwm_duty);
		return result;
	}
	else
	{
		return true;
	}
}

bool IrDisable(void)
{
	bool result = 1;

	if(ir_enable)
	{
		stop_ir_detection_task();
		IR_DISABLE_SIGNAL();
		result &= ir_pwm_stop();
		if(result) ir_enable = 0;
	}
	IR_DEBUG("<IR> disable, %d\r\n", ir_enable);

	return result;
}

bool IrEnable(void)
{
	bool result = 1;

	IR_ENABLE_SIGNAL();
	result &= ir_pwm_duty_calc();
	result &= ir_pwm_start();
	result &= start_ir_detection_task();
    ir_enable = result;
	IR_DEBUG("<IR> enable, %d\r\n", ir_enable);

	return result;
}

void IrHardwareInit(void)
{
	SETUP_IR_FEEDBACK();
	SETUP_IR_ENABLE_SIGNAL();
	ir_pwm_init();

	IR_DEBUG("<IR> init\r\n");

	return;
}


void UpdateIrTriggerConfig(void)
{
	static unsigned int prevIrTriggerEnable = IR_UNSETTING_STATUS;
	unsigned int currIrTriggerEnable = GET_SETTING(IrTriggerEnable);

	if(currIrTriggerEnable!= prevIrTriggerEnable) 
	{
		if(currIrTriggerEnable != IR_TRG_OFF) 
		{
			IrEnable();
		} 
		else 
		{
			IrDisable();
		}
	}
	prevIrTriggerEnable = currIrTriggerEnable;

	return;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ir_detection
 *  Description:  This function is called by interrupts every 1 ms, it use to detect 
 *                the IR feedback status to adjudge if there is object present to 
 *                scan window or not.
 * =====================================================================================
 */
#define IR_DEBOUNCE_TIME_MS			5
static void ir_detection(void)
{
	// IR signal debounce
	static int pre_polar;
	static int cur_polar;
	static int ir_debounce_timer = IR_DEBOUNCE_TIME_MS;

	cur_polar = READ_FIELD(GPIO2_PSR0, GPIO_BIT13_LSH, GPIO_BIT13_WID);

	if(pre_polar != cur_polar) {
		if(cur_polar == IR_STATUS_OUT) {
			ir_debounce_timer = GET_CONFIG_SETTING(IrOutDebounceTimeMs);
		} else {
			ir_debounce_timer = IR_DEBOUNCE_TIME_MS;
		}
	}

	pre_polar = cur_polar;

	if(ir_debounce_timer) {
		ir_debounce_timer--;
	} else {
		ir_input = pre_polar;
	}

	// IR state update
	UpdateIrState();

	return;	
}

static BOOL start_ir_detection_task(void)
{
	int ret;
	
	DISABLE_INTERRUPTS();
	UnRegisterOneMsTask(ir_detection);
	ObjectDetectionState = IR_DETECTION_STATE_BACKGROUND;
	ret = RegisterOneMsTask(ir_detection);
	RESTORE_INTERRUPTS();

	if (ret) {
		return true;
	} else {
		IR_DEBUG("<IR> PANIC: IR can't be registered!\r\n");
		return false;
	}
}

static BOOL stop_ir_detection_task(void)
{
	int ret;

	DISABLE_INTERRUPTS();
	ret = UnRegisterOneMsTask(ir_detection);
	RESTORE_INTERRUPTS();
	ObjectDetectionState = IR_DETECTION_STATE_BACKGROUND;

	if (ret) {
		return true;
	} else {
		IR_DEBUG("<IR> PANIC: IR can't be un-registered!\r\n");
		return false;
	}
}

ObjectDetectionState_t IrObjectDetectionState(void)
{
	return ObjectDetectionState;
}

bool IsIrTriggered(void)
{
	return (ObjectDetectionState == IR_DETECTION_STATE_OBJECT_IN) || (ObjectDetectionState == IR_DETECTION_STATE_OBJECT_STAY);
}


#define IR_EDGE_WIDTH_MS			20
inline static void UpdateIrState(void)
{
	static int ir_edge_timer = IR_EDGE_WIDTH_MS;
	switch(ObjectDetectionState)
	{
		case IR_DETECTION_STATE_OBJECT_STAY:
		{
			if(IR_STATUS_OUT == ir_input)
			{
				ObjectDetectionState = IR_DETECTION_STATE_OBJECT_OUT;
				ir_edge_timer = IR_EDGE_WIDTH_MS;
				ir_pwm_set_duty(ir_pwm_duty);
			}
			break;
		}
		case IR_DETECTION_STATE_OBJECT_IN:
		{
			// for keep the "Object In" signal 20 ms, to make it detectable with 60Hz scan rate
			if(IR_STATUS_OUT == ir_input)
			{
				ObjectDetectionState = IR_DETECTION_STATE_OBJECT_OUT;
				ir_edge_timer = IR_EDGE_WIDTH_MS;
				ir_pwm_set_duty(ir_pwm_duty);
			}
			else
			{
				if(ir_edge_timer)
				{
					ir_edge_timer--;
				}
				else
				{
					ObjectDetectionState = IR_DETECTION_STATE_OBJECT_STAY;
				}
			}
			break;
		}
		case IR_DETECTION_STATE_BACKGROUND:
		{
			if (IR_STATUS_IN == ir_input)
			{
				ObjectDetectionState = IR_DETECTION_STATE_OBJECT_IN;
				ir_edge_timer = IR_EDGE_WIDTH_MS;
				ir_pwm_set_duty(ir_pwm_duty + ir_pwm_duty_hysteresis);
			}
			break;
		}
		case IR_DETECTION_STATE_OBJECT_OUT:
		{
			if(IR_STATUS_OUT == ir_input)
			{
				// for keep the "Object Out" signal 20 ms, to make it detectable with 60Hz scan rate
				if(ir_edge_timer)
				{
					ir_edge_timer--;
				}
				else
				{
					ObjectDetectionState = IR_DETECTION_STATE_BACKGROUND;
				}
			}
			else
			{
				ObjectDetectionState = IR_DETECTION_STATE_OBJECT_IN;
				ir_edge_timer = IR_EDGE_WIDTH_MS;
				ir_pwm_set_duty(ir_pwm_duty + ir_pwm_duty_hysteresis);
			}
			break;
		}
		default:
		{
			// Exception
			break;
		}
	}

	return;
}

#define REPEAT_CHECK_NUM	20
#define IR_CHECK_OBJECT_IN	1
#define IR_CHECK_OBJECT_OUT	0
#define IR_CHECK_UNCERTAIN	-1
int IrRead(void)
{
	int object_in_num = 0;
	int object_out_num = 0;
	int cur_polar;

	for(int i = 0; i < REPEAT_CHECK_NUM; i++)
	{
		cur_polar = READ_FIELD(GPIO2_PSR0, GPIO_BIT13_LSH, GPIO_BIT13_WID);
		if(cur_polar == IR_STATUS_IN)
		{
			object_in_num++;
		}
		else
		{
			object_out_num++;
		}

		MsDelay(1);
	}

	if(REPEAT_CHECK_NUM == object_in_num)
	{
		return IR_CHECK_OBJECT_IN;
	}
	else if(REPEAT_CHECK_NUM == object_out_num)
	{
		return IR_CHECK_OBJECT_OUT;
	}
	else
	{
		return IR_CHECK_UNCERTAIN;
	}
}

#define ERR_BAD_ARGUMENT			-1
#define ERR_IR_DETECTION_FAILED		-2
#define ERR_PADDLE_TOO_CLOSE		-3
#define ERR_PWM_DUTY_STORE_FAILED	-4
#define ERR_IR_CONFIG_FAIL			-5
int IrConfig(int start_pwm_duty, int end_pwm_duty)
{
	int ret = 0;
	int new_pwm_duty = 0;
	BOOL ir_en = ir_enable;

    if ((end_pwm_duty <= start_pwm_duty) || (end_pwm_duty > IR_MAX_DUTY) || (start_pwm_duty < 0))
    {
        ret = ERR_BAD_ARGUMENT;
    }
	else
	{
		if(ir_enable) {
			ret = stop_ir_detection_task();
			IR_DEBUG("stop ir task, %d", ret);
			if(!ret) return ERR_IR_CONFIG_FAIL;

			//if(!stop_ir_detection_task()) return ERR_IR_CONFIG_FAIL;
		} else {
			IrEnable();
			if(!stop_ir_detection_task()) return -6;
		}

		// delay 100ms
		MsDelay(100);

		new_pwm_duty = CalibrateIrPwmDuty(start_pwm_duty, end_pwm_duty);

		if ((new_pwm_duty < start_pwm_duty) || (new_pwm_duty < 0))
		{
			ret = ERR_IR_DETECTION_FAILED;
		}
		else if (new_pwm_duty == start_pwm_duty)
		{
			ret = ERR_PADDLE_TOO_CLOSE;
		}
		else
		{
			// write IR pwm duty to flash
			if(new_pwm_duty != GET_CONFIG_SETTING(IrPwmDuty))
			{
				char sTemp[20];

				sprintf(sTemp, "0F2E01%d.", new_pwm_duty);
				HSTRING	Temp(sTemp,true);

				if(ProductConfiguration->Menu(&Temp) == MENU_PASSED)
				{
					FlashProductConfigurationChanges();
					if(new_pwm_duty == GET_CONFIG_SETTING(IrPwmDuty))
					{
						ret = new_pwm_duty;
					}
					else
					{
						ret = ERR_PWM_DUTY_STORE_FAILED;
					}
				}
				else
				{
					ret = ERR_PWM_DUTY_STORE_FAILED;
				}
			}
			else
			{
				ret = new_pwm_duty;
			}
		}

		if(ir_en) {
			IrEnable();
		} else{
			IrDisable();
		}
	}
	return ret;
}

int IrCheck(void)
{
	int result = -2;
	
	if(!ir_enable) {
		IrEnable();
		MsDelay(100);
	}

	if(ir_enable) {
		switch(IrObjectDetectionState()) {
			case IR_DETECTION_STATE_BACKGROUND:
				result = 0;
				break;

			case IR_DETECTION_STATE_OBJECT_STAY:
				result = 1;
				break;

			case IR_DETECTION_STATE_OBJECT_IN:
			case IR_DETECTION_STATE_OBJECT_OUT:
			default:
				result = -1;
				break;
		}
	}

	return result;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  CalibrateIrPwmDuty
 *  Description:  Find the target pwm duty value.
 *   Parameters:  start_pwm_duty - Input the allowed min pwm duty range (0~100)
 *                end_pwm_duty - Input the allowed max pwm duty range (0~100)
 *       Return:  error code / pwm duty result
 * =====================================================================================
 */
static
int CalibrateIrPwmDuty(int start_pwm_duty, int end_pwm_duty)
{
    int current_pwm_duty;
    int mid_val = 0;
	int min_val;
	int max_val;
	int new_mid_val;
	bool object_found;

	// Check the IR range
	ir_pwm_set_duty(start_pwm_duty);
	MsDelay(100);
	object_found = false;
	for(int i = 0; i < 10; i++)
	{
		if(IrRead() != IR_CHECK_OBJECT_OUT) 
		{
			IR_DEBUG("<IR> test out fail\r\n");
			object_found = true;
			break;
		}
		MsDelay(20);
	}
	if(object_found == true) return ERR_IR_DETECTION_FAILED;

	ir_pwm_set_duty(end_pwm_duty);
	MsDelay(100);
	object_found = true;
	for(int i = 0; i < 10; i++)
	{
		if(IrRead() != IR_CHECK_OBJECT_IN) 
		{
			IR_DEBUG("<IR> test stay fail\r\n");
			object_found = false;
			break;
		}
		MsDelay(20);
	}
	if(object_found == false) return ERR_IR_DETECTION_FAILED;

	// Find IR pwm duty set point
    current_pwm_duty = ERR_IR_DETECTION_FAILED;
	min_val = start_pwm_duty;
	max_val = end_pwm_duty;

	while(min_val < max_val)
	{
		new_mid_val = (max_val + min_val)/2;
		if(mid_val != new_mid_val) {
			mid_val = new_mid_val;
		} else {
			break;
		}

		IR_DEBUG("<IRCFG>min:%d, mid:%d, max:%d\r\n", min_val, mid_val, max_val);
		ir_pwm_set_duty(mid_val);

		MsDelay(100);

		object_found = 1;
		for(int i = 0; i < 5; i++)
		{
			object_found &=  (IrRead() == IR_CHECK_OBJECT_IN ?  1 : 0);
			MsDelay(20);
		}

		if(object_found)
		{
			max_val = mid_val;
			current_pwm_duty = mid_val;
		}
		else
		{
			min_val = mid_val;
		}
	}

	return current_pwm_duty;
}

#endif // IR_TRIGGER
