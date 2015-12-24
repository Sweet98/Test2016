# File Name - Mks_source_foundations.mk
# Include file for boot and app project prerequisite files.


#******************************************************************************************************
# Boot section
#******************************************************************************************************

# Source File Paths:
# Make searches here for source files and includes.
# A + will be replaced with space in paths by makefile.
# Using "" is helpful, can avoid problems with paths with drive letters.
# By default, DIR_SBOX_ROOT is ./ ie the directory where this file is the project root.
# Using / instead of \ is preferred to aviod line continuation and escape problems.

#DIR_SOURCE_BOOT += "$(DIR_SBOX_ROOT)/hhp+imaging" 		#example of a path with a name with a space, substituting a + for the space

DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_boot
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/OsKernel/temp_shared
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files/iMXL
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files/iMX27
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files/iMX25
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/Safe_FFS/src/common
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/Safe_FFS/src/nor
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/Safe_FFS/src/nor/phy/Numonyx_winbond
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/stringanddata
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/FoundationApp/Peripheral_Interfaces
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/FoundationApp/Flash
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/FoundationApp
# mocf
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/FoundationApp/mocf
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/FoundationApp/mocf/shared
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/FoundationApp/mocf/shared/sha1

.IF $(BOOTTARGET)== mxl_build
DIR_SOURCE_BOOT += $(DIR_SBOX_ROOT)/libraries/stringanddata
.END


#*****************************************************************************************
# BOOT Source Files:
#
# The macro MATRIX_SOURCES_BOOT is defined in the following section as the list of the
# basenames (i.e. w/o path or ext) of modules used to build the bootloader.
# These basenames have the the source file extensions (.c .cpp .s or .i) appended to
# reference the source files and the extension .o or .d appended to form compiler output
# names.
#*****************************************************************************************

# These files are from /libraries/OsKernel/temp_shared
MATRIX_SOURCES_BOOT += checksum
MATRIX_SOURCES_BOOT += MatrixHeader

# These files are from /libraries/OsKernel/db_boot
MATRIX_SOURCES_BOOT += db_boot
MATRIX_SOURCES_BOOT += db_chrio
MATRIX_SOURCES_BOOT += db_hdw
MATRIX_SOURCES_BOOT += db_nonmenucommands
MATRIX_SOURCES_BOOT += db_show
MATRIX_SOURCES_BOOT += db_syncommandparse
MATRIX_SOURCES_BOOT += vectors
MATRIX_SOURCES_BOOT += boot_main

.IF $(BOOTTARGET)== imx25_iram_Gen6_RAMTEST
MATRIX_SOURCES_BOOT += HardwareTest
MATRIX_SOURCES_BOOT += Full_RAM_Test
.END


# These files are from /libraries/OsKernel/db_shared
MATRIX_SOURCES_BOOT += db_crc
MATRIX_SOURCES_BOOT += db_mem
MATRIX_SOURCES_BOOT += db_mmu
MATRIX_SOURCES_BOOT += db_queue
MATRIX_SOURCES_BOOT += db_time
MATRIX_SOURCES_BOOT += db_uart
MATRIX_SOURCES_BOOT += db_xm1k

.IF $(BOOTTARGET)== mxl_build
MATRIX_SOURCES_BOOT += flash_id_from_ram
MATRIX_SOURCES_BOOT += strnicmp
.END

# flash file system
# These files are from /libraries/Safe_FFS/src/common
MATRIX_SOURCES_BOOT += fsmf
MATRIX_SOURCES_BOOT += fsm
MATRIX_SOURCES_BOOT += fsf
MATRIX_SOURCES_BOOT += port_s

# These files are from /libraries/Safe_FFS/src/nor
MATRIX_SOURCES_BOOT += flashdrv

# These files are from /libraries/Safe_FFS/src/nor/phy/Numonyx_winbond
# Put our ported file here
MATRIX_SOURCES_BOOT += num_m25pxx

# These files are from /FoundationApp/Foundation_Temp/Peripheral_Interfaces
MATRIX_SOURCES_BOOT += mx25Spi

# These files are from FoundationApp/mocf
MATRIX_SOURCES_BOOT += TlvContainerParser
MATRIX_SOURCES_BOOT += MocfParser
MATRIX_SOURCES_BOOT += Gen6ObjectParser
MATRIX_SOURCES_BOOT += sha1
MATRIX_SOURCES_BOOT += sha1_Rfc4634

#******************************************************************************************************
# App section
#******************************************************************************************************

