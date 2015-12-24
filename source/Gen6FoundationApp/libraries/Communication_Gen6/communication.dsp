# Microsoft Developer Studio Project File - Name="Communication" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Communication - Win32 Debug slib
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Communication.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Communication.mak" CFG="Communication - Win32 Debug slib"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Communication - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Communication - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Communication - Win32 Release slib" (based on "Win32 (x86) Static Library")
!MESSAGE "Communication - Win32 Debug slib" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Communication - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /c
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
TargetName=Communication
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Communication - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\Communicationd.lib"
# Begin Special Build Tool
TargetDir=.\Debug
TargetName=Communicationd
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Communication - Win32 Release slib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Communication___Win32_Release_slib"
# PROP BASE Intermediate_Dir "Communication___Win32_Release_slib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_slib"
# PROP Intermediate_Dir "Release_slib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DLL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release_slib\Communication_s.lib"
# Begin Special Build Tool
TargetDir=.\Release_slib
TargetName=Communication_s
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Communication - Win32 Debug slib"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Communication___Win32_Debug_slib"
# PROP BASE Intermediate_Dir "Communication___Win32_Debug_slib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_slib"
# PROP Intermediate_Dir "Debug_slib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "DLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Shared" /I "..\Compression" /I "..\StringAndData" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DLL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\Communicationd.lib"
# ADD LIB32 /nologo /out:"Debug_slib\Communication_sd.lib"
# Begin Special Build Tool
TargetDir=.\Debug_slib
TargetName=Communication_sd
SOURCE="$(InputPath)"
PostBuild_Cmds=copy   $(TargetDir)\$(TargetName).lib ..\..\lib	copy   $(TargetDir)\$(TargetName).bsc ..\..\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Communication - Win32 Release"
# Name "Communication - Win32 Debug"
# Name "Communication - Win32 Release slib"
# Name "Communication - Win32 Debug slib"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\HComm.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HCommandHandler.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HexFile.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\HModem.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HModemTransmitReceive.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\HTag.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HTagData.cpp
# End Source File
# Begin Source File

SOURCE=.\HTagDataResult.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HTagFile.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HTagImage.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HTagReaderCode.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HTagScript.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HTagVerifier.cpp
# End Source File
# Begin Source File

SOURCE=.\HTransferProcessing.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\HTransferStatus.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# Begin Source File

SOURCE=.\SynFECommandHandler.cpp
# ADD CPP /I "..\Configuration"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\HComm.h
# End Source File
# Begin Source File

SOURCE=.\HCommandHandler.h
# End Source File
# Begin Source File

SOURCE=.\HexFile.h
# End Source File
# Begin Source File

SOURCE=.\HHPCommunicationConfig.h
# End Source File
# Begin Source File

SOURCE=.\HModem.h
# End Source File
# Begin Source File

SOURCE=.\HModemTransmitReceive.h
# End Source File
# Begin Source File

SOURCE=.\HTag.h
# End Source File
# Begin Source File

SOURCE=.\HTagData.h
# End Source File
# Begin Source File

SOURCE=.\HTagDataResult.h
# End Source File
# Begin Source File

SOURCE=.\HTagFile.h
# End Source File
# Begin Source File

SOURCE=.\HTagImage.h
# End Source File
# Begin Source File

SOURCE=.\HTagReaderCode.h
# End Source File
# Begin Source File

SOURCE=.\HTagScript.h
# End Source File
# Begin Source File

SOURCE=.\HTagVerifier.h
# End Source File
# Begin Source File

SOURCE=.\HTransferProcessing.h
# End Source File
# Begin Source File

SOURCE=.\HTransferStatus.h
# End Source File
# Begin Source File

SOURCE=.\SynFECommandHandler.h
# End Source File
# End Group
# End Target
# End Project
