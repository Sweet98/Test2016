//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Trigger/HWTrigger.cpp $
//======================================================================================
#ifndef __TRIGGERIR_H__
#define __IRRGGERIR_H__
#if defined(IR_TRIGGER)

typedef enum {
	IR_DETECTION_STATE_OBJECT_IN = 0,		// 0b00000001
	IR_DETECTION_STATE_OBJECT_STAY = 1,		// 0b00000011
	IR_DETECTION_STATE_OBJECT_OUT = 2,		// 0b00000010
	IR_DETECTION_STATE_BACKGROUND = 3		// 0b00000000
} ObjectDetectionState_t;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ir_init
 *  Description:  Initialize ir detection hardware
 * =====================================================================================
 */
extern void IrHardwareInit(void); 

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ir_enable
 *  Description:  Enable/Disable ir detection function
 *   Parameters:  en - 1: enable ir detection; 0: disable ir detection
 * =====================================================================================
 */
extern bool IrEnable(void);
extern bool IrDisable(void);
extern void UpdateIrTriggerConfig(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  IrObjectDetectionState
 *  Description:  Return ir object detection status
 *       Return:  Object Dectection status, when IR is disabled the return ir status is 
 *                always IR_DETECTION_STATE_BACKGROUND.
 * =====================================================================================
 */
extern ObjectDetectionState_t IrObjectDetectionState(void);
extern bool IsIrTriggered(void);

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  IrConfig
 *  Description:  Auto Config ir DAC value to calibrate the ir detection range. Before 
 *                run this funtion, need set a white pad at required distance from scan 
 *                window.
 *   Parameters:  start_dac_val - Input the allowed min DAC range (0~255)
 *                max_dac_val - Input the allowed max DAC range (0~255)
 *       Return:  error code / Dac value
 * =====================================================================================
 */
extern int IrConfig(int start_dac_val, int max_dac_val); 

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  IrRead
 *  Description:  Testing IR detection funtion for factory usage
 *       Return:  IR_CHECK_OBJECT_IN / IR_CHECK_OBJECT_OUT / IR_CHECK_UNCERTAIN
 * =====================================================================================
 */
extern int IrCheck(void);

#endif	// defined(IR_TRIGGER)
#endif /* __TRIGGERIR_H__ */
