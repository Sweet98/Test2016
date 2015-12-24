# File Name - Mks_project_target_foundations.mk
# Include file for boot and app project unique build options for different targets

#import name of boot target - Commented out since it is set as a macro using a -D on the command line.
#.IMPORT : BOOTTARGET

#import name of APP target - Commented out since it is set as a macro using a -D on the command line.
#.IMPORT : APPTARGET

# Lets NULL out the variables first
CPP_DEF_EXTRA_BOOT =
ASM_PREDEF_EXTRA_BOOT =
TYPE_CPU_BOOT =
TARGET_CPP_FLAGS_BOOT =
TARGET_LINK_FLAGS_BOOT =

CPP_DEF_EXTRA_APP =
ASM_PREDEF_EXTRA_APP =
TYPE_CPU_APP =
TARGET_CPP_FLAGS_APP =
TARGET_LINK_FLAGS_APP =

# A type for each decoder. Set the type to '1' to enable the decoder.
# Decoder flags, source, and libs are processed in Mks_source_decoders.mk
# These can be overridden per target, for example RFBase has no decoders
DECODER_TYPE_ID = 1
DECODER_TYPE_FLD = 1
DECODER_TYPE_AD = 0
DECODER_TYPE_ULD = 0

FW_TYPE_BOOT = FwTypeBoot
FW_TYPE_APP_CORDED = FwTypeAppCorded
FW_TYPE_APP_RFBASE = FwTypeAppRfBase
FW_TYPE_APP_RFSCAN = FwTypeAppRfScan
FW_TYPE_APP_3310 = FwTypeAppVuquest3310
FW_TYPE_APP_HI2D = FwTypeAppHI2D
FW_TYPE_APP_YJ2D = FwTypeAppYJ2D

BUILD_RELEASE = BuildRelease


.IF $(APPTYPE)==BOOT

#******************************************************************************************************
# Boot section
#******************************************************************************************************
# Here is the table listing the currrent targets you can build:
#
# imxl_build
# imx25_build
# imx25_Gen6_Prototype_Build
# imx25_iram_Gen6_RAMTEST
#
# One string must be selected in the batch, and APPTYPE must be set to boot BOOT in the batch file.
# old mxl platform
.IF $(BOOTTARGET)==mxl_build

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=mxl
CPP_DEF_EXTRA_BOOT += -DNDEBUG
ASM_PREDEF_EXTRA_BOOT =  --predefine "platform_current SETS \"mxl\"" --predefine "Board_TARGET SETS \"MXL_Matrix\""
TYPE_CPU_BOOT = ARM920T
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x10000000  --rw-base 0x81000000  --first vectors.o(Vector)

#Load to iMX25 internal RAM for copying to SPI Flash
.ELSIF $(BOOTTARGET)==imx25_8100

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=imx25
CPP_DEF_EXTRA_BOOT+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_BOOT += -DNDEBUG
ASM_PREDEF_EXTRA_BOOT = --predefine "platform_current SETS \"imx25\""  --predefine "Board_TARGET SETS \"iMX25_Gen6_Board\""
TYPE_CPU_BOOT = ARM926EJ-S
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x78006000  --first vectors.o(Vector)
FW_TYPE = $(FW_TYPE_BOOT)


#Load to iMX25 Lite Settings
.ELSIF $(BOOTTARGET)==imx25_LITE

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=imx25
CPP_DEF_EXTRA_BOOT+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_BOOT += -DNDEBUG
CPP_DEF_EXTRA_BOOT += -DXENON_LITE_1500
ASM_PREDEF_EXTRA_BOOT = --predefine "platform_current SETS \"imx25\""  --predefine "Board_TARGET SETS \"iMX25_Gen6_Board\""
TYPE_CPU_BOOT = ARM926EJ-S
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x78006000  --first vectors.o(Vector)
FW_TYPE = $(FW_TYPE_BOOT)


