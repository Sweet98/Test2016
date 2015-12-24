# Microsoft Developer Studio Project File - Name="Imaging" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Imaging - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Imaging.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Imaging.mak" CFG="Imaging - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Imaging - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Imaging - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Imaging - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /I "..\JPEG" /I "..\Communication" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /c
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
TargetName=Imaging
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Imaging - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /I "..\JPEG" /I "..\Communication" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\Imagingd.lib"
# Begin Special Build Tool
TargetDir=.\Debug
TargetName=Imagingd
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Imaging - Win32 Release"
# Name "Imaging - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BmpImage.cpp
# End Source File
# Begin Source File

SOURCE=.\HImage.cpp
# End Source File
# Begin Source File

SOURCE=.\KimImage.cpp
# End Source File
# Begin Source File

SOURCE=.\OutlinedImage.cpp
# End Source File
# Begin Source File

SOURCE=.\TIFF_Fax.cpp
# End Source File
# Begin Source File

SOURCE=.\TIFF_Swab.cpp
# End Source File
# Begin Source File

SOURCE=.\TiffImage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BmpImage.h
# End Source File
# Begin Source File

SOURCE=.\HHPImagingConfig.h
# End Source File
# Begin Source File

SOURCE=.\HImage.h
# End Source File
# Begin Source File

SOURCE=.\KimImage.h
# End Source File
# Begin Source File

SOURCE=.\OutlinedImage.h
# End Source File
# Begin Source File

SOURCE=.\TIFF_CSB.h
# End Source File
# Begin Source File

SOURCE=.\TIFF_Fax.h
# End Source File
# Begin Source File

SOURCE=.\TIFF_FaxPriv.h
# End Source File
# Begin Source File

SOURCE=.\TIFF_G3States.h
# End Source File
# Begin Source File

SOURCE=.\TIFF_Swab.h
# End Source File
# Begin Source File

SOURCE=.\TIFF_T4.h
# End Source File
# Begin Source File

SOURCE=.\TIFF_TiffCon.hpp
# End Source File
# Begin Source File

SOURCE=.\TiffImage.h
# End Source File
# End Group
# End Target
# End Project
