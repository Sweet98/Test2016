;;
; MX1 Dragonball defines for MMU/Cache setup
; Date: 2/22/02
; Target: Matrix MX1 Development board
; Tools: ARM SDT 2.5 for now... will be ADS 1.1 or GNU
;
; Starting point was Digital defines from the days of the StrongArm (Now Intel's,
; but originally a Digital part)... code was changed for our hardware and the differences in the
; MX1 versus SA.
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: db_shared/mmumacro.s $
;; $Revision: 1.2 $
;; $Date: 2004/04/20 08:18:56EDT $
;;======================================================================================
;/*****************************************************************************

;       Copyright © 1995, 1996 Digital Equipment Corporation,
;                       Maynard, Massachusetts.

;                        All Rights Reserved

;Permission to use, copy, modify, and distribute this software and its
;documentation for any purpose and without fee is hereby granted, provided
;that the copyright notice and this permission notice appear in all copies
;of software and supporting documentation, and that the name of Digital not
;be used in advertising or publicity pertaining to distribution of the software
;without specific, written prior permission. Digital grants this permission
;provided that you prominently mark, as not part of the original, any
;modifications made to this software or documentation.

;Digital Equipment Corporation disclaims all warranties and/or guarantees
;with regard to this software, including all implied warranties of fitness for
;a particular purpose and merchantability, and makes no representations
;regarding the use of, or the results of the use of, the software and
;documentation in terms of correctness, accuracy, reliability, currentness or
;otherwise; and you rely on the software, documentation and results solely at
;your own risk.

;******************************************************************************

; Macros for SA-110 Coprocessor Access

;       $Id: db_shared/mmumacro.s 1.2 2004/04/20 08:18:56EDT keysb Exp  $

;******************************************************************************/


  INCLUDE mmu_h.s


; SA-110 *only* supports Coprocessor number 15
; Only MCR and MRC coprocessor instructions are supported - the others
; ...generate an UNDEFINED exception
;
; CP15 registers are architected as per the ARM V4 architecture spec
;
; Register0 ID register                   READ_ONLY
;       Register1       Control                       READ_WRITE
;       Register2       Translation Table Base        READ_WRITE
;       Register3       Domain Access Control         READ_WRITE
;       Register4       Reserved
;       Register5       Fault Status                  READ_WRITE
;       Register6       Fault Address                 READ_WRITE
;       Register7       Cache Operations              WRITE_ONLY
;       Register8       TLB Operations                WRITE_ONLY
;       Register9-14    Reserved
;       Register15      SA-110 specific tst/clk/idle  WRITE_ONLY

;; Defined Macros:
;;
;RDCP15_ID     Rx read of ID register
;RDCP15_Control    Rx   read of Control register
;WRCP15_Control    Rx write of Control register
;RDCP15_TTBase     Rx read of Translation Table Base reg.
;WRCP15_TTBase     Rx write of Translation Table Base reg.
;RDCP15_DAControl  Rx read of Domain Access Control reg.
;WRCP15_DAControl  Rx write of Domain Access Control reg.
;RDCP15_FaultStatus  Rx read of Fault Status register
;WRCP15_FaultStatus      Rx write of Fault Status register
;RDCP15_FaultAddress     Rx read Fault Address register
;WRCP15_FaultAddress   Rx write of Fault Address register
;WRCP15_FlushIC_DC   Rx cache control   Flush ICache + DCache
;;            Rx redundant but rqud for MACRO
;WRCP15_FlushIC    Rx cache control   Flush ICache
;;            Rx redundant but rqud for MACRO
;WRCP15_FlushDC    Rx cache control   Flush DCache
;;            Rx redundant but rqud for MACRO
;WRCP15_CacheFlushDentry Rx cache control   Flush DCache entry
;;            Rx source for VA
;WRCP15_CleanDCentry   Rx   cache control   Clean DCache entry
;;            Rx source for VA
;WRCP15_Clean_FlushDCentry Rx   cache control   Clean + Flush DCache entry
;;            Rx source for VA
;WRCP15_DrainWriteBuffer Rx Drain Write Buffer
;;            Rx redundant but rqud for MACRO
;WRCP15_FlushITB_DTB   Rx TLB control     Flush ITB's + DTB's
;;            Rx redundant but rqud for MACRO
;WRCP15_FlushITB   Rx TLB control     Flush ITB's
;;            Rx redundant but rqud for MACRO
;WRCP15_FlushDTB   Rx TLB control     Flush DTB's
;;            Rx redundant but rqud for MACRO
;WRCP15_FlushDTBentry    Rx TLB control     Flush DTB entry
;;            Rx source for VA
;WRCP15_EnableClockSW  Rx test/clock/idle control -Enable Clock Switching
;;            Rx redundant but rqud for MACRO
;WRCP15_DisableClockSW   Rx test/clock/idle control -Disable Clock Switching
;;            Rx redundant but rqud for MACRO
;WRCP15_DisablenMCLK   Rx test/clock/idle control -Disable nMCLK output
;;            Rx redundant but rqud for MACRO
;WRCP15_WaitInt    Rx test/clock/idle control -Wait for Interrupt
;;            Rx redundant but rqud for MACRO

