/* ////////////////////////////////////////////////////////////

File Name: ysmatrix3x3.cpp
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

#include <stdio.h>
#include "ysgeometry.h"

// This code is needed to cope with VC++'s stupid bug.
// VC++ cannot compile template parameters correctly when different
// template parameters are mixed in one .CPP file.

YSRESULT YsInvertMatrix3x3(double result[],const double from[])
{
	return YsInvertMatrixNxN <3> (result,from);
	// I want to call this template function directly, but VC++'s stupidity prevent me from doing so.
}

YSRESULT YsMulInverse3x3(double inv[],const double mat[],const double vec[])
{
	return YsMulInverseNxN <3> (inv,mat,vec);
	// I want to call this template function directly, but VC++'s stupidity prevent me from doing so.
}



////////////////////////////////////////////////////////////

const YsMatrix3x3 &YsMatrix3x3::operator*=(const class YsAtt3 &att)
{
	Rotate(att);
	return *this;
}

void YsMatrix3x3::Rotate(const YsRotation &rot)
{
	YsVec3 v;
	double a;
	rot.Get(v,a);
	Rotate(v.x(),v.y(),v.z(),a);
}

void YsMatrix3x3::Rotate(const class YsAtt3 &att)
{
	if(20140530<=YsAtt3::compatibilityLevel || YSOPENGL!=YsCoordSysModel)
	{
		RotateXZ(att.h());
		RotateZY(att.p());
		RotateXY(att.b());
	}
	else
	{
		RotateXZ(-att.h());
		RotateZY(-att.p());
		RotateXY(att.b());
	}
}

YSRESULT YsMatrix3x3::MulTranspose(YsVec3 &dst,const YsVec3 &src) const
{
	YsVec3 *o,tmp;

	if(&dst==&src)
	{
		o=&tmp;
	}
	else
	{
		o=&dst;
	}

	o->SetX(v11()*src[0]+v21()*src[1]+v31()*src[2]);
	o->SetY(v12()*src[0]+v22()*src[1]+v32()*src[2]);
	o->SetZ(v13()*src[0]+v23()*src[1]+v33()*src[2]);

	if(o==&tmp)
	{
		dst=tmp;
	}

	return YSOK;
}

YsVec3 YsMatrix3x3::MulTranspose(const YsVec3 &src) const
{
	YsVec3 dst;
	MulTranspose(dst,src);
	return dst;
}

YSRESULT YsMatrix3x3::MakeToXY(const YsVec3 &zDir)
{
	YsAtt3 att;
	att.SetForwardVector(zDir);
	att.SetB(0.0);

	LoadIdentity();
	(*this)*=att;

	Transpose();

	return YSOK;
}

