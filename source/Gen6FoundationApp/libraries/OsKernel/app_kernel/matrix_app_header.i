;/************************************************************************/
;//
;// HIM 44xx embedded firmware
;//
;//  Op Code Startup -- Application Header --
;//  Date:       7/17/08
;//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1, Metaware, RVCT Build 674
;//  Target:     Dragonball Development Board
;//
;//======================================================================================
;//	Revision information
;//======================================================================================
;// $RCSfile: app_kernel/matrix_app_header.i $
;// $Revision: 1.1 $
;// $Date: 2008/07/28 17:47:31EDT $
;//======================================================================================

;*------------------------------------------------------------------------------
;* Header:  MatrixAppHeader.i
;*
;* This File defines the Matrix Application Header
;* The Application Header is used as a keyword for download tools to gain information about the file
;*
;* This used to be located in db_start.s and was moved to make it more portable
;* and more assessable by other files.
;*------------------------------------------------------------------------------


   EXPORT	MATRIX_APP_HEADER


;*
;*	This header is used by download tools to check the integrity and type of the code file.  The header, "MATRIXAPP", which is null
;*	terminated, is followed by the length of the type definition followed by the type definition.  The type definition consists of
;*	a mask where each set bit corresponds to a device type that this file is compatible with.
;*
MATRIX_APP_HEADER
	DCB		"MATRIXAPP",0,0,0
	DCB		"DEVICETYPE",0,0



	IF	:DEF:	RFBASE
		DCD		8
		DCD		0xFFFFFFFF		; Let's make ourselves compatible with any Matrix device up to and including type 33 (light bar
		DCD		0x00000001		;	configuration / new radio)
		DCD		0xFFFFFFFF		; Start of text flag block.
	ELSE
		IF	:DEF:	RFSCAN
			DCD		8
			DCD		0xFFFFFFFF		; Let's make ourselves compatible with any Matrix device up to and including type 33 (light bar
			DCD		0x00000001		;	configuration / new radio)
			DCD		0xFFFFFFFF		; Start of text flag block.
		ELSE								;;Not a RF scanner or base
			DCD		8
			DCD		0xFFFFFFFF		; Let's make ourselves compatible with any Matrix device up to and including type 33 (light bar
			DCD		0x00000001		;	configuration / new radio)
			DCD		0xFFFFFFFF		; Start of text flag block.
		ENDIF	;:DEF:	RFSCAN
	ENDIF		;:DEF:	RFBASE



	IF :DEF: AFRFBASE
		IF :DEF: RAMONLY
			DCD		4				; # of entries in text flag block.
			DCD		8				; Size of first text flag block entry.
			DCB		"RAMONLY",0		; First text flag block entry.
			DCD		10				; Size of first text flag block entry.
			DCB		"AFRFBASE",0,0	; First text flag block entry.
			DCD		8				; Size of AUTOMMU flag entry.
			DCB		"AUTOMMU",0		; AUTOMMU flag entry.
			DCD		8				; Size of AUTOMMU flag entry.
			DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.

		ELSE
			DCD		3				; # of entries in text flag block.
			DCD		10				; Size of first text flag block entry.
			DCB		"AFRFBASE",0,0	; First text flag block entry.
			DCD		8				; Size of AUTOMMU flag entry.
			DCB		"AUTOMMU",0	;	 AUTOMMU flag entry.
			DCD		8				; Size of AUTOMMU flag entry.
			DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
		ENDIF						;:DEF: RAMONLY

	ELSE

		IF :DEF: AFRFSCAN
			IF :DEF: RAMONLY
				DCD		4				; # of entries in text flag block.
				DCD		8				; Size of first text flag block entry.
				DCB		"RAMONLY",0		; First text flag block entry.
				DCD		10				; Size of first text flag block entry.
				DCB		"AFRFSCAN",0,0	; First text flag block entry.
				DCD		8				; Size of AUTOMMU flag entry.
				DCB		"AUTOMMU",0		; AUTOMMU flag entry.
				DCD		8				; Size of AUTOMMU flag entry.
				DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
			ELSE
				DCD		3				; # of entries in text flag block.
				DCD		10				; Size of first text flag block entry.
				DCB		"AFRFSCAN",0,0	; First text flag block entry.
				DCD		8				; Size of AUTOMMU flag entry.
				DCB		"AUTOMMU",0		; AUTOMMU flag entry.
				DCD		8				; Size of AUTOMMU flag entry.
				DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
			ENDIF						;:DEF: RAMONLY

		ELSE

			IF :DEF: AFCORDED
				IF :DEF: RAMONLY
					DCD		4				; # of entries in text flag block.
					DCD		8				; Size of first text flag block entry.
					DCB		"RAMONLY",0		; First text flag block entry.
					DCD		8				; Size of AUTOMMU flag entry.
					DCB		"AUTOMMU",0		; AUTOMMU flag entry.
					DCD		8				; Size of AUTOMMU flag entry.
					DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
					DCD		10				; Size of CORDED flag entry.
					DCB		"AFCORDED",0,0	; CORDED flag entry.
				ELSE
					DCD		3				; # of entries in text flag block.
					DCD		8				; Size of AUTOMMU flag entry.
					DCB		"AUTOMMU",0		; AUTOMMU flag entry.
					DCD		8				; Size of AUTOMMU flag entry.
					DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
					DCD		10				; Size of CORDED flag entry.
					DCB		"AFCORDED",0,0	; CORDED flag entry.
				ENDIF						;:DEF: RAMONLY

			ELSE

				IF :DEF: RFBASE
					IF :DEF: RAMONLY
						DCD		4				; # of entries in text flag block.
						DCD		8				; Size of first text flag block entry.
						DCB		"RAMONLY",0		; First text flag block entry.
						DCD		8				; Size of first text flag block entry.
						DCB		"RFBASE",0,0		; First text flag block entry.
						DCD		8				; Size of AUTOMMU flag entry.
						DCB		"AUTOMMU",0		; AUTOMMU flag entry.
						DCD		8				; Size of AUTOMMU flag entry.
						DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
					ELSE
						DCD		3				; # of entries in text flag block.
						DCD		8				; Size of first text flag block entry.
						DCB		"RFBASE",0,0		; First text flag block entry.
						DCD		8				; Size of AUTOMMU flag entry.
						DCB		"AUTOMMU",0	;	 AUTOMMU flag entry.
						DCD		8				; Size of AUTOMMU flag entry.
						DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
					ENDIF						;:DEF: RAMONLY

				ELSE

					IF :DEF: RFSCAN
						IF :DEF: RAMONLY
							DCD		4				; # of entries in text flag block.
							DCD		8				; Size of first text flag block entry.
							DCB		"RAMONLY",0		; First text flag block entry.
							DCD		8				; Size of first text flag block entry.
							DCB		"RFSCAN",0,0		; First text flag block entry.
							DCD		8				; Size of AUTOMMU flag entry.
							DCB		"AUTOMMU",0		; AUTOMMU flag entry.
							DCD		8				; Size of AUTOMMU flag entry.
							DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
						ELSE
							DCD		3				; # of entries in text flag block.
							DCD		8				; Size of first text flag block entry.
							DCB		"RFSCAN",0,0		; First text flag block entry.
							DCD		8				; Size of AUTOMMU flag entry.
							DCB		"AUTOMMU",0		; AUTOMMU flag entry.
							DCD		8				; Size of AUTOMMU flag entry.
							DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
						ENDIF						;:DEF: RAMONLY

					ELSE

						IF :DEF: CORDED
							IF :DEF: RAMONLY
								DCD		4				; # of entries in text flag block.
								DCD		8				; Size of first text flag block entry.
								DCB		"RAMONLY",0		; First text flag block entry.
								DCD		8				; Size of AUTOMMU flag entry.
								DCB		"AUTOMMU",0		; AUTOMMU flag entry.
								DCD		8				; Size of AUTOMMU flag entry.
								DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
								DCD		8				; Size of CORDED flag entry.
								DCB		"CORDED",0,0	; CORDED flag entry.
							ELSE
								DCD		3				; # of entries in text flag block.
								DCD		8				; Size of AUTOMMU flag entry.
								DCB		"AUTOMMU",0		; AUTOMMU flag entry.
								DCD		8				; Size of AUTOMMU flag entry.
								DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
								DCD		8				; Size of CORDED flag entry.
								DCB		"CORDED",0,0	; CORDED flag entry.
							ENDIF						;:DEF: RAMONLY

						ELSE

							IF :DEF: PROD_CFG
								IF :DEF: RAMONLY
									DCD		4				; # of entries in text flag block.
									DCD		8				; Size of first text flag block entry.
									DCB		"RAMONLY",0		; First text flag block entry.
									DCD		8				; Size of AUTOMMU flag entry.
									DCB		"AUTOMMU",0		; AUTOMMU flag entry.
									DCD		8				; Size of AUTOMMU flag entry.
									DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
									DCD		8				; Size of PRODCFG flag entry.
									DCB		"PRODCFG",0		; PRODCFG flag entry.
								ELSE
									DCD		3				; # of entries in text flag block.
									DCD		8				; Size of AUTOMMU flag entry.
									DCB		"AUTOMMU",0		; AUTOMMU flag entry.
									DCD		8				; Size of AUTOMMU flag entry.
									DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
									DCD		8				; Size of PRODCFG flag entry.
									DCB		"PRODCFG",0		; PRODCFG flag entry.
								ENDIF						;:DEF: RAMONLY

							ELSE

								IF :DEF: FLASH_DISK_UTILITY
									DCD		3				; # of entries in text flag block.
									DCD		8				; Size of first text flag block entry.
									DCB		"RAMONLY",0		; First text flag block entry.
									DCD		8				; Size of AUTOMMU flag entry.
									DCB		"AUTOMMU",0		; AUTOMMU flag entry.
									DCD		8				; Size of AUTOMMU flag entry.
									DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
								ELSE
									IF :DEF: RAMONLY
										DCD		3				; # of entries in text flag block.
										DCD		8				; Size of first text flag block entry.
										DCB		"RAMONLY",0		; First text flag block entry.
										DCD		8				; Size of AUTOMMU flag entry.
										DCB		"AUTOMMU",0		; AUTOMMU flag entry.
										DCD		8				; Size of AUTOMMU flag entry.
										DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
									ELSE
										DCD		2				; # of entries in text flag block.
										DCD		8				; Size of AUTOMMU flag entry.
										DCB		"AUTOMMU",0		; AUTOMMU flag entry.
										DCD		8				; Size of AUTOMMU flag entry.
										DCB		"BOOTACK",0		; BOOTACK flag entry, indicating that this code will correctly ACK after being downloaded over boot code.
;										DCD		12
;										DCB		"RUNFROMROM",0,0
									ENDIF						;:DEF: RAMONLY
								ENDIF							;:DEF: FLASH_DISK_UTILITY
							ENDIF								;:DEF: PROD_CFG
						ENDIF									;:DEF: CORDED
					ENDIF										;:DEF: RFSCAN
				ENDIF											;:DEF: RFBASE
			ENDIF												;:DEF: AFCORDED
		ENDIF													;:DEF: AFRFSCAN
	ENDIF														;:DEF: AFRFBASE



	END



