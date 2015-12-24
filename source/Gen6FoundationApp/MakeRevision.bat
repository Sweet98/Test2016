@ECHO OFF
REM Batch file for build version number head file from git revision information.
REM
REM Command format:
REM 	MakeRevision.bat [App target name | APP_NULL] [Boot target name | BOOT_NULL]
REM 	[ App target ] or [ Boot target ] use to filt out the tag name
REM 	[ APP_NULL ] or [ BOOT_NULL ] means do not make the reversion for app or boot
REM 	Note:
REM 	'git descrbe' is a git command, used to return the most recent tag name.
REM 	Problem is, when there is multiple tags on same one commit, it will
REM 	return the most recent created tag instead of the tag real checked out.
REM 	As sometime we will have serval tag on same commit for different product
REM 	build, for walking around this problem, you should use the pattern to
REM 	'match' the right tag name. So please the config the APP_PATTERN or
REM 	BOOT_PATTERN when create new build target.
REM 	For 'git describe' details, refer: http://git-scm.com/docs/git-describe

REM begin localisation of Environment Variables
setlocal

set APP_VERSION_TEMPLATE=MakeMocf\ProjectRevisionApp.tmpl
set APP_VERSION_FILE=MakeMocf\ProjectRevisionApp.txt
set BOOT_VERSION_TEMPLATE=libraries\OsKernel\db_boot\ProjectRevisionBoot.tmpl
set BOOT_VERSION_FILE=libraries\OsKernel\db_boot\ProjectRevisionBoot.h
set PROJECT_VERSION_TEMPLATE=FoundationApp\ProjectRevision.tmpl
set PROJECT_VERSION_FILE=FoundationApp\ProjectRevision.h

IF [%1] == [] (
	echo Warning: App target is empty
	EXIT /B
) ELSE IF [%2] == [] (
	echo Warning: Boot target is empty
	EXIT /B
)

REM =============================================
REM update App version
REM =============================================
set APP_TARGET=%1
REM Parse product name from APP_TARGET(e.g. imx25_ID_FF3_8100)
set APP_PNAME=%APP_TARGET:~13,4%
IF [%APP_PNAME%] == [] (
	set APP_PATTERN=NULL
) ELSE (
	set APP_PATTERN=A?%APP_PNAME%*
)

set APP_REVERSION=0000
for /f %%A in ('git describe --dirty --match %APP_PATTERN% 2^>nul') do set APP_REVERSION=%%A
tools\ssed.exe s/\$WCREV\$/%APP_REVERSION%/ %APP_VERSION_TEMPLATE% >%APP_VERSION_FILE% 

REM =============================================
REM update Boot version
REM =============================================
set BOOT_TARGET=%2
REM Parse product name from BOOT_TARGET(e.g. imx25_8100)
set BOOT_PNAME=%BOOT_TARGET:~6,4%
IF [%BOOT_PNAME%] == [] (
	set BOOT_PATTERN=NULL
) ELSE (
	set BOOT_PATTERN=B?%BOOT_PNAME%*
)

set BOOT_REVERSION=0000
for /f %%A in ('git describe --dirty --match %BOOT_PATTERN% 2^>nul') do set BOOT_REVERSION=%%A
tools\ssed.exe s/\$WCREV\$/%BOOT_REVERSION%/ %BOOT_VERSION_TEMPLATE% >%BOOT_VERSION_FILE% 

REM =============================================
REM update Project version
REM =============================================
set PROJ_REVERSION=0000
for /f %%A in ('git describe --all --long --dirty 2^>nul') do set PROJ_REVERSION=%%A
tools\ssed.exe s:\$PROJREV\$:%PROJ_REVERSION%: %PROJECT_VERSION_TEMPLATE% >%PROJECT_VERSION_FILE% 

echo Building App revision: %APP_REVERSION%
echo Building Boot revision: %BOOT_REVERSION%
echo Project revision: %PROJ_REVERSION%
