REM Creates a Klocwork build spec for the MATRIX make target.
REM
REM $Source: Gen6FoundationApp/Klocwork/KW.bat $
REM $Date: 2010/01/20 09:43:03EST $
REM $Revision: 1.2 $

REM
set MAKE=".\tools\make"
set OUTDIR=..\..\Klocwork
set OUTFILE=%OUTDIR%\BuildSpec

REM used to set this include so kwinject below pulls correct include path for compiler below.
set ARMINC=C:\Program Files\\arm\rvct\data\4.0\400\include\windows

if "%1" EQU "" goto :Help

REM clean out the files so all files are built. Buildspec creating needs to build all the files
%MAKE% -r -c ..\.. -DAPPTYPE=APP -DBOOTTARGET=imx25_build -DAPPTARGET=%1 CLEANALL

REM generate the buildspec for MATRIX target
kwinject -o %OUTFILE%_raw.out %MAKE% -r -c ..\.. -DAPPTYPE=APP -DBOOTTARGET=imx25_build -DAPPTARGET=%1 MATRIX > %OUTFILE%.log

REM add in some defines for RVDS that get rid of parse errors in the klocwork tool.
..\..\sed -e 's/;arm;/;arm;-D__restrict=;-D__asm;/' %OUTFILE%_raw.out > %OUTFILE%.out

REM delete the klockworstuff in the workspace as we generated a new buildspec because we added file in the make
REM or switch to a different target. 
rmdir /S /Q %WORKSPACE_LOC%\.metadata\.plugins\org.eclipse.core.resources\.projects\%PROJECT_NAME%\com.klocwork.inforceeclipse\

goto :End

:Help
echo To run:  KW.bat AppTargetName
:End
