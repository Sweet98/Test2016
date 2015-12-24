#==========================================================================================================================================
# Make for GEN 6 Foundations Project, boot and app platforms 
#
# This file is a generic, boiler-plate makefile used for generating libs, executables, etc.  It achieves project indpendence
# via input macros.  For example, an APP build uses:  LIBS, LIBS_NOSOURCE, MATRIX_SOURCES_APP, DIR_SOURCE_APP.  You set these
# appropriately in the "sources" makefile (this makefile includes this).  We use the APPTYPE macro to indicate an library or
# application build and the APPTARGET macro to distinguish between different builds of the code.  This makefile constructs appropriate
# output folders based on these macros to ensure a unique folder for each project's output files.
#
###### Customization notes     ######
# This makefile imports two environment variables, DIR_SBOX_ROOT (where the root of the project files are) and TOOLSET (what
# toolchain to use (set to RVDS for Gen6 projects).  The Eclipse project settings for Gen6 has these environment variables preset.
#
# This makefile checks environment variables TEMP, then TMPDIR for a path to use for temporary files.  If neither are
# defined, "C:\TEMP" is asssumed as the temp file path.  The temp file path must be an existing directory.
#
# The path to the ARM (RVDS 3.1) toolchain executables is defined via the TOOLPATH := directive in this makefile to be:
#    "C:\Program Files\ARM\RVCT\Programs\3.1\569\win_32-pentium".
# If installed elsewhere, such as on a different drive (or changing versions) that definition must be changed.
###### End Customization notes ######
#
# To create a sanitzed build sandbox do the following steps:
#   0. Make sure the root of a sandbox of the project d:/MKS_Other/Tools/bin-cmd/tool-bin-cmds.pj is in your path.  This is
#      required for step 2 only.  (See: http://ny90w0013:7001/documentation/local/AccessingMKSUtilityProjects.html.)
#   1. Call this makefile, using APPTYPE=APP, for each APPTARGET to be provided.  (APP/CLEAN build opts in Eclipse.)
#      (Recommend using the Release_Builds batfile for this, even though we don't need the boot loader builds.)
#   2. Call this makefile, using APPTYPE=SANITIZE, and target SANITIZED_SANDBOX (instead of MATRIX), for each APPTARGET
#      to be provided.  When using MKS_make_foundations_gen6.bat, the batfile does this change to the target when the
#      APPTYPE is SANITIZE.  These are the build targets in folder "Build Targets/Distribution Package Construction"
#      in Eclipse.  This creates special output folder  "..\Output_Sanitized_Sandbox" for the sanitized sandbox.
#      It builds a *.plo file (a Partially Linked Object) from the object files for each sanitized group.
#      (See notes at "# App Source Files:" in the makefile: Mks_source_foundations.mk.)  For this mode, the macro
#      MATRIX_SOURCES_APP specifies all sources that will be provided.  The corresponding *.d files created in step 1 are
#      used to determine all source files that are provided to the customer in the sanitized sandbox.  It copies those files
#      to the sanitized sandbox, replicating the source file directory structure.  The *.plo files and all libraries created
#      or used in step 1 are copied to subdirectory: Prebuilt_LIBS_$(TARGET_NAME) so the *.plo files are correct for each
#      target.  In addition the following are copied to the Output_Sanitized_Sandbox tree:
#         - Modified copies of bat files: MKS_make_foundations_gen6.bat and MKS_clean_foundations_gen6.bat
#         - All makefiles.
#         - Tools and datafiles required for the make and link that are not part of the ARM toolchain.
#      If this step is rerun, the source copies are only done for files in the source whose file timestamps are newer than
#      the previous copy.
#      When testing is completed, the directory "..\Output_Sanitized_Sandbox" should be moved to an empty directory,
#      and renamed Gen6FoundationApp.  That fomerly empty directory becomes the sandbox to ship to the customer.
#      Since the compile is not called in this step, a second run is not required to optimize the feedback file.
#   3. The newly created and modified bat files will be used by the customer to build from their copy of the sanitized
#      sandbox.  The build targets they use are all in folder "Build Targets/Build from Distribution Package" in Eclipse
#      and all other build targets can be removed from their copy of the .cdtproject file.  (To be automated later.)
#      Note that they use APPTYPE=SAN_APP and target MATRIX in this mode.
#
# Use of the MKS_clean_foundations_gen6.bat file with each APPTYPE setting cleans files created by that step only, except that the sources
# copied in step two can presently only be deleted by deleting the Output_Sanitized_Sandbox tree.
#==========================================================================================================================================

# When YES, does not echo the recipe line, and does not issue any warnings when making a target with this attribute.
# This is equivalent to placing the @ command prefix before every recipe line in the rule.
.SILENT=yes
# uncommenting one or both of the lines below gives listing/assembler output

#DEBUG_LIST:=yes
#DBG_ASMOUT:=yes

# Set Null macros

PROLOG  =

# Retreive environment variables
# import system info, overriding any defaults.

.IMPORT         : MKSMAKE	# This must be defined in the environment!
COMSPEC :=
ComSpec :=
.IMPORT .IGNORE : COMSPEC
.IMPORT .IGNORE : ComSpec
TEMP :=
.IMPORT .IGNORE : TEMP
TMPDIR := $(TEMP)
.IMPORT .IGNORE : TMPDIR

#  If TEMP and TMPDIR are null in the environment, set a default:
.IF $(TMPDIR) == $(NULL)
TMPDIR := C:\TEMP
.END

