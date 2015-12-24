# Microsoft Developer Studio Project File - Name="StringAndData" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=StringAndData - Win32 Debug slib
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stringanddata.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stringanddata.mak" CFG="StringAndData - Win32 Debug slib"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StringAndData - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "StringAndData - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "StringAndData - Win32 Release slib" (based on "Win32 (x86) Static Library")
!MESSAGE "StringAndData - Win32 Debug slib" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "StringAndData - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /c
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
TargetName=stringanddata
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "StringAndData - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\StringAndDatad.lib"
# Begin Special Build Tool
TargetDir=.\Debug
TargetName=StringAndDatad
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "StringAndData - Win32 Release slib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "StringAndData___Win32_Release_slib"
# PROP BASE Intermediate_Dir "StringAndData___Win32_Release_slib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_slib"
# PROP Intermediate_Dir "Release_slib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release_slib\StringAndData_s.lib"
# Begin Special Build Tool
TargetDir=.\Release_slib
TargetName=StringAndData_s
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "StringAndData - Win32 Debug slib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "StringAndData___Win32_Debug_slib"
# PROP BASE Intermediate_Dir "StringAndData___Win32_Debug_slib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_slib"
# PROP Intermediate_Dir "Debug_slib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DLL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug_slib/StringAndData_sd.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\StringAndDatad.lib"
# ADD LIB32 /nologo /out:"Debug_slib\StringAndData_sd.lib"
# Begin Special Build Tool
TargetDir=.\Debug_slib
TargetName=StringAndData_sd
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib	copy   $(TargetDir)\$(TargetName).bsc ..\..\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "StringAndData - Win32 Release"
# Name "StringAndData - Win32 Debug"
# Name "StringAndData - Win32 Release slib"
# Name "StringAndData - Win32 Debug slib"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bitstring.cpp
# End Source File
# Begin Source File

SOURCE=.\CDataFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckSum.cpp
# End Source File
# Begin Source File

SOURCE=.\Crc.cpp
# End Source File
# Begin Source File

SOURCE=.\HString.cpp
# End Source File
# Begin Source File

SOURCE=.\HWString.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.c
# End Source File
# Begin Source File

SOURCE=.\WCDataFile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bitstring.h
# End Source File
# Begin Source File

SOURCE=.\CDataFile.h
# End Source File
# Begin Source File

SOURCE=.\CheckSum.H
# End Source File
# Begin Source File

SOURCE=.\Crc.h
# End Source File
# Begin Source File

SOURCE=.\DataArray.h
# End Source File
# Begin Source File

SOURCE=.\Hstring.h
# End Source File
# Begin Source File

SOURCE=.\HStringArray.h
# End Source File
# Begin Source File

SOURCE=.\HWString.h
# End Source File
# Begin Source File

SOURCE=.\HWStringArray.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\WCDataFile.h
# End Source File
# End Group
# End Target
# End Project