.IF $(APPTARGET) == imx25_RF_Scan_FF2
PRODUCT_TYPE = RfScan
.ELSIF $(APPTARGET) == imx25_ID_RF_Scan_FF2_LEAN
PRODUCT_TYPE = RfScan
.ELSIF $(APPTARGET) == imx25_ID_RF_Scan_FF2
PRODUCT_TYPE = RfScan
.ELSIF $(APPTARGET) == imx25_RF_Scan_FF1
PRODUCT_TYPE = RfScan
.ELSIF $(APPTARGET) == imx25_RF_Scan_StoneStreet_Prototype
PRODUCT_TYPE = RfScan
.ELSIF $(APPTARGET) == imx25_RF_Base_FF2_LEAN
PRODUCT_TYPE = RfBase
.ELSIF $(APPTARGET) == imx25_RF_Base_FF2
PRODUCT_TYPE = RfBase
.ELSIF $(APPTARGET) == imx25_RF_Base_FF1
PRODUCT_TYPE = RfBase
.ELSE
PRODUCT_TYPE = Corded
.END


### Definitions for libraries with source included:

.IF $(APPTYPE) == APP
##  Compresssion:
#   (leaf (Output_LIB_) must match OBJDIR in sources.mk in the Compression subproject.)
#   (libname (Compression) must match TARGET_NAME in same file.)
LIBS += libraries/Compression/Cmpr_LIB/Compression.a

#   Compresssion - Library build rule:
libraries/Compression/Cmpr_LIB/Compression.a :
[
	@$(MKSMAKE) -r -f $(PWD)/makefile -DLIB_MAKE=sources.mk -c libraries/Compression -DAPPTYPE=LIB -DPROJROOT=$(PWD) -DTYPE_CPU=$(TYPE_CPU)  OUTPUT_FOLDERS  Cmpr_LIB/$(@:b).a
]

#   Compresssion - Library clean rule:
Compression.a.clean :
[
	@$(MKSMAKE) -r -f $(PWD)/makefile -DLIB_MAKE=sources.mk -c libraries/Compression -DAPPTYPE=LIB -DPROJROOT=$(PWD) -DTYPE_CPU=$(TYPE_CPU)  CLEAN
]
# Special modification of APPTYPE to process the next section for:
#   APPTYPE == SANITIZE  --OR--  APPTYPE=SAN_APP
.ELSIF $(APPTYPE:s/SANITIZE/SAN_APP/) == SAN_APP
LIBS_NOSOURCE += libraries/Compression/Cmpr_LIB/Compression.a
.END
LIBS_NOSOURCE += FoundationApp/Imaging/ImageCore.a

### Definitions for libraries with no source for updating in this project such as the decoder or bluetooth stack
.IF $(PRODUCT_TYPE) == RfScan
LIBS_NOSOURCE += $(DIR_SBOX_ROOT)/libraries/BT_Stack/SS1_Library/Bluetopia.a
LIBS_NOSOURCE += $(DIR_SBOX_ROOT)/libraries/BT_Stack/SS1_Library/SS1BTHID.a
.ELSIF $(PRODUCT_TYPE) == RfBase
LIBS_NOSOURCE += $(DIR_SBOX_ROOT)/libraries/BT_Stack/SS1_Library/Bluetopia.a
.END


### This special check (for the StoneStreet lib) creates the app code from the lib that has most of the object code in it.
.IF $(APPTARGET) == MatrixLibStoneStreet
LIBS_NOSOURCE = Bluetopia.a db_app_SS.a
# no libs--they're all in db_app_SS.a
LIBS =
.END



# Source File Paths:
# Make searches here for source files and includes.
# A + will be replaced with space in paths by makefile.
# Using "" is helpful, can avoid problems with paths with drive letters.
# By default, DIR_SBOX_ROOT is ./ ie the directory where this file is the projrect root.
# Using / instead of \ is preferred to aviod line continuation and escape problems.

#DIR_SOURCE_APP += "$(DIR_SBOX_ROOT)/hhp+imaging" 		#example of a path with a name with a space, substituting a + for the space

DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/OsKernel/app_kernel
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/OsKernel/temp_shared
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files/iMXL
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files/iMX27
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/OsKernel/db_shared/Hardware_Header_Files/iMX25
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/Batch_Mode
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/Foundation_Temp
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/Imaging
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/Flash
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/Peripheral_Interfaces
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/PageButton
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/Trigger
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/menu
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/eci
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/LeanAutomationTest
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/dirt_test
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/PSOC

.IF $(PLUGINENA)==YES
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/Plugin
.END
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/hhpscandriver
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/stringanddata
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/CommandParsers
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/buffer_fifo_utils
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/Safe_FFS/src/common
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/Safe_FFS/src/nor
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/Safe_FFS/src/nor/phy/Numonyx_winbond
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/G6_configuration
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/Communication_Gen6
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/Communication_Gen6/Matrix
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/compression
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_hw
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_rf
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_wedge
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_wedge_hw
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/customization
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/Cryptography
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/G6_Imaging
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/HHPL_Script
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/HHPL_Script/Gen6

#USB OTG
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2_hw
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2/base_usb2
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2/cdcacm_usb2
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/hidbase_usb2
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/pos_usb2
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/surepos_usb2
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/kbdpc_usb2
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/shared_matrix_interfaces/interfaces_usb2/rem
#USB OTG END