# If SHELL not defined, use COMSPEC or ComSpec (only one will be defined), assumed to be command.com/cmd.exe.
# If SHELL is defined, it is assumed to be the MKS Toolkit Korn Shell
.IF $(ComSpec)
    COMSPEC := $(ComSpec)
.END

# Set the path name, stored in the make macro(GROUPSHELL), of the command interpreter(SHELL),
# because make calls command prompt SHELL to process group recipes
SHELL = $(COMSPEC)
GROUPSHELL := $(SHELL)

# Set up the MKS built in macros, used for the command line, which are used by group recipes ( [] ).

.IF $(SHELL) == $(COMSPEC)

    # The character currently being used to mark options in command lines
    SWITCHAR := -

    # Specifies a list of metacharacters. If make finds any metacharacter, it invokes the recipe using the shell specified by SHELL
    SHELLMETAS := *?"<>|

    # Specifies options to pass to the shell when invoking it to execute a single line recipe
    SHELLFLAGS := /c

    # Specifies options to pass to GROUPSHELL when make invokes it to execute a group recipe
    GROUPFLAGS := $(SHELLFLAGS)

    # Specifies a string for make to use as a suffix when creating group recipe files to be handed to the command interpreter
    GROUPSUFFIX := .bat

.ELSE
    SHELLMETAS := *?"<>|()&][$$\#`'
    SHELLFLAGS := -c
    GROUPFLAGS := $(NULL)
    GROUPSUFFIX := .ksh
.END



#==========================================================================================================================================
#     Set Paths, compiler, and build choices
#==========================================================================================================================================

# import sandbox root directory
.IMPORT  .IGNORE : DIR_SBOX_ROOT
.IF $(DIR_SBOX_ROOT) == $(NULL)
   DIR_SBOX_ROOT:=.
.END
# setup location of project directory, usually same as above
.IF $(PROJROOT) == $(NULL)
   PROJROOT := $(DIR_SBOX_ROOT)
.END

# set up location of library ".a" files
LIBDIR := $(PROJROOT)

# Specify make file to include when APPTYPE == LIB (to allow overrides only on the make command line)
.IF $(LIB_MAKE) == $(NULL)
   LIB_MAKE = Sources.mk
.END

# import type of compiler after setting default
TOOLSET := RVDS
.IMPORT .IGNORE : TOOLSET

#import type of application - Commented out since it is set as a macro using a -D on the command line.
#.IMPORT : APPTYPE

# Set up system variable for path, adding the tool path
.IMPORT : PATH
PATH := $(PATH);$(TOOLPATH);
.EXPORT : PATH

# Lets NULL out our variables
CPP_DEF =
ASM_PREDEF =
TARGET_CPP_FLAGS =
TARGET_LINK_FLAGS =
LNK_LST =

#***************************************************************
# Set compiler flag to build .c as .cpp
C_AS_CPP = -dotciscxx

#==========================================================================================================================================
#	Set up project specific defines
#==========================================================================================================================================

# SED is a stream editor used to modify the .d files, pass file name with .d extension.
# > passes the name of the library if the current target is a library member, with a .d extension.
COOK_DEPENDS = $(PROJROOT)\sed -f "$(PROJROOT)\format.sed"  $(%:b:+".d") > $(OBJDIR)\$(@:b:+".d")

.IF $(APPTYPE) == LIB
.INCLUDE : $(LIB_MAKE)
.ELSE
# include the the project list for the unique build options for the different targets
.INCLUDE : Mks_project_target_foundations.mk

# include the the source file dependency list
.INCLUDE : Mks_source_foundations.mk
.END

# Set up the location of the object file directory, not used if done below
#.IMPORT : OBJDIR

#***************************************************************
# Set up the path and name for target specific object file directory, to store output files
# default compressor progam name and path
COMPRESS = libraries\AppCompressorBinExe\ApplicationCompressor
CREATE_FW_OBJECT = MakeMocf\CreateFwObject
APPEND_TO_MOCF = MakeMocf\AppendToMocf
APP_ENCRYPT = Tools\AppEncrypt
APP_7ZIP = Tools\7za

.IF $(APPTYPE) == BOOT
DIR_SOURCE = $(DIR_SOURCE_BOOT)
MATRIX_SOURCES = $(MATRIX_SOURCES_BOOT)
LIBS:=
TARGET_NAME = $(BOOTTARGET)
# Here are the compiler "--cpu" flag processor type for BOOT
TYPE_CPU = $(TYPE_CPU_BOOT)
# Here are the ASM compiler common pre defines for BOOT.
ASM_PREDEF =  --predefine "MATRIX_BOOT_CODE SETA 1"
ASM_PREDEF += $(ASM_PREDEF_EXTRA_BOOT)
# Here are the CPP compiler pre processor common defines for BOOT
CPP_DEF = -DARM=1 -DTAKEOUTDEBUG -DDLL=0 -DMATRIX_BOOT_CODE=1 -DCORSICA=1 -Dimx27=7 -Dimx25=5 -Dmxl=1
CPP_DEF += -DNDEBUG=1
CPP_DEF += $(CPP_DEF_EXTRA_BOOT)
# Here are the target specific CPP and C flags, should only set here -O for -Ospace or -Otime
TARGET_CPP_FLAGS = $(TARGET_CPP_FLAGS_BOOT)
# Here are the target specific Linker flags, should only set here --ro-base, --rw-base (if needed), and --first
TARGET_LINK_FLAGS = $(TARGET_LINK_FLAGS_BOOT) --inline --tailreorder

