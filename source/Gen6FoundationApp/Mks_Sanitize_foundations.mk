# File Name - Mks_Sanitize_foundations.mk
# Include file to create partially links object files or libraries for santized builds.
# (Included only for APPTYPE == SANITIZE  --OR--  APPTYPE == SAN_APP)


# Determine/define the *.plo files needed for the Sanitized sandbox / sanitized build.
NEEDED_OBJECTS =  config.plo  eci.plo  flash.plo  hhpl.plo  plugin.plo
.IF $(PRODUCT_TYPE:s/Corded/YES/:s/RfBase/YES/) == YES
NEEDED_OBJECTS += if4683.plo  usb2.plo  wedge.plo
.END
.IF $(PRODUCT_TYPE:s/Corded/YES/:s/RfScan/YES/) == YES
NEEDED_OBJECTS += misc.plo  sd.plo
.END
.IF $(PRODUCT_TYPE) == RfBase		# for This, there is only the one module in this PLO
.IF $(PROVIDE_DATAEDIT_SOURCE) != YES
NEEDED_OBJECTS += misc.plo
.END
.END

# all libs will be in this directory for SAN_APP...
# (Directory is at root of new sandbox.)
PREBUILT_DIR = Prebuilt_LIBS_$(TARGET_NAME)
# Setup for specified APPTYPE
.IF $(APPTYPE) != SANITIZE			# (APPTYPE == SAN_APP)
# adjust paths of previously specified libs and add prebuild objects
LIBS_NOSOURCE := $(LIBS_NOSOURCE:f:^"$(PREBUILT_DIR)/") $(NEEDED_OBJECTS:^"$(PREBUILT_DIR)/")
.ELSE						# (APPTYPE == SANITIZE)
APP_OBJDIR   = Output_APP_$(TARGET_NAME)
NEW_SBXDIR   = ../Output_Sanitized_Sandbox
PREBUILT_DST = $(NEW_SBXDIR)/$(PREBUILT_DIR)
# Set OBJDIR for cleaning and BUILD_OPTS rules that are defined in the main makefile...  (Its not clear the substitute here works correctly...)
OBJDIR       = $(NEW_SBXDIR:s~/~\~)
ECLIPSE_DST1 = "$(OBJDIR)\Build Targets"
ECLIPSE_DST2 = "$(ECLIPSE_DST1:s/"//)\Build from Distribution Package"
ECLIPSE_DST3 = "$(ECLIPSE_DST2:s/"//)\DistributionBuilds.txt"

LNK_LST      = .\Prebuilt_plo_Map_$(TARGET_NAME)_$(%:b).lst

LFLAGS_PLO =  --remove --nolocals --nodebug  --info totals,common,debug,sizes,veneers,unused  --list $(LNK_LST)

# bat files and makefiles to copy (with sources)
MODIFIED_BAT_FILES := MKS_clean_foundations_gen6.bat  MKS_make_foundations_gen6.bat
COPIED_MISC_FILES := makefile  Mks_project_target_foundations.mk  Mks_source_foundations.mk  Mks_source_decoders.mk  Mks_Sanitize_foundations.mk
COPIED_MISC_FILES += sed.exe  libraries\AppCompressorBinExe\ApplicationCompressor.exe
.IF $(PRODUCT_TYPE) == Corded
COPIED_MISC_FILES += libraries\AppCompressorBinExe\AppDecompCorded.bin
.ELSIF $(PRODUCT_TYPE) == RfBase
COPIED_MISC_FILES += libraries\AppCompressorBinExe\AppDecompCorded.bin
COPIED_MISC_FILES += libraries\AppCompressorBinExe\AppDecompRfBase.bin
.ELSIF $(PRODUCT_TYPE) == RfScan
COPIED_MISC_FILES += libraries\AppCompressorBinExe\AppDecompCorded.bin
COPIED_MISC_FILES += libraries\AppCompressorBinExe\AppDecompRfScan.bin
.END
COPIED_MISC_FILES += .cdtproject .project LoadApp.inc
COPIED_MISC_FILES += MakeMocf\AppendToMocf.exe MakeMocf\CreateFwObject.exe MakeMocf\UnpackMocf.exe
.IF $(BOOTTARGET) == imx25_8100
COPIED_MISC_FILES += MakeMocf\Boot\CompatProdBoot.txt
.ELSIF $(PRODUCT_TYPE) == Corded
COPIED_MISC_FILES += MakeMocf\CordedApp\CompatProdApp_Corded.txt MakeMocf\CordedApp\SwVersionAppInternalUse_Corded.txt MakeMocf\CordedApp\SwVersionApp_Corded.txt
.ELSIF $(PRODUCT_TYPE) == RfBase
COPIED_MISC_FILES += MakeMocf\RfBaseApp\CompatProdApp_RfBase.txt MakeMocf\RfBaseApp\SwVersionAppInternalUse_RfBase.txt MakeMocf\RfBaseApp\SwVersionApp_RfBase.txt
.ELSIF $(PRODUCT_TYPE) == RfScan
COPIED_MISC_FILES += MakeMocf\RfScanApp\CompatProdApp_RfScan.txt MakeMocf\RfScanApp\SwVersionAppInternalUse_RfScan.txt MakeMocf\RfScanApp\SwVersionApp_RfScan.txt
.END