# mocf
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/mocf
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/mocf/shared
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/FoundationApp/mocf/shared/sha1

# Bluetopia stack folders
DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/libraries/BT_Stack/SS1_Include
DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/libraries/BT_Stack/SS1_Profiles/HID
#DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/libraries/BT_Stack/SS1_Library

#Cordless folders
DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/FoundationApp/Cordless
DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/FoundationApp/Cordless/BlueCore
#StoneStreet stack interface folders
DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/FoundationApp/Cordless/BTPSKRNL
DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/FoundationApp/Cordless/HCITRANS
DIR_SOURCE_CORDLESS += $(DIR_SBOX_ROOT)/FoundationApp/Cordless/vendor


.IF $(APPTARGET)== imx25_RF_Scan_FF2
DIR_SOURCE_APP += $(DIR_SOURCE_CORDLESS)
.ELSIF $(APPTARGET) == imx25_ID_RF_Scan_FF2
DIR_SOURCE_APP += $(DIR_SOURCE_CORDLESS)
.ELSIF $(APPTARGET)==imx25_RF_Scan_FF1
DIR_SOURCE_APP += $(DIR_SOURCE_CORDLESS)
.ELSIF $(APPTARGET)== imx25_RF_Base_FF2
DIR_SOURCE_APP += $(DIR_SOURCE_CORDLESS)
.ELSIF $(APPTARGET)== imx25_RF_Base_FF1
DIR_SOURCE_APP += $(DIR_SOURCE_CORDLESS)
.ELSIF $(APPTARGET)==imx25_RF_Scan_StoneStreet_Prototype
DIR_SOURCE_APP += $(DIR_SOURCE_CORDLESS)
.END #endof IF $(APPTARGET)== imx25_RF_Scan_FF2


# This creates the app code from the lib that has most of the object code in it.
.IF $(APPTARGET)==MatrixLibStoneStreet
DIR_SOURCE_APP = $(DIR_SBOX_ROOT)
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/Hon_Includes
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/SS1_Includes
.END


#*****************************************************************************************
# App Source Files:
#
# The macro MATRIX_SOURCES_APP is defined in the following section as the list of the
# basenames (i.e. w/o path or ext) of modules used to build the app code.
# These basenames have the the source file extensions (.c .cpp .s or .i) appended to
# reference the source files and the extension .o or .d appended to form compiler output
# names.
#
# Some modules have their names first added to macros defining members of some functional
# groups.  These group names include the following:
#
#     ECI_SOURCES
#     FLASH_SOURCES
#     HHPL_SOURCES
#     IF4683_SOURCES
#     PLUGIN_SOURCES
#     MATRIX_SOURCES_RF
#     SD_SOURCES
#     USB2_SOURCES
#     WEDGE_SOURCES
#
# These functional groups permit selection of which source files will be provided for
# santized build environments for customers.
#
# For APPTYPE == APP, all these group names are added to (i.e. included in or appended to)
# macro MATRIX_SOURCES_APP.
#
# For APPTYPE == SANITIZE or SAN_APP, the only group name added to MATRIX_SOURCES_APP is
# MATRIX_SOURCES_RF.  The other groups are modules whose source is not provided in a
# 'Sanitized build sandbox'.  Their object code will be provided via the SANITIZE operation
# in a 'Partially Linked Object'.  Additional group names are defined (for modules whose
# source is not provided) where groups members are scattered thru this file.  For these
# groups, the module names are either added to their group macro name --OR-- to the primary
# group name, MATRIX_SOURCES_APP. (Thus those group names are not later added to
# macro MATRIX_SOURCES_APP.)  These groups are named:
#
#     CONFIG_SOURCES
#     MISC_SOURCES
#
# The default sanitized build sandbox will NOT include source for the functional groups
# other than MATRIX_SOURCES_RF.  At this time, the only option for sanitzed builds is the
# dataedit module.  If the macro name  PROVIDE_DATAEDIT_SOURCE  is defined == YES on the
# call to the main makefile, i.e. with the argument string:
#    -DPROVIDE_DATAEDIT_SOURCE=YES
# then that source module will not be part of the MISC_SOURCES group, so its source
# WILL be provided.  (See the documentation at the beginning of the main makefile for
# more information.)
#
#*****************************************************************************************


# Note:  RECOMPILE_ALWAYS works only for a single file.  TODO:  when we go to GNU make, perhaps we can recursively call make and thus be able to genericaly re-compile/assemble any file.
.IF $(APPTARGET)==MatrixLibStoneStreet
RECOMPILE_ALWAYS = ProjectRevision.c
.ELSE
RECOMPILE_ALWAYS = $(DIR_SBOX_ROOT)/FoundationApp/ProjectRevision.c
.END
# These files are from /libraries/OsKernel/app_kernel
MATRIX_SOURCES_APP += app_start
MATRIX_SOURCES_APP += db_chrio
MATRIX_SOURCES_APP += db_Heap
MATRIX_SOURCES_APP += heap1a
MATRIX_SOURCES_APP += heapalloc
MATRIX_SOURCES_APP += db_hdw
MATRIX_SOURCES_APP += Hardware_Clocks
MATRIX_SOURCES_APP += Abort
MATRIX_SOURCES_APP += interrupts
MATRIX_SOURCES_APP += OsTimer
MATRIX_SOURCES_APP += thread
MATRIX_SOURCES_APP += threads_interupts
MATRIX_SOURCES_APP += syncobjects