# Special modification of APPTYPE to process the next section for:
#   APPTYPE == APP  --OR--  APPTYPE=SAN_APP
.ELSIF $(APPTYPE:s/SAN_APP/APP/) == APP

DIR_SOURCE = $(DIR_SOURCE_APP)
MATRIX_SOURCES = $(MATRIX_SOURCES_APP)
TARGET_NAME = $(APPTARGET)
# Here are the compiler "--cpu" flag processor type for APP
TYPE_CPU = $(TYPE_CPU_APP)
# Here are the ASM compiler common pre defines for APP.
ASM_PREDEF += $(ASM_PREDEF_EXTRA_APP)
#Here are the CPP compiler pre processor common defines for APP
CPP_DEF =  -DARM=1  -DTAKEOUTDEBUG=1  -DDLL=0 -Dimx27=7 -Dimx25=5 -Dmxl=1
CPP_DEF += -DUNICODE_FOR_INTERFACES=1 -DMATRIX=1 -DTCC=0  -DCODE93B=0  -DCYBERANDY=0 -DCONCURRENT=0
#CPP_DEF += -DNDEBUG=1
# Here is define for Plugin
.IF $(PLUGINENA)==YES
CPP_DEF += -DPLUGIN_DEV=1
.END
CPP_DEF += $(CPP_DEF_EXTRA_APP)
# this is to define patametes at build time ie. -DEXTRA_DEFS="-DDUAL_LENS"
CPP_DEF += $(EXTRA_DEFS)
# Here are the target specific CPP and C flags, should only set here -O for -Ospace or -Otime
TARGET_CPP_FLAGS = $(TARGET_CPP_FLAGS_APP)
# Here are the target specific Linker flags for APP
TARGET_LINK_FLAGS = $(TARGET_LINK_FLAGS_APP)

.ELSIF $(APPTYPE) == LIB

# Any paths and target information must be set in the included makefile specified by LIB_MAKE
# This includes TARGET_NAME and OBJDIR

.ELSIF $(APPTYPE) == SANITIZE

doBuild := Sanitize
DIR_SOURCE = $(DIR_SOURCE_APP)
MATRIX_SOURCES = $(MATRIX_SOURCES_APP)
TARGET_NAME = $(APPTARGET)

.ELSE
doBuild := no
.END


.IF $(DEBUG_LIST)
DBG_FLAGS += --list
MOVE_LIST =  @IF EXIST "$(<:b).lst" move "$(<:b).lst" "$(OBJDIR)"
.END
.IF $(DBG_ASMOUT)
#DBG_FLAGS+= --asm --interleave
DBG_FLAGS+= --asm
.END



#==========================================================================================================================================
#     RVDS (Real View) toolChain definitions
#==========================================================================================================================================
# **************************** CPP FLAG HELP GUIDE:
# Command -c is run compiler not linker, -02 is high optimization, the default, and -0time is optimize for speed, -Ospace for size.
# Command --split_sections causes one ELF section per function.
# Command --debug enables generation of debug tables.
# Command --cpu= , we set cpu=ARM920T for the mxl platform, and cpu=ARM926EJ-S for the mx27.
# Command -W suppresses all warning messages.
# Command --md turns on .d dependency file output.
# Command "--depend filename" replaces --md above, plus instructs the assembler to save source file dependency lists to dependfile (.d).
# Commands " --asm --interleave " can be added in CPPFLAGS for assembler output (.txt) on c and cpp files (after cpu=).
# Command "--diag_style=ide" option specifies the style used to display diagnostic messages.
#    "ide" includes the line number and character count for any line that is in error.
# Command --no_depend_system_headers disables the output of system include dependency lines when generating make file dependency
#    information using either the -M option or the --md option.
# **************************** ASM FLAG HELP GUIDE:
# Command --keep instructs the assembler to keep local labels in the symbol table of the object file, for use by the debugger.
# Command --no_warn turns off warning messages.
# Command "--cpu name" sets the target CPU. "--fpu" selects the target floating-point unit (FPU) architecture, in this case, none.
# Command "--depend filename" instructs the assembler to save source file dependency lists to dependfile (.d).
# Command  --debug enables generation of debug tables (-g is the same with some compilers).
# Command  --list= file instructs the assembler to output a detailed listing of the assembly language produced by the assembler to file.
#    If no file is given, use --list= to send the output to inputfile.lst.
# Command --diag_warning=1546 gives warning "Stack pointer update potentially breaks 8 byte stack alignment", for require8, perserve8, use if required
# **************************** LINK FLAG HELP GUIDE:
# Command "--info totals" gives totals of the object code and data (RO Data, RW Data, ZI Data, and Debug Data) size for
#    each input object file.
# Command --ro-base sets address of the start of the read only output section.
# Command --map creates an image map.
# Command --no_locals instruct the linker not to add local symbols to the output symbol table.  *Do not use if using debuggger.*
# Command --symbols lists each local and global symbol used in the link step, and its value.
# Command --xref lists all cross-references between input sections.
# Command --callgraph creates a static callgraph of functions.
# Command --mangled instructs the linker to display mangled or unmangled C++ symbol names in diagnostic messages.
# Command --list redirects the diagnostics from output of the --info, --map, --symbols, --verbose, --xref, --xreffrom,
#    and --xrefto commands to file (.sym).
# Command "--first FileName(area name)", gives the location of the start of code in LFLAGS.
# Command "--datacompressor on|off" , is enabled by default, use "off" to turn off RW data compression.
# Command --no_debug to exclude debug information from the output file. The ELF image is smaller, but you cannot debug it at
#    the source level. The linker discards any debug input section it finds in the input objects and library members, and does
#    not include the symbol and string table in the image. This only affects the image size as loaded into the debugger. It has
#    no effect on the size of any resulting binary image that is downloaded to the target.  *Do not use if using debuggger.*
# Command --no_remove disables the removal of unused input sections from the image.  Use when debugging to retain all input
#    sections in the final image even if they are unused.  Use the --keep option to retain specific sections in a normal build.
#    *May want to use --no_remove if using debuggger.*
# Command --via tells the tool to load command line arguments/options from the given file.



