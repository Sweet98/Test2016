.. contents::

==========
IO Control
==========

The scan driver's client issues IO Control commands to the scan
driver.  This commands are issued by calling SCN_IOControl.  On
Windows CE systems, the client will call DeviceIoControl which calls
SCN_IOContrl.

Buffer management
=================

In a typical IOControl driver interface, when the driver's client
retrives data from the driver, it will pass a pointer to a buffer to a
read or ioctrl function.  This function will then copy data into this
buffer.  Using this type of interface to transfer images to the client
would be ineffecient.  Because, it would require every image to be
copied from a DMA buffer managed by the scan driver, into another
buffer managed by the client.  At the data rates in which images are
streamed, the copying would use a significant amount of the DRAM bus
bandwidth.  For this reason, the scan driver allows the client to
directly access image buffers.

Windows CE
----------

.. _VirtualCopy: http://msdn.microsoft.com/en-us/library/aa450977.aspx
.. _VirtualAlloc: http://msdn.microsoft.com/en-us/library/ms913486.aspx

Under Windows CE the client is given access to the image DMA buffer by
binding the physical DMA scan buffer to a virtual addressed owned by
the client.  

The client will allocate a virtual address space from the OS kernel by
calling VirtualAlloc_ with the MEM_RESERVE and PAGE_NOACCESS options.
Then, the client will register this buffer with the
HHPSD_IOCTL_REGISTER_SCAN_BUFFER_.

When the scan driver returns an image to the client, it will use
VirtualCopy_ to bind a physical DMA buffer to the client's virtual
buffer.

HHPSD_IOCTL_REGISTER_SCAN_BUFFER Example for Windows CE:

::

    HANDLE hSession; // A session handle from SCN_Open.
    void * pImageVirtualAddress; // Address from VirtualAlloc.
    SCN_IOControl(  hSession,
                    HHPSD_IOCTL_REGISTER_SCAN_BUFFER,
		    pImageVirtualAddress,
                    sizeof(pImageVirutalAddress),
		    NULL,0,NULL));


Hhp OS
------

Under Hhp OS, it easier to give the client access to image buffers.
The client and scan driver code run in the same process / memory
space.  So, the scan can return an actual pointer to the image DMA
buffer.  There is no need to map memory across process boundaries.

However, the concept of binding the a buffer is still used under Hhp
OS.  The client will register the address of a pointer to the image
with HHPSD_IOCTL_REGISTER_SCAN_BUFFER_.  When the scan driver returns
an image to the client, it will place the pointer to DMA buffer into
the address that was registered.

HHPSD_IOCTL_REGISTER_SCAN_BUFFER Example for Hhp OS:

::


    HANDLE hSession; // A session handle from SCN_Open.
    UCHAR *pPixels;
    SCN_IOControl(  hSession,
                    HHPSD_IOCTL_REGISTER_SCAN_BUFFER,
		    (PBYTE)&pPixels,
                    sizeof(&pPixels),
		    NULL,0,NULL));




HHPSD_IOCTL_REGISTER_SCAN_BUFFER
--------------------------------

This registers a client's virtual image buffer with the scan driver.

HHPSD_IOCTL_UNREGISTER_SCAN_BUFFER
----------------------------------

This unregisters a client's virtual image buffer with the scan driver.
	

HHPSD_IOCTL_GET_SCAN_HEIGHT and HHPSD_IOCTL_GET_SCAN_WIDTH
----------------------------------------------------------

These IOctls retrieve the maximum width and height of an image that
can be captured by the camera.  Under Windows CE, the client can use
this information to determine the amount of address space to resever
with VirtualAlloc_

Example::

    HANDLE hSession; // A session handle from SCN_Open.
    DWORD nHeight,nWidth;
    SCN_IOControl( hSession,
                   HHPSD_IOCTL_GET_SCAN_HEIGHT,
                   NULL, 0,
                   (PBYTE)&nHeight,sizeof(nHeight),
                   NULL );
    SCN_IOControl( hSession,
                   HHPSD_IOCTL_GET_SCAN_WIDTH,
                   NULL, 0,
                   (PBYTE)&nWidth,sizeof(nWidth),
                   NULL );



Image streaming
===============

HHPSD_IOCTL_START_SCANNING
--------------------------

This commands tells the scan driver to turn image streaming on.  That
is, to continuously capture image streams from the camera.  The
streaming will continue until the client sends the
HHPSD_IOCTL_STOP_SCANNING command.

HHPSD_IOCTL_STOP_SCANNING
-------------------------

Turns image streaming off.

HHPSD_IOCTL_GET_NEW_SCAN
------------------------

This command returns most recent image captured from a stream.  It is
an error to issue this command unless scanning has been started.  See
HHPSD_IOCTL_START_SCANNING_

Single frame capture
====================

