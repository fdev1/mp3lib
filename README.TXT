How to compile source code using make:
======================================

1. Open makefile and update the paths to your XC16 tools.
   You may also update the optimization switch and the
   device option.

2. Run "make p33f" to compile for dsPIC33F or "make p33e" 
   to compile for dsPIC33E devices.

How to compile using MPLAB
==========================

Just open the mp3lib.mcp project file on your MPLAB IDE
and check that the correct toolchain is selected and select
the target device. Then click on Project->Build All.

Using the library
=================

A sample project (mp3lib_test folder) is included with all 
packages to demonstrate the simples way to use mp3lib using
the state machine functions defined in mp3.h. Additionally
the source code for the state machine (mp3.c) is included with
all packages as a guide for using the lower level API.
