;;
; MX1 Dragonball defines for MMU/Cache setup
; Date: 3/08/02
; Target: Matrix MX1 Development board
; Tools: ARM SDT 2.5 for now... will be ADS 1.1 or GNU
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: db_shared/db_mmu.s $
;; $Revision: 1.25.2.18 $
;; $Date: 2009/10/08 13:28:29EDT $
;;======================================================================================
;
; Starting point was Digital defines from the days of the StrongArm (Now Intel's,
; but originally a Digital part)... code was changed for our hardware and the differences in the
; MX1 versus SA.
;
;; this code is intended to provide all the necessary routines for
;; enabling MMU, Icache and Dcache for this hardware.
; Op_code will only be stored in Flash... will be run out of RAM @ C0000000.
;
; So, intent of MMU setup is:
;	1.  1<->1 mapping of VA and PA address space
; 	2.  1MB sections for 4MB of DRAM
;	3.  64KB pages for Flash  -- might not do because no code or data will be here
;	4.  Cacheable and bufferable access to DRAM
;	5.  RO user access to Flash
; 	6.  Invalid accesses (aborts) configured for unsupported mem space
; 	7.  Noncacheable and nonburferable I/O -- and -- Internal Reg space
;

	INCLUDE	mmumacro.s
	INCLUDE	db_memap.s

		REQUIRE8
		PRESERVE8

	IF {CPU}="ARM926EJ-S"
;; cache specifics arm926EJS
LG2_CSIZE	equ	14						;; log 2 of cache size in bytes 14= 16kb
LG2_LSIZE	equ	5						;; log 2 of line size in bytes 5= 32 bytes
LG2_NWAY	equ	2						;; 920 is 64 way 926 is 4 way
SET_SHFT	equ	5						;; number of bits to shift to get to lsb of set index
LG2_SSIZE	equ	(LG2_CSIZE-LG2_LSIZE-LG2_NWAY) ;; lo2 of set size
WAY_SHIFT	equ	(32-LG2_NWAY)

	ELSE
;; cache specifics arm920T
LG2_CSIZE	equ	14						;; log 2 of cache size in bytes 14= 16kb
LG2_LSIZE	equ	5						;; log 2 of line size in bytes 5= 32 bytes
LG2_NWAY	equ	6						;; 920 is 64 way
SET_SHFT	equ	5						;; number of bits to shift to get to lsb of set index
LG2_SSIZE	equ	(LG2_CSIZE-LG2_LSIZE-LG2_NWAY) ;; lo2 of set size
WAY_SHIFT	equ	(32-LG2_NWAY)
	ENDIF

	AREA	MMU_mvectorss,CODE, READONLY
	EXTERN  Vector_Init_Block[WEAK]
	EXPORT	Move_Vector_Table_To_Zero

;;Move_Vector_Table_To_Zero------------------------------------
;; use there example, but avoid using r9 as it may be used for relocateability
;; move vector table from some other location to virtual location 0x0
;; this will essentially move the vector table (8 words) and the associative pointers (8 words immediately following) that are used
;;
Move_Vector_Table_To_Zero
     stmfd		sp!,{r0-r10,lr}
     ldr		r10,=VectorTablePhysicalAddress	;; destination
     ldr		r14,=Vector_Init_Block					;; source
     ldmia		r14!,{r0-r7}							;; Copy vectors
     stmia		r10!,{r0-r7}
     ldmia		r14!,{r0-r7}							;; Copy and associative pointers
     stmia		r10!,{r0-r7}
     ldmfd		sp!,{r0-r10,pc}




	AREA	MMU_code,CODE, READONLY

	EXPORT	MMU_init
	EXPORT	MMU_reset
	EXPORT	MMU_CleanAndInvalidate
	EXPORT	RelocateAndRun

;;;;
;
; Please note:
;
; --> Dcache w/out write buffer is not allowed
; --> Base register updated before tables ASSUMES MMU is off
; --> MMU enabled with SR bits 00 --> AP bits govern access
; --> Page tables must be aligned to their size
;		level1 tables occupy 16KB

;
; 1st thing I need to do is write the translation base register
; set the domain reg: domain0 CLIENT, domain 1-15 NO_ACCESS
;
; **** Here's where the assumption may hurt... reassigning the base
;	register before the TTB are set up assumes MMU off
;   but I said that above... so I am sure you were listening.
; ****
;
; set up page tables -- all for domain0, flat address map
;  	4x1MB sections for DRAM  -- Read/write all
;							 -- cacheable, bufferable
;   1MB section for Flash -- Read/write spvr, Read only user
;			   	-- cacheable, bufferable
;				-- note... for now, I will probably leave this alone entirely
;
;  ** Special section ** reserved for clean loops
; 		**** dedicated VA space for faster access
; 		**** mapped to SSRAM for fastest access ... can't do in my system because no SRAM
;		**** exeption to flat map translation rule
;	no access for aliased DRAM, Flash
; 	I/O map enabled noncacheable, nonbufferable incl aliases
;
;