# These files are from /libraries/OsKernel/temp_shared
MATRIX_SOURCES_APP += checksum
MATRIX_SOURCES_APP += MatrixHeader

# These files are from /libraries/OsKernel/db_shared
MATRIX_SOURCES_APP += db_mem
MATRIX_SOURCES_APP += db_mmu
MATRIX_SOURCES_APP += db_queue
MATRIX_SOURCES_APP += db_time
MATRIX_SOURCES_APP += db_uart
MATRIX_SOURCES_APP += product

# These files are from /FoundationApp
MATRIX_SOURCES_APP += OS_Start
MATRIX_SOURCES_APP += hab_header
MATRIX_SOURCES_APP += RemRouter
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += dataedit
.ELSIF $(PROVIDE_DATAEDIT_SOURCE) == YES
MATRIX_SOURCES_APP += dataedit
.ELSE
MISC_SOURCES       += dataedit
.END

MATRIX_SOURCES_APP += beep
MATRIX_SOURCES_APP += usage_statistics
MATRIX_SOURCES_APP += StreamManager
.IF $(APPTARGET) != imx25_RF_Scan_StoneStreet_Prototype
MATRIX_SOURCES_APP += ProjectRevision
.END

.IF $(APPTARGET) == imx25_ID_FF3_LEAN
MATRIX_SOURCES_APP += LeanAutomation
.END
.IF $(APPTARGET) == imx25_ID_RF_Scan_FF2_LEAN
MATRIX_SOURCES_APP += LeanAutomation
.END
.IF $(APPTARGET) == imx25_Base_FF2_LEAN
MATRIX_SOURCES_APP += LeanAutomation
.END


.IF $(PRODUCT_TYPE) != RfBase
MATRIX_SOURCES_APP += DecodeIFace
.END

# These files are from /FoundationApp/Foundation_Temp
MATRIX_SOURCES_APP += ReportStartupTiming
MATRIX_SOURCES_APP += PowerManagement
MATRIX_SOURCES_APP += iMX25_Illumination_Control

# These files are from /FoundationApp/eci
ECI_SOURCES += transcode
ECI_SOURCES += transcodeex
ECI_SOURCES += codepagesystemcp
ECI_SOURCES += codepagetables7bit
ECI_SOURCES += codepagetables8bit
ECI_SOURCES += transutf16
ECI_SOURCES += transutf8
ECI_SOURCES += codepagetablecp932
ECI_SOURCES += codepagetablecp936
ECI_SOURCES += codepagetablecp950
ECI_SOURCES += transcp932
ECI_SOURCES += transcp936
ECI_SOURCES += transcp950
ECI_SOURCES += eci_base
ECI_SOURCES += eci_macro
ECI_SOURCES += eci_charset
ECI_SOURCES += eci
ECI_SOURCES += ConvertUTF
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(ECI_SOURCES)
.END

# These files are from /FoundationApp/Flash
FLASH_SOURCES += flashapplicationcode
FLASH_SOURCES += StartupFlashDisk
FLASH_SOURCES += FileHandler

.IF $(PLUGINENA)==YES
# These files are from /FoundationApp/Plugin
PLUGIN_SOURCES += HONPlugin
PLUGIN_SOURCES += PluginFileProcessing
PLUGIN_SOURCES += HONPluginConfiguration
PLUGIN_SOURCES += tinystr
PLUGIN_SOURCES += tinyxml
PLUGIN_SOURCES += tinyxmlparser
PLUGIN_SOURCES += tinyxmlerror
PLUGIN_SOURCES += PluginInterface
PLUGIN_SOURCES += PluginMenuSettings
PLUGIN_SOURCES += PluginMenuProcess
PLUGIN_SOURCES += PluginProcessDecoding
PLUGIN_SOURCES += PluginGpioInterface
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(PLUGIN_SOURCES)
.END
.END

# These files are from /FoundationApp/menu
MATRIX_SOURCES_APP += Menu
MATRIX_SOURCES_APP += MenuGlueDriver
MATRIX_SOURCES_APP += MenuFlash
MATRIX_SOURCES_APP += Interface_Support_Functions
MATRIX_SOURCES_APP += AutoCableSelect