#Load to iMX25 Vuquest3310 Settings
.ELSIF $(BOOTTARGET)==imx25_3310

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=imx25
CPP_DEF_EXTRA_BOOT+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_BOOT += -DNDEBUG
CPP_DEF_EXTRA_BOOT += -DVUQUEST3310
ASM_PREDEF_EXTRA_BOOT = --predefine "platform_current SETS \"imx25\""  --predefine "Board_TARGET SETS \"iMX25_Gen6_Board\""
TYPE_CPU_BOOT = ARM926EJ-S
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x78006000  --first vectors.o(Vector)
FW_TYPE = $(FW_TYPE_BOOT)


#Load to iMX25 HI2D Settings
.ELSIF $(BOOTTARGET)==imx25_5100

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=imx25
CPP_DEF_EXTRA_BOOT+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_BOOT += -DNDEBUG
CPP_DEF_EXTRA_BOOT += -DHI2D_T
ASM_PREDEF_EXTRA_BOOT = --predefine "platform_current SETS \"imx25\""  --predefine "Board_TARGET SETS \"iMX25_Gen6_Board\""
TYPE_CPU_BOOT = ARM926EJ-S
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x78006000  --first vectors.o(Vector)
FW_TYPE = $(FW_TYPE_BOOT)

#Load to iMX25 YJ2D Settings
.ELSIF $(BOOTTARGET)==imx25_YJ2D

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=imx25
CPP_DEF_EXTRA_BOOT+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_BOOT += -DNDEBUG
CPP_DEF_EXTRA_BOOT += -DHI2D_T
CPP_DEF_EXTRA_BOOT += -DYJ2D_BASED_ON_HI2D
ASM_PREDEF_EXTRA_BOOT = --predefine "platform_current SETS \"imx25\""  --predefine "Board_TARGET SETS \"iMX25_Gen6_Board\""
TYPE_CPU_BOOT = ARM926EJ-S
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x78006000  --first vectors.o(Vector)
FW_TYPE = $(FW_TYPE_BOOT)

#Load to iMX25 OEM Settings
.ELSIF $(BOOTTARGET)==imx25_OEM

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=imx25
CPP_DEF_EXTRA_BOOT += -DNDEBUG
CPP_DEF_EXTRA_BOOT += -DOEMBOOT
ASM_PREDEF_EXTRA_BOOT = --predefine "platform_current SETS \"imx25\""  --predefine "Board_TARGET SETS \"iMX25_Gen6_Board\"" --predefine "OEMBOOT SETL {TRUE}"
TYPE_CPU_BOOT = ARM926EJ-S
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x78006000  --first vectors.o(Vector)
FW_TYPE = $(FW_TYPE_BOOT)

#Load to iMX25 iRAM - Boot will run only in iRAM.
.ELSIF $(BOOTTARGET)==imx25_iram_Gen6_RAMTEST

CPP_DEF_EXTRA_BOOT = -DPLATFORM_CURRENT=imx25 -DRAMBASEDBOOT
CPP_DEF_EXTRA_BOOT += -DNDEBUG
ASM_PREDEF_EXTRA_BOOT = --predefine "platform_current SETS \"imx25\""  --predefine "Board_TARGET SETS \"iMX25_Gen6_Board\"" --predefine "RAMBASEDBOOT SETL {TRUE}"
TYPE_CPU_BOOT = ARM926EJ-S
TARGET_CPP_FLAGS_BOOT =  -Ospace
TARGET_LINK_FLAGS_BOOT =  --ro-base 0x78006000  --first vectors.o(Vector)
FW_TYPE = $(FW_TYPE_BOOT)

.END


# Special modification of APPTYPE to process the next section for:
#   APPTYPE == APP  --OR--  APPTYPE == SANITIZE  --OR--  APPTYPE=SAN_APP
.ELSIF $(APPTYPE:s/SANITIZE/APP/:s/SAN_APP/APP/) == APP


#******************************************************************************************************
# App section
#******************************************************************************************************
# Here is the table listing the currrent targets you can build:
#
# mxl_build
# imx25_build
# imx25_Brototype_Build
# imx25_RF_Base
# imx25_RF_Scan
#
# One string must be selected in the batch, and APPTYPE must be set to boot BOOT in the batch file.



