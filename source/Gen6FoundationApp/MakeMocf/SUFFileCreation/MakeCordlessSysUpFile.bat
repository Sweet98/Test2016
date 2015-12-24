REM $Source: Gen6FoundationApp/MakeMocf/SUFFileCreation/MakeCordlessSysUpFile.bat $
REM $Date: 2011/05/11 09:32:53EDT $
REM $Revision: 1.2 $

REM
REM Will build This bat file will build all of the standard cordless release SUF files. 
REM It will also output the MD5 Sums into a text file.
REM First set all of the global variables and copy the needed files into 
REM   the same folder as this bat, these files should be copied from the released directory.
REM
REM To use:  MakeCordlessSysUpFile.bat
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
@set FILE_CRC_OUTPUT=CordlessSufFileChecksum.txt

::******Global Sets******
@set BOOT_PART=BF000006AAD
@set BASE_APP_PART=BK000034AAA
@set SCANNER_APP_PART=BJ000032AAA
@set MENU_SETTINGS=BF000015AAE.txt


::******2D SUF***********
@set BASE_ST2D_SUF_PART=BK000037AAC
@set SCANNER_ST2D_SUF_PART=BJ000036AAC

::******OCR SUF***********
@set SCANNER_OCR_SUF_PART=BJ000037AAC
@set OCR_SW_KEYS=BF000006AAG.txt

::******Linear Only SUF******
@set SCANNER_LONLY_SUF_PART=BJ000038AAC
@set LONLY_SW_KEYS=BF000010AAG.txt




@set SCANNER_APP_MOCF=%SCANNER_APP_PART%.moc
@set BASE_APP_MOCF=%BASE_APP_PART%.moc
@set BOOT_MOCF=%BOOT_PART%.moc

@set BOOT_OBJ=Objects\Gen6BootFw-1.object

echo Extract Boot Firmware ojbect
%UNPACK_MOCF% -f %BOOT_MOCF%
REN %cd%\Objects\Gen6BootFw-1.object Gen6BootFw.object

echo %SCANNER_APP_PART%.moc >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SCANNER_APP_PART%.moc >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

echo %BASE_APP_PART%.moc >> %FILE_CRC_OUTPUT%
call FileCRC.exe %BASE_APP_PART%.moc >> %FILE_CRC_OUTPUT%
echo " " >> %FILE_CRC_OUTPUT%

echo Create System Update MOCF BASE 2D
set SUF=%BASE_ST2D_SUF_PART%.suf
copy /Y %BASE_APP_MOCF% %SUF%
%APPEND_MOCF% -m %SUF% -t object -f %BOOT_OBJ%
%APPEND_MOCF% -m %SUF% --type MenuSettings -f %MENU_SETTINGS%
%APPEND_MOCF% -m %SUF% --type SwKeys --PartNum %BASE_ST2D_SUF_PART%
echo %SUF% >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SUF% >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

echo Create System Update MOCF SCANNER 2D
set SUF=%SCANNER_ST2D_SUF_PART%.suf
copy /Y %SCANNER_APP_MOCF% %SUF%
%APPEND_MOCF% -m %SUF% -t object -f %BOOT_OBJ%
%APPEND_MOCF% -m %SUF% --type MenuSettings -f %MENU_SETTINGS%
%APPEND_MOCF% -m %SUF% --type SwKeys --PartNum %SCANNER_ST2D_SUF_PART%
echo %SUF% >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SUF% >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

echo Create System Update MOCF WITH SW Keys file OCR
set SUF=%SCANNER_OCR_SUF_PART%.suf
copy /Y %SCANNER_APP_MOCF% %SUF%
%APPEND_MOCF% -m %SUF% -t object -f %BOOT_OBJ%
%APPEND_MOCF% -m %SUF% --type MenuSettings -f %MENU_SETTINGS%
%APPEND_MOCF% -m %SUF% --type SwKeys -f %OCR_SW_KEYS% --PartNum %SCANNER_OCR_SUF_PART%
echo %SUF% >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SUF% >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

echo Create System Update MOCF For Linear Only 
set SUF=%SCANNER_LONLY_SUF_PART%.suf
copy /Y %SCANNER_APP_MOCF% %SUF%
%APPEND_MOCF% -m %SUF% -t object -f %BOOT_OBJ%
%APPEND_MOCF% -m %SUF% --type MenuSettings -f %MENU_SETTINGS%
%APPEND_MOCF% -m %SUF% --type SwKeys -f %LONLY_SW_KEYS% --PartNum %SCANNER_LONLY_SUF_PART%
echo %SUF% >> %FILE_CRC_OUTPUT%
call FileCRC.exe %SUF% >> %FILE_CRC_OUTPUT%
echo .>> %FILE_CRC_OUTPUT%

@ECHO ON
