;************************************************************************
;* MX1 Dragonball, relocate and execute downloaded images routines	*
;* Tools: ARM SDT 2.5 for now... will be ADS 1.1 or GNU                 *
;* Assembler: ARM Software Development Toolkit (ARMasm) version 2.50	*
;* Target: Matrix MX1 Development board                                 *
;* Target: Strong Arm 1100 on PCB 08862 Rev A 				*
;*					      				*
;*					      				*
;************************************************************************
;;
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: app_kernel/flash_write_from_ram.s $
;; $Revision: 1.4 $
;; $Date: 2008/11/25 14:41:21EST $
;;======================================================================================


;*
;*	File description:
;*		This file contains flash management routines that must run in RAM.
;*		In the past, our application code was always running in RAM, but we
;*		now can optionally run out of ROM.  When querying flash for flash
;*		device type, waiting for erase to complete, and waiting for writes
;*		to complete, flash reads return status information rather than what's
;*		stored in a particular flash location.  So, if we launch, for example,
;*		an erase operation while running in ROM, the processor will attempt to
;*		fetch the next instruction, but will get status information rather than
;*		the next instruction in flash, and we WOOF!!!!!!!!!  This file exists
;*		to perform operations in RAM, as subroutines, that cannot be performed
;*		in ROM.  Each of the three operations (query, erase, and write) has a
;*		relocatable routine to perform the corresponding operation, a routine
;*		that returns the size and address of the relocatable routine, and a routine
;*		that the application calls to actually perform the operation.  The
;*		application may then allocate space anywhere in RAM for the relocatable
;*		routine, copy it, and then run it.  The routine that the application calls
;*		to actually perform the operation is supplied with the address of the
;*		relocated routine.  The called routine then sets the PC to the address of
;*		the relocated routine.  The reason for doing this is that a straight
;*		branch and link may not work if the address difference between the application
;*		and the relocated routine is greater than 24 bits.
;*
;;


		INCLUDE	db_memap.s		; Defines how we're allocating RAM for application code, stacks, etc.
		INCLUDE	db_hdw.s			; Defines Motorola specific device registers



		AREA	FlashManagementServices, CODE, READONLY



		EXPORT ActualWriteFlashAMD
		EXPORT WriteFlashAMD
		EXPORT GetSizeAndAddressOfWriteFlashAMD

		EXPORT ActualWriteFlashIntel
		EXPORT WriteFlashIntel
		EXPORT GetSizeAndAddressOfWriteFlashIntel



;;
;;*****************************************************************************************************************
;;
;;	AMD flash write routines
;;
;;*****************************************************************************************************************
;;




;;
;;*****************************************************************************************************************
;;
;;	ActualWriteFlashAMD
;;		Erase and write AMD flash.  Note that the "C" source lines are included to explain what's going on in the
;;		assembly code.
;;
;;		Note: It is assumed that new data being written, start sector data being preserved, and end sector data
;;			being preserved have sizes that are multiples of 2 and have addresses that fall on a 2 byte boundary!
;;
;;		C prototype: void ActualWriteFlashAMD (FLASH_WRITE_PARAMETERS *WriteParameters);
;;