.IF $(TOOLSET)==RVDS

#TOOLPATH := C:\Program Files\ARM\RVCT\Programs\3.1\569\win_32-pentium
# ARM 4.x toolchain:
TOOLPATH := C:\Program Files\ARM\RVCT\Programs\4.0\400\win_32-pentium

#Set the compiler macros for this tool chain

CPP_  = "$(TOOLPATH)/armcc"
CC_   = "$(TOOLPATH)/armcc"
ASM_  = "$(TOOLPATH)/armasm"
LINK_ = "$(TOOLPATH)/armlink"
AR_   = "$(TOOLPATH)/armar"  -ru
CPPFLAGS_ =  -c  -O2  $(TARGET_CPP_FLAGS)  --cpp  --feedback=$(FEDBACK_NAME) --debug  --cpu=$(TYPE_CPU) --apcs=/interwork  --depend=$(DEPEND_FILE)  --diag_style=ide  --no_depend_system_headers $(CPP_INCLUDE)
CFLAGS_ =   -c  -O2  $(TARGET_CPP_FLAGS)         --feedback=$(FEDBACK_NAME) --debug  --cpu=$(TYPE_CPU) --apcs=/interwork  --depend=$(DEPEND_FILE)  --diag_style=ide  --no_depend_system_headers $(CPP_INCLUDE)
ASMFLAGS_ = --debug  --list=$(ASM_LST)  --cpu=$(TYPE_CPU) --apcs=/interwork --fpu None  $(ASM_PREDEF) --depend $(DEPEND_FILE)  $(S_INCLUDE)
LFLAGS_ =  --diag_style=ide --info totals --info unused --map  --symbols  --xref  --callgraph  --mangled  --list $(LNK_LST) --feedback=$(FEDBACK_NAME) --datacompressor off $(TARGET_LINK_FLAGS)

# do not build if compiler was not chosen
.ELSE
doBuild := no
.END



#============================================================================================================================================================
#     Matrix project source file definitions
#============================================================================================================================================================

.IF $(doBuild) != Sanitize

.IF $(APPTYPE) != LIB
OBJDIR = Output_$(APPTYPE)_$(TARGET_NAME)
.END
LNK_LST = $(OBJDIR)/$(TARGET_NAME)_Map.lst
FEDBACK_NAME =  $(OBJDIR)/$(TARGET_NAME)_Unusedfuncs.fbf

# Set up the paths for assmbler list output files
ASM_LST = $(%:db:+".lst")


# Set up the paths to the depend files for the compiler and assembler source file name in OBJDIR with .d extension
DEPEND_FILE = $(OBJDIR)/$(@:b:+".d")

# Include file directory path string, add prefix(:^) -I(), then do a string substitution(:s) replacing + with spaces
S_INCLUDE := $(DIR_SOURCE:^"-I":s/+/ /)
CPP_INCLUDE := $(DIR_SOURCE:^"-I":s/+/ /)

# set OBJS to be the list of matrix sources, adding .o as a suffix and object directory name as a prefix
OBJS := $(MATRIX_SOURCES:+".o":^"$(OBJDIR)/")

# set DEPS to be the list of matrix sources, adding .d as a suffix and object directory name as a prefix
DEPS := $(MATRIX_SOURCES:+".d":^"$(OBJDIR)/")

# Set make directives to source file directory paths, then do a string substitution(s) replacing + with spaces
.SOURCE.c   : $(DIR_SOURCE:s/+/ /)
.SOURCE.cpp : $(DIR_SOURCE:s/+/ /)
.SOURCE.s   : $(DIR_SOURCE:s/+/ /)

.END


#=============================================================================
# Only build if all variables were set above
#=============================================================================
.IF $(doBuild) != no

#=============================================================================
# Create a header file with the git revision info for 'git describe':
#=============================================================================
CREATE_SVN_INFO:
	[
.IF $(APPTYPE) == BOOT
		MakeRevision.bat APP_NULL $(BOOTTARGET)
.ELSIF $(APPTYPE) == APP
		MakeRevision.bat $(APPTARGET) BOOT_NULL
.ELSE
		MakeRevision.bat APP_NULL BOOT_NULL
.END
	]

#=============================================================================
# Load alternate rules for Sanitizing
# or modify lib list for Sanitized App build
#=============================================================================
.IF $(APPTYPE:s/SANITIZE/YES/:s/SAN_APP/YES/) == YES
.INCLUDE : Mks_Sanitize_foundations.mk
.END

.IF $(APPTYPE) != SANITIZE

