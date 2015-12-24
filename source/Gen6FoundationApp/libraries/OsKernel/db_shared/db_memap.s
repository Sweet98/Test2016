;/************************************************************************/
;//
;// HHP 4480 embedded firmware
;// Memory map
;//  Date:       2/21/02
;//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
;//  Target:     Dragon Ball MX1 based 4480
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: db_shared/db_memap.s $
;; $Revision: 1.21.2.10 $
;; $Date: 2009/10/06 17:11:17EDT $
;;======================================================================================
;************************************************************************
;************************************************************************


;; Flash fixed addresses (1MB)
;;
;;	0x00000000 to 0x00003FFF	Boot code
;;	0x00004000 to 0x00005FFF	Backup (User Defaults) Configuration data
;;	0x00006000 to 0x00007FFF	Configuration data
;;	0x00008000 to 0x0000FFFF	Manufacturing configuration area
;;	0x00010000 to 0x000EFFFF	Operational code
;;	0x000F0000 to 0x000FFFF7	Reserved
;;	0x000FFFF8 to 0x000FFFFB	Operational code size
;;	0x000FFFFC to 0x000FFFFF	Operational code CRC


;; RAM fixed addresses (16 MB)
;;
;;    0x08000000 Op code code then data then heap
;;    various stacks run from top down
;;    0x08ffffff top of RAM


;; DDR SDRAM fixed addresses (128 MB)
;;
;;    0xA0000000 Op code code then data then heap for iMX27
;;    various stacks run from top down
;;    0xA7FFFFFF top of RAM 

;; DDR SDRAM fixed addresses (128 MB)
;;
;;    0x80000000 Op code code then data then heap for iMX25
;;    various stacks run from top down
;;    0xA7FFFFFF top of RAM 


;;
;;This is set by assembler build options in the Make file 
;;
;;GBLS    platform_current
;platform_current	SETS	"mxl"		;;"mxl"
;platform_current	SETS	"imx27"		;;"imx27"
;platform_current	SETS	"imx25"		;;"imx25"

;;
;; various sizes 

SZ_1G          EQU       0x40000000
SZ_128M        EQU       0x08000000
SZ_64M         EQU       0x04000000
SZ_32M         EQU       0x02000000
SZ_16M         EQU       0x01000000
SZ_8M          EQU       0x00800000
SZ_4M          EQU       0x00400000
SZ_2M          EQU       0x00200000
SZ_1M          EQU       0x00100000
SZ_512K        EQU       0x00080000

;; 2 areas of significant memory blocks are the  external SDRAM, and the external Flash ROM
;;Configure them by size and platform here
;***********************************************************************************
	IF platform_current = "imx25"
;***********************************************************************************

ExternalRamBase     	EQU 0x80000000      ;; beginning of SDRAM, CSD0
ExternalRamLimit    	EQU 0x88000000
ExternalRamSize     	EQU 0x08000000
	IF :DEF: CACHEWRITETHRU
DRAM_ACCESS				EQU 0xc0a       ; AP=11, domain0, C=B=1
	ELSE
DRAM_ACCESS				EQU 0xc0e       ; AP=11, domain0, C=B=1
	ENDIF	
NCDRAM_ACCESS			EQU 0xc02       ; AP=11, domain0, C=B=0

;; External Flash ROM range
ExternalRomStart    	EQU  0xA0000000
ExternalRomBase     	EQU  ExternalRomStart

;; CACHE
CachedExternalRomBase	EQU	ExternalRomStart
UncachedExternalRomBase	EQU 0x00400000
ExternalRomSize			EQU	0x00100000
FLASH_CACHED_ACCCESS	EQU	0x80a		; AP=10, domain0, c=1, b=0
FLASH_UNCACHED_ACCCESS	EQU	0x802		; AP=10, domain0, c=0, b=0

;; x25 i/o space, the BASE Address should be aligned to 1M boundaries which are used for the MMU table

IO1_SIZE				EQU 0x1			; I/O area is actually 156K in size, but 1M is the minimum
IO1_ACCESS				EQU 0xc02   	; AP=11, domain0, c=b=0
IO1_BASE				EQU 0x43F00000	; Mx25 internal registers and i/o
IO2_SIZE				EQU 0x1			;1 Mb 
IO2_ACCESS				EQU 0xc02		; non cache non buffer
IO2_BASE				EQU 0x50000000	; Mx25 internal registers and i/o
IO3_SIZE				EQU 0x1			;1 Mb 
IO3_ACCESS				EQU 0xc02		; non cache non buffer
IO3_BASE				EQU 0x53F00000	; Mx25 internal registers and i/o
IO4_SIZE				EQU 0x1			;1 Mb 
IO4_ACCESS				EQU 0xc02		; non cache non buffer
IO4_BASE				EQU 0x68000000	; Mx25 internal registers and i/o (asic interrupt controller registers)
IO5_SIZE				EQU 0x1			;1 Mb 
IO5_ACCESS				EQU 0xc02		; non cache non buffer
IO5_BASE				EQU 0xB8000000	; Mx25 internal registers and i/o (sdram controller m3if weim emi)