HHPSD_IOCTL_SINGLE_FRAME_CAPTURE
--------------------------------



HHPSD_IOCTL_SET_ILLUMINATORS_STATE
-----------------------------------

HHPSD_IOCTL_SET_AIMER_STATE
---------------------------

HHPSD_IOCTL_SET_EXPOSURE_SETTINGS
---------------------------------

HHPSD_IOCTL_GET_EXPOSURE_SETTINGS
---------------------------------

HHPSD_IOCTL_RETURN_SCAN
-----------------------

HHPSD_IOCTL_SET_SCAN_LIGHTS_MODE
--------------------------------

HHPSD_IOCTL_GET_ENGINE_TYPE
---------------------------

HHPSD_IOCTL_SET_EXPOSURE_MODE
-----------------------------

HHPSD_IOCTL_SET_ILLUMINATOR_INTENSITY
-------------------------------------

HHPSD_IOCTL_SET_AIMER_INTENSITY
-------------------------------


HHPSD_IOCTL_NEW_IMAGE_AVAILABLE
-------------------------------

HHPSD_IOCTL_PREVENT_AIMER_IN_IMAGES
-----------------------------------

HHPSD_IOCTL_GET_IMAGE_ATTRIBUTES
--------------------------------

HHPSD_IOCTL_RECONFIG_ENGINE
---------------------------

HHPSD_IOCTL_GET_REVISION_STRING
-------------------------------

HHPSD_IOCTL_GET_IMAGER_PROPERTIES
---------------------------------

HHPSD_IOCTL_POWER_OFF_IMAGER
----------------------------

HHPSD_IOCTL_SET_ENGINE_CONFIG
-----------------------------

HHPSD_IOCTL_GET_ENGINE_CONFIG
-----------------------------

HHPSD_IOCTL_WRITE_PSOC_REG
--------------------------

HHPSD_IOCTL_READ_PSOC_REG
-------------------------

HHPSD_IOCTL_WRITE_PSOC
----------------------

HHPSD_IOCTL_READ_PSOC
---------------------

HHPSD_IOCTL_ENGINE_DIAGNOSTIC_MODE
----------------------------------

HHPSD_IOCTL_ENGINE_HAS_PSOC
---------------------------

HHPSD_IOCTL_SET_LIGHT_SETTINGS
------------------------------

This IOCTL accepts a tagged data structure.  The tags are:

============================  ===============================================
Tag                           Description
============================  ===============================================
LI_SCAN_MODE                  Sets the lighting mode to use while scanning.
                              See the description of lighting modes below.
LI_IDLE_MODE                  Sets the lighting mode to use while not 
                              scanning. See the description of lighting 
                              modes below.  Note, only the first four modes
                              which are not linked to the exposure time are 
                              allowed.
LI_ILLLUMINATION_SOURCE       Control the illumination source.  This is used
                              on red/green sensors.
LI_ILLUMINATION_INTENSITY     Set intensity.  Normally, this is 0-100, but 
                              can go up to 150 in boost mode.
LI_AIMER_INTENSITY            Set aimer intensity.  (0-100)
LI_ILLUMINATION_CONTROL       Give the scan driver control of the boost 
                              current.  1-scan driver controls boost.  
                              0-scan driver does not control boost.
============================  ===============================================


The light modes supported by the scan driver:

============================== ======== ============
MODE                           Illum    Aimer
============================== ======== ============
HHPSD_LM_OFF                   Off      Off
HHPSD_LM_AIM_ON                Off      On
HHPSD_LM_ILLUM_ON              On       Off
HHPSD_LM_ILLUM_ON_AIM_ON       On       On
HHPSD_LM_CONCURRENT            On       Not Exposing
HHPSD_LM_INTERLACED            Exposing Not Exposing
HHPSD_LM_FLASH_deprecated      DO NOT USE
------------------------------ ---------------------
HHPSD_LM_BLINK                 Exposing Off
HHPSD_LM_AIM_ONLY_NON_EXPOSURE Off      Not Exposing
============================== ======== ============





HHPSD_IOCTL_GET_LIGHT_SETTINGS
------------------------------

HHPSD_IOCTL_SCANNING_ENABLED
----------------------------

HHPSD_IOCTL_GET_EXPOSURE_MODE
-----------------------------

HHPSD_IOCTL_UPDATE_PSOC_REG
---------------------------

HHPSD_IOCTL_RESET_PSOC
----------------------

HHPSD_IOCTL_RELOAD_PSOC_REGS_COPY
---------------------------------

HHPSD_IOCTL_SET_IMAGER_GRAYSCALE
--------------------------------

HHPSD_IOCTL_UNSET_IMAGER_GRAYSCALE
----------------------------------

HHPSD_IOCTL_ATTACH_TO_SAME_BUFFER
---------------------------------

==============
Hardware layer
==============

=====================
Hand Held Scanner EVT
=====================