;Coprocessor read of ID register
;
    MACRO
    RDCP15_ID $reg_number
    MRC p15, 0, $reg_number, c0, c0 ,0
    MEND

;Coprocessor read of Control register
;
    MACRO
    RDCP15_Control $reg_number
    MRC p15, 0, $reg_number, c1, c0 ,0

    MEND

;Coprocessor write of Control register
;
    MACRO
    WRCP15_Control $reg_number
    MCR p15, 0, $reg_number, c1, c0 ,0

    MEND

;Coprocessor read of Translation Table Base reg.
;
    MACRO
    RDCP15_TTBase $reg_number
    MRC p15, 0, $reg_number, c2, c0 ,0

    MEND

;Coprocessor write of Translation Table Base reg.
;
    MACRO
    WRCP15_TTBase $reg_number
    MCR p15, 0, $reg_number , c2, c0 ,0

    MEND

;Coprocessor read of Domain Access Control reg.
;
    MACRO
    RDCP15_DAControl $reg_number
    MRC p15, 0, $reg_number, c3, c0 ,0

    MEND

;Coprocessor write of Domain Access Control reg.
;
    MACRO
    WRCP15_DAControl $reg_number
    MCR p15, 0, $reg_number, c3, c0 ,0

    MEND

;Coprocessor read of Fault Status register
;
    MACRO
    RDCP15_FaultStatus $reg_number
    MRC p15, 0, $reg_number, c5, c0 ,0

    MEND

;Coprocessor write of Fault Status register
;
    MACRO
    WRCP15_FaultStatus $reg_number
    MCR p15, 0, $reg_number, c5, c0 ,0

    MEND

;Coprocessor read of Fault Address register
;
    MACRO
    RDCP15_FaultAddress $reg_number
    MRC p15, 0, $reg_number, c6, c0 ,0

    MEND

;Coprocessor write of Fault Address register
;
    MACRO
    WRCP15_FaultAddress $reg_number
    MCR p15, 0, $reg_number, c6, c0 ,0

    MEND

;Coprocessor cache control
;Flush ICache + DCache
;
    MACRO
    WRCP15_FlushIC_DC $reg_number
    MCR p15, 0, $reg_number, c7, c7 ,0

    MEND

;Coprocessor cache control
;Flush ICache
;
    MACRO
    WRCP15_FlushIC $reg_number
    MCR p15, 0, $reg_number, c7, c5 ,0

    MEND

;Coprocessor cache control
;Flush DCache
;
    MACRO
    WRCP15_FlushDC $reg_number
    MCR p15, 0, $reg_number, c7, c6 ,0

    MEND

;Coprocessor cache control
;Flush DCache entry
;
    MACRO
    WRCP15_CacheFlushDentry $reg_number
    MCR p15, 0, $reg_number, c7, c6 ,1

    MEND

;Coprocessor cache control
;Clean DCache entry
;
    MACRO
    WRCP15_CleanDCentry $reg_number
    MCR p15, 0, $reg_number, c7, c10 ,1

    MEND

;Coprocessor cache control
;Clean + Flush DCache entry
;
    MACRO
    WRCP15_Clean_FlushDCentry $reg_number
    MCR p15, 0, $reg_number, c7, c14 ,1

    MEND

;Coprocessor Drain Write Buffer
;
    MACRO
    WRCP15_DrainWriteBuffer $reg_number
    MCR p15, 0, $reg_number, c7, c10 ,4

    MEND