# Libs to copy (with sources)
COPIED_LIBS = $(LIBS_NOSOURCE)

# Set DEPS to be the list of matrix sources that will be preserved in the sanitized sandboxes,
# adding .d as a suffix and object directory name as a prefix
# We'll process the .d files from the reference build to find all source to copy to the sanitized sandbox.
DEPS := $(MATRIX_SOURCES:+".d":^"$(APP_OBJDIR)/")


SANITIZED_SANDBOX :  OUTPUT_FOLDERS  $(NEEDED_OBJECTS:^"$(PREBUILT_DST)/")
SANITIZED_SANDBOX :  REPLICATE_PROVIDED_SOURCES_AND_LIBS
SANITIZED_SANDBOX :  $(NEW_SBXDIR)/Sanitized_sandbox_root.txt

# To make this rule work, where OBJDIR and PREBUILT_DST have no embedded quotes or spaces,
# while ECLIPSE_DST1, ECLIPSE_DST2, and ECLIPSE_DST3 have both, we have to be carefull here.
# For some reason, quoting the arg to md lets it handle paths with embedded '/' changing to '\'.
OUTPUT_FOLDERS :
[
	@IF NOT EXIST $(OBJDIR) @echo Creating $(OBJDIR)              &&  md "$(OBJDIR)"
	@IF NOT EXIST $(PREBUILT_DST) @echo Creating $(PREBUILT_DST)  &&  md "$(PREBUILT_DST)"
	@IF NOT EXIST $(ECLIPSE_DST1) echo Creating $(ECLIPSE_DST1)   &&  md "$(ECLIPSE_DST1:s/"//)"
	@IF NOT EXIST $(ECLIPSE_DST2) echo Creating $(ECLIPSE_DST2)   &&  md "$(ECLIPSE_DST2:s/"//)"
	@IF NOT EXIST $(ECLIPSE_DST3) echo Creating $(ECLIPSE_DST3)   &&  @echo dummy > "$(ECLIPSE_DST3:s/"//)"
]


$(NEW_SBXDIR)/Sanitized_sandbox_root.txt :  FORCE
[
	@echo off
	@echo $% - Setup complete...
	@echo>>$% This is the root directory of the sanitized sandbox for target: $(TARGET_NAME)
	@echo>>$% (Last Make run: %DATE%  %TIME:~0,8%)
]