# These files are from /FoundationApp/Peripheral_Interfaces
MATRIX_SOURCES_APP += mx25Spi
MATRIX_SOURCES_APP += i2c_drvr
MATRIX_SOURCES_APP += PWM_Driver
MATRIX_SOURCES_APP += ADC_Driver
MATRIX_SOURCES_APP += RTC_Driver
MATRIX_SOURCES_APP += IndicatorLEDs
MATRIX_SOURCES_APP += HardwareToken
MATRIX_SOURCES_APP += Timer3_Driver
MATRIX_SOURCES_APP += IndicatorTfb

.IF $(PRODUCT_TYPE) != RfBase
.IF $(IMAGER_TYPE) == BYD_IMAGER
MATRIX_SOURCES_APP += Hi704A
.ELSE
MATRIX_SOURCES_APP += jade
.END
MATRIX_SOURCES_APP += Flash_Illumination_Control
.END

# These files are from /FoundationApp/PageButton
.IF $(PRODUCT_TYPE) = RfBase
MATRIX_SOURCES_APP += HWPageButton
.END
# These files are from /FoundationApp/Trigger
.IF $(PRODUCT_TYPE) != RfBase
#MATRIX_SOURCES_APP += Trigger
#MATRIX_SOURCES_APP += TriggerHandler
#MATRIX_SOURCES_APP += TriggerOptical
#MATRIX_SOURCES_APP += TrigModeAuto
#MATRIX_SOURCES_APP += TrigModeManual
#MATRIX_SOURCES_APP += TrigModeNotifyHost
#MATRIX_SOURCES_APP += TrigModePresentation
#MATRIX_SOURCES_APP += TrigModeScanStand
#MATRIX_SOURCES_APP += TrigModeStreamingPresentation
MATRIX_SOURCES_APP += HWTrigger
MATRIX_SOURCES_APP += Trigger_5_5
MATRIX_SOURCES_APP += IRTrigger
.END

# These files are from /FoundationApp/Imaging
.IF $(PRODUCT_TYPE) != RfBase
MATRIX_SOURCES_APP += imaging
MATRIX_SOURCES_APP += Image_Send_Binarize_Himage
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += ImagingInternal
.ELSE
MISC_SOURCES       += ImagingInternal
.END
MATRIX_SOURCES_APP += ScanDriverImagingInterface
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += sigcapture
MATRIX_SOURCES_APP += sigcaptureut
.ELSE
MISC_SOURCES       += sigcapture
MISC_SOURCES       += sigcaptureut
.END
.END
# These files are from /FoundationApp/dirt_test
.IF $(PRODUCT_TYPE) != RfBase
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += Api
.ELSE
MISC_SOURCES       += Api
.END
.END
# These files are from /FoundationApp/PSOC
.IF $(PRODUCT_TYPE) != RfBase
MATRIX_SOURCES_APP += PSoC_Engine_ISSP
MATRIX_SOURCES_APP += ScanEnginePSOC
.END
# These files are from /libraries/shared_matrix_interfaces/interfaces
MATRIX_SOURCES_APP += driverbase
MATRIX_SOURCES_APP += if_table
MATRIX_SOURCES_APP += ioman
MATRIX_SOURCES_APP += parsehelper
MATRIX_SOURCES_APP += rxfifo
MATRIX_SOURCES_APP += serialparameter
MATRIX_SOURCES_APP += termid
MATRIX_SOURCES_APP += nulldriver
MATRIX_SOURCES_APP += drvsimpleserialnothread
MATRIX_SOURCES_APP += threadeddriver
MATRIX_SOURCES_APP += Threadrealtime
MATRIX_SOURCES_APP += ThreadSimple
MATRIX_SOURCES_APP += MLFlatbedSerialParser
MATRIX_SOURCES_APP += drvserialstd
MATRIX_SOURCES_APP += serialbase
MATRIX_SOURCES_APP += usb2resu
MATRIX_SOURCES_APP += DrvMenuConfig
MATRIX_SOURCES_APP += DirectIoParser
.IF $(PRODUCT_TYPE) != RfScan
IF4683_SOURCES += CommandHandler4683
IF4683_SOURCES += 4683isr
IF4683_SOURCES += 4683tx
IF4683_SOURCES += 4683xlat
IF4683_SOURCES += 4683pro
IF4683_SOURCES += crc4683
IF4683_SOURCES += DrvSerial4683
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(IF4683_SOURCES)
.END
.END

