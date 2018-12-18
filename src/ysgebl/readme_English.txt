Polygon Editor - Polygon Crest
By Captain YS
http://www.ysflight.com




[Introduction]

This program is a polygon editor.  Maybe more commonly called a polygon modeler.  You can call whichever comfortable for you.  The main purpose is to build aircraft and ground visual models for YS FLIGHT SIMULATOR, but you can use it for making general polygonal models.  If you make a correct solid model, you can export the data for 3D printing, for example.  I wanted to make it easy to create aircraft models, so this program can quickly make a popular airfoils, and extrude along the wing leading and trailing edges.  It also has basic boolean operations and rounding functions as well.

The usage can be found:
http://polycre.help.en.ysflight.com/   (English version)
http://polycre.help.jp.ysflight.com/   (Japanese version)

The above help pages are highly incomplete and cover only a fraction of the functionality.  More explanations will be added in the future.

Binary distribution package for MacOSX and Linux are merged in 20150810 version.  Please see below for how to launch the program in Linux.




[Donations welcome!]
If you are generous and don't mind donating some money, please send some money via PayPal from the following URL.

    https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=WNHT4QQ753B86&lc=US&item_name=YSFLIGHT%2eCOM%20%2f%20Development%20of%20Free%20Flight%20Simulator%2c%20Tools%2c%20Polygon%20Editor%2c%20etc%2e&item_number=1&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted

I will use your donation for upgrading my developing environment, buying books to study new programming techniques, maintaining and adding contents of YSFLIGHT.COM.  I appreciate your generousity very much!

Thank you!

(Since YSFLIGHT.COM is not a non-profit organization, I don't think donating to this project won't count toward your tax deduction, though.)




[Starting the program]
In MacOSX double click bin/PolygonCrest

In Linux start "bin/Polygon Crest.app/ysgebl32" (32bit binary) or "bin/Polygon Crest.app/ysgebl64" (64bit binary).  

In Windows, double click bin\ysgebl32.exe (32bit binary) or bin\ysgebl64.exe (64bit binary)

By the way, only 64-bit binary is provided for MacOSX.




[Source Code]

The source code is included in the same archive.  I have compiled and tested with Visual Studio 2012, GCC 4.6, and clang that comes with XCode 7.

If you extract the files as:

  +--(Home Directory)
     +--PolygonCrest
        +--bin
        +--src

Then you can build the program by typing:

% cd ~/PolygonCrest
% mkdir build
% cd build
% cmake ../src
% make

in MacOSX and Linux, or

> cd %USERPROFILE%/PolygonCrest
> mkdir build
> cd build
> cmake ../src
> msbuild Project.sln

in Windows Visual Studio Command Prompt.

Visual Studio 2013 or higher is required to compile in Windows.  GCC 4.6, X11 libraries, and OpenGL libraries are required to compile in Linux.  XCode 7.x or higher and Command Line Tools is required to compile in MacOSX.  Also cmake 3.0 or newer is required.

I have written all source files except glext.h, glxext.h, and wglext.h.  I have downloaded these three files from http://www.opengl.org, and the redistribution is allowed as described in the files.  These three files are included to eliminate external dependency in Visual C++ environment.




It will create C:\lib in Windows, or ~/ysbin in Linux and MacOSX.  If you donÅft have a write-permission of the C drive, set environment variable called USERLIBDIR and USERINCDIR, and give the locations where libraries and headers are copied.






[License]

Version 20160220 and earlier versions are free based on BSD license.  

It might change in the future versions, but the executable and the source code can be used as stated below.

/* ////////////////////////////////////////////////////////////

PolygonCrest
Copyright (c) 2014 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */
