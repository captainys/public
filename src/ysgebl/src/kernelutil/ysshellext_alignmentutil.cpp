/* ////////////////////////////////////////////////////////////

File Name: ysshellext_alignmentutil.cpp
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

#include "ysshellext_alignmentutil.h"


void YsShellExt_AlignmentUtil::CleanUp(void)
{
	selVtHd.CleanUp();
	selPlHd.CleanUp();
	selCeHd.CleanUp();
	selFgHd.CleanUp();
}

double YsShellExt_AlignmentUtil::GetTargetValue(const YsShellExt &shl,int axis,ALIGNTYPE alignType) const
{
	YSBOOL first=YSTRUE;
	double min=0.0,max=0.0;

	for(auto vtHd : selVtHd)
	{
		auto pos=shl.GetVertexPosition(vtHd);
		if(YSTRUE==first)
		{
			min=pos[axis];
			max=pos[axis];
			first=YSFALSE;
		}
		else
		{
			YsMakeSmaller(min,pos[axis]);
			YsMakeGreater(max,pos[axis]);
		}
	}
	for(auto plHd : selPlHd)
	{
		double emin,emax;
		GetMinMax(emin,emax,shl.Conv(),axis,plHd);
		if(YSTRUE==first)
		{
			min=emin;
			max=emax;
			first=YSFALSE;
		}
		else
		{
			YsMakeSmaller(min,emin);
			YsMakeGreater(max,emax);
		}
	}
	for(auto ceHd : selCeHd)
	{
		double emin,emax;
		GetMinMax(emin,emax,shl.Conv(),axis,ceHd);
		if(YSTRUE==first)
		{
			min=emin;
			max=emax;
			first=YSFALSE;
		}
		else
		{
			YsMakeSmaller(min,emin);
			YsMakeGreater(max,emax);
		}
	}
	for(auto fgHd : selFgHd)
	{
		double emin,emax;
		GetMinMax(emin,emax,shl.Conv(),axis,fgHd);
		if(YSTRUE==first)
		{
			min=emin;
			max=emax;
			first=YSFALSE;
		}
		else
		{
			YsMakeSmaller(min,emin);
			YsMakeGreater(max,emax);
		}
	}

	switch(alignType)
	{
	case ALIGN_MINIMUM:
		return min;
	case ALIGN_CENTER:
		return (max+min)/2.0;
	case ALIGN_MAXIMUM:
		return max;
	}

	return 0.0;
}

void YsShellExt_AlignmentUtil::GetMinMax(double &min,double &max,const YsShellExt &shl,int axis,YsShell::PolygonHandle plHd) const
{
	min=0.0;
	max=0.0;
	YSBOOL first=YSTRUE;
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	for(auto pos : plVtPos)
	{
		if(YSTRUE==first)
		{
			min=pos[axis];
			max=pos[axis];
			first=YSFALSE;
		}
		else
		{
			YsMakeSmaller(min,pos[axis]);
			YsMakeGreater(max,pos[axis]);
		}
	}
}
void YsShellExt_AlignmentUtil::GetMinMax(double &min,double &max,const YsShellExt &shl,int axis,YsShellExt::ConstEdgeHandle ceHd) const
{
	min=0.0;
	max=0.0;
	YSBOOL first=YSTRUE;
	for(auto vtHd : shl.GetConstEdgeVertex(ceHd))
	{
		auto pos=shl.GetVertexPosition(vtHd);
		if(YSTRUE==first)
		{
			min=pos[axis];
			max=pos[axis];
			first=YSFALSE;
		}
		else
		{
			YsMakeSmaller(min,pos[axis]);
			YsMakeGreater(max,pos[axis]);
		}
	}
}
void YsShellExt_AlignmentUtil::GetMinMax(double &min,double &max,const YsShellExt &shl,int axis,YsShellExt::FaceGroupHandle fgHd) const
{
	min=0.0;
	max=0.0;
	YSBOOL first=YSTRUE;
	for(auto plHd : shl.GetFaceGroup(fgHd))
	{
		YsArray <YsVec3,4> plVtPos;
		shl.GetPolygon(plVtPos,plHd);
		for(auto pos : plVtPos)
		{
			if(YSTRUE==first)
			{
				min=pos[axis];
				max=pos[axis];
				first=YSFALSE;
			}
			else
			{
				YsMakeSmaller(min,pos[axis]);
				YsMakeGreater(max,pos[axis]);
			}
		}
	}
}