;;Vector Table
VectorTableVirtualAddress	EQU	0x00000000

	IF :DEF: RAMBASEDBOOT
VectorTablePhysicalAddress	EQU	0x78000000
VRAM_Size				EQU	0x00020000		;; starts at 0x78000000 - 0x78020000
VRAM_Limit				EQU VRAM_BASE + VRAM_Size
	ELSE
VectorTablePhysicalAddress	EQU	ExternalRamBase
	ENDIF

VECTOR_TABLE_ACCESS     	EQU 0xffe       ; AP=11, domain0, C=B=1
VECTOR_TABLE_SIZE     		EQU 256			; 256 4K entries
;; on chip ram possible location for vectors 45kb  
VRAM_NUMSECTIONS		EQU	1				;; use 1 level 1 sction 
VRAM_BASE				EQU	0x78000000		;; start of 1M section that vram is in vram actually
VRAM_ACCESS				EQU	0xc0e 			;; cachable bufferable 


;***********************************************************************************
	ELSE
		IF platform_current = "imx27"
;***********************************************************************************

;; External DDR SDRAM ranges
ExternalRamBase     	EQU 0xA0000000      ;; beginning of SDRAM, CSD0
ExternalRamLimit    	EQU 0xA8000000
ExternalRamSize     	EQU 0x08000000
DRAM_ACCESS				EQU 0xc0e       ; AP=11, domain0, C=B=1
NCDRAM_ACCESS			EQU 0xc02       ; AP=11, domain0, C=B=0

;; External Flash ROM range
ExternalRomStart    	EQU  0xC0000000
ExternalRomBase     	EQU  ExternalRomStart

;; CACHE
CachedExternalRomBase	EQU	ExternalRomStart
UncachedExternalRomBase	EQU 0x00400000
ExternalRomSize			EQU	0x00100000
FLASH_CACHED_ACCCESS	EQU	0x80a		; AP=10, domain0, c=1, b=0
FLASH_UNCACHED_ACCCESS	EQU	0x802		; AP=10, domain0, c=0, b=0

;; x27 i/o space
IO1_SIZE				EQU 0x1			; I/O area is actually 156K in size, but 1M is the minimum
IO1_ACCESS				EQU 0xc02   	; AP=11, domain0, c=b=0
IO1_BASE				EQU 0x10000000	; Mx27 internal registers and i/o
;;csi io registers
IO2_SIZE				EQU	1			;1 1MB sectiion
IO2_ACCESS				EQU	0xc02		;no cache no buffer
IO2_BASE				EQU	0x80000000	;csi registers located here
;; external memory interface registers
IO3_SIZE				EQU 1
IO3_ACCESS				EQU 0xc02
IO3_BASE				EQU 0xd8000000	; external memory registers ddr weim etc
;; not used size = 0
IO4_SIZE				EQU 0x0			
IO4_ACCESS				EQU 0x0		
IO4_BASE				EQU 0x0
IO5_SIZE				EQU 0x0			
IO5_ACCESS				EQU 0x0		
IO5_BASE				EQU 0x0	

;;Vector Table
VectorTableVirtualAddress	EQU	0x00000000
VectorTablePhysicalAddress	EQU	ExternalRamBase
VECTOR_TABLE_ACCESS     	EQU 0xffe       ; AP=11, domain0, C=B=1
VECTOR_TABLE_SIZE     		EQU 256			; 256 4K entries
;; on chip ram possible location for vectors 45kb  
VRAM_NUMSECTIONS		EQU	1				;; use 1 level 1 sction 
VRAM_BASE				EQU	0xfff00000		;; start of 1M section that vram is in vram actually
											;; starts at 0xffff4c00
VRAM_ACCESS				EQU	0xc0e 			;; cachable bufferable 

;***********************************************************************************
		ELSE		;;MXL
;***********************************************************************************

;; External SDRAM ranges
ExternalRamBase     	EQU 0x08000000      ;; beginning of SDRAM, CSD0
	IF :DEF: SixteenMB
ExternalRamLimit    	EQU 0x09000000
ExternalRamSize     	EQU 0x01000000
	ELSE
ExternalRamLimit    	EQU 0x08800000
ExternalRamSize     	EQU 0x00800000
	ENDIF
	
DRAM_ACCESS				EQU 0xc0e       ; AP=11, domain0, C=B=1
NCDRAM_ACCESS			EQU 0xc02       ; AP=11, domain0, C=B=0

