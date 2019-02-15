/* ////////////////////////////////////////////////////////////

File Name: sketch.cpp
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

#include "sketch.h"

void PolyCreSketchInterface::StrokePoint::Initialize(void)
{
	prjPlHd=NULL;
}


////////////////////////////////////////////////////////////


PolyCreSketchInterface::PolyCreSketchInterface()
{
	drawing=YSFALSE;
}
PolyCreSketchInterface::~PolyCreSketchInterface()
{
}
/* static */ PolyCreSketchInterface *PolyCreSketchInterface::Create(void)
{
	return new PolyCreSketchInterface;
}
/* static */ void PolyCreSketchInterface::Delete(PolyCreSketchInterface *ptr)
{
	delete ptr;
}
void PolyCreSketchInterface::StartSketchInterface(const YsShellExt &shl,const Ys3DDrawingEnvironment &viewControl)
{
	this->shl=&shl;
	this->viewCtrl=&viewControl;
	ltc.DisablePolygonToCellHashTable();
	ltc.DisableInsideOutsideInfo();
	ltc.SetDomain(shl.Conv(),shl.GetNumPolygon()*10);
}

void PolyCreSketchInterface::BeginStroke(void)
{
	stroke.CleanUp();
	drawing=YSTRUE;
}

void PolyCreSketchInterface::AddStrokePoint(const YsVec2 &winCoord)
{
	if(YSTRUE==drawing)
	{
		stroke.Increment();
		stroke.Last().Initialize();
		stroke.Last().winCoord=winCoord;
	}
}

void PolyCreSketchInterface::EndStroke(void)
{
	drawing=YSFALSE;
}

void PolyCreSketchInterface::ClearStroke(void)
{
	stroke.CleanUp();
}

YSBOOL PolyCreSketchInterface::IsDrawing(void) const
{
	return drawing;
}

void PolyCreSketchInterface::AdHocSmoothingInWindowCoordinate(void)
{
	for(YSSIZE_T i=1; i<=stroke.GetN()-2; i+=2)
	{
		stroke[i].winCoord=(stroke[i-1].winCoord+stroke[i+1].winCoord)/2.0;
	}
}

