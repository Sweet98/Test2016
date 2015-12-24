;************************************************************************
;* MX1 Dragonball, relocate and execute downloaded images routines
;* Author: Barry Keys			       		11/30/98
;* Tools: ARM SDT 2.5 for now... will be ADS 1.1 or GNU
;* Assembler: ARM Software Development Toolkit (ARMasm) version 2.50
;* Target: Matrix MX1 Development board
;* Target: Strong Arm 1100 on PCB 08862 Rev A
;*
;*
;************************************************************************
;;
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: db_shared/flash_id_from_ram.s $
;; $Revision: 1.4 $
;; $Date: 2008/09/18 10:06:53EDT $
;;======================================================================================


;*
;*	File description:
;*		This file contains flash ID routines that must run in RAM.
;*		In the past, our application code was always running in RAM, but we
;*		now can optionally run out of ROM.  When querying flash for flash
;*		device type, flash reads return status information rather than what's
;*		stored in a particular flash location, so the processor will attempt to
;*		fetch the next instruction, but will get status information rather than
;*		the next instruction in flash, and we WOOF!!!!!!!!!  This file exists
;*		to perform operations in RAM, as subroutines, that cannot be performed
;*		in ROM.  The query operation has a relocatable routine to perform the
;*		corresponding operation, a routine that returns the size and address of
;*		the relocatable routine, and a routine that the application calls to
;*		actually perform the operation.  The application may then allocate
;*		space anywhere in RAM for the relocatable routine, copy it, and then run
;*		it.  The routine that the application calls to actually perform the
;*		operation is supplied with the address of the relocated routine.  The
;*		called routine then sets the PC to the address of the relocated routine.
;*		The reason for doing this is that a straight branch and link may not work
;*		if the address difference between the application and the relocated routine
;*		is greater than 24 bits.
;*
;;





		AREA	FlashQueryServices, CODE, READONLY



		EXPORT ActualQueryFlashType
		EXPORT QueryFlashType
		EXPORT GetSizeAndAddressOfQueryFlashType




;;
;;Flash type ID table
;;
UNKNOWN_FLASH_DEVICE	EQU		0xFF		; Keep this to a single byte; the relocated routine to ID flash can only
									; return 8 bit constants, since that's all ARM will allow to be embedded in
									; an instruction.
AMD_1MB_FLASH_DEVICE	EQU		0x0
INTEL_FLASH_DEVICE		EQU		0x1
AMD_2MB_FLASH_DEVICE	EQU		0x2
ST_29W800DB				EQU		0x3
MX_29LV800BTC			EQU		0x4
INTEL_1MB_TOP_BOOT		EQU		0x5
INTEL_1MB_BOTTOM_BOOT	EQU		0x6
INTEL_2MB_TOP_BOOT		EQU		0x7
INTEL_2MB_BOTTOM_BOOT	EQU		0x8
INTEL_4MB_TOP_BOOT		EQU		0x9
INTEL_4MB_BOTTOM_BOOT	EQU		0xA
INTEL_8MB_TOP_BOOT		EQU		0xB
INTEL_8MB_BOTTOM_BOOT	EQU		0xC
SPANSION_32NV2			EQU		0xD


;;
;;*****************************************************************************************************************
;;
;;
;;	ActualQueryFlashType
;;		ID the flash type and return to caller.  Note that the "C" source lines are included
;;		to explain what's going on in the assembly code.
;;
;;		C prototype: ULONG ActualQueryFlashType (void *FlashBaseAddress)
;;
;;		FlashBaseAddress passed in r0
;;
ActualQueryFlashType
	stmdb	sp!,{r1-r5,lr}				; Save registers
	ldr		r3,=0x90
	strh	r3,[r0]		;FLASH_BYTE_POINTER[0x0000] = 0x90;	// read config table
	ldrh	r4,[r0]		;MID = FLASH_BYTE_POINTER[0x0000];
	ldrh	r5,[r0,#2] 	;DID = FLASH_BYTE_POINTER[0x0001];
	ldr		r3,=0xFF
	strh	r3,[r0]		;FLASH_BYTE_POINTER[0x0000] = 0xFF;	// read array mode
		;if ( MID == 0x89 )
		;{

	cmp		r4,#0x89
	bne		NotIntelFlash

			;if(( DID & 0xFFFFFFF0 ) == 0x88C0 )
			;{
	ldr		r0,=INTEL_FLASH_DEVICE	;default intel
	mov		r4,r5
	bic		r4,r4,#0x0F
	ldr		r2,=0x8800		; To get 0x88C0 into r2, move 0x8800 first, then add 0xC0 to accommodate 8 bit constant maximum
	add		r2,r2,#0xC0
	cmp		r4,r2
	bne		FinishQueryFlashType
	bic		r5,r5,r2		; Clear these bits in the DID.  If this check works, then only the lowest byte should be non-zero.

				;if(( DID >= 0 ) && ( DID <= 5 ))
				;{
					;FlashType = INTEL_1MB_TOP_BOOT + DID;
				;}
	cmp		r5,#0x05
	bgt		IntelFlashCheck64MB
	ldr		r0,=INTEL_1MB_TOP_BOOT
	add		r0,r0,r5
	b		FinishQueryFlashType

				;else if(( DID >= 0x0C ) && ( DID <= 0x0D ))
				;{
				;FlashType = INTEL_8MB_TOP_BOOT + DID - 0x0C;
				;}

IntelFlashCheck64MB
	cmp		r5,#0x0C
	blt		FinishQueryFlashType
	cmp		r5,#0x0D
	ldrle	r0,=(INTEL_8MB_TOP_BOOT - 0x0c)
	addle	r0,r0,r5
	b		FinishQueryFlashType

				;else
				;{
				;	FlashType = INTEL_FLASH_DEVICE;	// INTEL
				;}
			;}
			;else
			;{
			;	FlashType = INTEL_FLASH_DEVICE;	// INTEL
			;}
		;}


		;else
		;{

NotIntelFlash
	adr		r1,NotIntelTab
	ldmia	r1!,{r2,r3}
	strh	r3,[r0,r2]		;FLASH_WORD_POINTER[0x555] = 0xAA;
	ldmia	r1!,{r2,r3}
	strh	r3,[r0,r2]		;FLASH_WORD_POINTER[0x2AA] = 0x55;
	ldmia	r1!,{r2,r3}
	strh	r3,[r0,r2]		;FLASH_WORD_POINTER[0x555] = 0x90;	// read config table		
	ldrh	r4,[r0]			;MID = FLASH_BYTE_POINTER[0x0000];
	ldrh	r5,[r0,#2]		;DID = FLASH_BYTE_POINTER[0x0002];
	ldrh	r1,[r0,#0x1c]
	ldr		r3,=0xF0
	strh	r3,[r0]

			;if ( MID == 0x01 )
			;{
				;if ( DID == 0x49 )
				;{
					;FlashType = AMD_2MB_FLASH_DEVICE;	// AMD
				;}
				;else
				;{
					;FlashType = AMD_1MB_FLASH_DEVICE;	// AMD
				;}
			;}
	and		r5,r5,#0xff
	cmp		r4,#0x01
	bne		NotAMDFlash
	ldr		r0,=AMD_1MB_FLASH_DEVICE	;default to 1M device
	cmp		r5,#0x7e					;spansion
	beq		spansion
	cmp		r5,#0x49
	ldreq	r0,=AMD_2MB_FLASH_DEVICE
	b		FinishQueryFlashType
spansion
	and		r1,r1,#0xff
	cmp		r1,#0x1d		;spansion 32n 4Mbyte
	ldreq	r0,=SPANSION_32NV2
	b		FinishQueryFlashType
	
			;else
			;{

NotAMDFlash


	;INCLUDE_OTHER_1MB_FLASH_TYPE_CHECKS	EQU	1
	;IF	:DEF:	INCLUDE_OTHER_1MB_FLASH_TYPE_CHECKS

	cmp		r4,#0x20
	bne		NotSTFlash
	cmp		r5,#0x5B
	bne		NotSTFlash
	ldr		r0,=ST_29W800DB
	b		FinishQueryFlashType

					;else
					;{

NotSTFlash
	cmp		r4,#0xC2
	cmpeq	r5,#0x5B
	ldreq	r0,=MX_29LV800BTC

							;else
							;{



	ldrne	r0,=UNKNOWN_FLASH_DEVICE

								;FlashType = UNKNOWN_FLASH_DEVICE;	// unknown
							;}				//end else if not ( DID == 0x5B )
						;}					//end if ( MID == 0xC2 )
					;}						//end else if not( DID == 0x5B )
				;}							//end if ( MID == 0x20 )
			;}								//end if not ( MID == 0x01 )
		;}									//end else if not ( MID == 0x89 )


FinishQueryFlashType
	ldmia	sp!,{r1-r5,pc}				; Restore registers (input link is written directly to PC)
;; command table address offest,data
NotIntelTab
	DCD	0xaaa,0xaa
	DCD	0x554,0x55
	DCD	0xaaa,0x90

EndOfQueryFlashType
;;
;;*****************************************************************************************************************
;;





;;
;;*****************************************************************************************************************
;;
;;	QueryFlashType
;;		Call the actual QueryFlashType by jumping to the address supplied by the caller.
;;
;;		C prototype: ULONG QueryFlashType (void *FlashBaseAddress, void *AddressOfQueryFlashType)
;;
;;		FlashBaseAddress passed in r0, to be used by ActualQueryFlashType
;;		AddressOfQueryFlashType passed in r1
;;
QueryFlashType
	mov		pc,r1
;;
;;*****************************************************************************************************************
;;






;;
;;*****************************************************************************************************************
;;
;;	GetSizeAndAddressOfQueryFlashType
;;		Return the address of ActualQueryFlashType and the size of ActualQueryFlashType
;;		as the difference between the address of EndOfQueryFlashType and ActualQueryFlashType.
;;
;;		C prototype: void GetSizeAndAddressOfQueryFlashType (ULONG *AddressOfQueryFlashType, ULONG *SizeOfQueryFlashType)
;;
GetSizeAndAddressOfQueryFlashType

	adr	r2,ActualQueryFlashType	; Return the address of ActualQueryFlashType to caller
	str	r2,[r0]
	adr	r0,EndOfQueryFlashType		; Return the size of ActualQueryFlashType
	sub	r0,r0,r2
	str	r0,[r1]
	mov	pc,lr				; Restore registers (input link is written directly to PC)
;;
;;*****************************************************************************************************************
;;



	END



;; $Log: db_shared/flash_id_from_ram.s  $
;; Revision 1.4 2008/09/18 10:06:53EDT CoyleL 
;; undid comment out of EXPORT ActualQueryFlashType
;; Revision 1.3 2008/09/12 15:05:52EDT WalczykJ 
;; clean up and add spansion id
;; Revision 1.2 2008/07/16 16:03:18EDT CoyleL 
;; change push on stack to an even # of registers, by using lr as a local register
;; Revision 1.1 2008/07/14 15:54:50EDT CoyleL
;; Initial revision
;; Member added to project d:/MKS_IT/Matrix/Matrix_Os.pj
;;