; Flush ITB and DTBs
; Flush Icache
; Enable MMU, alignment faulting, and conditionally the Icache, Dcache, and Write buffer

;;
; reminder... goal here is to keep everything 1 to 1... otherwise my brain will really start
; to hurt
;;-----------------------------------------------------------------------------------------



MMU_init
	stmfd	sp!,{r3,lr}
	ldr		r0, =MMU_LEVEL_1_TABLE
	ldr		r14,[r0]						; Use r14 to hold the level 1 table address
	WRCP15_TTBase		r14					; initialize TTBR
	ldr		r0, =1
	WRCP15_DAControl	r0					; initialize Domain access register for 1 useable domain
	; 1st things 1st... Clear all TT entries -- FAULT
	ldr		r0, =0							; clear r0 this is what we write to table 0 is fault
	mov		r1, r14							; r1 has start of level 1 table
	ldr		r3, =L1_TABLE_ENTRIES			; r3 has number of level 1 enties used to loop to fill table
TTclrloop
	str		r0, [r1], #4					; clear 32 bit entry
	subs	r3,r3,#1						; decrement number of entries to go
	bne		TTclrloop						; loop until done

;;
; DRAM configuration
;	Configure according to parameters in shared table.
;
	ldr		r0,	=CACHED_MEMORY_BASE
	ldr		r1,[r0]
	ldr		r0,	=MEMORY_SIZE
	ldr		r2,[r0]
	add		r1,r14,r1, lsr #18				; Shift right by 256K to get offset
	ldr		r0,=DRAM_ACCESS+ExternalRamBase ; This is the physical address we want to map to
Dramloop
	str		r0, [r1], #4					; store level 1 TT entry
	add		r0, r0, #0x00100000				; keep doing for amount of 1MB sections
	subs	r2, r2,#0x00100000
	bne		Dramloop
;; non cached mapping of dram
	ldr		r0,	=NON_CACHED_MEMORY_BASE
	ldr		r1,[r0]
	ldr		r0,	=MEMORY_SIZE
	ldr		r2,[r0]
	add		r1,r14,r1, lsr #18					; Shift right by 256K to get offset
	ldr		r0,=NCDRAM_ACCESS+ExternalRamBase 	; This is the physical address we want to map to
NCDramloop
	str r0, [r1], #4							; store level 1 TT entry
	add	r0, r0, #0x00100000						; keep doing for amount of 1MB sections
	subs r2, r2,#0x00100000
	bne	NCDramloop
	IF (platform_current = "mxl") 				;; mxl targets have nor flash othes do not

; Remapped cached flash.
	ldr	r0,	=CACHED_ROM_BASE
	ldr	r1,[r0]
	ldr	r0,	=ROM_SIZE
	ldr	r2,[r0]
	cmp	r2,#0
	beq	IO_SPACE_START
	add	r1,r14,r1, lsr #18						; Shift right by 256K to get offset
	ldr	r0,=FLASH_CACHED_ACCCESS+ExternalRomBase 	; This is the physical address we want to map to
CachedFlashLoop
	str r0, [r1], #4							; store level 1 TT entry
	add	r0, r0, #0x00100000						; keep doing for amount of 1MB sections
	subs r2, r2,#0x00100000
	bne	CachedFlashLoop
; Remapped uncached flash.
	ldr	r0,	=NON_CACHED_ROM_BASE
	ldr	r1,[r0]
	ldr	r0,	=ROM_SIZE
	ldr	r2,[r0]
	add	r1,r14,r1, lsr #18							; Shift right by 256K to get offset
	ldr	r0, =FLASH_UNCACHED_ACCCESS+ExternalRomBase ; Configure for no caching
													; This is the physical address we want to map to
NonCachedFlashLoop
	str r0, [r1], #4						; store level 1 TT entry
	add	r0, r0, #0x00100000					; keep doing for amount of 1MB sections
	subs r2, r2,#0x00100000
	bne	NonCachedFlashLoop
	ENDIF								;; mxl targets with nor flash
IO_SPACE_START
	adr	r0,IO_MAP_TABLE