#Load to imx27 eval board
# old mxl platform
.IF $(APPTARGET)==mxl_build

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=mxl  -DFLASH_DEBUG_ON=1  -DSIMPLE_IO_ONLY -DCORDED=1
CPP_DEF_EXTRA_APP += -DNDEBUG
ASM_PREDEF_EXTRA_APP =  --predefine "platform_current SETS \"mxl\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"MXL_Matrix\""
TYPE_CPU_APP = ARM920T
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x08001000  --first threads_interupts.o(threads_interupts)

#Load imx25 FF1 FF1.5 corded ID decoder
.ELSIF $(APPTARGET)==imx25_ID_FF1
CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1 -DPROTO_HW
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1
CPP_DEF_EXTRA_APP +=-DDISABLE_AUTO_CBL=1
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)


#Load imx25_build (Corded)(FF2)ID decoder
.ELSIF $(APPTARGET)==imx25_ID_FF2

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1
CPP_DEF_EXTRA_APP +=-DDISABLE_AUTO_CBL=1
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25_build (Corded)(FF3)ID decoder
.ELSIF $(APPTARGET)==imx25_ID_FF3_8100

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25_build (Corded)(FF3)ID decoder, Customize version of GongJin 
.ELSIF $(APPTARGET)==imx25_ID_FF3_8213

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DSM8213
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

.ELSIF $(APPTARGET)==imx25_ID_FF3_5130

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DSM5130
CPP_DEF_EXTRA_APP += -DDISABLE_AUTO_CBL
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
IMAGER_TYPE = BYD_IMAGER
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_HI2D)

#Load imx25_build (Corded)(FF3)ID decoder Engineering Build
.ELSIF $(APPTARGET)==imx25_ID_FF3_ENG

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP +=  -DDEVELOPMENT_BUILD
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25_build (Corded)(FF3)ID decoder for Lite
.ELSIF $(APPTARGET)==imx25_ID_FF3_LITE

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DXENON_LITE_1500
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25_build (OEM Engine)(FF3)ID decoder Build
.ELSIF $(APPTARGET)==imx25_ID_FF3_OEM_ENGINE

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1 -DOEM_ENGINE
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS -DDIRT_TEST
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP +=  -DDEVELOPMENT_BUILD
#stop on division by zero
#CPP_DEF_EXTRA_APP += -DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25_build (Corded)(FF3)
.ELSIF $(APPTARGET)==imx25_FF3

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP += -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
PLUGINENA=YES
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25_build FF2 (Corded)
.ELSIF $(APPTARGET)==imx25_FF2

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1
CPP_DEF_EXTRA_APP +=-DDISABLE_AUTO_CBL=1
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
PLUGINENA=YES
TARGET_CPP_FLAGS_APP =  -Otime --fpmode=ieee_fixed
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25 FF1 FF1.5 corded
.ELSIF $(APPTARGET)==imx25_FF1

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1 -DPROTO_HW
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1
CPP_DEF_EXTRA_APP +=-DDISABLE_AUTO_CBL=1
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
PLUGINENA=YES
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_CORDED)

#Load imx25 - RF BASE FF2
.ELSIF $(APPTARGET)==imx25_RF_Base_FF2

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DRFBASE=1 -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "RFBASE SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
PLUGINENA=YES
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
# don't need any decoders for base.
DECODER_TYPE_ID=0
DECODER_TYPE_FLD=0
FW_TYPE = $(FW_TYPE_APP_RFBASE)

#Load imx25 - RF BASE FF1
.ELSIF $(APPTARGET)==imx25_RF_Base_FF1

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DRFBASE=1 -DPROTO_HW
CPP_DEF_EXTRA_APP += -DNDEBUG
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "RFBASE SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
PLUGINENA=YES
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_RFBASE)

#Load imx25 - RF SCANNER FF2
.ELSIF $(APPTARGET)==imx25_RF_Scan_FF2

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DRFSCAN=1 -DFLASH_LEDS -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "RFSCAN SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
PLUGINENA=YES
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_RFSCAN)