#=============================================================================
# Default rule: build libs and main app
#=============================================================================
#Rule checks to see if db_app.afx is up to date, and if not,
#will cause the meta-rule for prerequisite file db_app.afx to run next.

MATRIX : CREATE_SVN_INFO OUTPUT_FOLDERS $(OBJDIR)\db_app.sm



# This target builds the lib with all files except SPPTest.cpp & ProjectRevision.c for StoneStreet.
SS_RELEASE_DIR = $(OBJDIR)\Release
SS_INCLUDES_DIR = $(SS_RELEASE_DIR)\SS1_Includes
HON_INCLUDES_DIR = $(SS_RELEASE_DIR)\Hon_Includes
CORDLESS_DIR = FoundationApp\Cordless
MATRIX_LIB_SS : OUTPUT_FOLDERS $(OBJDIR)\db_app_SS.a
	[
		@IF NOT EXIST $(SS_RELEASE_DIR) @echo creating $(SS_RELEASE_DIR)  && md "$(SS_RELEASE_DIR)"
		@IF NOT EXIST $(SS_INCLUDES_DIR) @echo creating $(SS_INCLUDES_DIR)  && md "$(SS_INCLUDES_DIR)"
		@IF NOT EXIST $(HON_INCLUDES_DIR) @echo creating $(HON_INCLUDES_DIR)  && md "$(HON_INCLUDES_DIR)"

		copy $(CORDLESS_DIR)\MakeApp_StoneStreet.bat $(SS_RELEASE_DIR)
		copy $(OBJDIR)\db_app_SS.a $(SS_RELEASE_DIR)
		copy FoundationApp\ProjectRevision.c $(SS_RELEASE_DIR)

		copy $(CORDLESS_DIR)\SPPTest.cpp $(SS_RELEASE_DIR)
		copy $(CORDLESS_DIR)\BTStackLL.h $(HON_INCLUDES_DIR)
		copy $(CORDLESS_DIR)\SPPTest.h $(HON_INCLUDES_DIR)
		copy $(CORDLESS_DIR)\HHPBtapi.h $(HON_INCLUDES_DIR)
		copy $(CORDLESS_DIR)\btpskrnl\btpskrnl.h $(HON_INCLUDES_DIR)
		copy $(CORDLESS_DIR)\HCITRANS\HCITRANS.h $(HON_INCLUDES_DIR)

		copy FoundationApp\ProjectRevision.h $(HON_INCLUDES_DIR)
		copy libraries\OsKernel\db_shared\InterruptMacros.h $(HON_INCLUDES_DIR)

		copy libraries\BT_Stack\SS1_Library\Bluetopia.a $(SS_RELEASE_DIR)
		xcopy libraries\BT_Stack\SS1_Include $(SS_INCLUDES_DIR) /e

		copy makefile $(SS_RELEASE_DIR)
		copy Mks_project_target_foundations.mk $(SS_RELEASE_DIR)
		copy Mks_source_foundations.mk $(SS_RELEASE_DIR)

		@echo.
		@echo Done.
	]

# This builes the actual lib for MATRIX_LIB_SS target.
# TODO:  it would be nice if we used the APPTYPE=LIB instead of APPTYPE=APP.  That way we wouldn't duplicate this rule/recipe.
# TODO:  the resulting lib is large (over 30 MB) which means we can't email it.  An object file might be smaller or look into stripping symbols, etc. to reduce the size of the lib.
$(OBJDIR)\db_app_SS.a : $(OBJS) $(LIBS) $(LIBS_NOSOURCE)
	[
	   @$(AR_)  "$%" --via <+ $(OBJS) $(LIBS) $(LIBS_NOSOURCE) +>
	]


OUTPUT_FOLDERS:
	[
		@IF NOT EXIST $(TMPDIR) @echo creating $(TMPDIR)  && md "$(TMPDIR)"
		@echo making matrix code build output directories under  $(PWD)
		@IF NOT EXIST $(OBJDIR) @echo creating $(OBJDIR)  && md "$(OBJDIR)"
		@IF NOT EXIST "$(OBJDIR)/errors" @echo creating "$(OBJDIR)/errors"  && md "$(OBJDIR)/errors"
		@IF NOT EXIST "$(LIBDIR)" @echo creating $(LIBDIR)  && md "$(LIBDIR)"
		@IF NOT EXIST $(FEDBACK_NAME) ECHO ";Empty feedback file" > "$(FEDBACK_NAME)"
	]




#=============================================================================
# Build  db_app.axf if out of date. Also build raw and compressed bin files
#=============================================================================
# This Meta-rule calls the linker and then calls fromelf for the bin files.
# The target file is db_app.axf, and the prerequisets files are listed in OBJS,
# which is built from the source file dependency list, included at the top of this file.
# It will cause all the file-type specific Meta-rules above to be run, depending on the file types
# that are in the file list OBJS. (Note: '-' below is equiv. to .IGNORE)
# Because the large number of object files, we write the list to a file and pass this file to the linker (--via)
# To ensure our timestamped revision file gets built every time, it's handled separately.  However, not all projects
# will have this file, so that's why we see the .IF here.

