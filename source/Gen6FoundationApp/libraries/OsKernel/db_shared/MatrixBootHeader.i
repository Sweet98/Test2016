;***************************************************************************************/
;*
;* HIM 48X0G GENERATION 6 SOFTWARE
;*
;*  HARDWARE MACROS 
;*  Date:       6/06/2008
;*  Tools:      RVCT Build 674
;*  Target:     Freescale iMX27
;*
;*======================================================================================
;* Revision information
;*======================================================================================
;* $RCSfile: db_shared/MatrixBootHeader.i $
;* $Revision: 1.3 $
;* $Date: 2010/04/14 14:20:39EDT $
;*======================================================================================
;*------------------------------------------------------------------------------
;* Header:  MatrixBootHeader.i
;*
;* This File defines the Matrix Boot Header
;* The Boot Header is used as a keyword for download tools to gain information about the file
;*
;* This used to be located in db_boot.s and was moved to make it more portable
;* and more assessable by other files.
;* 
;* 
;* TODO:  Gen6 no longer uses DEVICETYPE, so we can remove this header.  We didn't
;* do if because we were so close to qual and removing this header meant touching
;* some assembly language that populates the shared tabels.  Then we would need to ensure
;* the boot & app would work OK without this shared table info.  That extra work
;* can be done later.  So, for now, we just set DEVICETYPE to 0 to preven any
;* Gen5 device from flashing Gen6 boot firmware.
;*------------------------------------------------------------------------------  
  
	EXPORT	MATRIX_BOOT_HEADER
	EXPORT DeviceTypeLocation
;*	MATRIX BOOT HEADER
;*
;*	This header is used by download tools to check the integrity and type of the code file.  The header, "MATRIXBOOT", which is null
;*	terminated, is followed by the device type that this code corresponds to.
;*
;*	Device Types:
;*		33 - New Radio
;*		34 - 3800GLP
;*
;*	Create structure (string,NULL,NULL,string,NULL,NULL,word) below:
;*
MATRIX_BOOT_HEADER
	DCB		"BULLETBOOT",0,0
	DCB		"DEVICETYPE",0,0
DeviceTypeLocation
	IF ( platform_current = "imx25" )
		DCD		0                     ; Gen6 is no longer using device type
	ELSE
	IF :DEF: NEW_RADIO
		DCD		33		      ;Device type for new radio
	ELSE
		DCD		1                     ; Device type for non-interleaved Matrix configurations is 1.
	ENDIF
	ENDIF ;; endof IF ( platform_current = "imx25" )
	
	END