;; External Flash ROM range
ExternalRomStart    	EQU  0x10000000
ExternalRomBase     	EQU  ExternalRomStart
;; CACHE
CachedExternalRomBase	EQU	0x10000000
UncachedExternalRomBase	EQU 0x00400000
ExternalRomSize			EQU	0x00100000
FLASH_CACHED_ACCCESS	EQU	0x80a		; AP=10, domain0, c=1, b=0
FLASH_UNCACHED_ACCCESS	EQU	0x802		; AP=10, domain0, c=0, b=0
;; i/o space processor internal periph registers
IO1_SIZE				EQU 0x1			; I/O area is actually 156K in size, but 1M is the minimum
IO1_ACCESS				EQU 0xc02   	; AP=11, domain0, c=b=0
IO1_BASE				EQU 0x00200000	; Mx1 internal registers and i/o
;; cs 4 used for 2020 uart mapped no cache no buffer
IO2_SIZE				EQU 0x1			; I/O area is actually 32 bytes in size, but 1M is the minimum
IO2_ACCESS				EQU	0xc02       ; AP=11, domain0, C=B=0
IO2_BASE				EQU	0x15000000  ; RF Base CS4 base
;; not used size = 0
IO3_SIZE				EQU 0x0			
IO3_ACCESS				EQU 0x0		
IO3_BASE				EQU 0x0	
IO4_SIZE				EQU 0x0			
IO4_ACCESS				EQU 0x0		
IO4_BASE				EQU 0x0
IO5_SIZE				EQU 0x0			
IO5_ACCESS				EQU 0x0		
IO5_BASE				EQU 0x0	



;; on chip ram possible location for vectors 45kb  
VRAM_NUMSECTIONS		EQU	0				;; skip this enty for mxl
VRAM_BASE				EQU	0		;; 
											;; starts at 0xffff4c00
VRAM_ACCESS				EQU	0 			;; cachable bufferable 

;;Vector Table
VectorTableVirtualAddress	EQU	0x00000000
VectorTablePhysicalAddress	EQU	ExternalRamBase
VECTOR_TABLE_ACCESS     	EQU 0xffe       ; AP=11, domain0, C=B=1
VECTOR_TABLE_SIZE     		EQU 256			; 256 4K entries

;***********************************************************************************
		ENDIF		;;end platform iMX27, MXL
	ENDIF			;;end platform iMX25
;***********************************************************************************

;;
;; Fixed address space configuration at the top of RAM.  The MMU level 1 translation tables and the cache disable buffer
;; are at the top of RAM, because their sizes should not be changing.  The stacks are below them, with the system mode / user
;; mode stack at the bottom, since it is the most likely candidate for growth.
;;
Level1tabSize				EQU	0x4000									;; MMU level 1 translation table size is 16K

	IF :DEF: RAMBASEDBOOT
Level1tab               	EQU VRAM_Limit - Level1tabSize				;; MMU level 1 translation table 16K below top of VRAM
	ELSE
Level1tab               	EQU ExternalRamLimit - Level1tabSize		;; MMU level 1 translation table 16K below top of RAM
	ENDIF

Level2VectorTableSize		EQU	0x1000									;; MMU level 2 vector table translation table size is 4K
Level2VectorTable       	EQU Level1tab - Level2VectorTableSize 		;; MMU level 2 vector translation table 4K below level 1 table
                            	                                	  	;; MMU level 1 translation table



;;CORSICA build  ;; used to be CF2D, but this is really a Corsica change (which includes mxl)

;; External Ram Model
SVC_StackSize			EQU	1024
ABT_StackSize			EQU	1024
UNDEF_StackSize			EQU	1024
IRQ_StackSize			EQU	1024
FIQ_StackSize			EQU	1024

	IF :DEF: RAMBASEDBOOT
		;;for Ram Based Boot make the SYS Stack smaller b/c its in Vram, also account for the Shared Table here
SYS_StackSize			EQU	0x2800
SharedTableSize			EQU 0x400
	ELSE
SYS_StackSize			EQU	0x62000
	ENDIF

Stack_Limit             EQU Level2VectorTable ; give 1K to all stacks and put them at the top of RAM
SVC_Stack               EQU Stack_Limit
ABT_Stack               EQU SVC_Stack-SVC_StackSize
UNDEF_Stack             EQU ABT_Stack-ABT_StackSize
IRQ_Stack               EQU UNDEF_Stack-UNDEF_StackSize
FIQ_Stack               EQU IRQ_Stack-IRQ_StackSize
USR_Stack               EQU FIQ_Stack-FIQ_StackSize
SYS_Stack               EQU USR_Stack

	IF :DEF: RAMBASEDBOOT
SharedTableLocation 	EQU USR_Stack-SYS_StackSize - SharedTableSize ;RAMStartOpCode - 0x400
Lower_Stack_Limit       EQU SharedTableLocation   ; User / system mode stack is ~400K
	ELSE
Lower_Stack_Limit       EQU USR_Stack-SYS_StackSize   ; User / system mode stack is ~400K
	ENDIF

End_Of_Heap				EQU Lower_Stack_Limit

;;END CORSICA



ROMStartBootCode         EQU       0x0

RAMStartOpCode           EQU       ExternalRamBase +0x1000


     INCLUDE   db_sharedtable_h.s            ;; this will integrate the associative shared boot/app table into all places it is necessary


     END