#Load imx25 - RF SCANNER FF2 with ID
.ELSIF $(APPTARGET)==imx25_ID_RF_Scan_FF2

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DRFSCAN=1 -DFLASH_LEDS -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "RFSCAN SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
PLUGINENA=YES
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_RFSCAN)

#Load imx25 - RF SCANNER FF1
.ELSIF $(APPTARGET)==imx25_RF_Scan_FF1

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DRFSCAN=1 -DPROTO_HW
CPP_DEF_EXTRA_APP += -DNDEBUG
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "RFSCAN SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
PLUGINENA=YES
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_RFSCAN)

#Load imx25 - RF SCANNER for StoneStreet.  This is used to create the lib that has everything except SPPTest (and ProjectRevision) modules.
.ELSIF $(APPTARGET)==imx25_RF_Scan_StoneStreet_Prototype

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DRFSCAN=1 -DPROTO_HW
CPP_DEF_EXTRA_APP += -DNDEBUG
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "RFSCAN SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_RFSCAN)

# This is for building the matrix app from the big lib created by imx25_RF_Scan_StoneStreet_* (to be used by StoneStreet)
.ELSIF $(APPTARGET)==MatrixLibStoneStreet

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DRFSCAN=1 -DPROTO_HW
CPP_DEF_EXTRA_APP += -DNDEBUG
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "RFSCAN SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)

#Load imx25 - Vuquest 3310, base on imx25_build (Corded)(FF3)ID decoder
.ELSIF $(APPTARGET)==imx25_ID_FF3_3310

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DVUQUEST3310
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_3310)

.ELSIF $(APPTARGET)==imx25_ID_FF3_5100

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DSM5100
CPP_DEF_EXTRA_APP += -DIR_TRIGGER
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
IMAGER_TYPE = BYD_IMAGER
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_HI2D)

.ELSIF $(APPTARGET)==imx25_ID_FF3_4100

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DSM4100
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
IMAGER_TYPE = BYD_IMAGER
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_HI2D)

.ELSIF $(APPTARGET)==imx25_ID_FF3_3100

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DSM3100
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
IMAGER_TYPE = BYD_IMAGER
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_HI2D)

.ELSIF $(APPTARGET)==imx25_ID_FF3_3110

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DSM3110
CPP_DEF_EXTRA_APP += -DDISABLE_AUTO_CBL
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
IMAGER_TYPE = BYD_IMAGER
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_HI2D)

.ELSIF $(APPTARGET)==imx25_ID_FF3_YJ2D

CPP_DEF_EXTRA_APP = -DPLATFORM_CURRENT=imx25 -DCORDED=1
CPP_DEF_EXTRA_APP += -DGEN6_DEVELOPMENT=1 -DCONCURRENT=1 -DFLASH_LEDS
CPP_DEF_EXTRA_APP+= -DUSE_DUAL_SPI
CPP_DEF_EXTRA_APP += -DNDEBUG
CPP_DEF_EXTRA_APP += -DHI2D_T
CPP_DEF_EXTRA_APP += -DYJ2D_BASED_ON_HI2D
#stop on division by zero
#CPP_DEF_EXTRA_APP+=-DSHOW_LIB_TRAPS -DSHOW_ABORTS
PLUGINENA=YES
ASM_PREDEF_EXTRA_APP = --predefine "platform_current SETS \"imx25\""  --predefine "CORDED SETL {TRUE}" --predefine "Board_TARGET SETS \"iMX25_Development\""
TYPE_CPU_APP = ARM926EJ-S
TARGET_CPP_FLAGS_APP =  -Otime
TARGET_LINK_FLAGS_APP =  --ro-base 0x80001000  --first threads_interupts.o(threads_interupts)
COMPRESS_BIN = libraries/AppCompressorBinExe/AppDecompCorded.bin
FW_TYPE = $(FW_TYPE_APP_YJ2D)

.END