IO_SPACE
; I/O configuration area
;	1M of internal registers, non cacheable, non bufferable
	ldr		r2, [r0],#4						; r2 is # of 1M blocks
	cmp		r2,#-1							; if -1 this is the end of table (IO_MAP_TABLE) we are done
	beq		IO_END
	ldr		r1, [r0],#4						; shifted Phys add of 1M block (offset into Lev 1 table)
	ldr		r3, [r0],#4						; get section descriptor info (virt add+ access)
	add		r1,r1,r14
IO_loop
	subs 	r2,r2,#1						; decr number of sections 0 in table below  skips enty
	bmi		IO_SPACE						; next table entry (IO_MAP_TABLE)
	str		r3, [r1], #4					; store TT entry
	add		r3, r3,#(1<<20)					; add section field
	b		IO_loop
IO_END
; Now that we've set up everything else, let's remap the virtual vector address to the physical vector location.
	ldr	r1, =(VectorTableVirtualAddress:SHR:(20-2))	;Load the level 1 table entry for the level 2 vector table
	add	r1,r1,r14								;r1= address  of vector table entry in L1 table
	ldr	r0, =MMU_LEVEL_2_TABLE
	ldr	r2,[r0]									;Use r2 to hold the level 2 table address
	add	r2,r2,#1								;Note that bit 1 must be 0 and bit 0 must be 1.
	str	r2, [r1]								;;store TT entry
	sub	r2,r2,#1								; address of L2 table again
	ldr	r3,=(VECTOR_TABLE_ACCESS:AND:0x0f)		; AP3-AP0 read only, cached, buffered, small page reference
	add	r3,r3,#VectorTablePhysicalAddress		;; this is 0x8000000 i.e beginning of ram for gen 6 0xa0000000
	ldr	r0,=VECTOR_TABLE_SIZE					;R3 contains the physical address + the page configuration information
												;; r0 contains number of enties (256 for large page)
VectorTableLoop
	str		r3,[r2],#4							;Load the page configuration.
	mov		r3,#0							;; all entries except first no accsess
	subs		r0,r0,#1
	bne		VectorTableLoop
;; flush TLBs flush Icache
	ldr		r0,=0
	mcr		p15,0,r0,c7,c7,0		;; flush both caches
	WRCP15_FlushITB_DTB	r0						; flush ITB and DTB
	RDCP15_Control	r0
	ldr		r1, =EnableMMU+EnableDcache+EnableIcache+EnableMMU_R+EnableAlignFault+EnableMMU_RR
	orr		r0,r0,r1
	WRCP15_Control	r0				; TURN ON MMU
	ldmia	sp!,{r3,pc}				; return from whence I came...


IO_MAP_TABLE
	DCD	IO1_SIZE						; # of 1M blocks
	DCD	(IO1_BASE:SHR:(20-2))		;shifted virtual address of first 1M section
	DCD	IO1_BASE+IO1_ACCESS			;Phys addr and accsess permitions S+R+C+B
	DCD	IO2_SIZE						; # of 1M blocks
	DCD	(IO2_BASE:SHR:(20-2))		;shifted virtual address of first 1M section
	DCD	IO2_BASE+IO2_ACCESS			;Phys addr and accsess permitions S+R+C+B
	DCD	IO3_SIZE						; # of 1M blocks
	DCD	(IO3_BASE:SHR:(20-2))		;shifted virtual address of first 1M section
	DCD	IO3_BASE+IO3_ACCESS			;Phys addr and accsess permitions S+R+C+B
	DCD	IO4_SIZE						; # of 1M blocks
	DCD	(IO4_BASE:SHR:(20-2))		;shifted virtual address of first 1M section
	DCD	IO4_BASE+IO4_ACCESS			;Phys addr and accsess permitions S+R+C+B
	DCD	IO5_SIZE						; # of 1M blocks
	DCD	(IO5_BASE:SHR:(20-2))		;shifted virtual address of first 1M section
	DCD	IO5_BASE+IO5_ACCESS			;Phys addr and accsess permitions S+R+C+B

	DCD	VRAM_NUMSECTIONS
	DCD	(VRAM_BASE:SHR:(20-2))		;shifted virtual address of first 1M section
	DCD	VRAM_BASE+VRAM_ACCESS
	DCD	-1


;
;;	MMU_reset routine...
;; Cleans and flushes d cache
;; flushes i cache
;; turns cahes and mmu off
MMU_reset
;**	For now, don't do a store multiple because of the SDRAM / store multiple problem on MX1.
	stmfd		sp!,{r0,lr}
	bl		MMU_CleanAndInvalidateBothCaches
	mov		r0,#0
	mcr		p15,0,r0,c7,c10,4			;; wait for wite buffer to drain before disabling
	RDCP15_Control	r0
	ldr	lr, =EnableMMU+EnableDcache+EnableIcache+EnableMMU_R+EnableAlignFault+EnableMMU_RR
	bic	r0,r0,lr
	WRCP15_Control	r0			;; reset it
	nop
	nop
	nop
	nop
	ldmfd	sp!,{r0,pc}


	AREA	MMU_CacheCode,CODE, READONLY

