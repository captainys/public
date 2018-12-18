/* ////////////////////////////////////////////////////////////

File Name: yspredefined.h
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

#ifndef YSPREDEFINED_IS_INCLUDED
#define YSPREDEFINED_IS_INCLUDED
/* { */

const class YsVec2 &YsOrigin2(void);
const class YsVec2 &YsXVec2(void);
const class YsVec2 &YsYVec2(void);
const class YsVec2 &YsXYVec2(void);

const class YsVec3 &YsOrigin(void);
const class YsAtt3 &YsZeroAtt(void);

const class YsVec3 &YsXVec(void);
const class YsVec3 &YsYVec(void);
const class YsVec3 &YsZVec(void);
const class YsVec3 &YsXYZ(void);

const class YsPlane &YsXYPlane(void);
const class YsPlane &YsXZPlane(void);
const class YsPlane &YsYZPlane(void);

const class YsColor &YsBlack(void);
const class YsColor &YsBlue(void);
const class YsColor &YsRed(void);
const class YsColor &YsMagenta(void);
const class YsColor &YsGreen(void);
const class YsColor &YsCyan(void);
const class YsColor &YsYellow(void);
const class YsColor &YsWhite(void);
const class YsColor &YsDarkBlue(void);
const class YsColor &YsDarkRed(void);
const class YsColor &YsDarkMagenta(void);
const class YsColor &YsDarkGreen(void);
const class YsColor &YsDarkCyan(void);
const class YsColor &YsDarkYellow(void);
const class YsColor &YsGray(void);
const class YsColor &YsDarkGray(void);
const class YsColor &YsThreeBitColor(int idx);
const class YsColor YsGrayScale(const double &scale);

const class YsMatrix4x4 &YsIdentity4x4(void);
const class YsMatrix3x3 &YsIdentity3x3(void);

/* } */
#endif