ActualWriteFlashAMD
	stmdb		sp!,{r2-r12,lr}				; Save registers

		;// Assume no errors yet.
		;WriteParameters->WriteStatus = FLASH_WRITE_OK;

	ldr		r9,=0
	str		r9,[r0,#0x38]

		;if( WriteParameters->FlashSupportsBulkErase )
		;{

	ldr		r9,[r0,#0x50]
	cmp		r9,#0
	beq		IndividualSectorEraseOperation



;************************************************************************************************************************************
;************************************************************************************************************************************
;************************************************************************************************************************************
;	BULK SECTOR ERASE OPERATION - Start erase commands on all sectors to be erased, then wait for last one to complete
;************************************************************************************************************************************
;************************************************************************************************************************************
;************************************************************************************************************************************


			;// Set previous timer reading to current system timer.
			;PreviousTimerReading = read_timer();

	ldr		r4,=RTC_BASE_ADDR_UPPER	; We're using R4 to store the TCN1 address.  Once again, because of 8 bit constant
						; limitation, we're loading the address in two parts.
	add		r4,r4,#RTC_BASE_ADDR_LOWER
	add		r4,r4,#RTC_BASE_ADDR_RTC_SECOND_OFFSET
	ldr		r5,[r4]			; Store the current timer reading in R5

	ldr		r2,[r0,#0x14]	; R2 is number of sectors to erase
	ldr		r1,[r0,#0x18]	; R1 is AddressesOfSectorsToErase
	ldr		r3,[r0,#0x1C]	; R3 is EraseTimeout

			;// Issue all the erase commands at once.
			;while(NumberOfSectorsToErase)
			;{

BulkEraseNextAMDSector
	cmp		r2,#0
	ble		FinishedIssuingAMDEraseCommands

				;AddressOfNextSectorToErase = AddressesOfSectorsToErase[i] + FLASH_WRITE_BASE;	// Start erasing with first sector

	ldr		r6,[r1],#4			; R6 is next sector to erase
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r6,r6,r8

				;FLASH_WORD_POINTER[0x555] = 0xAA;		// Issue the commands to flash to do the erase

	ldr		r7,=0xAA0	; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0xAA
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x2AA] = 0x55;

	ldr		r7,=0x550	; To get 0x554 into r1, move 0x550 first, then add 0x4 to accommodate 8 bit constant maximum
	ldr		r8,=0x4
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0x55
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x555] = 0x80;

	ldr		r7,=0xAA0		; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0x80
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x555] = 0xAA;		// Issue the commands to flash to do the erase

	ldr		r7,=0xAA0		; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0xAA
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x2AA] = 0x55;

	ldr		r7,=0x550		; To get 0x554 into r1, move 0x550 first, then add 0x4 to accommodate 8 bit constant maximum
	ldr		r8,=0x4
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0x55
	strh		r8,[r7]

				;FLASH_WORD_POINTER[AddressOfNextSectorToErase] = 0x30;

	ldr		r7,=0x30
	strh		r7,[r6]

				;--NumberOfSectorsToErase;
			;}	//end while (NumberOfSectorsToErase)

	RESET_WATCHDOG_TIMER

	sub		r2,r2,#1
	b		BulkEraseNextAMDSector

FinishedIssuingAMDEraseCommands

			;// Now that the sector erase commands have been issued, wait until 0xFF is seen in last sector or the timer is exhausted.
			;while((FLASH_BYTE_POINTER[AddressOfNextSectorToErase] != 0xff) && (EraseTimer))
			;{

CheckToSeeIfAMDBulkFlashEraseIsFinished
	ldr		r8,=0xFF
	ldr		r7,=0				; Clear all 4 bytes
	ldrb		r7,[r6]
	cmp		r8,r7
	beq		AMDFlashEraseIsFinished
	cmp		r3,#0
	beq		AMDFlashEraseIsFinished

				;// Get new timer count.
				;NewTimerReading = read_timer();

	ldr		r7,[r4]

				;// If the timer has rolled over (it is less than the previous reading), then counts elapsed is equal to the new timer reading +
				;// the complement of the previous reading + 1; otherwise, the counts elapsed is the difference between new and previous.
				;if(NewTimerReading < PreviousTimerReading)
				;{

	cmp		r7,r5
	bge		NoAMDBulkEraseTimerRollOver

					;CountsElapsed = NewTimerReading + (~PreviousTimerReading) + 1;
					;CountsElapsed = NewTimerReading + (60 - PreviousTimerReading);	// Note that we're using a 60 second real-time clock.
 				;}

	ldr		r8,=60
	sub		r8,r8,r5
;	mvn		r8,r5
;	add		r8,r8,#1
	add		r8,r8,r7
	b		AMDBulkEraseCheckTimer

				;else
				;{
					;CountsElapsed = NewTimerReading - PreviousTimerReading;
				;}

NoAMDBulkEraseTimerRollOver
	mov		r8,r7
	sub		r8,r8,r5

AMDBulkEraseCheckTimer

				;// Decrement the timer by the number of counts elapsed, unless the number of counts elapsed is greater.
				;if(CountsElapsed > EraseTimer)
				;{
					;WriteParameters->WriteStatus = FLASH_ERASE_FAILURE;
				;}

	cmp		r8,r3
	blt		AMDBulkEraseTimerNotExpired
	ldr		r9,=2
	str		r9,[r0,#0x38]
	ldr		r3,=0
	b		AMDBulkEraseStoreTimerReading

				;else
				;{
					;EraseTimer -= CountsElapsed;
				;}

AMDBulkEraseTimerNotExpired
	sub		r3,r3,r8

AMDBulkEraseStoreTimerReading

				;// Store the new reading.
				;PreviousTimerReading = NewTimerReading;

	mov		r5,r7

			;}	//end while ((FLASH_BYTE_POINTER[AddressOfNextSectorToErase] != 0xff) && (EraseTimer))

	RESET_WATCHDOG_TIMER
	b		CheckToSeeIfAMDBulkFlashEraseIsFinished


		;}
		;else	//else if not ( WriteParameters->FlashSupportsBulkErase )
		;{




;************************************************************************************************************************************
;************************************************************************************************************************************
;************************************************************************************************************************************
;	INDIVIDUAL SECTOR ERASE OPERATION - Start and finish erase command on one sector before going on to the next sector.
;************************************************************************************************************************************
;************************************************************************************************************************************
;************************************************************************************************************************************

IndividualSectorEraseOperation
	ldr		r2,[r0,#0x14]	; R2 is number of sectors to erase
	ldr		r1,[r0,#0x18]	; R1 is AddressesOfSectorsToErase
	mov		r9,#0
	str		r9,[r0,#0x38]

			;// Issue all the erase commands at once.
			;while((NumberOfSectorsToErase) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))
			;{

EraseNextAMDSector
	cmp		r2,#0
	ble		AMDFlashEraseIsFinished

				;AddressOfNextSectorToErase = AddressesOfSectorsToErase[i] + FLASH_WRITE_BASE;	//Start erasing with first sector

	ldr		r6,[r1],#4		; R6 is next sector to erase
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r6,r6,r8

				;FLASH_WORD_POINTER[0x555] = 0xAA;		// Issue the commands to flash to do the erase

	ldr		r7,=0xAA0	; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0xAA
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x2AA] = 0x55;

	ldr		r7,=0x550	; To get 0x554 into r1, move 0x550 first, then add 0x4 to accommodate 8 bit constant maximum
	ldr		r8,=0x4
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0x55
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x555] = 0x80;

	ldr		r7,=0xAA0	; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0x80
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x555] = 0xAA;		// Issue the commands to flash to do the erase

	ldr		r7,=0xAA0	; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0xAA
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x2AA] = 0x55;

	ldr		r7,=0x550	; To get 0x554 into r1, move 0x550 first, then add 0x4 to accommodate 8 bit constant maximum
	ldr		r8,=0x4
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0x55
	strh		r8,[r7]

				;FLASH_WORD_POINTER[AddressOfNextSectorToErase] = 0x30;

	ldr		r7,=0x30
	strh		r7,[r6]

				;// Now that the next sector erase command has been issued, wait until 0xFF is seen in the sector or the timer is exhausted.
				;while((FLASH_BYTE_POINTER[AddressOfNextSectorToErase] != 0xff) && (EraseTimer))
				;{

	ldr		r10,=0x00FFFFFF	; R10 is wait count

CheckToSeeIfAMDSectorFlashEraseIsFinished
	subs	r10,r10,#1
	beq		EraseNextAMDSectorError
	ldrh	r7,[r6]
	ldrh	r8,[r6]
	ldrh	r9,[r6]
	eor		r9,r8,r9
	eor		r8,r7,r8
	and		r9,r9,r8
	ands	r9,r9,#0x40		
	beq		EraseNextAMDSectorIsFinished	;no toggle program done??
	ands	r7,r7,#0x20
	beq		CheckToSeeIfAMDSectorFlashEraseIsFinished
EraseNextAMDSectorError
	mov		r9,#2
	str		r9,[r0,#0x38]
	b		AMDFlashEraseIsFinished


EraseNextAMDSectorIsFinished

	RESET_WATCHDOG_TIMER
	sub		r2,r2,#1
	b		EraseNextAMDSector

		;}	//end else if not ( WriteParameters->FlashSupportsBulkErase )

AMDFlashEraseIsFinished

		;// If we failed during the erase operation, WOOF!!!!
		;if(WriteParameters->WriteStatus != FLASH_WRITE_OK)
		;{
			;WriteParameters->AddressOfFailure = WriteParameters->Destination;
			;return;
		;}

	ldr		r9,[r0,#0x38]
	cmp		r9,#0
	beq		AMDStartFlashWrite
	str		r6,[r0,#0x3C]
	b		AMDFlashWriteFinished



;********************************************
;********************************************
;********************************************
;	WRITE OPERATION
;********************************************
;********************************************
;********************************************





;;
;;*****************************************************************************************************************
;;
;* We're doing the write in three iterations: preserved data in the start sector, new data, and preserved data in the
;* end sector.  We'll use R11 as an iteration counter and setup R5 (source address), R6 (destination address), and
;* R2 (amount of data).
;;
;;*****************************************************************************************************************
;;

AMDStartFlashWrite
	ldr		r11,=1

AMDFlashWriteDoNextDataWriteIteration
	cmp		r11,#1
	bne		AMDFlashWriteTryIteration2

;; Iteration #1
	ldr		r5,[r0,#0x20]		; source
	ldr		r6,[r0,#0x24]		; destination + FLASH_WRITE_BASE
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r6,r6,r8
	ldr		r2,[r0,#0x28]		; amount of data in bytes - still needs to be converted to word count
	b		AMDFlashWriteStartNewWriteIteration

AMDFlashWriteTryIteration2
	cmp		r11,#2
	bne		AMDFlashWriteTryIteration3

;; Iteration #2

		;// Setup up new data addresses and size
		;Source = ((WORD *) DataToWriteToFlash);		// Set up source and destination addresses
		;Destination = ((WORD *) (FlashAddressToWriteTo + FLASH_WRITE_BASE));
		;NumberOfWordsToWrite = WriteParameters->NumberOfBytesToWrite;

	ldr		r5,[r0]			; R5 is source address
	ldr		r6,[r0,#0x4]		; R6 is destination address
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r6,r6,r8
	ldr		r2,[r0,#0x8]		; R2 will hold # of words to be written
	b		AMDFlashWriteStartNewWriteIteration

AMDFlashWriteTryIteration3
	cmp		r11,#3
	bne		AMDFlashWriteDataWriteIterationsFinished

;; Iteration #3
	ldr		r5,[r0,#0x2C]		; source
	ldr		r6,[r0,#0x30]		; destination + FLASH_WRITE_BASE
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r6,r6,r8
	ldr		r2,[r0,#0x34]		; amount of data in bytes - still needs to be converted to word count
	b		AMDFlashWriteStartNewWriteIteration
	RESET_WATCHDOG_TIMER

AMDFlashWriteStartNewWriteIteration

		;// Convert number of bytes to write to a word count.
		;words =(bytes+1)/2

	add		r2,r2,#1
	mov		r2,r2, LSR #1

		;while((NumberOfWordsToWrite) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))
		;{

AMDFlashWriteNextWord
	cmp		r2,#0
	beq		AMDFlashWriteDataWriteIterationComplete
	ldr		r9,[r0,#0x38]
	cmp		r9,#0
	bne		AMDFlashWriteDataWriteIterationComplete

	
			;WordNotWritten = TRUE;			// Word not successfully written yet

	ldr		r4,=1			; R4 will indicate write not finished

			;while((WordNotWritten) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))
			;{

AMDFlashWriteWaitForWriteComplete

				;FLASH_WORD_POINTER[0x555] = 0xAA;	// Issue write command

	ldr		r7,=0xAA0	; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0xAA
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x2AA] = 0x55;

	ldr		r7,=0x550	; To get 0x554 into r1, move 0x550 first, then add 0x4 to accommodate 8 bit constant maximum
	ldr		r8,=0x4
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0x55
	strh		r8,[r7]

				;FLASH_WORD_POINTER[0x555] = 0xA0;

	ldr		r7,=0xAA0	; To get 0xAAA into r1, move 0xAA0 first, then add 0xA to accommodate 8 bit constant maximum
	ldr		r8,=0xA
	add		r7,r7,r8
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r7,r7,r8
	ldr		r8,=0xA0
	strh		r8,[r7]

				;WriteParameters->Destination = WriteParameters->Source;	// Write the next word

	ldrh		r9,[r5]
	strh		r9,[r6]

				;WaitForFlashStability = WriteParameters->WaitForFlashStabilityCycleCount;

	ldr		r10,=0x00FFFFFF	; R10 is wait count

				;// If the flash chip doesn't support bulk erase, also use the Data Polling algorithm instead of the Toggle Bit algorithm
				;if( WriteParameters->FlashSupportsBulkErase )
				;{


					;while(WaitForFlashStability)
					;{

AMDFlashWriteWaitForStabilize
	subs	r10,r10,#1
	beq		AMDNextFlashWriteError
	ldrh	r7,[r6]
	ldrh	r8,[r6]
	ldrh	r9,[r6]
	eor		r9,r8,r9
	eor		r8,r7,r8
	and		r9,r9,r8
	ands	r9,r9,#0x40		
	beq		ProgDone	;no toggle program done??
	ands	r7,r7,#0x20
	beq		AMDFlashWriteWaitForStabilize
	
;; error 	
AMDNextFlashWriteError						;if (S2 == 0)
	ldr		r7,=0xf0
	strh	r7,[r6]		;had an error so reset the flash
	ldrh	r7,[r6]						;S2 = S1 ^ FLASH_BYTE_POINTER[0x0];
	ldrh	r9,[r5]
	cmp		r9,r7
	bne		AAMDFlashWriteFailure	;no toggle program done??
				;if(WriteParameters->Destination == WriteParameters->Source)
ProgDone			;{

	ldrh	r7,[r5]
	ldrh	r8,[r6]
	cmp		r7,r8
	bne		AAMDFlashWriteFailure

					;WordNotWritten = FALSE;

	ldr		r4,=0	; R4 will indicate write finished

					;WriteParameters->Source++;

	add		r5,r5,#2

					;WriteParameters->Destination++;

	add		r6,r6,#2

					;NumberOfWordsToWrite--;

	sub		r2,r2,#1
	b		AMDFlashWriteNextWord
AAMDFlashWriteFailure

						;WriteParameters->WriteStatus = FLASH_WRITE_FAILURE;

	ldr		r9,=1
	str		r9,[r0,#0x38]

						;WriteParameters->AddressOfFailure = WriteParameters->Destination;
					;}

	str		r6,[r0,#0x3C]

;;
;;* Increment data write iteration counter
;;
AMDFlashWriteDataWriteIterationComplete
	add		r11,r11,#1
	b		AMDFlashWriteDoNextDataWriteIteration

AMDFlashWriteDataWriteIterationsFinished

AMDFlashWriteFinished


		;// If we're supposed to reset, do it now; otherwise, return.
		;if(WriteParameters->ForceWatchdogResetAfterWrite)
		;{
			;FORCE_WATCHDOG_RESET();
		;}

	ldr		r9,[r0,#0x4C]
	cmp		r9,#0
	beq		AMDFlashWriteReturnNormally
	FORCE_WATCHDOG_RESET

AMDFlashWriteReturnNormally
	ldmia	sp!,{r2-r12,pc}		; Restore registers (input link is written directly to PC)

EndOfWriteFlashAMD
;;
;;*****************************************************************************************************************
;;







;;
;;*****************************************************************************************************************
;;
;;	WriteFlashAMD
;;		Call the actual WriteFlashAMD by jumping to the address supplied by the caller.
;;
;;		C prototype: void WriteFlashAMD (FLASH_WRITE_PARAMETERS *WriteParameters, void *AddressOfWriteFlashRoutine);
;;

WriteFlashAMD
	mov		pc,r1		; Jump to ActualWriteFlashAMD address supplied by caller
;;
;;*****************************************************************************************************************
;;






;;
;;*****************************************************************************************************************
;;
;;	GetSizeAndAddressOfWriteFlashAMD
;;		Return the address of ActualWriteFlashAMD and the size of ActualWriteFlashAMD
;;		as the difference between the address of EndOfWriteFlashAMD and ActualWriteFlashAMD.
;;
;;		C prototype: void GetSizeAndAddressOfWriteFlashAMD (ULONG *AddressOfWriteFlashAMD, ULONG *SizeOfWriteFlashAMD);
;;

GetSizeAndAddressOfWriteFlashAMD
	stmdb		sp!,{r2-r3,lr}				; Save registers

	ldr		r2,=ActualWriteFlashAMD	; Return the address of ActualWriteFlashAMD to caller
	str		r2,[r0]

	ldr		r2,=EndOfWriteFlashAMD		; Return the size of ActualWriteFlashAMD
	ldr		r3,=ActualWriteFlashAMD
	sub		r2,r2,r3
	str		r2,[r1]

	ldmia		sp!,{r2-r3,pc} 			; Restore registers (input link is written directly to PC)
;;
;;*****************************************************************************************************************
;;





;;
;;*****************************************************************************************************************
;;
;;	Intel flash write routines
;;
;;
;;*****************************************************************************************************************
;;





;;
;;*****************************************************************************************************************
;;
;;	ActualWriteFlashIntel
;;		Write Intel flash.  Note that the "C" source lines are included to explain what's going on in the
;;		assembly code.
;;
;;		Note: It is assumed that new data being written, start sector data being preserved, and end sector data
;;			being preserved have sizes that are multiples of 2 and have addresses that fall on a 2 byte boundary!
;;
;;		C prototype: void ActualWriteFlashIntel (FLASH_WRITE_PARAMETERS *WriteParameters);
;;

ActualWriteFlashIntel
 	stmdb		sp!,{r2-r11,lr}				; Save registers

		;// Assume no errors yet.
		;WriteParameters->WriteStatus = FLASH_WRITE_OK;

	ldr		r9,=0
	str		r9,[r0,#0x38]



;********************************************
;********************************************
;********************************************
;	ERASE OPERATION
;********************************************
;********************************************
;********************************************


	ldr		r4,=RTC_BASE_ADDR_UPPER	; We're using R4 to store the TCN1 address.  Once again, because of 8 bit constant
						; limitation, we're loading the address in two parts.
	add		r4,r4,#RTC_BASE_ADDR_LOWER
	add		r4,r4,#RTC_BASE_ADDR_RTC_SECOND_OFFSET
	ldr		r5,[r4]			; Store the current timer reading in R5

	ldr		r2,[r0,#0x14]	; R2 is number of sectors to erase
	ldr		r1,[r0,#0x18]	; R1 is AddressesOfSectorsToErase
	ldr		r3,[r0,#0x1C]	; R3 is EraseTimeout

		;// Issue all the erase commands at once.
		;while((NumberOfSectorsToErase) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))
		;{

EraseNextIntelSector
	cmp		r2,#0
	ble		FinishedIntelErase
	ldr		r9,[r0,#0x38]
	cmp		r9,#0
	bne		FinishedIntelErase

;;* Set timer count as resolution of system timer in seconds multiplied by the number of seconds to wait for a
;;* successful sector erase.

			;EraseTimer = FLASH_SECTOR_ERASE_TIMEOUT_INTEL * timer_granularity();
			;mov		r9,r3		; Store next erase countdown in R9

			;// Set previous timer reading to current system timer.
			;PreviousTimerReading = read_timer();

	ldr		r5,[r4]		; Store the current timer reading in R5

			;AddressOfNextSectorToErase = AddressesOfSectorsToErase[i];	// Start erasing with first sector

	ldr		r6,[r1],#4			; R6 is next sector to erase
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r6,r6,r8

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0x60;	// Issue the commands to flash to unlock the block.

	ldr		r7,=0x60
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0xD0;

	ldr		r7,=0xD0
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0xFF;	// Return to read array mode

	ldr		r7,=0xFF
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0x20;	// Issue the commands to flash to do the erase

	ldr		r7,=0x20
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0xD0;	// Issue the commands to flash to do the confirm

	ldr		r7,=0xD0
	strh		r7,[r6]

			;// Now that the sector erase command has been issued, wait for this sector to complete.
			;while( ((FLASH_BYTE_POINTER[0] & 0x80) == 0x00) && (EraseTimer))
			;{

CheckToSeeIfIntelFlashSectorEraseIsFinished
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r8,=0x0
	ldrb		r7,[r8]
	ldr		r8,=0x0
	and		r7,r7,#0x80
	cmp		r8,r7
	bne		IntelFlashSectorEraseIsFinished
	cmp		r3,#0
;	cmp		r9,#0
	beq		IntelFlashSectorEraseIsFinished

				;// Get new timer count.
				;NewTimerReading = read_timer();

	ldr		r7,[r4]

;;* If the timer has rolled over (it is less than the previous reading), then counts elapsed is equal to the new timer reading +
;;* the complement of the previous reading + 1; otherwise, the counts elapsed is the difference between new and previous.

				;if( NewTimerReading < PreviousTimerReading )
				;{

	cmp		r7,r5
	bge		NoIntelEraseTimerRollOver

					;CountsElapsed = NewTimerReading + (~PreviousTimerReading) + 1;
					;CountsElapsed = NewTimerReading + (~PreviousTimerReading) + 1;
					;CountsElapsed = NewTimerReading + (60 - PreviousTimerReading);	// Note that we're using a 60 second real-time clock.
		 		;}

	ldr		r8,=60
	sub		r8,r8,r5
;	mvn		r8,r5
;	add		r8,r8,#1
	add		r8,r8,r7
	b		IntelEraseCheckTimer

				;else
				;{
					;CountsElapsed = NewTimerReading - PreviousTimerReading;
				;}

NoIntelEraseTimerRollOver
	mov		r8,r7
	sub		r8,r8,r5

IntelEraseCheckTimer
;;* Decrement the timer by the number of counts elapsed, unless the number of counts elapsed is greater.

				;if(CountsElapsed > EraseTimer)
				;{
					;EraseTimer = 0;
				;}

	cmp		r8,r3
	blt		IntelEraseTimerNotExpired
	ldr		r9,=2
	str		r9,[r0,#0x38]
	ldr		r3,=0
;	ldr		r9,=0
	b		IntelEraseStoreTimerReading

				;else
				;{
					;EraseTimer -= CountsElapsed;
				;}

IntelEraseTimerNotExpired
	sub		r3,r3,r8
;	sub		r9,r9,r8

IntelEraseStoreTimerReading

				;// Store the new reading.
				;PreviousTimerReading = NewTimerReading;

	mov		r5,r7
	RESET_WATCHDOG_TIMER
	b		CheckToSeeIfIntelFlashSectorEraseIsFinished

			;}		//end while ( ((FLASH_BYTE_POINTER[0] & 0x80) == 0x00) && (EraseTimer))

IntelFlashSectorEraseIsFinished

;;* Log an error if not success, else go to the next sector to erase.

			;if ((!EraseTimer) || ( FLASH_BYTE_POINTER[0] & 0x20 == 0x20 ))
			;{

	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r8,=0x0
	ldrb		r7,[r8]
	and		r7,r7,#0x20
	ldr		r8,=0x20
	cmp		r8,r7
	beq		IntelFlashSectorEraseError
	cmp		r3,#0
;	cmp		r9,#0
	bne		IntelFlashUnlockSector

				;WriteParameters->WriteStatus = FLASH_ERASE_FAILURE;
			;}

IntelFlashSectorEraseError
	ldr		r9,=2
	str		r9,[r0,#0x38]

IntelFlashUnlockSector

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0x60;	// Issue the commands to flash to relock the block.

	ldr		r7,=0x60
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0x01;

	ldr		r7,=0x01
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0xFF;	// Return to read array mode

	ldr		r7,=0xFF
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[0x00] = 0x50;			// clear status

	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r8,=0x0
	ldr		r7,=0x50
	strh		r7,[r8]

			;FLASH_BYTE_POINTER[0x00] = 0xff;			// read array mode

	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r8,=0x0
	ldr		r7,=0xFF
	strh		r7,[r8]

			;--NumberOfSectorsToErase;
		;}		//end while ((NumberOfSectorsToErase) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))

	RESET_WATCHDOG_TIMER
	sub		r2,r2,#1
	b		EraseNextIntelSector

FinishedIntelErase

		;FLASH_BYTE_POINTER[0x00] = 0x50;			// clear status

	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r8,=0x0
	ldr		r7,=0x50
	strh		r7,[r8]

		;FLASH_BYTE_POINTER[0x00] = 0xff;			// read array mode

	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r8,=0x0
	ldr		r7,=0xFF
	strh		r7,[r8]

		;// If we failed during the erase operation, WOOF!!!!
		;if(WriteParameters->WriteStatus != FLASH_WRITE_OK)
		;{
			;return;
		;}

	ldr		r9,[r0,#0x38]
	cmp		r9,#0
	bne		IntelFlashWriteFinished



;********************************************
;********************************************
;********************************************
;	WRITE OPERATION
;********************************************
;********************************************
;********************************************


		;// Unlock sectors to be written.
		;while(NumberOfSectorsToUnlock)
		;{

	ldr		r9,[r0,#0x14]			; Load number of sectors to be unlocked
	ldr		r10,[r0,#0x18]			; Load pointer to addresses of sectors to be unlocked

IntelFlashWriteUnlockSectors
	cmp		r9,#0
	ble		IntelFlashWriteFinishedUnlockingSectors

			;AddressOfNextSectorToUnlock = AddressesOfSectors[i];	// Address of next sector to be unlocked

	ldr		r6,[r10],#4
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r6,r6,r8

			;FLASH_BYTE_POINTER[AddressOfNextSectorToUnlock] = 0x60;	// Issue the commands to flash to unlock the block.

	ldr		r7,=0x60
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToUnlock] = 0xD0;	//second half of unlock block command

	ldr		r7,=0xD0
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0xFF;	// Return to read array mode

	ldr		r7,=0xFF
	strh		r7,[r6]

			;--NumberOfSectorsToUnlock

	sub		r9,r9,#1
	b		IntelFlashWriteUnlockSectors

		;}		//end  while ( NumberOfSectorsToUnlock )

;;
;;*****************************************************************************************************************
;;
;* We're doing the write in three iterations: preserved data in the start sector, new data, and preserved data in the
;* end sector.  We'll use R11 as an iteration counter and setup R5 (source address), R6 (destination address), and
;* R2 (amount of data).
;;
;;*****************************************************************************************************************
;;

IntelFlashWriteFinishedUnlockingSectors
	ldr		r11,=1

IntelFlashWriteDoNextDataWriteIteration
	cmp		r11,#1
	bne		IntelFlashWriteTryIteration2

;; Iteration #1
	ldr		r5,[r0,#0x20]		; source
	ldr		r6,[r0,#0x24]		; destination
	ldr		r2,[r0,#0x28]		; amount of data in bytes - still needs to be converted to word count
	b		IntelFlashWriteStartNewWriteIteration

IntelFlashWriteTryIteration2
	cmp		r11,#2
	bne		IntelFlashWriteTryIteration3

;; Iteration #2
		;// Setup up new data addresses and size
		;Source = ((WORD *) DataToWriteToFlash);		// Set up source and destination addresses
		;Destination = ((WORD *) FlashAddressToWriteTo);
		;NumberOfWordsToWrite = WriteParameters->NumberOfBytesToWrite;

	ldr		r5,[r0]				; R5 is source address
	ldr		r6,[r0,#0x4]		; R6 is destination address
	ldr		r2,[r0,#0x8]		; R2 will hold # of words to be written
	b		IntelFlashWriteStartNewWriteIteration

IntelFlashWriteTryIteration3
	cmp		r11,#3
	bne		IntelFlashWriteDataWriteIterationsFinished

;; Iteration #3
	ldr		r5,[r0,#0x2C]		; source
	ldr		r6,[r0,#0x30]		; destination
	ldr		r2,[r0,#0x34]		; amount of data in bytes - still needs to be converted to word count
	b		IntelFlashWriteStartNewWriteIteration

IntelFlashWriteStartNewWriteIteration
	ldr		r8,[r0,#0x48]		; FLASH_WRITE_BASE
	add		r6,r6,r8

		;// Convert number of bytes to write to a word count.
		;NumberOfWordsToWrite = NumberOfBytesToWrite / 2 + (NumberOfBytesToWrite & 1);

	mov		r9,r2
	mov		r2,r2, LSR #1
	and		r9,r9,#1			; Clear all but the lowest bit
	add		r2,r2,r9

;;* Keep writing words to flash while there are more to go and no errors have occurred

		;while((NumberOfWordsToWrite) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))
		;{

IntelFlashWriteNextWord
	cmp		r2,#0
	beq		IntelFlashWriteDataWriteIterationComplete
	ldr		r9,[r0,#0x38]
	cmp		r9,#0
	bne		IntelFlashWriteDataWriteIterationComplete

			;RetryCount = WriteParameters->NumberOfRetries;	// Reset retry count for next write cycle

	ldr		r3,[r0,#0x0C]		; R3 will hold retry count

			;WordNotWritten = TRUE;			// Word not successfully written yet

	ldr		r4,=1				; R4 will indicate write not finished

			;while((WordNotWritten) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))
			;{

IntelFlashWriteWaitForWriteComplete
	cmp		r4,#0
	beq		IntelFlashWriteWordComplete
	ldr		r9,[r0,#0x38]
	cmp		r9,#0
	bne		IntelFlashWriteWordComplete

				;WriteParameters->Destination = 0x40;	// Write command

	ldr		r9,=0x40
	strh		r9,[r6]

				;WriteParameters->Destination = WriteParameters->Source;		// Write the next word

	ldrh		r9,[r5]
	strh		r9,[r6]

				;while ( (FLASH_BYTE_POINTER[0] & 0x80) == 0 )
				;{};				// wait for completion

IntelFlashWriteWaitForWriteStatus
	ldr		r7,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r7,=0
	ldrb		r8,[r7]
	and		r8,r8,#0x80
	cmp		r8,#0
	beq		IntelFlashWriteWaitForWriteStatus

				;// could check for (FLASH_BYTE_POINTER[0] == 0x00)
				;FLASH_BYTE_POINTER[0x00] = 0x50;	// clear status

	ldr		r7,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r7,=0
	ldr		r8,=0x50
	strh		r8,[r7]

				;FLASH_BYTE_POINTER[0x00] = 0xff;	// read array mode

	ldr		r7,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r7,=0
	ldr		r8,=0xFF
	strh		r8,[r7]

				;if(WriteParameters->Destination == WriteParameters->Source)
				;{

	ldr		r7,=0			; Make sure that all four bytes are zeroed
	ldr		r8,=0
	ldrh		r7,[r5]
	ldrh		r8,[r6]
	cmp		r7,r8
	bne		IntelFlashWriteFailRetry

					;WordNotWritten = FALSE;

	ldr		r4,=0				; R4 will indicate write finished

					;WriteParameters->Source++;

	add		r5,r5,#2

					;WriteParameters->Destination++;

	add		r6,r6,#2

					;NumberOfWordsToWrite--;

	sub		r2,r2,#1

				;}	//end if (WriteParameters->Destination == WriteParameters->Source)

	b		IntelFlashWriteStabilityCheckComplete

				;else		//else if not (WriteParameters->Destination == WriteParameters->Source)
				;{

IntelFlashWriteFailRetry

;;* Flash word was not successfully written.  If number of retries has been exceed, log an error and return, else rewrite word and update
;;* retry count.
					;if(RetryCount)
					;{

	cmp		r3,#0
	beq		IntelFlashWriteFailure

						;--RetryCount;
					;}

	sub		r3,r3,#1
	b		IntelFlashWriteRetryCountComplete

					;else
					;{

IntelFlashWriteFailure

						;WriteParameters->WriteStatus = FLASH_WRITE_FAILURE;

	ldr		r9,=1
	str		r9,[r0,#0x38]

						;WriteParameters->AddressOfFailure = WriteParameters->Destination;
					;}

	str		r6,[r0,#0x3C]

IntelFlashWriteRetryCountComplete

				;}		//end else if not (WriteParameters->Destination == WriteParameters->Source)

IntelFlashWriteStabilityCheckComplete
	RESET_WATCHDOG_TIMER
	b		IntelFlashWriteWaitForWriteComplete

			;}		//end while ((WordNotWritten) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))

IntelFlashWriteWordComplete
	b		IntelFlashWriteNextWord

		;}		//end while ((NumberOfWordsToWrite) && (WriteParameters->WriteStatus == FLASH_WRITE_OK))

;;* Increment data write iteration counter
IntelFlashWriteDataWriteIterationComplete
	add		r11,r11,#1
	b		IntelFlashWriteDoNextDataWriteIteration

IntelFlashWriteDataWriteIterationsFinished

;;* Relock the sectors just written.

		;while(NumberOfSectorsToLock)
		;{

	ldr		r9,[r0,#0x14]			; Load number of sectors to be locked
	ldr		r10,[r0,#0x18]			; Load pointer to addresses of sectors to be locked

IntelFlashWriteLockSectors
	cmp		r9,#0
	ble		IntelFlashWriteFinishedLockingSectors

			;AddressOfNextSectorToLock = AddressesOfSectors[i];	// Address of next sector to be unlocked

	ldr		r6,[r10],#4
	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
	add		r6,r6,r8

			;FLASH_BYTE_POINTER[AddressOfNextSectorToUnlock] = 0x60;	// Issue the commands to flash to relock the block.

	ldr		r7,=0x60
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToUnlock] = 0x01;

	ldr		r7,=0x01
	strh		r7,[r6]

			;FLASH_BYTE_POINTER[AddressOfNextSectorToErase] = 0xFF;	// Return to read array mode

	ldr		r7,=0xFF
	strh		r7,[r6]

			;--NumberOfSectorsToUnlock

	sub		r9,r9,#1
	b		IntelFlashWriteLockSectors

		;}		//end while (NumberOfSectorsToLock)

IntelFlashWriteFinishedLockingSectors

		;FLASH_BYTE_POINTER[0x00] = 0x50;			// clear status

	ldr		r7,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r7,=0x0
	ldr		r8,=0x50
	strh		r8,[r7]

		;FLASH_BYTE_POINTER[0x00] = 0xff;			// read array mode

	ldr		r7,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r7,=0x0
	ldr		r8,=0xFF
	strh		r8,[r7]

IntelFlashWriteFinished

		;FLASH_BYTE_POINTER[0x00] = 0xff;			// read array mode

	ldr		r8,[r0,#0x48]	; FLASH_WRITE_BASE
;	ldr		r8,=0x0
	ldr		r7,=0xFF
	strh		r7,[r8]

;****	No longer used
;****;// If we're supposed to transfer control to another address, do it now; otherwise, return.
		;****;if(WriteParameters->JumpToAddressAfterWriteCompletion)
		;****;{
		;****;move WriteParameters->AddressToJumpTo to PC
		;****;}
;****	ldr		r9,[r0,#0x40]
;****	cmp		r9,#0
;****	beq		IntelFlashWriteReturnNormally
;****	ldr		r9,[r0,#0x44]
;****	mov		pc,r9


;;* If we're supposed to reset, do it now; otherwise, return.

		;if(WriteParameters->ForceWatchdogResetAfterWrite)
		;{
			;FORCE_WATCHDOG_RESET();
		;}

	ldr		r9,[r0,#0x4C]
	cmp		r9,#0
	beq		IntelFlashWriteReturnNormally
	FORCE_WATCHDOG_RESET

IntelFlashWriteReturnNormally
	ldmia	sp!,{r2-r11,pc}				; Restore registers (input link is written directly to PC)

EndOfWriteFlashIntel
;;
;;*****************************************************************************************************************
;;






;;
;;*****************************************************************************************************************
;;
;;	WriteFlashIntel
;;		Call the actual WriteFlashIntel by jumping to the address supplied by the caller.
;;
;;		C prototype: void WriteFlashIntel (FLASH_WRITE_PARAMETERS *WriteParameters, void *AddressOfWriteFlashRoutine);
;;

WriteFlashIntel
	mov		pc,r1		; Jump to ActualWriteFlashIntel address supplied by caller
;;
;;*****************************************************************************************************************
;;





;;
;;*****************************************************************************************************************
;;
;;	GetSizeAndAddressOfWriteFlashIntel
;;		Return the address of ActualWriteFlashIntel and the size of ActualWriteFlashIntel
;;		as the difference between the address of EndOfWriteFlashIntel and ActualWriteFlashIntel.
;;
;;		C prototype: void GetSizeAndAddressOfWriteFlashIntel (ULONG *AddressOfWriteFlashIntel, ULONG *SizeOfWriteFlashIntel);
;;

GetSizeAndAddressOfWriteFlashIntel
	stmdb		sp!,{r2-r3,lr}				; Save registers

	ldr		r2,=ActualWriteFlashIntel	; Return the address of ActualWriteFlashIntel to caller
	str		r2,[r0]

	ldr		r2,=EndOfWriteFlashIntel		; Return the size of ActualWriteFlashIntel
	ldr		r3,=ActualWriteFlashIntel
	sub		r2,r2,r3
	str		r2,[r1]

	ldmia		sp!,{r2-r3,pc}				; Restore registers (input link is written directly to PC)
;;
;;*****************************************************************************************************************
;;



	END


