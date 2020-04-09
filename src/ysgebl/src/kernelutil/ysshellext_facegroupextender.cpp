/* ////////////////////////////////////////////////////////////

File Name: ysshellext_boundaryutil.h
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



#include "ysshellext_facegroupextender.h"
#include "ysshellext_boundaryutil.h"
#include "ysshellext_trackingutil.h"



YSRESULT YsShellExt_FaceGroupExtender::ExtendFaceGroup(YsShellExt &shl,YsConstArrayMask <YsShell::PolygonHandle> fgPlHd,const YsVec3 box[2])
{
	YsShellExt_BoundaryInfo boundary;
	YsShellPolygonStore fgPlStore(shl.Conv());
	for(auto plHd : fgPlHd)
	{
		fgPlStore.Add(plHd);
	}

	boundary.MakeInfo(shl.Conv(),fgPlStore);
	for(auto &contour : boundary.GetContourAll())
	{
		ExtendFaceGroupContour(shl,fgPlStore,contour,box);
	}

	return YSOK;
}



YSRESULT YsShellExt_FaceGroupExtender::ExtendFaceGroupContour(YsShellExt &shl,const YsShellPolygonStore &fgPlHd,YsConstArrayMask <YsShell::VertexHandle> contour,const YsVec3 boxIn[2])
{
	YsBoundingBoxMaker <YsVec3> box;
	box.Make(2,boxIn);

	YsArray <YsShell::VertexHandle> extVtHd;
	extVtHd.resize(contour.size());
	for(YSSIZE_T idx=0; idx<contour.size(); ++idx)
	{
		extVtHd[idx]=nullptr;

		YsShell::PolygonHandle plHd0=nullptr;
		for(auto plHd : shl.FindPolygonFromEdgePiece(contour[idx],contour.GetCyclic(idx+1)))
		{
			if(YSTRUE==fgPlHd.IsIncluded(plHd))
			{
				plHd0=plHd;
				break;
			}
		}
		if(nullptr==plHd0)
		{
			continue;
		}

		YsShell::PolygonHandle lastPlHd;
		YsShellExt::PassInPolygonStorePassAllEdge cond;
		cond.plgStorePtr=&fgPlHd;
		auto fanVtHd=YsShellExt_TrackingUtil::MakeVertexFanAroundVertex(lastPlHd,shl.Conv(),contour[idx],contour.GetCyclic(idx+1),plHd0,cond);
		if(fanVtHd.size()<2)
		{
			continue;
		}

		// Make it on the ball.
		YsArray <YsVec3> vecOnBall;
		vecOnBall.resize(fanVtHd.size());
		for(YSSIZE_T idx=0; idx<fanVtHd.size(); ++idx)
		{
			YsVec3 vec=shl.GetVertexPosition(fanVtHd[idx])-shl.GetVertexPosition(contour[idx]);
			vecOnBall[idx]=YsVec3::UnitVector(vec);
		}

		YsArray <double> fanAngle;
		double totalFanAngle=0.0;
		fanAngle.resize(vecOnBall.size()-1);
		for(YSSIZE_T i=0; i<fanAngle.size(); ++i)
		{
			double dotProd=vecOnBall[i]*vecOnBall[i+1];
			dotProd=YsBound(dotProd,-1.0,1.0);
			fanAngle[idx]=acos(dotProd);
			totalFanAngle+=fanAngle[idx];
		}
		if(totalFanAngle<YsTolerance)
		{
			continue;
		}

		YsVec3 midVec=YsVec3::Origin();
		double fanAngleAccum=0.0;
		for(YSSIZE_T i=0; i<fanAngle.size(); ++i)
		{
			if(totalFanAngle/2.0<=fanAngleAccum+fanAngle[i])
			{
				const double t=(totalFanAngle/2.0-fanAngleAccum)/fanAngle[i];
				midVec=-(vecOnBall[i]*(1.0-t)+vecOnBall[i+1]*t);
			}
			fanAngleAccum+=fanAngle[i];
		}
		if(midVec.Normalize()!=YSOK)
		{
			continue;
		}

		YsVec3 org=shl.GetVertexPosition(contour[idx]);
		if(org.x()<=box[0].x() || org.y()<=box[0].y() || org.z()<=box[0].z() ||
		   org.x()>=box[1].x() || org.y()>=box[1].y() || org.z()>=box[1].z())
		{
			continue;
		}

		midVec*=box.GetDiagonal().GetLength();

		YsVec3 clipped[2];
		if(YSOK==YsClipLineSeg3(clipped[0],clipped[1],org,org+midVec,box[0],box[1]))
		{
			if(clipped[0]==org)
			{
				extVtHd[idx]=shl.AddVertex(clipped[1]);
			}
			else if(clipped[1]==org)
			{
				extVtHd[idx]=shl.AddVertex(clipped[0]);
			}
		}
	}
	return YSOK;
}
