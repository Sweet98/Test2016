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
;; $RCSfile: db_shared/mmu_h.s $
;; $Revision: 1.10.1.5 $
;; $Date: 2009/05/27 17:20:24EDT $
;;======================================================================================
;/* ****************************************************************************
;
;       Copyright © 1995, 1996 Digital Equipment Corporation,
;                       Maynard, Massachusetts.
;
;                       All Rights Reserved
;
;Permission to use, copy, modify, and distribute this software and its
;documentation for any purpose and without fee is hereby granted, provided
;that the copyright notice and this permission notice appear in all copies
;of software and supporting documentation, and that the name of Digital not
;be used in advertising or publicity pertaining to distribution of the
;software without specific, written prior permission. Digital grants this
;permission provided that you prominently mark, as not part of the original,
;any modifications made to this software or documentation.
;
;Digital Equipment Corporation disclaims all warranties and/or guarantees
;with regard to this software, including all implied warranties of fitness
;for a particular purpose and merchantability, and makes no representations
;regarding the use of, or the results of the use of, the software and
;documentation in terms of correctness, accuracy, reliability, currentness
;or otherwise; and you rely on the software, documentation and results
;solely at your own risk.
;
;****************************************************************************
;
;  SA-110 MMU specifics - constants, registers etc.
;
;
;***************************************************************************/


DCACHE_SIZE			EQU 0X4000		; MX1 has a 16KB Dcache
DCACHE_LINE			EQU 0X20		; 32B cache line entry
L1_TABLE_ENTRIES	EQU 0x1000  	; 16kB table (word entries)
L2_TABLE_ENTRIES  	EQU 0x100		;  1kB table (word entries)
L2_CONTROL			EQU 0x1			; domain0, page table pointer


; Definitions used in conditional assembly of Icache, Dcache and Write Buffer
; options

IC_ON     EQU 0x1000
IC_OFF    EQU 0x0

DC_ON     EQU 0x4
DC_OFF    EQU 0x0

WB_ON     EQU 0x8
WB_OFF    EQU 0x0


; Bit definitions for the control register:

; enables are logically OR'd with the control register
; use bit clears (BIC's) to disable functions
; *** all bits cleared on RESET ***

EnableMMU         EQU 0x1
EnableAlignFault  EQU 0x2
EnableDcache      EQU 0x4      ;0x4
EnableWB          EQU 0x8
EnableBigEndian   EQU 0x80
EnableMMU_S       EQU 0x100   ; selects MMU access checks
EnableMMU_R       EQU 0x200   ; selects MMU access checks
EnableIcache      EQU 0x1000
EnableMMU_RR	  EQU 0x4000

  END