.IF $(IMAGER_TYPE)==BYD_IMAGER
CPP_DEF_EXTRA_APP += -DHI2D_T
CPP_DEF_EXTRA_APP += -DBYD_SENSOR
.ELSIF
# imager driver defininatin of MT9V022
.END

#ENABLE 2 LINES BELOW FOR WATCHDOG DEBUGGING
#CPP_DEF_EXTRA_APP += -DWDOGDEBUG
#ASM_PREDEF_EXTRA_APP += --predefine "WDOGDEBUG SETL {TRUE}"

#ENABLE 1 LINE BELOW FOR ABORTS
#CPP_DEF_EXTRA_APP += -DSHOW_LIB_TRAPS -DSHOW_ABORTS

.END


#	MOCF Handling
.IF $(FW_TYPE) == $(FW_TYPE_APP_CORDED)
.IF $(APPTARGET)==imx25_ID_FF3_LITE
COMPAT_PROD_FILE=MakeMocf/CordedApp1500/CompatProdApp_Corded.txt
.ELSE
COMPAT_PROD_FILE=MakeMocf/CordedApp/CompatProdApp_Corded.txt
.END # endof .IF $(APPTARGET) == imx25_ID_FF3_LITE
SW_VER_FILE=MakeMocf/ProjectRevisionApp.txt
FW_OBJ_OPT=a

.ELSIF $(FW_TYPE)==$(FW_TYPE_APP_RFBASE)
COMPAT_PROD_FILE=MakeMocf/RfBaseApp/CompatProdApp_RfBase.txt
SW_VER_FILE=MakeMocf/ProjectRevisionApp.txt
FW_OBJ_OPT=a

.ELSIF $(FW_TYPE)==$(FW_TYPE_APP_RFSCAN)
COMPAT_PROD_FILE=MakeMocf/RfScanApp/CompatProdApp_RfScan.txt
SW_VER_FILE=MakeMocf/ProjectRevisionApp.txt
FW_OBJ_OPT=a

.ELSIF $(FW_TYPE) == $(FW_TYPE_APP_3310)
COMPAT_PROD_FILE=MakeMocf/Vuquest3310App/CompatProdApp_Vuquest3310.txt
SW_VER_FILE=MakeMocf/ProjectRevisionApp.txt
FW_OBJ_OPT=a

.ELSIF $(FW_TYPE) == $(FW_TYPE_APP_HI2D)
COMPAT_PROD_FILE=MakeMocf/HI2DApp/CompatProdApp_HI2D.txt
SW_VER_FILE=MakeMocf/ProjectRevisionApp.txt
FW_OBJ_OPT=a

.ELSIF $(FW_TYPE) == $(FW_TYPE_APP_YJ2D)
COMPAT_PROD_FILE=MakeMocf/YJ2DApp/CompatProdApp_YJ2D.txt
SW_VER_FILE=MakeMocf/ProjectRevisionApp.txt
FW_OBJ_OPT=a

.ELSIF $(FW_TYPE)==$(FW_TYPE_BOOT)
.IF $(BOOTTARGET) == imx25_LITE
COMPAT_PROD_FILE=MakeMocf/Boot1500/CompatProdBoot.txt
.ELSIF $(BOOTTARGET) == imx25_3310
COMPAT_PROD_FILE=MakeMocf/Boot3310/CompatProdBoot.txt
.ELSIF $(BOOTTARGET) == imx25_5100
COMPAT_PROD_FILE=MakeMocf/BootHI2D/CompatProdBoot.txt
.ELSIF $(BOOTTARGET) == imx25_YJ2D
COMPAT_PROD_FILE=MakeMocf/BootYJ2D/CompatProdBoot.txt
.ELSE
COMPAT_PROD_FILE=MakeMocf/Boot/CompatProdBoot.txt
.END
FW_OBJ_OPT=b
SW_VER_FILE=libraries/OsKernel/db_boot/ProjectRevisionBoot.h

.END  #endof .IF $(FW_TYPE)==$(FW_TYPE_CORDED)

