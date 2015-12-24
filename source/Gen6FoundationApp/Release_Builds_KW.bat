REM
REM Will build - Clean - build all and scan the targets in klocwork. Runs on weekly klocwork build server.
REM 09/25/2009

REM begin localisation of Environment Variables
setlocal

@ECHO OFF
BREAK=ON
set ARMINC=C:\Program Files\ARM\RVCT\Data\4.0\400\include\windows

::CORDED
CALL MKS_make_foundations_gen6.bat APP imx25_ID_FF3_8100
CALL MKS_clean_foundations_gen6.bat APP imx25_ID_FF3_8100
kwinject.exe -o buildspec.out MKS_make_foundations_gen6.bat APP imx25_ID_FF3_5100 > buildspecHI2D.log
sed -e 's/;arm;/;arm;-D__restrict=;-D__asm;-D__int64=long long;/' buildspec.out > buildspecHI2D.out



::Do analysis ---The project names here have to exist on the server machine already using kwadmin command.
kwbuildproject.exe --port 8082 --license-host www.sweetcorn.com --license-port 27001 --project NG2D_HI2D --force -j auto --verbose buildspecHI2D.out --tables-directory "c:\Program Files\Klocwork\Server 9.2\kwtables\NG2D_Hi2D"
kwadmin load NG2D_HI2D "c:\Program Files\Klocwork\Server 9.2\kwtables\NG2D_HI2D"



@ECHO ON