Relocate_Run


; priviledged mode -- SYSTEM mode.  The intent is to never run in user mode.
;

	msr		cpsr_c,#0xdf ;;Mode_SYS:OR:I_Bit:OR:F_Bit
	adr		r14,st_stack
	ldr		r14,[r14]
	ldr  	sp,[r14]
 	mov		lr,r1		;; start address of new ap in lr
 	cmp		r1,r0
 	bge		cpyrevstart
cpyfwd
	ldr		r3,[r0],#4
	str		r3,[r1],#4
	subs	r2,r2,#4
	bne		cpyfwd
	b		MMU_CleanAndInvalidateBothCaches
st_stack
	DCD		SYS_STACK
cpyrevstart
	add		r0,r0,r2
	add		r1,r1,r2
cpyrev
	ldr		r3,[r0,#-4]!
	str		r3,[r1,#-4]!
	subs	r2,r2,#4
	bne		cpyrev
;; MMU_CleanAndInvalidateBothCaches
;; Cleans and flushes d cache
;; flushes i cache

	EXPORT	MMU_CleanAndInvalidateBothCaches
MMU_CleanAndInvalidateBothCaches
	stmfd	sp!,{r0,lr}
	bl		MMU_CleanAndInvalidate
	mov		r0,#0
	WRCP15_FlushIC r0
	ldmfd	sp!,{r0,pc}


;;============ MMU_CleanAndInvalidate =================================
;; clean and invalidate d cache
;;MXL arm920 uses set/way method
;;MX27 arm926EJS uses test clean
;; irqs should be off
;; uses R0

	IF {CPU}="ARM926EJ-S"
;; use test clean
MMU_CleanAndInvalidate
	RDCP15_Control	r0
	tst		r0,#EnableMMU
	beq		MMU_CleanAndInvalidate_Finish
	IF :DEF: CACHEWRITETHRU
	mov		r0,#0
	mcr		p15,0,r0,c7,c6,0			;; for writethu cache invalidate only
MMU_CleanAndInvalidate_Finish
	mov		pc,lr
	ELSE
MMU_CleanAndInvalidate_loop
	mrc		p15,0,pc,c7,c14,3			;; for 926 use test clean use of pc as shown in arm 926 TRM
	bne		MMU_CleanAndInvalidate_loop
MMU_CleanAndInvalidate_Finish
	bx		lr
	ENDIF
	ELSE
;; use set/way method
MMU_CleanAndInvalidate
	RDCP15_Control	r0
	tst		r0,#EnableMMU
	beq		MMU_CleanAndInvalidate_Finish
	mov		r0,#0
MMU_CleanAndInvalidate_loop
	mcr		p15,0,r0,c7,c14,2
	add		r0,r0,#1<<(SET_SHFT)			;; increment the set index
	tst		r0,#1<<(LG2_SSIZE+SET_SHFT) 	;; test overflow
	beq		MMU_CleanAndInvalidate_loop		;; not the last set so loop
	bic		r0,r0,#1<<(LG2_SSIZE+SET_SHFT)	;; clear the overflow i.e clear bit we tested above
	adds	r0,r0,#1<<26					;; increment way index
	bcc		MMU_CleanAndInvalidate_loop		;; an ovreflow means we are done since msb of way is always 31
MMU_CleanAndInvalidate_Finish
	bx		lr
	ENDIF
Relocate_Run_End
;
;	MMU_IsEnabled
;		Return 1 if MMU is enabled, 0 otherwise.
;
	EXPORT	MMU_IsEnabled
MMU_IsEnabled
	RDCP15_Control	r0
	and		r0,r0,#EnableMMU
	bx		lr

; 	void RelocateAndRun(int TRANSFER_ADDRESS, int EXECUTABLE_RELOCATION_ADDRESS,
;										int SizeOfExecutableImage);
;
RelocateAndRun
	ldr		r3,=HEAP_END
	ldr		r3,[r3]
	add		r4,r3,#(Relocate_Run_End-Relocate_Run)

	adr		r5,Relocate_Run_End
copy_mmu
	ldr		r6,[r5,#-4]!
	str		r6,[r4,#-4]!
	cmp		r3,r4
	bne		copy_mmu
	bl		MMU_CleanAndInvalidateBothCaches
	bx		r3						;; jump to where we loaded the relocator




	AREA	MMU_Cache_ON_OFF,CODE, READONLY
	EXPORT DCacheOn
DCacheOn	
	RDCP15_Control	r0
	ldr		r1, =EnableDcache
	orr		r0,r0,r1
	WRCP15_Control	r0	; urn on icache
	bx		lr


	EXPORT DCacheOff
DCacheOff
	mov	r1,lr		;; save link register call below changes it
	bl	MMU_CleanAndInvalidate ;; flush and invalidate cache
	mov	lr,r1				;;restore link register	
	RDCP15_Control	r0
	ldr		r1, =EnableDcache
	bic		r0,r0,r1
	WRCP15_Control	r0	; urn on icache
	bx	lr
	
	EXPORT ICacheOn
ICacheOn	
	RDCP15_Control	r0
	ldr		r1, =EnableIcache
	orr		r0,r0,r1
	WRCP15_Control	r0	; urn on icache
	bx	lr


	EXPORT ICacheOff
ICacheOff
	mov		r0,#0
	WRCP15_FlushIC r0
	RDCP15_Control	r0
	ldr		r1, =EnableIcache
	bic		r0,r0,r1
	WRCP15_Control	r0	; urn on icache
	bx	lr


;
;	********************************************************************************************************************************
;
;	All code within this IF is unneeded by boot.
;
;	********************************************************************************************************************************
;
	IF	:DEF: MATRIX_BOOT_CODE

	ELSE




	EXPORT		ICacheLockdown
;	r0 = pointer to array of addresses of code segments to be locked down
;	r1 = pointer to array of sizes of code segments to be locked down
;	r2 = number of code segments to be locked down
ICacheLockdown
	stmfd	sp!,{r3-r12, lr}
; Prepare to jump to non-cache RAM to actually perform the lockdown, so that no part of this routine is loaded into ICache during the
; lockdown process.
	ldr		r6,=NON_CACHED_MEMORY_BASE
	ldr		r3,[r6]									; r3 = base address of non-cache RAM
	ldr		r6,=CACHED_MEMORY_BASE
	ldr		r4,[r6]									; r4 = base address of cache RAM
	adr		r5,ICacheLockdownNonCacheOffsetJump
	sub		r5,r5,r4
	add		r5,r5,r3								; r5 = address of ICacheLockdownNonCacheOffsetJump in non-cache RAM
	bx		r5
ICacheLockdownNonCacheOffsetJump
	ldr		r3,=0								; Lockdown base
	ldr		r4,=0								; Start with clean ICache
	WRCP15_FlushIC r4
ICacheLockdownNextCodeSegmentLoop
	subs	r2,r2,#1							; Keep looping through code segments until we reach the end
	blt		ICacheLockdownFinish
	ldr		r4,[r0],#4								; r4 = start address of next code segment
	ldr		r5,[r1],#4
	add		r5,r5,r4								; r5 = end address of next code segment
	and		r4,r4,#0xFFFFFFE0						; Mask start address to a linefill boundary
	mcr		p15,0,r3,c9,c0,1						; Lockdown base index 0
ICacheLockdownCodeSegmentLoadLoop
	mcr		p15,0,r4,c7,c13,1					; Prefetch next line
	add		r4,r4,#32							; r4 = next line in code segment
	and		r6,r4,#0xe0							; Did the segment roll over into a new tag?
	cmp		r6,#0x0
	addeq	r3,r3,#0x1<<26						; Increment base index and lockdown if the segment rolled over into a new tag
	mcreq	p15,0,r3,c9,c0,1
	cmp		r4,r5									; Keep looping until we're done with this segment
	ble		ICacheLockdownCodeSegmentLoadLoop
	cmp		r6,#0x0								; If more lines were added after the last base index increment and lockdown, then lock
	addne	r3,r3,#0x1<<26						; the additional lines
	mcrne	p15,0,r3,c9,c0,1
	b		ICacheLockdownNextCodeSegmentLoop
ICacheLockdownFinish							; Return to caller
	ldmfd	sp!,{r3-r12, pc}


	AREA	MMU_ReadITLB_DTLB,CODE, READONLY


;
;	Return the current value of the instruction TLB register to the caller via R0
;
	EXPORT		ITLB_Read
ITLB_Read
	mrc		p15,0,r0,c10,c0,1
	bx		lr




;
;	Return the current value of the data TLB register to the caller via R0
;
	EXPORT		DTLB_Read
DTLB_Read
	mrc		p15,0,r0,c10,c0,0
	bx		lr
;
;	********************************************************************************************************************************
;
;	All code within this IF is unneeded by boot.
;
;	********************************************************************************************************************************
;
	ENDIF
	END


