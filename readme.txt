=========================================
LTE Game Engine 2.2 SDK Source Code
=========================================
		http://www.ltestudios.com

1. Introduction

First of all thanks for downloading and using LTE Game Engine 2.2 SDK version,
this engine is based on Irrlicht 1.0 and almost all of Irrlicht functions are
supported here.

In this archive you will find the documentation in chm format
for the online html-style documentation go to

  http://www.ltestudios.com/3d/docs 
  
To access the svn:
  
  svn://svn.ltestudios.com/engine
  
  or
  
  http://svn.ltestudios.com/
 
To request an account with read-write access for the svn send an email
to < haxormail@gmail.com >.

===========================================================================
2. Requirements
===========================================================================


To use the LTE Game Engine you must have Toolchains installed, if you are running
under win32 you can find the downloadable compiled version here:

 http://xorloser.com/PS2DevWin32.zip

PSPGL is included in this archive, you need to use this version of PSPGL because
i've fixed some bugs in it, unfortunately i've lost engine pspgl source code.


===========================================================================
3. Some suggestions
===========================================================================

This engine has no limitations except of hardware limitations, you have to
remember that you are programming on PSP and it has got only 24MB of free
memory for homebrews. So, if I can give you some suggestions use mipmapped
textures, don't use meshes that has got 30000 vertices, etc..


===========================================================================
4. How to start
===========================================================================

Just run the Makefile contained in the same directory of this file, and
include files and libraries will be installed in your computer.

You can find documented examples in this archive that will explain you the basic
of this engine. For any question please go to the LTE Game Engine forum at:

http://www.ltestudios.com/forum

To run examples you have to extract the media.zip in your ms0:/ folder, 
just compile the example doing 'make' or 'make kxploit'.

===========================================================================
5. License
===========================================================================

LTE Game Engine Source Code is released under the GNU Lesser General Public License,
for further information ead license.txt


