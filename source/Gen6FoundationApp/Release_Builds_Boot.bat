REM begin localisation of Environment Variables
setlocal

@if not "%1" == "" @set Version=%1
@if /I "%2" == "-noboot"	@echo ## Warning - No boot code will be built!!

@set OUTPUT_BIN=db_app.bin
@set OUTPUT_MOCF=db_app.moc

@ECHO OFF
BREAK=ON
set StartTime=%TIME%

REM move and rename the bin files into the new 'released' directory
set cwd=%cd%

RD /Q /S "Released"
md "Released"

@if /I "%2" == "-noboot"	       Goto  :end

::BOOT HI2D
  CALL MKS_clean_foundations_gen6.bat BOOT imx25_5100
  CALL MKS_make_foundations_gen6.bat BOOT imx25_5100 %1
  CALL MKS_clean_foundations_gen6.bat BOOT imx25_5100
  CALL MKS_make_foundations_gen6.bat BOOT imx25_5100 %1

  REM Output_BOOT_imx25_5100
  set der=Output_BOOT_imx25_5100
  xcopy %cwd%\%der%\%OUTPUT_MOCF% %cwd%\Released
  REN %cwd%\Released\%OUTPUT_MOCF% CB------XXX%Version%.moc

:end
@ECHO The Build is Complete:
@ECHO Start Time: %StartTime%
@ECHO End Time:   %TIME%
@ECHO ON
