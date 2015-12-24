REM $Source: Gen6FoundationApp/MakeMocf/SUFFileCreation/MakeCordedSysUpFile.bat $
REM $Date: 2011/05/11 09:32:53EDT $
REM $Revision: 1.3 $

REM
REM Will build This bat file will build all of the standard release SUF files for the Corded Build. 
REM It will also output the MD5 Sums into a text file.
REM First set all of the global variables and copy the needed files into 
REM   the same folder as this bat, these files should be copied from the released directory.
REM
REM To use:  MakeCordedSysUpFile.bat
REM
REM 04/15/2011

REM begin localisation of Environment Variables
setlocal

@ECHO OFF

@set RELEASE_DIR=..
@set CREATE_FW_OBJ=%RELEASE_DIR%\CreateFwObject.exe
@set APPEND_MOCF=%RELEASE_DIR%\AppendToMocf_suf.exe
@set UNPACK_MOCF=%RELEASE_DIR%\UnpackMocf.exe

::Name of the output file
@set FILE_CRC_OUTPUT=CordedSufFileChecksum.txt 

::******Global Sets******
@set BOOT_PART=BF000006AAD
@set APP_PART=BI000075AAA
@set MENU_SETTINGS=BF000015AAE.txt

::******2D SUF***********
@set ST2D_SUF_PART=BI000089AAC

::******OCR SUF***********
@set OCR_SUF_PART=BI000090AAC
@set OCR_SW_KEYS=BF000006AAG.txt

::******Linear Only SUF******
@set LONLY_SUF_PART=BI000092AAC
@set LONLY_SW_KEYS=BF000010AAG.txt



@set APP_MOCF=%APP_PART%.moc
@set BOOT_MOCF=%BOOT_PART%.moc

@set BOOT_OBJ=Objects\Gen6BootFw-1.object

echo Extract Boot Firmware ojbect
%UNPACK_MOCF% -f %BOOT_MOCF%

echo %APP_PART%.moc >> %FILE_CRC_OUTPUT%
call FileCRC.exe %APP_PART%.moc >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

echo Create System Update MOCF 2D
set SUF=%ST2D_SUF_PART%.suf
copy /Y %APP_MOCF% %SUF%
%APPEND_MOCF% -m %SUF% -t object -f %BOOT_OBJ%
%APPEND_MOCF% -m %SUF% --type MenuSettings -f %MENU_SETTINGS%
%APPEND_MOCF% -m %SUF% --type SwKeys --PartNum %ST2D_SUF_PART%
echo %SUF% >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SUF% >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

echo Create System Update MOCF WITH SW Keys file OCR
set SUF=%OCR_SUF_PART%.suf
copy /Y %APP_MOCF% %SUF%
%APPEND_MOCF% -m %SUF% -t object -f %BOOT_OBJ%
%APPEND_MOCF% -m %SUF% --type MenuSettings -f %MENU_SETTINGS%
%APPEND_MOCF% -m %SUF% --type SwKeys -f %OCR_SW_KEYS% --PartNum %OCR_SUF_PART%
echo %SUF% >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SUF% >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

echo Create System Update MOCF For Linear Only 
set SUF=%LONLY_SUF_PART%.suf
copy /Y %APP_MOCF% %SUF%
%APPEND_MOCF% -m %SUF% -t object -f %BOOT_OBJ%
%APPEND_MOCF% -m %SUF% --type MenuSettings -f %MENU_SETTINGS%
%APPEND_MOCF% -m %SUF% --type SwKeys -f %LONLY_SW_KEYS% --PartNum %LONLY_SUF_PART%
echo %SUF% >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SUF% >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

@ECHO ON
