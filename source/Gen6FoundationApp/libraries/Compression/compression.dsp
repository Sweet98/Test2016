# Microsoft Developer Studio Project File - Name="Compression" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Compression - Win32 Debug slib
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Compression.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Compression.mak" CFG="Compression - Win32 Debug slib"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Compression - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Compression - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Compression - Win32 Debug slib" (based on "Win32 (x86) Static Library")
!MESSAGE "Compression - Win32 Release slib" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Compression - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Shared" /I "..\StringAndData" /I "..\JPEG" /I "..\ZLIBUT" /I "..\LZO" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
TargetDir=.\Release
TargetName=Compression
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Compression - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "..\StringAndData" /I "..\JPEG" /I "..\ZLIBUT" /I "..\LZO" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\Compressiond.lib"
# Begin Special Build Tool
TargetDir=.\Debug
TargetName=Compressiond
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Compression - Win32 Debug slib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Compression___Win32_Debug_slib"
# PROP BASE Intermediate_Dir "Compression___Win32_Debug_slib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_slib"
# PROP Intermediate_Dir "Debug_slib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "..\StringAndData" /I "..\JPEG" /I "..\ZLIBUT" /I "..\LZO" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "..\StringAndData" /I "..\JPEG" /I "..\ZLIBUT" /I "..\LZO" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DLL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug_slib/Compression_sd.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\Compressiond.lib"
# ADD LIB32 /nologo /out:"Debug_slib\Compression_sd.lib"
# Begin Special Build Tool
TargetDir=.\Debug_slib
TargetName=Compression_sd
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib	copy   $(TargetDir)\$(TargetName).bsc ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Compression - Win32 Release slib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Compression___Win32_Release_slib"
# PROP BASE Intermediate_Dir "Compression___Win32_Release_slib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_slib"
# PROP Intermediate_Dir "Release_slib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\Shared" /I "..\StringAndData" /I "..\JPEG" /I "..\ZLIBUT" /I "..\LZO" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Shared" /I "..\StringAndData" /I "..\JPEG" /I "..\ZLIBUT" /I "..\LZO" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release_slib\Compression_s.lib"
# Begin Special Build Tool
TargetDir=.\Release_slib
TargetName=Compression_s
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Compression - Win32 Release"
# Name "Compression - Win32 Debug"
# Name "Compression - Win32 Debug slib"
# Name "Compression - Win32 Release slib"
# Begin Group "Compression Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\compression.cpp
# End Source File
# End Group
# Begin Group "Compression Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\compression.h
# End Source File
# Begin Source File

SOURCE=.\HHPCompressionConfig.h
# End Source File
# End Group
# Begin Group "JPEG Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Jpeg\Jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jccolor.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jchuff.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcinit.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcparam.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jcsample.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jctrans.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdinput.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdsample.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jerror.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jidctint.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jidctred.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jmemnobs.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jquant1.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jquant2.c
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jutils.c
# End Source File
# End Group
# Begin Group "JPEG Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Jpeg\Jchuff.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jconfig.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdct.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jdhuff.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jerror.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jinclude.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jmemsys.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jmorecfg.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jpegint.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jpeglib.h
# End Source File
# Begin Source File

SOURCE=..\Jpeg\Jversion.h
# End Source File
# End Group
# Begin Group "LZO Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\LZO\lzo1_d.ch
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x_1.c
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x_9x.c
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x_c.ch
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x_d.ch
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x_d1.c
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x_o.c
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x_oo.ch
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_init.c
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_mchw.ch
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_ptr.c
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_str.c
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_swd.ch
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_util.c
# End Source File
# End Group
# Begin Group "LZO Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\LZO\config1x.h
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo1x.h
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_conf.h
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_dict.h
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_ptr.h
# End Source File
# Begin Source File

SOURCE=..\LZO\lzo_util.h
# End Source File
# Begin Source File

SOURCE=..\LZO\lzoconf.h
# End Source File
# End Group
# Begin Group "ZLIB Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\zlibut\adler32.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\deflate.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\infblock.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\inffast.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\inflate.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\infutil.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\trees.c
# End Source File
# Begin Source File

SOURCE=..\zlibut\zutil.c
# End Source File
# End Group
# Begin Group "ZLIB Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\zlibut\deflate.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\infblock.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\infcodes.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\inffast.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\infutil.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\trees.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zlibut\ZLIB.H
# End Source File
# Begin Source File

SOURCE=..\zlibut\zutil.h
# End Source File
# End Group
# End Target
# End Project
