REM This batch file is used to create a Matrix app for StoneStreet One.  It creates the appropriate RF scanner
REM firmware for their hardware.
REM
REM  $Source: Gen6FoundationApp/MakeApp_StoneStreet.bat $
REM  $Revision: 1.2 $


set TOOLSET=RVDS
set DIR_SBOX_ROOT=%cd%

REM *** you may need to change this path to MKS make ***
set MKSMAKE=".\tools\make"



%MKSMAKE% -r  -DAPPTYPE=APP -DBOOTTARGET=imx25_build -DAPPTARGET=imx25_RF_Scan