$(PREBUILT_DST)/config.plo  :  $(CONFIG_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/eci.plo     :  $(ECI_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/flash.plo   :  $(FLASH_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/hhpl.plo    :  $(HHPL_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/if4683.plo  :  $(IF4683_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/misc.plo    :  $(MISC_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/plugin.plo  :  $(PLUGIN_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/sd.plo      :  $(SD_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/usb2.plo    :  $(USB2_SOURCES:+".o":^"$(APP_OBJDIR)/")

$(PREBUILT_DST)/wedge.plo   :  $(WEDGE_SOURCES:+".o":^"$(APP_OBJDIR)/")


%.plo .PRECIOUS :
[
	@echo off
	@echo $%
	$(LINK_) $(LFLAGS_PLO) --partial --output $%  $&
]




REPLICATE_PROVIDED_SOURCES_AND_LIBS : $(PRODUCT_TYPE)_Complete_Sanitized_Sandbox.bat
[
	@echo off
	@echo Calling $< for $%  (log to $(<:b).log )
	$(SHELL) > $(<:b).log 2>&1  /c Call $<
]


# Create a batfile to edit batfiles; copy makefiles; copy libs; and find and copy the provided sources to the sanitized sandbox
#   (This only works when SHELL is cmd.exe)
$(PRODUCT_TYPE)_Complete_Sanitized_Sandbox.bat .PRECIOUS : FORCE
[
# boiler plate
	echo @echo off> $%
	echo setlocal ENABLEEXTENSIONS>> $%
	echo set  COPYCMD=>> $%
# create command to edit batfiles for Sanitized sandbox
	sed -f <+ s`\(.*\)`sed -e 's/BOOT or APP/Must be SAN_APP/' < \1 > $(NEW_SBXDIR:s~/~\\~)\\\1` +> <+ $(MODIFIED_BAT_FILES:t"\n") +>  >> $%
	echo.>> $%  # (Adds end of line for last item...)
# Conditionally copy other files
	sed -f <+ s`\(.*\)`call  :Copy_if_newer  \1  $(NEW_SBXDIR:s~/~\\~)\\\1` +> <+ $(COPIED_MISC_FILES:t"\n") +>  >> $%
	echo.>> $%
# Conditionally copy libs -- strip DIR_SBOX_ROOT (using a wildcard for any dir separators) then call :Copy_if_newer for each...
	sed -f <+ s`/`\\`g
		s`$(DIR_SBOX_ROOT:s~\~.~:s~/~.~).``
		s`\(.*\\\)\(.*\)`call  :CopyLib_if_newer  \1\2` +>  <+ $(COPIED_LIBS:t"\n") +>  >> $%
	echo.>> $%
# Conditionally copy sources (by formatting each line to a call to the sub script at the end of this batfile (Copy_if_newer).
# (find all source and include files referenced in dependancies of objects not in the prebuilt stuff.
#   - The :t"" modifier didn't work except as the last.
#   - The :^"" modifier won't insert a space.  (I tried: copy <+ $(DEPS:^"type ":s~/~\~:t"\n") +>  > Concatenate_Sources.bat)
#   - I have to quote or escape the '^' in the sed command by entering it twice.
	sed -e 's/^^/type /' <+ $(DEPS:s~/~\~:t"\n") +>  > Concatenate_Sources.bat
	echo.>> Concatenate_Sources.bat
	$(SHELL) /Q /c Concatenate_Sources.bat  > sources_list_1.txt
	sed -f <+ s`/`\\`g
		s`.*: $(DIR_SBOX_ROOT:s~\~.~:s~/~.~).`` +> sources_list_1.txt | sort | uniq -i > sources_list_2.txt
	sed -f <+ s`\(.*\)`call  :Copy_if_newer  \1  $(NEW_SBXDIR:s~/~\\~)\\\1` +>  sources_list_2.txt  >> $%
	del /q  Concatenate_Sources.bat  sources_list_1.txt  sources_list_2.txt
# Add epilog
	echo goto  :EOF>> $%
	echo.>> $%
# These called proceedures will only copy files/libraries whose source file timestamp is newer than the destination.
# They are called with the relative path (W.R.T. the directory containing this makefile) and filename of the source file.
# (e.g. FoundationApp\OS_Start.c or libraries\Compression\Cmpr_LIB\Compression.a)
# We use only the path as the destination as all copies use the original source filename,
# and this keeps xcopy from asking whether the destination is a file or directory.
# For 'Copy_if_newer', we provide the new relative path as the second argument.  Path W.R.T sandbox root is the same.
# For 'CopyLib_if_newer', the destination path is always the same, $(PREBUILT_DIR) of the new sandbox.
	echo :Copy_if_newer>> $%
	echo xcopy /D/F/H/K/R/Y  %%1  %%~p2\>> $%
	echo goto  :EOF>> $%
	echo.>> $%
	echo :CopyLib_if_newer>> $%
	echo xcopy /D/F/H/K/R/Y  %%1  $(PREBUILT_DST:s~/~\~)\>> $%
]


# Clean PREBUILT directory
CLEANLIBS :
[
	@echo \# Cleaning libs in: $(PWD)\$(PREBUILT_DST:s~/~\~)
	@IF EXIST $(PREBUILT_DST:s~/~\~)\*.plo       del    $(PREBUILT_DST:s~/~\~)\*.plo
	@IF EXIST $(PREBUILT_DST:s~/~\~)\*.a         del /F $(PREBUILT_DST:s~/~\~)\*.a
]

.END  #endof .IF $(APPTYPE) != SANITIZE