%.axf : $(OBJS) $(LIBS) $(LIBS_NOSOURCE)
	[
		@echo off
		@echo $%
.IF $(RECOMPILE_ALWAYS:b)
.IF $(C_AS_CPP) == -dotciscxx
 		$(CPP_) $(CPPFLAGS_) $(CPP_DEF) -o "$(OBJDIR)/$(RECOMPILE_ALWAYS:b).o" "$(RECOMPILE_ALWAYS)"
.ELSE
 		$(CC_) $(CFLAGS_) $(CPP_DEF) -o "$(OBJDIR)/$(RECOMPILE_ALWAYS:b).o" "$(RECOMPILE_ALWAYS)"
.END
.END
		$(LINK_) $(LFLAGS_) --Output $%  --via <+ $(OBJS) $(LIBS) $(LIBS_NOSOURCE) +>
	]



.IF $(COMPRESS_BIN)
%.uncompressed.bin : %.axf
[
	"$(TOOLPATH)/fromelf"  -c  --output $%  --bin $^
]

%.bin : %.uncompressed.bin
[
	$(COMPRESS) $% $<  $(COMPRESS_BIN)
]

.ELSE
%.bin : %.axf
[
	"$(TOOLPATH)/fromelf"  -c  --output $%  --bin $^
]
.END

%.ebin : %.bin
[
	$(APP_ENCRYPT) -i $< -o $%
]

%.object : %.ebin
[
	$(CREATE_FW_OBJECT) -$(FW_OBJ_OPT) $< -c $(COMPAT_PROD_FILE) -s $(SW_VER_FILE) -o $%
]

%.moc : %.object
[
	$(APPEND_TO_MOCF) -m $% -t CompatProd -f $(COMPAT_PROD_FILE) -d
	$(APPEND_TO_MOCF) -m $% -t object -f $<
]

%.sm : %.moc
[   
	@echo ^> generate sm
	@echo off
	$(APP_7ZIP) a $% -pnM%%Z4WaP -mhe ./$< >nul
	@echo.
	@echo Done.
]

#=============================================================================
# Build an object file from a CPP file
#=============================================================================
#Meta-rule below searches for infered prerequisite .cpp file for update, if update needed,
#runs the PROLOG code to remove target .o file of same name,
#then runs recipe to build new target object file of same name ($% is target, $< is prerequisite .cpp).

$(OBJDIR)/%.o .PROLOG : %.cpp
 	[-
 		$(CPP_) $(CPPFLAGS_) $(CPP_DEF) $(DBG_FLAGS) -o "$%" "$<"
 		$(MOVE_LIST)
	]



#=============================================================================
# Build an object file from a C file
#=============================================================================
#Meta-rule below searches for infered prerequisite .c file for update, if update needed,
#runs the PROLOG code to remove target .o file of same name,
#then runs recipe to build new target object file of same name ($% is target, $< is prerequisite .c).
# Use cpp compiler if C_AS_CPP == -dotciscxx, other wise, build with C compiler.

$(OBJDIR)/%.o  .PROLOG : %.c
	[-
.IF $(C_AS_CPP) == -dotciscxx
 		$(CPP_) $(CPPFLAGS_) $(CPP_DEF) $(DBG_FLAGS) -o "$%" "$<"
.ELSE
 		$(CC_) $(CFLAGS_) $(CPP_DEF)  $(DBG_FLAGS) -o "$%" "$<"
.END
		$(MOVE_LIST)

	 ]



#=============================================================================
# Build an object file from a .S file
#=============================================================================
#Meta-rule below searches for infered prerequisite .s file for update, if update needed,
#runs the PROLOG code to remove target .o file of same name,
#then runs recipe to build new target object file of same name ($% is target, $< is prerequisite .o).

$(OBJDIR)/%.o .PROLOG: %.s
	[
		$(ASM_) $(ASMFLAGS_)  "$<"  -o "$%"
	]



.IF $(APPTYPE)==LIB
#=============================================================================
# Build rule for libraries as determined by dependancies specified in the
# included LIB_MAKE file..
#
# (Not used / needed for CLEAN operations.)
#=============================================================================

%.a : $(OBJS)
[
	@echo $%
	@IF EXIST "$%" del "$%"
	@$(AR_)  "$%" $(OBJS)
]
.ELSE  # for .IF $(APPTYPE)==LIB
# Build rule to build each each library is defined where the library is added to the LIBS macro.
# (They recurse into the makefile to do it in the proper directory.)
.END   #endof .IF $(APPTYPE)==LIB

# Rule to clean all libraries, invoking a separate rule to clean each, defined with the build rule.
CLEANLIBS : $(LIBS:b:+".a.clean")


# Lets include .d dependency files to see if header files have changed and we need to rebuild
.IF $(DEPS)
.INCLUDE .IGNORE : $(DEPS)
.END

.END  # endof IF doBuild == Sanitize


# Dummy target to use as dependancy to force always invoking a rule.  See $(OBJDIR)/$(TARGET_NAME)_build_opts.txt below.
FORCE :


#=============================================================================
# BUILD_OPTS special target to show macros at end of run in formatted text file.
#=============================================================================
BUILD_OPTS  : $(APPTYPE)_$(TARGET_NAME)_build_opts.txt

$(APPTYPE)_$(TARGET_NAME)_build_opts.txt  .PRECIOUS :  FORCE
	@echo>  $% ** Build Options: %DATE%  %TIME%
	@echo>> $% *
	@echo>> $% *   GROUPFLAGS:             $(GROUPFLAGS)
	@echo>> $% *   PATH:                   $(PATH)
	@echo>> $% *   TMPDIR:                 $(TMPDIR)
	@echo>> $% *   TOOLPATH:               $(TOOLPATH)
	@echo>> $% *   TOOLSET:                $(TOOLSET)
	@echo>> $% *   SHELL:                  $(SHELL)
	@echo>> $% *   SHELLFLAGS:             $(SHELLFLAGS)
	@echo>> $% *   SHELLMETAS:             $(SHELLMETAS)
	@echo>> $% *   SWITCHAR:               $(SWITCHAR)
	@echo>> $% *