.IF $(PRODUCT_TYPE) != RfScan # most interface modules don't go with RfScan
# These files are from /libraries/shared_matrix_interfaces/interfaces_wedge
WEDGE_SOURCES += Wedgeengineibm
WEDGE_SOURCES += Wedgeengine
WEDGE_SOURCES += wedgethread
WEDGE_SOURCES += wedgelabel
WEDGE_SOURCES += wedgemultikey
WEDGE_SOURCES += wedgeinternational
WEDGE_SOURCES += wedgetranslate
WEDGE_SOURCES += wedgeprokey
WEDGE_SOURCES += wedgecmd
WEDGE_SOURCES += wedgebase
WEDGE_SOURCES += prokeytb
WEDGE_SOURCES += drvwedgebase
WEDGE_SOURCES += drvwedgeibm
WEDGE_SOURCES += wedgescancode
WEDGE_SOURCES += wedgekey
WEDGE_SOURCES += wedgekeynumber
WEDGE_SOURCES += wedgechar2key
WEDGE_SOURCES += wedgecntrlkeys
WEDGE_SOURCES += keytabf
WEDGE_SOURCES += keyintf
WEDGE_SOURCES += keyintf_uni
WEDGE_SOURCES += Gpio
WEDGE_SOURCES += WedgeHal
WEDGE_SOURCES += Portdetails
WEDGE_SOURCES += timer
WEDGE_SOURCES += Wedgecapslock

# Add for usb keyboard
WEDGE_SOURCES += wedgeengineusb
WEDGE_SOURCES += keyint
WEDGE_SOURCES += keytab

# These files are from /libraries/shared_matrix_interfaces/interfaces_wedge_hw
WEDGE_SOURCES += Bitbanger
WEDGE_SOURCES += bbibm
WEDGE_SOURCES += bitbangerisr

# USB OTG
# These files are from /libraries/shared_matrix_interfaces/interfaces_usb2/base_usb2
USB2_SOURCES += usb2driverbase
USB2_SOURCES += usb2base
USB2_SOURCES += usb2stringdesc
USB2_SOURCES += usb2enum
USB2_SOURCES += DescriptorQueues

# These files are from /libraries/shared_matrix_interfaces/interfaces_usb2/cdcacm_usb2
USB2_SOURCES += usb2cdcacm
USB2_SOURCES += drvusb2comemu

# These files are from /libraries/shared_matrix_interfaces/interfaces_usb2_hw
USB2_SOURCES += usb2chip

# These files are from /libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/hidbase_usb2
USB2_SOURCES += usb2hid
USB2_SOURCES += usb2hidhlp

# These files are from /libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/pos_usb2
USB2_SOURCES += drvusb2pos
USB2_SOURCES += usb2hidpos

# These files are from /libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/surepos_usb2
USB2_SOURCES += drvusb2surepos
USB2_SOURCES += usb2hidsurepos
USB2_SOURCES += usb2sureposxlat

# These files are from /libraries/shared_matrix_interfaces/interfaces_usb2/hid_usb2/kbdpc_usb2
USB2_SOURCES += drvusb2kbd
USB2_SOURCES += usb2hidkbd
USB2_SOURCES += usb2kbdbase

# rem
USB2_SOURCES += UsbRem
USB2_SOURCES += drvUsbRem
#USB OTG END

.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(WEDGE_SOURCES) $(USB2_SOURCES)
.END

.END # endof .IF $(PRODUCT_TYPE) != RfScan # most interface modules don't go with RfScan

.IF $(PRODUCT_TYPE) == RfScan

# These files are from /libraries/shared_matrix_interfaces/interfaces_wedge
WEDGE_SOURCES += Wedgeengine
WEDGE_SOURCES += wedgethread
WEDGE_SOURCES += wedgelabel
WEDGE_SOURCES += wedgemultikey
WEDGE_SOURCES += wedgeinternational
WEDGE_SOURCES += wedgetranslate
WEDGE_SOURCES += wedgeprokey
WEDGE_SOURCES += wedgecmd
WEDGE_SOURCES += wedgebase
WEDGE_SOURCES += prokeytb
WEDGE_SOURCES += drvwedgebase
WEDGE_SOURCES += wedgescancode
WEDGE_SOURCES += wedgekey
WEDGE_SOURCES += wedgekeynumber
WEDGE_SOURCES += wedgechar2key
WEDGE_SOURCES += wedgecntrlkeys
WEDGE_SOURCES += keytabf
WEDGE_SOURCES += keyintf
WEDGE_SOURCES += keyintf_uni
WEDGE_SOURCES += Gpio
WEDGE_SOURCES += WedgeHal
WEDGE_SOURCES += Portdetails
WEDGE_SOURCES += timer
WEDGE_SOURCES += Wedgecapslock

# These files are from /libraries/shared_matrix_interfaces/interfaces_wedge_hw
WEDGE_SOURCES += Bitbanger
WEDGE_SOURCES += bitbangerisr

.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(WEDGE_SOURCES) 
.END

.END





# These files are from /libraries/shared_matrix_interfaces/interfaces_customization
MATRIX_SOURCES_APP += if_custom

# These files are from /libraries/shared_matrix_interfaces/interfaces_hw
MATRIX_SOURCES_APP += uart

# These files are from /libraries/buffer_fifo_utils
MATRIX_SOURCES_APP += fifobase
MATRIX_SOURCES_APP += barcodedata
MATRIX_SOURCES_APP += barcodefifo
MATRIX_SOURCES_APP += barcodedataman
MATRIX_SOURCES_APP += bufferpool
MATRIX_SOURCES_APP += managedbuffer