YSBOOL PolyCreSketchInterface::DropHanging(void)
{
	for(YSSIZE_T i=0; i<stroke.GetN()-1; ++i)
	{
		const YsVec2 lineI[2]={stroke[i].winCoord,stroke[i+1].winCoord};

		for(YSSIZE_T j=i+2; j<stroke.GetN()-1; ++j) // Why i+2?  Because p[i]-p[i+1] and p[i+1]-p[i+2] are neighbors and always intersect.
		{
			const YsVec2 lineJ[2]={stroke[j].winCoord,stroke[j+1].winCoord};

			YsVec2 crs;
			if(YSOK==YsGetLineIntersection2(crs,lineI[0],lineI[1],lineJ[0],lineJ[1]) &&
			   YSTRUE==YsCheckInBetween2(crs,lineI[0],lineI[1]) &&
			   YSTRUE==YsCheckInBetween2(crs,lineJ[0],lineJ[1]))
			{
				stroke.Resize(j);  // Drop beyond j+1
				stroke.Delete(0,i+1);
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}

YSRESULT PolyCreSketchInterface::PreliminaryProjection(void)
{
	for(auto &strokePoint : stroke)
	{
		YsVec3 itsc;
		strokePoint.prjPlHd=FindPolygonFromWindowCoordinate(itsc,strokePoint.winCoord);
		if(NULL!=strokePoint.prjPlHd)
		{
			strokePoint.prjCoord=itsc;
		}
	}
	return YSOK;
}

YSRESULT PolyCreSketchInterface::UnprojectStrokePoint(YSSIZE_T idx)
{
	if(YSTRUE==stroke.IsInRange(idx))
	{
		stroke[idx].prjPlHd=NULL;
		return YSOK;
	}
	return YSERR;
}

YSRESULT PolyCreSketchInterface::ExtendProjection(void)
{
	for(;;)
	{
		YSBOOL retry=YSFALSE;
		for(YSSIZE_T idx=0; idx<stroke.GetN(); ++idx)
		{
			if(NULL==stroke[idx].prjPlHd)
			{
				YsShellPolygonHandle prjPlHd=NULL;
				if(YSTRUE==stroke.IsInRange(idx-1) && NULL!=stroke[idx-1].prjPlHd)
				{
					prjPlHd=stroke[idx-1].prjPlHd;
				}
				else if(YSTRUE==stroke.IsInRange(idx+1) && NULL!=stroke[idx+1].prjPlHd)
				{
					prjPlHd=stroke[idx+1].prjPlHd;
				}

				if(NULL!=prjPlHd)
				{
					YsArray <YsVec3,4> plVtPos;
					shl->GetPolygon(plVtPos,prjPlHd);

					YsPlane pln;
					if(YSOK==pln.MakeBestFitPlane(plVtPos))
					{
						YsVec3 org,vec;
						viewCtrl->TransformScreenCoordTo3DLine(org,vec,stroke[idx].winCoord.x(),stroke[idx].winCoord.y());

						YsVec3 itsc;
						if(YSOK==pln.GetIntersection(itsc,org,vec))
						{
							stroke[idx].prjPlHd=prjPlHd;
							stroke[idx].prjCoord=itsc;
							retry=YSTRUE;
						}
					}
				}
			}
		}
		if(YSTRUE!=retry)
		{
			break;
		}
	}

	for(auto &strokePoint : stroke)
	{
		if(NULL==strokePoint.prjPlHd)
		{
			return YSERR;
		}
	}

	return YSOK;
}

YSRESULT PolyCreSketchInterface::MakeClockwise(void)
{
	YsArray <YsVec2> plg;
	for(auto s : stroke)
	{
		plg.push_back(s.winCoord);
	}

	auto orientation=YsCheckFlipDirection2(plg.size(),plg.data());
	if(YSFLIPCCW==orientation)
	{
		ReverseOrder();
		return YSOK;
	}
	else if(YSFLIPCW==orientation)
	{
		return YSOK;
	}
	else // if(YSFLIPUNKNOWN==orientation)
	{
		return YSERR;
	}
}

YSRESULT PolyCreSketchInterface::ReverseOrder(void)
{
	stroke.Invert();
	return YSOK;
}

YSRESULT PolyCreSketchInterface::ProjectToShell(void)
{
	PreliminaryProjection();
	return ExtendProjection();
}

YSRESULT PolyCreSketchInterface::ProjectToPlane(void)
{
	YsPlane prjPln(viewCtrl->GetViewTarget(),viewCtrl->GetViewDirection());
	return ProjectToPlane(prjPln);
}

YSRESULT PolyCreSketchInterface::ProjectToPlane(const YsPlane &prjPln)
{
	YSRESULT res=YSOK;
	for(auto &strokePoint : stroke)
	{
		YsVec3 org,vec;
		viewCtrl->TransformScreenCoordTo3DLine(org,vec,strokePoint.winCoord.x(),strokePoint.winCoord.y());

		YsVec3 itsc;
		if(YSOK!=prjPln.GetIntersectionHighPrecision(itsc,org,vec))
		{
			res=YSERR;
		}

		strokePoint.prjPlHd=nullptr;
		strokePoint.prjCoord=itsc;
	}
	return res;
}

YsShellPolygonHandle PolyCreSketchInterface::FindPolygonFromWindowCoordinate(YsVec3 &itsc,const YsVec2 &winCoord) const
{
	YsVec3 org,vec;
	viewCtrl->TransformScreenCoordTo3DLine(org,vec,winCoord.x(),winCoord.y());

	YsShellPolygonHandle itscPlHd;
	if(YSOK==ltc.FindFirstIntersection(itscPlHd,itsc,org,vec))
	{
		return itscPlHd;
	}
	return NULL;
}

YsConstArrayMask <PolyCreSketchInterface::StrokePoint> PolyCreSketchInterface::GetStroke(void) const
{
	return YsConstArrayMask <StrokePoint> (stroke.GetN(),stroke.GetArray());
}

void PolyCreSketchInterface::SetStroke(YsConstArrayMask <YsVec2> winCoord)
{
	stroke.resize(winCoord.size());
	for(auto idx : winCoord.AllIndex())
	{
		stroke[idx].winCoord=winCoord[idx];
	}
}