#	@echo>> $% *   COOK_DEPENDS:           $(COOK_DEPENDS)   ## (this has a output redirect that keep this from listing correctly.)
	@echo>> $% *   DIR_SBOX_ROOT:          $(DIR_SBOX_ROOT)
	@echo>> $% *   LIBDIR:                 $(LIBDIR)
	@echo>> $% *   LIB_MAKE:               $(LIB_MAKE)
	@echo>> $% *   PROJROOT:               $(PROJROOT)
	@echo>> $% *
	@echo>> $% *   APPTYPE:                $(APPTYPE)
	@echo>> $% *   APPTARGET:              $(APPTARGET)
	@echo>> $% *   BOOTTARGET:             $(BOOTTARGET)
	@echo>> $% *   C_AS_CPP:               $(C_AS_CPP)
	@echo>> $% *   COMPAT_PROD_FILE:       $(COMPAT_PROD_FILE)
	@echo>> $% *   FW_TYPE:                $(FW_TYPE)
	@echo>> $% *   FW_OBJ_OPT:             $(FW_OBJ_OPT)
	@echo>> $% *   PLUGINENA:              $(PLUGINENA)
	@echo>> $% *   PRODUCT_TYPE:           $(PRODUCT_TYPE)
	@echo>> $% *   SANITIZE_DATAEDIT:      $(SANITIZE_DATAEDIT)
	@echo>> $% *   SW_VER_FILE:            $(SW_VER_FILE)
	@echo>> $% *
	@echo>> $% *
	@echo>> $% *   LIBS:                   $(LIBS)
	@echo>> $% *
	@echo>> $% *   LIBS_NOSOURCE:          $(LIBS_NOSOURCE)
	@echo>> $% *
	@echo>> $% *
	@echo>> $% *   DIR_SOURCE_APP:         $(DIR_SOURCE_APP)
	@echo>> $% *
	@echo>> $% *   DIR_SOURCE_BOOT:        $(DIR_SOURCE_BOOT)
	@echo>> $% *
	@echo>> $% *   DIR_SOURCE:             $(DIR_SOURCE)
	@echo>> $% *
	@echo>> $% *
	@echo>> $% *   MATRIX_SOURCES_APP:     $(MATRIX_SOURCES_APP)
	@echo>> $% *
	@echo>> $% *   MATRIX_SOURCES_BOOT:    $(MATRIX_SOURCES_BOOT)
	@echo>> $% *
	@echo>> $% *   MATRIX_SOURCES_RF:      $(MATRIX_SOURCES_RF)
	@echo>> $% *
	@echo>> $% *   MATRIX_SOURCES:         $(MATRIX_SOURCES)
	@echo>> $% *
	@echo>> $% *   CONFIG_SOURCES:         $(CONFIG_SOURCES)
	@echo>> $% *
	@echo>> $% *   ECI_SOURCES:            $(ECI_SOURCES)
	@echo>> $% *
	@echo>> $% *   FLASH_SOURCES:          $(FLASH_SOURCES)
	@echo>> $% *
	@echo>> $% *   HHPL_SOURCES:           $(HHPL_SOURCES)
	@echo>> $% *
	@echo>> $% *   IF4683_SOURCES:         $(IF4683_SOURCES)
	@echo>> $% *
	@echo>> $% *   MISC_SOURCES:           $(MISC_SOURCES)
	@echo>> $% *
	@echo>> $% *   PLUGIN_SOURCES:         $(PLUGIN_SOURCES)
	@echo>> $% *
	@echo>> $% *   SD_SOURCES:             $(SD_SOURCES)
	@echo>> $% *
	@echo>> $% *   USB2_SOURCES:           $(USB2_SOURCES)
	@echo>> $% *
	@echo>> $% *   WEDGE_SOURCES:          $(WEDGE_SOURCES)
	@echo>> $% *
	@echo>> $% *   OBJDIR:                 $(OBJDIR)
	@echo>> $% *
	@echo>> $% *   APP_OBJDIR:             $(APP_OBJDIR)
	@echo>> $% *
	@echo>> $% *   NEW_OBJDIR:             $(NEW_OBJDIR)
	@echo>> $% *
	@echo>> $% *   PREBUILT_DIR:           $(PREBUILT_DIR)
	@echo>> $% *
	@echo>> $% *   COPIED_LIBS:            $(COPIED_LIBS)
	@echo>> $% *
	@echo>> $% *   NEEDED_OBJECTS:         $(NEEDED_OBJECTS)
	@echo>> $% *
