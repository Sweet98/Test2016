/* ==============================================================================
   Matrix Beeper Module Source
   ==============================================================================

   $RCSfile: Gen6FoundationApp/FoundationApp/beep.h $
   $Revision: 1.16 $
   $Date: 2011/11/18 17:03:35EST $

   ==============================================================================
   
   ============================================================================== */

#ifndef	_BEEP_H
#define	_BEEP_H



// CAUTION: If you miss a beep sequence here, there is a good reason it disapeared from public scope!
// Usually there is an official function to to that action.
// I cleaned up the beeper because someone broke the USB startup by using QueueBeepSequence()
//	 instead of SoundPowerupBeep().

extern void QueueBeepSequence(unsigned int const*);
// end of deprecated functions

#define	BEEP_IN_PROGRESS			IsBeepInProgress()
void ConfigurePWMtoGPIOForBeepConvert(void);
void BeepInit(void);
void BeepOff(void);
bool IsBeepInProgress(void);
void ResetBeeper(void);
void ServiceBeeper(void);
void SoundStandardBeep(unsigned int);
void SoundDifferentMenuBeep(void);
void SoundGoodReadBeep();	//Host ACK [ESC] 7
void SoundBatchReadBeep();
void SoundBatchUploadedBeep();
void DecodeBeep();
void NoDecodeBeep();
void SoundErrorBeep(bool bIsMenuReadError = false);		//Host ACK [ESC] b,8
void BeepBip();         	//append beep
void Click();
void TrigClick();
void SoundMenuBeep(void);	//Host ACK [ESC] a
bool ScannerCantLink(void);

bool ExecuteNoReadBeepSequence(void);
bool GoodMenuBeep(void); 		// Host Ack [esc]a
bool ErrorMenuBeep(void);		// Host Ack [esc]b
bool IllumGreen135mS(void);	//Host ACK [ESC] 1
bool IllumGreen2S(void);		//Host ACK [ESC] 2
bool IllumGreen5S(void);		//Host ACK [ESC] 3
bool BeepLow(void);        	//Host ACK [ESC] 4
bool BeepMed(void);        	//Host ACK [ESC] 5
bool BeepHi(void);	       	//Host ACK [ESC] 6
bool ExecuteGoodReadBeepSequence(void); //Host ACK [ESC] 7
bool SoundDifferentScanMenuBeep(void);
bool ExecuteHostAckBeep(unsigned char BeepType); // executes host ack beeps a,b, and 1-8 returns false if not one of these

void SoundPowerupBeep(void);
void SoundPowerupBeepNoTest(void);
void SoundBadPsocCommunication(void);
bool IsBeepQueEmpty(void);
void SoundOposBeep(void);
void SoundBTConnectBeep(void);
void EnableRxLights(void);
void DisableRxLights(void);
void ShowRxDataLights(void);
void ShowDiagnosticError(void);
void BlinkCustomDefaultIndicator(void);

#if defined (RFBASE) || defined (RFSCAN)
void SoundOutOfRangeBeep(void);
#endif

bool Beep(bool ind,unsigned int freq,unsigned int dur);
void DebugBeep(void);
void DebugBadBeep(void);

#ifdef RFSCAN 
void SoundPagingBeep();
#endif
#ifdef VERIFIER
void FailVerificationBeep(void);
void PassVerificationBeep(void);
#endif

void FlashImageDownloadBeep(void);
void FlashImageDownloadCompleteBeep(void);
#ifdef STREAM_CAPTURE
void SoundOOMBeep(void);
#endif //STREAM_CAPTURE


#ifdef DEVELOPMENT_SETTINGS
void SecondTriggerStartBeep();
void SecondTriggerEndBeep();
#endif

#ifdef STREAM_CAPTURE
void SoundOOMBeep(void);
#endif //STREAM_CAPTURE

bool Is_PWM_Signal_State_High(void);

#ifdef RFBASE
bool HEAP_Led_PWM_Dim(int dim);
#endif

#endif 	 // _BEEP_H


   