# These files are from /libraries/stringanddata
MATRIX_SOURCES_APP += hwstring
MATRIX_SOURCES_APP += bitstring
MATRIX_SOURCES_APP += stricmp
MATRIX_SOURCES_APP += hstring
MATRIX_SOURCES_APP += strnicmp
MATRIX_SOURCES_APP += crc
MATRIX_SOURCES_APP += md5

# These files are from /libraries/hhpl
HHPL_SOURCES += hhplimagedata
HHPL_SOURCES += hhplinstructionset
HHPL_SOURCES += hhplipb
HHPL_SOURCES += hhplitemtable
HHPL_SOURCES += hhploutput
HHPL_SOURCES += hhplprocessor
HHPL_SOURCES += hhplprogramprofile
HHPL_SOURCES += EditScript
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(HHPL_SOURCES)
.END
.IF $(PRODUCT_TYPE) != RfBase
MATRIX_SOURCES_APP += mainhhplproc
MATRIX_SOURCES_APP += mainscript
MATRIX_SOURCES_APP += OutputModes
.END

# These files are from /hhpscandriver
.IF $(PRODUCT_TYPE) != RfBase
.IF $(IMAGER_TYPE) == BYD_IMAGER
SD_SOURCES += ScanDriverHi704A
.ELSE
SD_SOURCES += ScanDriverJade
.END
SD_SOURCES += scandriverscannerselect
SD_SOURCES += scandriverscanmanager
SD_SOURCES += scandriveropencontext
SD_SOURCES += scandrivermatrixinterface
SD_SOURCES += ScanDriverLightControl
SD_SOURCES += scandriverinterface
SD_SOURCES += ScanDriverGlobalConfig
SD_SOURCES += ScanDriverEngine
SD_SOURCES += scandriverdebug
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(SD_SOURCES)
.END
.END


# flash file system
# These files are from /libraries/Safe_FFS/src/common
FLASH_SOURCES += fsmf
FLASH_SOURCES += fsm
FLASH_SOURCES += fsf
FLASH_SOURCES += port_s

# These files are from /libraries/Safe_FFS/src/nor
FLASH_SOURCES += flashdrv

# These files are from /libraries/Safe_FFS/src/nor/phy/Numonyx_winbond
# Put our ported file here
FLASH_SOURCES += num_m25pxx

.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += $(FLASH_SOURCES)
.END

#These files are from /libraries/CommandParsers
MATRIX_SOURCES_APP += SynCommandParser
MATRIX_SOURCES_APP += NonMenucommands
MATRIX_SOURCES_APP += NonMenuCommandFlash
MATRIX_SOURCES_APP += ESCCommandparse
MATRIX_SOURCES_APP += MatrixSynFECommandHandler
.IF $(PRODUCT_TYPE)== RfScan
MATRIX_SOURCES_APP += CodeXMLCommandParser
.END

# These files are from /libraries/Cryptography
MATRIX_SOURCES_APP += aes_core
MATRIX_SOURCES_APP += AesEncryption
MATRIX_SOURCES_APP += encryption


#These files are from /libraries/G6_configuration
MATRIX_SOURCES_APP += CPUPowerSpeedMenuSettings
MATRIX_SOURCES_APP += DecoderMenuSettings
MATRIX_SOURCES_APP += DiagnosticAndDebugMenuSettings
MATRIX_SOURCES_APP += ImagingMenuSettings
MATRIX_SOURCES_APP += IndicatorMenuSettings
MATRIX_SOURCES_APP += InterfaceMenuSettings
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += productconfiguration
.ELSE
CONFIG_SOURCES     += productconfiguration
.END
MATRIX_SOURCES_APP += readerconfiguration
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += readerconfigurationstorage
.ELSE
CONFIG_SOURCES     += readerconfigurationstorage
.END
MATRIX_SOURCES_APP += SystemMenuSettings
MATRIX_SOURCES_APP += LicenseControlMenuSettings
MATRIX_SOURCES_APP += TriggeringMenuSettings
MATRIX_SOURCES_APP += OutputMenuSettings
MATRIX_SOURCES_APP += PlugAndPlayMenuSettings
MATRIX_SOURCES_APP += ScriptingMenuSettings
MATRIX_SOURCES_APP += PinfoCreator

#These files are from /libraries/Communication_Gen6
MATRIX_SOURCES_APP += hcomm
MATRIX_SOURCES_APP += hcommandhandler
MATRIX_SOURCES_APP += HexFile
MATRIX_SOURCES_APP += hmodem
MATRIX_SOURCES_APP += htag
MATRIX_SOURCES_APP += HTagData
MATRIX_SOURCES_APP += HTagDataResult
MATRIX_SOURCES_APP += HTagFile
MATRIX_SOURCES_APP += htagimage
MATRIX_SOURCES_APP += HTagReaderCode
MATRIX_SOURCES_APP += htagscript
MATRIX_SOURCES_APP += HTagVerifier
MATRIX_SOURCES_APP += htransferprocessing
MATRIX_SOURCES_APP += htransferstatus