#	@echo>> $% *   DEPS:                   $(DEPS)			## variable too long to display...
#	@echo>> $% *
#	@echo>> $% *   OBJS:                   $(OBJS)			## variable too long to display...
#	@echo>> $% *
	@echo>> $% *   TARGET_NAME:            $(TARGET_NAME)
	@echo>> $% *
	@echo>> $% *   ASMFLAGS_:              $(ASMFLAGS_)
	@echo>> $% *   ASM_LST:                $(ASM_LST)
	@echo>> $% *
	@echo>> $% *   ASM_PREDEF:             $(ASM_PREDEF)
	@echo>> $% *   ASM_PREDEF_EXTRA_APP:   $(ASM_PREDEF_EXTRA_APP)
	@echo>> $% *   ASM_PREDEF_EXTRA_BOOT:  $(ASM_PREDEF_EXTRA_BOOT)
	@echo>> $% *
	@echo>> $% *   CFLAGS_:                $(CFLAGS_)
	@echo>> $% *   CPPFLAGS_:              $(CPPFLAGS_)
	@echo>> $% *
	@echo>> $% *   CPP_DEF:                $(CPP_DEF)
	@echo>> $% *   CPP_DEF_EXTRA_APP:      $(CPP_DEF_EXTRA_APP)
	@echo>> $% *   CPP_DEF_EXTRA_BOOT:     $(CPP_DEF_EXTRA_BOOT)
	@echo>> $% *
	@echo>> $% *   CPP_INCLUDE:            $(CPP_INCLUDE)
	@echo>> $% *   DBG_FLAGS:              $(DBG_FLAGS)
	@echo>> $% *   DEBUG_LIST:             $(DEBUG_LIST)
	@echo>> $% *   DEPEND_FILE:            $(DEPEND_FILE)
	@echo>> $% *   FEDBACK_NAME:           $(FEDBACK_NAME)
	@echo>> $% *   LFLAGS_:                $(LFLAGS_)
	@echo>> $% *   LFLAGS_PLO:             $(LFLAGS_PLO)
	@echo>> $% *   LNK_LST:                $(LNK_LST)
	@echo>> $% *   MOVE_LIST:              $(MOVE_LIST)
	@echo>> $% *
	@echo>> $% *   TARGET_CPP_FLAGS:       $(TARGET_CPP_FLAGS)
	@echo>> $% *   TARGET_CPP_FLAGS_APP:   $(TARGET_CPP_FLAGS_APP)
	@echo>> $% *   TARGET_CPP_FLAGS_BOOT:  $(TARGET_CPP_FLAGS_BOOT)
	@echo>> $% *
	@echo>> $% *   TARGET_LINK_FLAGS:      $(TARGET_LINK_FLAGS)
	@echo>> $% *   TARGET_LINK_FLAGS_APP:  $(TARGET_LINK_FLAGS_APP)
	@echo>> $% *   TARGET_LINK_FLAGS_BOOT: $(TARGET_LINK_FLAGS_BOOT)
	@echo>> $% *
	@echo>> $% *   TYPE_CPU:               $(TYPE_CPU)
	@echo>> $% *   TYPE_CPU_APP:           $(TYPE_CPU_APP)
	@echo>> $% *   TYPE_CPU_BOOT:          $(TYPE_CPU_BOOT)


#=============================================================================
# .PROLOG deletes target file in the object file directory
#=============================================================================
.GROUPPROLOG :
	[
		@echo $%
		@echo off
		@IF EXIST  $(OBJDIR)\$(%:f) del  $(OBJDIR)\$(%:f)
	]




#=============================================================================
# File Clean make rules
#=============================================================================

CLEANALL :  CLEAN CLEANLIBS


#=============================================================================
# Remove files from the sand box root directory ( *.d, *.lst only )
#=============================================================================
CLEAND:
	[
		# NOTE: $(PWD)gives you the current directory name
		# Note - /Q option means no prompt for delete

		@echo Cleaning depenency files from $(PWD)

		@IF EXIST *.d del /Q *.d
		@IF EXIST *.lst del /Q *.lst
	 ]



#=============================================================================
# Remove files from object directory
#=============================================================================
# Remove All *.o files plus all other files in object directory.
# Note that the PREBUILT directory for APPTYPE == SANITIZE is cleaned by
# the CLEANLIBS rule in Mks_Sanitize_foundations.mk.

CLEAN:
	[
		# NOTE: $(PWD)gives you the current directory name
		# Note - /Q option means no prompt for delete
		@echo \# Cleaning build directory: $(PWD)\$(OBJDIR)
		@IF EXIST $(OBJDIR)\*.a      del $(OBJDIR)\*.a
		@IF EXIST $(OBJDIR)\*.axf    del $(OBJDIR)\*.axf
		@IF EXIST $(OBJDIR)\*.d      del $(OBJDIR)\*.d
		@IF EXIST $(OBJDIR)\*.d1     del $(OBJDIR)\*.d1
		@IF EXIST $(OBJDIR)\*.lst    del $(OBJDIR)\*.lst
		@IF EXIST $(OBJDIR)\*.o      del $(OBJDIR)\*.o
		@IF EXIST $(OBJDIR)\*.ti     del $(OBJDIR)\*.ti
		@IF EXIST $(OBJDIR)\*.txt    del $(OBJDIR)\*.txt
		@IF EXIST $(OBJDIR)\*.bin    del $(OBJDIR)\*.bin
		@IF EXIST $(OBJDIR)\*.ebin   del $(OBJDIR)\*.ebin
		@IF EXIST $(OBJDIR)\*.object del $(OBJDIR)\*.object
		@IF EXIST $(OBJDIR)\*.moc    del $(OBJDIR)\*.moc
		@IF EXIST $(OBJDIR)\*.sm    del $(OBJDIR)\*.sm
.IF $(APPTYPE) != SANITIZE
		@IF EXIST $(OBJDIR)\errors\*.err del $(OBJDIR)\errors\*.err
.END
	 ]

#=============================================================================
.END  #endof IF doBuild != no

