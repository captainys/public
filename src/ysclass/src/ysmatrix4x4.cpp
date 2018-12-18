/* ////////////////////////////////////////////////////////////

File Name: ysmatrix4x4.cpp
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

YSRESULT YsInvertMatrix4x4(double result[],const double from[])
{
	return YsInvertMatrixNxN <4> (result,from);
}

YSRESULT YsMulInverse4x4(double inv[],const double mat[],const double vec[])
{
	return YsMulInverseNxN <4> (inv,mat,vec);
}


////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////

const YsMatrix4x4 &YsMatrix4x4::operator=(const YsMatrix &mat)
{
	int r,c;

	if((mat.nr()!=3 || mat.nc()!=3) && (mat.nr()!=4 || mat.nc()!=4))
	{
		printf("YsMatrix4x4::operator=(const YsMatrix &mat)\n");
		printf("  Warning:\n");
		printf("  Right hand side of the substitution is not 3x3 nor 4x4 matrix.\n");
	}

	for(r=1; r<=4 && r<=mat.nr(); r++)
	{
		for(c=1; c<=4 && c<=mat.nc(); c++)
		{
			Set(r,c,mat.v(r,c));
		}
	}

	if(mat.nr()==3 && mat.nc()==3)
	{
		Set(1,4,0.0);
		Set(2,4,0.0);
		Set(3,4,0.0);
		Set(4,4,1.0);
		Set(4,1,0.0);
		Set(4,2,0.0);
		Set(4,3,0.0);
	}

	return *this;
}

const YsMatrix4x4 &YsMatrix4x4::operator*=(const class YsAtt3 &att)
{
	Rotate(att);
	return *this;
}

void YsMatrix4x4::Rotate(const YsRotation &rot)
{
	YsVec3 v;
	double a;
	rot.Get(v,a);
	Rotate(v.x(),v.y(),v.z(),a);
}

void YsMatrix4x4::Rotate(const YsAtt3 &att)
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
		RotateXY( att.b());
	}
}


YSRESULT YsMatrix4x4::MakeToXY(const YsVec3 &cen,const YsVec3 &zDir)
{
	YsAtt3 att;
	att.SetForwardVector(zDir);
	att.SetB(0.0);

	LoadIdentity();
	Rotate(att);
	Transpose();

	Translate(-cen);

	return YSOK;
}