#These files are from /libraries/Communication_Gen6/Matrix
MATRIX_SOURCES_APP += MatrixHcomm
MATRIX_SOURCES_APP += MatrixNonMenuHmodem
MATRIX_SOURCES_APP += Matrixhtransferstatus
MATRIX_SOURCES_APP += synfecommandhandler

# These files are from FoundationApp/mocf
MATRIX_SOURCES_APP += TlvContainerParser
MATRIX_SOURCES_APP += MocfHandler
MATRIX_SOURCES_APP += MocfParser
MATRIX_SOURCES_APP += Gen6ObjectHandler
MATRIX_SOURCES_APP += Gen6ObjectParser
MATRIX_SOURCES_APP += sha1
MATRIX_SOURCES_APP += sha1_Rfc4634

#These files are from /libraries/G6_Imaging
.IF $(PRODUCT_TYPE)!= RfBase
MATRIX_SOURCES_APP += TIFF_Swab
MATRIX_SOURCES_APP += tiffimage
MATRIX_SOURCES_APP += bmpimage
MATRIX_SOURCES_APP += himage
MATRIX_SOURCES_APP += LZWCompressor
.IF $(APPTYPE) == APP
MATRIX_SOURCES_APP += himageProprietary
.ELSE
MISC_SOURCES       += himageProprietary
.END
MATRIX_SOURCES_APP += kimimage
MATRIX_SOURCES_APP += outlinedimage
MATRIX_SOURCES_APP += TIFF_Fax
.END

#These files are from /FoundationApp/Batch_Mode
.IF $(PRODUCT_TYPE)== RfScan
MATRIX_SOURCES_APP += BatchManager
MATRIX_SOURCES_APP += InventoryEntry
.END

#These files are from /FoundationApp/Cordless
MATRIX_SOURCES_RF += Cordless
MATRIX_SOURCES_RF += BTStackLL
MATRIX_SOURCES_RF += BatteryManagement
.IF $(PRODUCT_TYPE)== RfBase
MATRIX_SOURCES_RF += BaseUpdateScanners
MATRIX_SOURCES_RF += ScannerAssociation
.END

#These files are from /FoundationApp/Cordless/*.  They are the StoneStreet stack interface files
MATRIX_SOURCES_RF += BTPSKRNL
MATRIX_SOURCES_RF += HCITRANS
MATRIX_SOURCES_RF += Vendor

#These files are from /FoundationApp/Cordless/BlueCore
MATRIX_SOURCES_RF += ubcsp
MATRIX_SOURCES_RF += BlueCoreTransport
MATRIX_SOURCES_RF += BlueCorePsKey
MATRIX_SOURCES_RF += BlueCoreCmd
MATRIX_SOURCES_RF += BlueCoreChip
MATRIX_SOURCES_RF += BlueCoreChipHw

# These files are from /libraries/shared_matrix_interfaces/interfaces_rf
.IF $(PRODUCT_TYPE) == RfScan
MATRIX_SOURCES_RF += DrvBluetoothHid
MATRIX_SOURCES_RF += BitBangBluetoothHid
MATRIX_SOURCES_RF += WedgeEngineBt
MATRIX_SOURCES_RF += WedgeHalBt
.END
MATRIX_SOURCES_RF += DrvSPPSerialPort
MATRIX_SOURCES_RF += DrvSerialRf

#These files are from /libraries/G6_configuration
MATRIX_SOURCES_RF += RFMenuSettings

# These files are from /libraries/buffer_fifo_utils
MATRIX_SOURCES_RF += genericfifo


.IF $(APPTARGET) != imx25_RF_Scan_StoneStreet_Prototype
.IF $(PRODUCT_TYPE) == RfScan
MATRIX_SOURCES_RF += HidServer
.END
MATRIX_SOURCES_RF += SPPTest
MATRIX_SOURCES_RF += Spp
MATRIX_SOURCES_RF += LinkKeyDatabase
MATRIX_SOURCES_RF += SppConnectionsList
.END



.IF $(PRODUCT_TYPE) ==  RfScan
MATRIX_SOURCES_APP += $(MATRIX_SOURCES_RF)
.ELSIF $(PRODUCT_TYPE) == RfBase
MATRIX_SOURCES_APP += $(MATRIX_SOURCES_RF)
.END

# This creates the app code from the lib that has most of the object code in it.
.IF $(APPTARGET)==MatrixLibStoneStreet
MATRIX_SOURCES_APP = SPPTest
MATRIX_SOURCES_APP += ProjectRevision
.END #endof .IF $(APPTARGET)==MatrixLibStoneStreet

.IF $(PRODUCT_TYPE) != RfBase
# include the decoders dependency list
.INCLUDE : Mks_source_decoders.mk
.END

## end
##

