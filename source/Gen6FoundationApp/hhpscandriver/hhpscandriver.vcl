<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: HHPScanDriver - Win32 (WCE ARMV4T) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\JDHarper\LOCALS~1\Temp\RSP153.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D _WIN32_WCE=400 /D "ARM" /D "_ARM_" /D "WCE_PLATFORM_STANDARDSDK" /D "THUMB" /D "_THUMB_" /D "ARMV4T" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "HHPSCANDRIVER_EXPORTS" /Fp"ARMV4TDbg/HHPScanDriver.pch" /YX /Fo"ARMV4TDbg/" /Fd"ARMV4TDbg/" /QRarch4T /QRinterwork-return /MC /TP /c 
"C:\sb5000\hhp imaging\hhpscandriver\scandriverdebug.cpp"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverGlobalConfig.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverICMedia.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverInterface.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverLightControl.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverOpenContext.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverOV7120.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverScanManager.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverScannerSelect.c"
"C:\sb5000\hhp imaging\hhpscandriver\scandrivertoshiba.c"
"C:\sb5000\hhp imaging\hhpscandriver\ScanDriverMT9V022.c"
]
Creating command line "clthumb.exe @C:\DOCUME~1\JDHarper\LOCALS~1\Temp\RSP153.tmp" 
Creating temporary file "C:\DOCUME~1\JDHarper\LOCALS~1\Temp\RSP154.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:yes /pdb:"ARMV4TDbg/HHPScanDriver.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4TDbg/HHPScanDriver.dll" /implib:"ARMV4TDbg/HHPScanDriver.lib" /subsystem:windowsce,4.00 /MACHINE:THUMB 
".\ARMV4TDbg\scandriverdebug.obj"
".\ARMV4TDbg\ScanDriverGlobalConfig.obj"
".\ARMV4TDbg\ScanDriverICMedia.obj"
".\ARMV4TDbg\ScanDriverInterface.obj"
".\ARMV4TDbg\ScanDriverLightControl.obj"
".\ARMV4TDbg\ScanDriverOpenContext.obj"
".\ARMV4TDbg\ScanDriverOV7120.obj"
".\ARMV4TDbg\ScanDriverScanManager.obj"
".\ARMV4TDbg\ScanDriverScannerSelect.obj"
".\ARMV4TDbg\scandrivertoshiba.obj"
".\ARMV4TDbg\ScanDriverMT9V022.obj"
]
Creating command line "link.exe @C:\DOCUME~1\JDHarper\LOCALS~1\Temp\RSP154.tmp"
<h3>Output Window</h3>
Compiling...
scandriverdebug.cpp
ScanDriverGlobalConfig.c
ScanDriverICMedia.c
c:\sb5000\hhp imaging\hhpscandriver\scandrivericmedia.c(1703) : warning C4018: '>=' : signed/unsigned mismatch
c:\sb5000\hhp imaging\hhpscandriver\scandrivericmedia.c(1722) : warning C4018: '>' : signed/unsigned mismatch
c:\sb5000\hhp imaging\hhpscandriver\scandrivericmedia.c(1756) : warning C4018: '<=' : signed/unsigned mismatch
c:\sb5000\hhp imaging\hhpscandriver\scandrivericmedia.c(1756) : warning C4018: '<=' : signed/unsigned mismatch
c:\sb5000\hhp imaging\hhpscandriver\scandrivericmedia.c(1764) : warning C4018: '>=' : signed/unsigned mismatch
c:\sb5000\hhp imaging\hhpscandriver\scandrivericmedia.c(1764) : warning C4018: '<=' : signed/unsigned mismatch
ScanDriverInterface.c
ScanDriverLightControl.c
ScanDriverOpenContext.c
ScanDriverOV7120.c
ScanDriverScanManager.c
ScanDriverScannerSelect.c
scandrivertoshiba.c
ScanDriverMT9V022.c
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(613) : warning C4018: '<' : signed/unsigned mismatch
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(692) : warning C4244: '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(693) : warning C4244: '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(884) : warning C4244: '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(885) : warning C4244: '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(900) : warning C4244: '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(987) : warning C4101: 'nAimerState' : unreferenced local variable
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(988) : warning C4101: 'bAim' : unreferenced local variable
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(988) : warning C4101: 'bIll' : unreferenced local variable
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(1012) : warning C4101: 'nAimerState' : unreferenced local variable
c:\sb5000\hhp imaging\hhpscandriver\scandrivermt9v022.c(1026) : warning C4101: 'nAimerState' : unreferenced local variable
Linking...
   Creating library ARMV4TDbg/HHPScanDriver.lib and object ARMV4TDbg/HHPScanDriver.exp




<h3>Results</h3>
HHPScanDriver.dll - 0 error(s), 17 warning(s)
</pre>
</body>
</html>
