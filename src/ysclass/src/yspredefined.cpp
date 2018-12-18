/* ////////////////////////////////////////////////////////////

File Name: yspredefined.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
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

#include "ysclass.h"


const class YsVec2 &YsOrigin2(void)
{
	static YsVec2 stock(0.0,0.0);
	return stock;
}

const class YsVec2 &YsXVec2(void)
{
	static YsVec2 stock(1.0,0.0);
	return stock;
}

const class YsVec2 &YsYVec2(void)
{
	static YsVec2 stock(0.0,1.0);
	return stock;
}

const class YsVec2 &YsXYVec2(void)
{
	static YsVec2 stock(1.0,1.0);
	return stock;
}



const class YsVec3 &YsOrigin(void)
{
	static YsVec3 o(0.0,0.0,0.0);
	return o;
}

const class YsAtt3 &YsZeroAtt(void)
{
	static YsAtt3 a(0.0,0.0,0.0);
	return a;
}


const class YsVec3 &YsXVec(void)
{
	static YsVec3 v(1.0,0.0,0.0);
	return v;
}

const class YsVec3 &YsYVec(void)
{
	static YsVec3 v(0.0,1.0,0.0);
	return v;
}

const class YsVec3 &YsZVec(void)
{
	static YsVec3 v(0.0,0.0,1.0);
	return v;
}

const class YsVec3 &YsXYZ(void)
{
	static YsVec3 v(1.0,1.0,1.0);
	return v;
}

const class YsPlane &YsXYPlane(void)
{
	static YsPlane pln(YsVec3(0.0,0.0,0.0),YsVec3(0.0,0.0,1.0));
	return pln;
}

const class YsPlane &YsXZPlane(void)
{
	static YsPlane pln(YsVec3(0.0,0.0,0.0),YsVec3(0.0,-1.0,0.0));
	return pln;
}

const class YsPlane &YsYZPlane(void)
{
	static YsPlane pln(YsVec3(0.0,0.0,0.0),YsVec3(1.0,0.0,0.0));
	return pln;
}


const class YsColor &YsBlack(void)
{
	static YsColor col(0.0,0.0,0.0);
	return col;
}

const class YsColor &YsBlue(void)
{
	static YsColor col(0.0,0.0,1.0);
	return col;
}

const class YsColor &YsRed(void)
{
	static YsColor col(1.0,0.0,0.0);
	return col;
}

const class YsColor &YsMagenta(void)
{
	static YsColor col(1.0,0.0,1.0);
	return col;
}

const class YsColor &YsGreen(void)
{
	static YsColor col(0.0,1.0,0.0);
	return col;
}

const class YsColor &YsCyan(void)
{
	static YsColor col(0.0,1.0,1.0);
	return col;
}

const class YsColor &YsYellow(void)
{
	static YsColor col(1.0,1.0,0.0);
	return col;
}

const class YsColor &YsWhite(void)
{
	static YsColor col(1.0,1.0,1.0);
	return col;
}

const class YsColor &YsDarkBlue(void)
{
	static YsColor col(0.0,0.0,0.5);
	return col;
}

const class YsColor &YsDarkRed(void)
{
	static YsColor col(0.5,0.0,0.0);
	return col;
}

const class YsColor &YsDarkMagenta(void)
{
	static YsColor col(0.5,0.0,0.5);
	return col;
}

const class YsColor &YsDarkGreen(void)
{
	static YsColor col(0.0,0.5,0.0);
	return col;
}

const class YsColor &YsDarkCyan(void)
{
	static YsColor col(0.0,0.5,0.5);
	return col;
}

const class YsColor &YsDarkYellow(void)
{
	static YsColor col(0.5,0.5,0.0);
	return col;
}

const class YsColor &YsGray(void)
{
	static YsColor col(0.5,0.5,0.5);
	return col;
}

const class YsColor &YsDarkGray(void)
{
	static YsColor col(0.25,0.25,0.25);
	return col;
}

const class YsColor &YsThreeBitColor(int idx)
{
	static YsColor
	    black(0.25,0.25,0.25),
	    blue(0.0,0.0,1.0),
	    red(1.0,0.0,0.0),
	    magenta(1.0,0.0,1.0),
	    green(0.0,1.0,0.0),
	    cyan(0.0,1.0,1.0),
	    yellow(1.0,1.0,0.0),
	    white(0.75,0.75,0.75);

	idx=idx%8;
	switch(idx)
	{
	case 0:
		return black;
	case 1:
		return blue;
	case 2:
		return red;
	case 3:
		return magenta;
	case 4:
		return green;
	case 5:
		return cyan;
	case 6:
		return yellow;
	case 7:
		return white;
	}
	return black;
}

const class YsColor YsGrayScale(const double &scale)
{
	YsColor col(scale,scale,scale);
	return col;
}

const class YsMatrix4x4 &YsIdentity4x4(void)
{
	static YsMatrix4x4 *mat=NULL;
	if(mat==NULL)
	{
		mat=new YsMatrix4x4(YSTRUE);
	}
	return *mat;
}

const class YsMatrix3x3 &YsIdentity3x3(void)
{
	static YsMatrix3x3 *mat=NULL;
	if(mat==NULL)
	{
		mat=new YsMatrix3x3(YSTRUE);
	}
	return *mat;
}