;Coprocessor TLB control
;Flush ITB + DTB
;
    MACRO
    WRCP15_FlushITB_DTB $reg_number
    MCR p15, 0, $reg_number, c8, c7 ,0

    MEND

;Coprocessor TLB control
;Flush ITB
;
    MACRO
    WRCP15_FlushITB $reg_number
    MCR p15, 0, $reg_number, c8, c5 ,0

    MEND

;Coprocessor TLB control
;Flush DTB
;
    MACRO
    WRCP15_FlushDTB $reg_number
    MCR p15, 0, $reg_number, c8, c6 ,0

    MEND

;Coprocessor TLB control
;Flush DTB entry

    MACRO
    WRCP15_FlushDTBentry $reg_number
    MCR p15, 0, $reg_number, c8, c6 ,1

    MEND

;Coprocessor test/clock/idle control
;Enable Clock Switching
;
    MACRO
    WRCP15_EnableClockSW $reg_number
    MCR p15, 0, $reg_number, c15, c1 ,2

    MEND

;Coprocessor test/clock/idle control
;Disable Clock Switching
;
    MACRO
    WRCP15_DisableClockSW $reg_number
    MCR p15, 0, $reg_number, c15, c2 ,2

    MEND

;Coprocessor test/clock/idle control
;Disable nMCLK output
;
    MACRO
    WRCP15_DisablenMCLK $reg_number
    MCR p15, 0, $reg_number, c15, c4 ,2

    MEND

;Coprocessor test/clock/idle control  ;; this is correct for ARM920T.
;Wait for Interrupt
;
    MACRO
    WRCP15_WaitInt $reg_number
    MCR p15, 0, $reg_number, c7, c0 ,4
 
    MEND




;
; MMUMACRO_CleanAndInvalidate
;		Clean and invalidate DCACHE and drain write buffer to clear all
;		DCACHE hits and buffered writes.
;
	MACRO
	MMUMACRO_CleanAndInvalidate		$reg1, $reg2, $reg3
	
	RDCP15_Control	$reg1
	ands	$reg1,$reg1,#EnableMMU
	beq		MMUMACRO_CleanAndInvalidateBothCaches_Finish

	mov		$reg3, #(8 - 1) << 5 ; 8 segments
l_seg
	orr		$reg2, $reg3, #(64 - 1) << 26 ; 64 entries
l_ent
 	mcr		p15, 0, $reg2, c7, c14, 2		; clean+invalidate D index
	subs	$reg2, $reg2, #1 << 26
	bcs		l_ent				; entries 63 to 0
	subs	$reg3, $reg3, #1 << 5
	bcs		l_seg				; segments 7 to 0

	mov		$reg2, #0
	mcr		p15, 0, $reg2, c7, c5, 0		; invalidate I cache
	mcr		p15, 0, $reg2, c7, c10, 4		; drain WB

MMUMACRO_CleanAndInvalidate_Finish

	MEND




;
; MMUMACRO_CleanAndInvalidateBothCaches
;		Clean and invalidate DCACHE, drain write buffer, and invalidate ICACHE to clear all
;		cache hits and buffered writes.  This macro is relocatable, as all constants are
;		embedded in the code.
;
	MACRO
	MMUMACRO_CleanAndInvalidateBothCaches	$reg1, $reg2, $reg3
	
	RDCP15_Control	$reg1
	ands	$reg1,$reg1,#EnableMMU
	beq		MMUMACRO_CleanAndInvalidateBothCaches_Finish

	mov		$reg1, #7
MMUMACRO_CleanAndInvalidateBothCaches_SegmentLoop
	mov		$reg2, #63
MMUMACRO_CleanAndInvalidateBothCaches_IndexLoop
	mov		$reg3,$reg2, lsl #26
	orr		$reg3,$reg3,$reg1, lsl #5

	mcr		p15, 0, $reg3, c7, c14, 2   ;// Clean and Invalidate D Cache line

	subs	$reg2,$reg2,#1
	bpl		MMUMACRO_CleanAndInvalidateBothCaches_IndexLoop


	subs	$reg1,$reg1,#1
	bpl		MMUMACRO_CleanAndInvalidateBothCaches_SegmentLoop


	mcr		p15, 0, $reg2, c7, c10, 4   ;// Drain the write buffer
	
	mcr		p15, 0, $reg2, c7, c5, 0		; invalidate I cache

MMUMACRO_CleanAndInvalidateBothCaches_Finish

	MEND




    END
    
    
    

