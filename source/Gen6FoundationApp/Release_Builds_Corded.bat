REM begin localisation of Environment Variables
setlocal

@if "%1" == "" (
	ECHO Error: Please specific a product name! E.g. Release_Builds_Corded.bat 8100
	EXIT /b
)
@if not "%2" == "" @set Version=%2

@set OUTPUT_MOCF=db_app.moc
@set OUTPUT_SMF=db_app.sm
@set APP_TARGET=imx25_ID_FF3_%1
@set CWD=%cd%
@set DER=Output_APP_%APP_TARGET%

@ECHO OFF
BREAK=ON
set StartTime=%TIME%

IF "%2" == "beta" (
	goto:Beta_build
) ELSE IF "%2" == "clean" (
	goto:Clean_build
) ELSE (
	goto:Release_build
)
goto:end

:Beta_build
	CALL MKS_make_foundations_gen6.bat APP %APP_TARGET% %2
	goto:end

:Clean_build
	CALL MKS_clean_foundations_gen6.bat APP %APP_TARGET%
	goto:end

:Release_build
	CALL MKS_clean_foundations_gen6.bat APP %APP_TARGET%
	CALL MKS_make_foundations_gen6.bat APP %APP_TARGET% %2
	CALL MKS_clean_foundations_gen6.bat APP %APP_TARGET%
	CALL MKS_make_foundations_gen6.bat APP %APP_TARGET% %2
	REM move and rename the bin files into the new 'released' directory
	RD /Q /S "Released"
	md "Released"
	echo %CWD%
	echo %DER%
	xcopy %CWD%\%DER%\%OUTPUT_MOCF% %CWD%\Released
	xcopy %CWD%\%DER%\%OUTPUT_SMF% %CWD%\Released
	for /f %%A in ('git describe --dirty --match A?%1* 2^>nul') do set APP_REVERSION=%%A
	echo %APP_REVERSION%
	REN %CWD%\Released\%OUTPUT_MOCF% %APP_REVERSION%%Version%.moc
	REN %CWD%\Released\%OUTPUT_SMF% %APP_REVERSION%%Version%.sm
	goto:end

:end
@ECHO The Build is Complete:
@ECHO Start Time: %StartTime%
@ECHO End Time:   %TIME%
@ECHO ON