DBGSDR
======

The DBGSDR menu command controls the type of debug output generated by
the scan driver.

======   ===========
DBGSDR   Description
======   ===========
0        No debug output
1        Reserved for QA
3        Turn on all debug messages
======   ===========

DBGSDR values greater than 3 are interpretted as binary bit fields
that turn on certain types of debug messages if the corresponding bit
is set.

===   ======   ===========
Bit   DBGSDR   Description
===   ======   ===========
2     4        Calls from scan driver to HW layer are logged
3     8        Exposure control is logged.
4     16       Commands from the client are logged.
===   ======   ===========


Compilation
===========

Verify that code builds without error on all platforms.  
Run Gen6FoundationApp\Release_Builds.bat.

Fixed exposure mode
===================

In fixed exposure mode, verify that exposure changes go into effect
immediately even when streaming.  

1. Set fixed exposure mode,  with EXPMOD0.
#. Set SDRTIM to a value large enough such that streaming will
   continue after the trigger is released, such as SDRTIM60000.
#. Set DBGSDR12 so that exposure control and camera IIC operations are logged.
#. Press and release the trigger.
#. Adjust EXPFEX and EXPGEX and verify the corresponding IIC updates.
   For Example:

::

    EXPFEX20[ACK].
    WriteIIC(0x80, 0x96)
    Fixed Exp: 103 Gain: 256
    EXPFGX1[ACK].
    WriteIIC(0x82, 0x0)
    Fixed Exp: 103 Gain: 64
    EXPFEX4[ACK].
    WriteIIC(0x80, 0x1D)
    Fixed Exp: 21 Gain: 64
    EXPFGX2[ACK].
    WriteIIC(0x82, 0x100)
    Fixed Exp: 21 Gain: 128

Verify that after fixed exposure is changed, the exposure data
returned by IsAcceptable is correct, including two frame delay.

Presentaion Mode
================

1. Set fixed exposure mode,  with EXPMOD3.
#. Set SDRTIM to a value large enough such that streaming will
#. Set DBGSDR12 so that exposure control and camera IIC operations are logged.
#. Press and hold trigger.
#. Move the scanner between light and dark environments.  Verify that 
   presentation mode is correctly stepping through the exposures. For Example:

::

    I:150,E:102,G:64 - 2
    I:122,E:102,G:64 - 2
    I:38,E:102,G:64 - 1
    CHANGE_EXP - e:310, g:64 state:2 to 1
    WriteIIC(0x80, 0x1C7)
    WriteIIC(0x82, 0x0)
    I:4,E:102,G:64 - 1
    I:8,E:310,G:64 - 0
    CHANGE_EXP - e:310, g:128 state:1 to 0
    WriteIIC(0x80, 0x1C7)
    WriteIIC(0x82, 0x100)
    I:18,E:310,G:128 - 0
    I:103,E:310,G:128 - 0
    I:255,E:310,G:128 - 1
    CHANGE_EXP - e:310, g:64 state:0 to 1
    WriteIIC(0x80, 0x1C7)
    WriteIIC(0x82, 0x0)
    I:209,E:310,G:64 - 1
    I:255,E:310,G:64 - 2
    CHANGE_EXP - e:102, g:64 state:1 to 2
    WriteIIC(0x80, 0x94)
    WriteIIC(0x82, 0x0)
    I:255,E:310,G:64 - 2
    I:111,E:102,G:64 - 2
    I:67,E:102,G:64 - 1
    CHANGE_EXP - e:310, g:64 state:2 to 1
    WriteIIC(0x80, 0x1C7)
    WriteIIC(0x82, 0x0)
    I:19,E:102,G:64 - 1
    I:10,E:310,G:64 - 0
    CHANGE_EXP - e:310, g:128 state:1 to 0
    WriteIIC(0x80, 0x1C7)
    WriteIIC(0x82, 0x100)
    I:16,E:310,G:128 - 0
    I:12,E:310,G:128 - 0


Auto exposure and context sensitive
===================================
1. Set fixed exposure mode,  with EXPMOD2.
#. Set DBGSDR8 so that exposure control is logged.
#. Turn off 3 frame startup.
#. Using both dark and bright lighting extremes, verify the exposure control
   code settles on an exposure in no more then 6 frames without oscillating.



Extreme Ambient
===============

Verify that all exposure modes work in sunlight conditions and
complete darkness.

Depth of field
==============
Verify that autoexposure and context sensitive works at the depth of
field extremes in complete darkness.


Change exposure modes during streams
====================================

Test that we're able to switch to different exposure modes while
scanning is running.

Test TRGMOD8
============


Trigger to beep
===============
Test that trigger to beep time for sweet spot decoding is low in all
exposure modes.  DBGPTR1 turns on last decode time. SHWDCT1 give
capture and decode time.

Lighting modes
==============





