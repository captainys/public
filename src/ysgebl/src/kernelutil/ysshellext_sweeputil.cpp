/* ////////////////////////////////////////////////////////////

File Name: ysshellext_sweeputil.cpp
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

#include "ysshellext_sweeputil.h"

/*static*/ YsShellExt_SweepInfo *YsShellExt_SweepInfo::Create(void)
{
	return new YsShellExt_SweepInfo;
}

/*static*/ void YsShellExt_SweepInfo::Delete(YsShellExt_SweepInfo *ptr)
{
	delete ptr;
}

YsShellExt_SweepInfo::YsShellExt_SweepInfo()
{
	CleanUp();
}

YsShellExt_SweepInfo::~YsShellExt_SweepInfo()
{
	CleanUp();
}

void YsShellExt_SweepInfo::CleanUp(void)
{
	YsShellExt_BoundaryInfo::CleanUp();
	YsShellExt_Mapping <YsShellVertexHandle,YsShellVertexHandle>::CleanUp();
	srcEdVtHd.CleanUp();	
	allSrcVtHd.CleanUp();
}

void YsShellExt_SweepInfo::MakeInfo(
    const YsShellExt &shl,
    const YsConstArrayMask <YsShellPolygonHandle> &plHdArray,
    const YsConstArrayMask <YsShellExt::ConstEdgeHandle> &ceHdArray)
{
	MakeInfo(shl,plHdArray.size(),plHdArray,ceHdArray.size(),ceHdArray);
}

void YsShellExt_SweepInfo::MakeInfo(
    const YsShellExt &shl,
    YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[],
    YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[])
{
	CleanUp();

	YsShellExt_BoundaryInfo::MakeInfo(*(const YsShell *)&shl,nPl,plHdArray);

	allSrcVtHd.SetShell((const YsShell &)shl);
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetPolygon(plVtHd,plHdArray[idx]);
		for(auto vtHd : plVtHd)
		{
			allSrcVtHd.AddVertex(vtHd);
		}
	}
	for(YSSIZE_T idx=0; idx<nCe; ++idx)
	{
		YsArray <YsShellVertexHandle,4> ceVtHd;
		YSBOOL isLoop;
		shl.GetConstEdge(ceVtHd,isLoop,ceHdArray[idx]);
		for(auto vtHd : ceVtHd)
		{
			allSrcVtHd.AddVertex(vtHd);
		}
	}

	for(YSSIZE_T ceIdx=0; ceIdx<nCe; ++ceIdx)
	{
		YSBOOL isLoop;
		YsArray <YsShellVertexHandle,16> ceVtHd;
		shl.GetConstEdge(ceVtHd,isLoop,ceHdArray[ceIdx]);

		if(2<=ceVtHd.GetN())
		{
			if(YSTRUE==isLoop)
			{
				YsShellVertexHandle first=ceVtHd[0];
				ceVtHd.Append(first);
			}

			for(int edIdx=0; edIdx<ceVtHd.GetN()-1; ++edIdx)
			{
				if(YSTRUE!=boundaryEdgeHash.IsIncluded(ceVtHd[edIdx],ceVtHd[edIdx+1]))
				{
					boundaryEdgeHash.AddEdge(ceVtHd[edIdx],ceVtHd[edIdx+1]);
					srcEdVtHd.Append(ceVtHd[edIdx]);
					srcEdVtHd.Append(ceVtHd[edIdx+1]);
				}
			}
		}
	}
}

YsArray <YsShellVertexHandle> YsShellExt_SweepInfo::GetVertexAll(void) const
{
	YsArray <YsShellVertexHandle> vtHdArray;
	for(auto vtHd : allSrcVtHd)
	{
		vtHdArray.Append(vtHd);
	}
	return vtHdArray;
}

////////////////////////////////////////////////////////////

void YsShellExt_SweepInfoMultiStep::MidPoint::Initialize(void)
{
	vtHd=NULL;
}

void YsShellExt_SweepInfoMultiStep::Layer::Initialize(void)
{
	pointArray.CleanUp();
}

void YsShellExt_SweepInfoMultiStep::CleanUpLayer(void)
{
	srcVtKeyToMidPointIndex.CleanUp();
	layerArray.CleanUp();
}

YSRESULT YsShellExt_SweepInfoMultiStep::SetUpParallelSweepWithPath(
	const YsShellExt &shl,const YsConstArrayMask <YsShellVertexHandle> &pathVtHd,const double scaling[])
{
	return SetUpParallelSweepWithPath(shl,pathVtHd.GetN(),pathVtHd,scaling);
}

YSRESULT YsShellExt_SweepInfoMultiStep::SetUpParallelSweepWithPath(const YsShellExt &shl,YSSIZE_T nPathVt,const YsShellVertexHandle pathVtHdArray[],const double scaling[])
{
	Option opt;
	opt.pathVtHd.Set(nPathVt,pathVtHdArray);
	opt.scaling.Set(nPathVt,scaling);
	return SetUpParallelSweepWithPath(shl,opt);
}

YSRESULT YsShellExt_SweepInfoMultiStep::SetUpParallelSweepWithPath(const YsShellExt &shl,const Option &opt)
{
	CleanUpLayer();

	YSSIZE_T nPathVt=0;
	if(0<opt.pathPnt.size() && 0<opt.pathVtHd.size())
	{
		return YSERR;
	}
	else if(0<opt.pathPnt.size())
	{
		nPathVt=opt.pathPnt.size();
	}
	else if(0<opt.pathVtHd.size())
	{
		nPathVt=opt.pathVtHd.size();
	}

	if(0<opt.scaling.size() && opt.scaling.size()!=nPathVt)
	{
		return YSERR;
	}
	if(0<opt.twistAngle.size() && opt.twistAngle.size()!=nPathVt)
	{
		return YSERR;
	}

	if(2<=nPathVt)
	{
		YsArray <YsVec3> pathPnt(nPathVt,NULL);
		if(0<opt.pathPnt.size())
		{
			pathPnt=opt.pathPnt;
		}
		else
		{
			for(YSSIZE_T idx=0; idx<nPathVt; ++idx)
			{
				pathPnt[idx]=shl.GetVertexPosition(opt.pathVtHd[idx]);
			}
		}

		SetUpNLayer(shl,nPathVt);
		for(auto vtHd : allSrcVtHd)
		{
			YSSIZE_T indexInLayer;
			if(YSOK==srcVtKeyToMidPointIndex.FindElement(indexInLayer,shl.GetSearchKey(vtHd)))
			{
				if(0<opt.pathVtHd.size() && vtHd==opt.pathVtHd[0])
				{
					for(auto idx : layerArray.AllIndex())
					{
						auto &point=layerArray[idx].pointArray[indexInLayer];
						point.vtHd=opt.pathVtHd[idx+1];
						point.pos=shl.GetVertexPosition(opt.pathVtHd[idx+1]);
					}
				}
				else
				{
					for(auto idx : layerArray.AllIndex())
					{
						const double scaling=(0<opt.scaling.size() ? opt.scaling[idx+1] : 1.0);
						auto &point=layerArray[idx].pointArray[indexInLayer];
						point.pos=shl.GetVertexPosition(vtHd);
						if(0<opt.twistAngle.size())
						{
							point.pos-=opt.twistCenter;
							YsRotation rot(opt.twistAxis,opt.twistAngle[idx+1]);
							rot.RotatePositive(point.pos,point.pos);
							point.pos+=opt.twistCenter;
						}
						point.pos=(point.pos-pathPnt[0])*scaling+pathPnt[idx+1];
					}
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

YsArray <YsShellExt_SweepInfoMultiStep::Quad> YsShellExt_SweepInfoMultiStep::MakeSideFaceAndFirstToLastVertexMapping(const YsShellExt &shl)
{
	YsShellExt_Mapping <YsShellVertexHandle,YsShellVertexHandle>::CleanUp();

	YsArray <Quad> quadArray;
	for(YSSIZE_T edIdx=0; edIdx<=srcEdVtHd.GetN()-2; edIdx+=2)
	{
		YSSIZE_T idxInLayer[2];
		if(YSOK==srcVtKeyToMidPointIndex.FindElement(idxInLayer[0],shl.GetSearchKey(srcEdVtHd[edIdx])) &&
		   YSOK==srcVtKeyToMidPointIndex.FindElement(idxInLayer[1],shl.GetSearchKey(srcEdVtHd[edIdx+1])))
		{
			YsShellVertexHandle curEdVtHd[2]={srcEdVtHd[edIdx],srcEdVtHd[edIdx+1]};
			for(auto &layer : layerArray)
			{
				YsShellVertexHandle nxtEdVtHd[2]={layer.pointArray[idxInLayer[0]].vtHd,layer.pointArray[idxInLayer[1]].vtHd};
				quadArray.Increment();
				quadArray.Last().quadVtHd[0]=curEdVtHd[1];
				quadArray.Last().quadVtHd[1]=curEdVtHd[0];
				quadArray.Last().quadVtHd[2]=nxtEdVtHd[0];
				quadArray.Last().quadVtHd[3]=nxtEdVtHd[1];

				curEdVtHd[0]=nxtEdVtHd[0];
				curEdVtHd[1]=nxtEdVtHd[1];
			}

			if(YSTRUE!=YsShellExt_VertexToVertexMapping::CheckMappingExist(shl,srcEdVtHd[edIdx]))
			{
				YsShellExt_VertexToVertexMapping::AddMapping(shl,srcEdVtHd[edIdx],  curEdVtHd[0]);
			}
			if(YSTRUE!=YsShellExt_VertexToVertexMapping::CheckMappingExist(shl,srcEdVtHd[edIdx+1]))
			{
				YsShellExt_VertexToVertexMapping::AddMapping(shl,srcEdVtHd[edIdx+1],curEdVtHd[1]);
			}
		}
	}
	return quadArray;
}

void YsShellExt_SweepInfoMultiStep::SetUpNLayer(const YsShellExt &shl,YSSIZE_T nLayer)
{
	// For nLayers, layerArray needs to be nLayer-1 long.

	layerArray.Set(nLayer-1,NULL);
	for(auto &layer : layerArray)
	{
		layer.Initialize();
	}

	for(auto srcVtHd : allSrcVtHd)
	{
		const YSSIZE_T nextIdx=layerArray[0].pointArray.GetN();
		layerArray[0].pointArray.Increment();
		srcVtKeyToMidPointIndex.UpdateElement(shl.GetSearchKey(srcVtHd),nextIdx);
	}

	for(auto idx : layerArray.AllIndex())
	{
		if(0<idx)
		{
			layerArray[idx].pointArray.Set(layerArray[0].pointArray.GetN(),NULL);
		}
		for(auto &point : layerArray[idx].pointArray)
		{
			point.Initialize();
		}
	}
}


/*static*/ YsArray <double> YsShellExt_SweepInfoMultiStep::CalculateScalingForParallelSweepWithPathAndGuideLine(
    const YsVec3 &sweepDir,const YsArray <YsVec3> &pathArray,const YsArray <YsVec3> &guideArray)
{
	if(1>=pathArray.GetN() || 1>=guideArray.GetN())
	{
		YsArray <double> empty;
		return empty;
	}

	YsArray <double> scaling(pathArray.GetN(),NULL);
	for(auto &s : scaling)
	{
		s=1.0;
	}

	for(auto pathIndex : pathArray.AllIndex())
	{
		auto &s=scaling[pathIndex];
		auto &pathPnt=pathArray[pathIndex];

		const YsPlane cutPln(pathPnt,sweepDir);
		YSBOOL done=YSFALSE;
		for(auto &guidePos : guideArray)
		{
			if(YSTRUE==cutPln.CheckOnPlane(guidePos))
			{
				s=(guidePos-pathPnt).GetLength();
				done=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=done)
		{
			for(auto guideIndex : guideArray.AllIndex())
			{
				if(guideIndex<guideArray.GetN()-1)
				{
					auto &pos0=guideArray[guideIndex];
					auto &pos1=guideArray[guideIndex+1];

					YsVec3 itsc;
					if(YSOK==cutPln.GetPenetrationHighPrecision(itsc,pos0,pos1))
					{
						s=(itsc-pathPnt).GetLength();
						done=YSTRUE;
						break;
					}
					else if(0==guideIndex)
					{
						if(YSOK==cutPln.GetIntersection(itsc,pos0,pos1-pos0) &&
						   (0.0<(itsc-pos0)*(pos0-pos1) || 2==guideArray.GetN()))  // If guideArray consists of only two points, which side of line doesn't matter.
						{
							s=(itsc-pathPnt).GetLength();
							done=YSTRUE;
							break;
						}
					}
					else if(guideArray.GetN()-2==guideIndex)
					{
						if(YSOK==cutPln.GetIntersection(itsc,pos0,pos1-pos0) &&
						   0.0<(itsc-pos1)*(pos1-pos0))
						{
							s=(itsc-pathPnt).GetLength();
							done=YSTRUE;
							break;
						}
					}
				}
			}
		}
	}

	const double ref=scaling[0];
	for(auto &s : scaling)
	{
		s/=ref;
	}
	scaling[0]=1.0;

	return scaling;
}

/*static*/ YsArray <double> YsShellExt_SweepInfoMultiStep::CalculateScalingForParallelSweepWithPathAndGuideLine(
    const YsShellExt &shl,const YsVec3 &sweepDir,const YsArray <YsShellVertexHandle> &pathVtHdArray,const YsArray <YsShellVertexHandle> &guideVtHdArray)
{
	YsArray <YsVec3> pathVtPosArray,guideVtPosArray;
	for(auto vtHd : pathVtHdArray)
	{
		pathVtPosArray.Append(shl.GetVertexPosition(vtHd));
	}
	for(auto vtHd : guideVtHdArray)
	{
		guideVtPosArray.Append(shl.GetVertexPosition(vtHd));
	}
	return CalculateScalingForParallelSweepWithPathAndGuideLine(sweepDir,pathVtPosArray,guideVtPosArray);
}

YSRESULT YsShellExt_SweepInfoMultiStep::SetUpNonParallelSweepWithPath(
    const YsShellExt &shl,
    const YsConstArrayMask <YsShellVertexHandle> &pathVtHdArray,YSBOOL isLoop,
    const double scaling[],
    ORIENTATION_CONTROL_TYPE oriconTypeMid,ORIENTATION_CONTROL_TYPE oriconTypeLast)
{
	return SetUpNonParallelSweepWithPath(shl,pathVtHdArray.GetN(),pathVtHdArray,isLoop,scaling,oriconTypeMid,oriconTypeLast);
}

YSRESULT YsShellExt_SweepInfoMultiStep::SetUpNonParallelSweepWithPath(
    const YsShellExt &shl,
    YSSIZE_T nPathVt,const YsShellVertexHandle pathVtHdArrayIn[],YSBOOL isLoop,
    const double scaling[],
    ORIENTATION_CONTROL_TYPE oriconTypeMid,ORIENTATION_CONTROL_TYPE oriconTypeLast)
{
	YsArray <YsShellVertexHandle> pathVtHd(nPathVt,pathVtHdArrayIn);
	for(auto idx=pathVtHd.GetN()-1; 0<idx; --idx)
	{
		if(pathVtHd[idx]==pathVtHd[idx-1])
		{
			pathVtHd.Delete(idx);
		}
	}

	if(0<pathVtHd.GetN() && pathVtHd.Last()==pathVtHd[0])
	{
		pathVtHd.DeleteLast();
		isLoop=YSTRUE;
	}

	if(2>pathVtHd.GetN() || (YSTRUE==isLoop && 3>pathVtHd.GetN()))
	{
		return YSERR;
	}



	const auto &allVtHd=GetVertexAll();
	YsArray <YsVec3> iniPos;
	for(auto vtHd : allVtHd)
	{
		iniPos.Append(shl.GetVertexPosition(vtHd));
	}
	YsVec3 lfpNom;
	if(YSOK!=YsFindLeastSquareFittingPlaneNormal(lfpNom,iniPos))
	{
		return YSERR;
	}

	if(0.0>lfpNom*(shl.GetVertexPosition(pathVtHd[1])-shl.GetVertexPosition(pathVtHd[0])))
	{
		lfpNom=-lfpNom;
	}



	YsArray <YsVec3> nomArray(nPathVt,NULL);
	nomArray[0]=lfpNom;

	for(YSSIZE_T idx=1; idx<nPathVt-1; ++idx)
	{
		switch(oriconTypeMid)
		{
		case ORICON_PREVIOUS_SEGMENT:
			nomArray[idx]=YsUnitVector(shl.GetVertexPosition(pathVtHd[idx])-shl.GetVertexPosition(pathVtHd[idx-1]));
			break;
		case ORICON_NEXT_SEGMENT:
			nomArray[idx]=YsUnitVector(shl.GetVertexPosition(pathVtHd[idx+1])-shl.GetVertexPosition(pathVtHd[idx]));
			break;
		case ORICON_AVERAGE_ANGLE:
			{
				auto v1=YsUnitVector(shl.GetVertexPosition(pathVtHd[idx])-shl.GetVertexPosition(pathVtHd[idx-1]));
				auto v2=YsUnitVector(shl.GetVertexPosition(pathVtHd[idx+1])-shl.GetVertexPosition(pathVtHd[idx]));
				nomArray[idx]=YsUnitVector(v1+v2);
			}
			break;
		case ORICON_FROMPREVPOINT_TO_NEXTPOINT:
			nomArray[idx]=YsUnitVector(shl.GetVertexPosition(pathVtHd[idx+1])-shl.GetVertexPosition(pathVtHd[idx-1]));
			break;
		}
	}

	switch(oriconTypeLast)
	{
	case ORICON_PREVIOUS_SEGMENT:
	case ORICON_NEXT_SEGMENT:
		nomArray.Last()=YsUnitVector(shl.GetVertexPosition(pathVtHd[nPathVt-1])-shl.GetVertexPosition(pathVtHd[nPathVt-2]));
		break;
	case ORICON_AVERAGE_ANGLE:               // There's no next point
	case ORICON_FROMPREVPOINT_TO_NEXTPOINT:  // There's no next point
		if(YSTRUE==isLoop)
		{
			YsVec3 v1=YsUnitVector(shl.GetVertexPosition(pathVtHd[0])-shl.GetVertexPosition(pathVtHd[nPathVt-1]));
			YsVec3 v2=YsUnitVector(shl.GetVertexPosition(pathVtHd[nPathVt-1])-shl.GetVertexPosition(pathVtHd[nPathVt-2]));
			v1.Normalize();
			v2.Normalize();
			nomArray.Last()=YsUnitVector(v1+v2);
		}
		else
		{
			nomArray.Last()=YsUnitVector(shl.GetVertexPosition(pathVtHd[nPathVt-1])-shl.GetVertexPosition(pathVtHd[nPathVt-2]));
		}
		break;
	}

	return SetUpNonParallelSweepWithPath(shl,nPathVt,pathVtHd,isLoop,nomArray,scaling);
}

YSRESULT YsShellExt_SweepInfoMultiStep::SetUpNonParallelSweepWithPath(const YsShellExt &shl,YSSIZE_T nPathVt,const YsShellVertexHandle pathVtHdArrayIn[],YSBOOL isLoop,const YsVec3 nom[],const double scaling[])
{
	const auto &allVtHd=GetVertexAll(); // Need the original normal.


	CleanUpLayer();

	if(2<=nPathVt)
	{
		YsConstArrayMask <YsShellVertexHandle> pathVtHd(nPathVt,pathVtHdArrayIn);
		YsArray <YsVec3> pathPnt(nPathVt,NULL);
		for(YSSIZE_T idx=0; idx<pathVtHd.GetN(); ++idx)
		{
			pathPnt[idx]=shl.GetVertexPosition(pathVtHd[idx]);
		}

		const YSSIZE_T nLayer=(YSTRUE==isLoop ? nPathVt : nPathVt-1);
		SetUpNLayer(shl,nLayer+1);   // First set of vertices are not counted as a layer.  Number of layers will be nPathVt-1.

		// Rotation must be progressive.  Why?  Assume nom[0] and nom[5] are 180 degrees opposite.
		// If the rotation is calculated for each layer by YsRotation::MakeAtoB(nom[0],nom[layerIndex]),
		// rotational axis for nom[5] may become different from other layers, and thus the continuity is lost.
		// The solution is calculating the rotation for each step progressively.

		YsArray <YsMatrix3x3> rot(nPathVt-1,NULL);
		YsMatrix3x3 prevRot;
		for(int layerIdx=0; layerIdx<nPathVt-1; ++layerIdx)
		{
			YsRotation thisRot;
			thisRot.MakeAtoB(nom[layerIdx],nom[layerIdx+1]);
			prevRot*=thisRot;
			rot[layerIdx]=prevRot;
		}

		for(auto layerIdx=0; layerIdx<nPathVt-1; ++layerIdx)
		{
			YsMatrix4x4 tfm;

			tfm.Translate(pathPnt[layerIdx+1]);
			tfm*=rot[layerIdx];
			tfm.Scale(scaling[layerIdx+1],scaling[layerIdx+1],scaling[layerIdx+1]);
			tfm.Translate(-pathPnt[0]);

			for(auto vtHd : allVtHd)
			{
				YSSIZE_T indexInLayer;
				if(YSOK==srcVtKeyToMidPointIndex.FindElement(indexInLayer,shl.GetSearchKey(vtHd)))
				{
					auto &point=layerArray[layerIdx].pointArray[indexInLayer];
					if(vtHd==pathVtHd[0])
					{
						point.vtHd=pathVtHd[layerIdx+1];
						point.pos=shl.GetVertexPosition(pathVtHd[layerIdx+1]);
					}
					else
					{
						point.vtHd=NULL;
						point.pos=tfm*shl.GetVertexPosition(vtHd);
					}
				}
			}
		}

		if(YSTRUE==isLoop)
		{
			for(auto vtHd : allVtHd)
			{
				YSSIZE_T indexInLayer;
				if(YSOK==srcVtKeyToMidPointIndex.FindElement(indexInLayer,shl.GetSearchKey(vtHd)))
				{
					auto &point=layerArray[nLayer-1].pointArray[indexInLayer];
					point.vtHd=vtHd;
					point.pos=shl.GetVertexPosition(vtHd);
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt_SweepInfoMultiStep::SetUpSolidOfRevolution(
    const YsShellExt &shl,const YsVec3 &axiso,const YsVec3 &axisv,int nStep,const double stepAngle,const YsVec3 &stepOffset,YSBOOL closeSolid)
{
	if(0<nStep)
	{
		const auto &allVtHd=GetVertexAll(); // Need the original normal.

		const YSSIZE_T nMidLayer=(YSTRUE==closeSolid ? nStep-1 : nStep);
		SetUpNLayer(shl,nStep+1); // 2015/03/05 I think I ended up making the second parameter to be (number of layers)+1....

		for(auto layerIdx=0; layerIdx<nMidLayer; ++layerIdx)
		{
			const double angle=(double)(layerIdx+1)*stepAngle;
			YsRotation rot(axisv,angle);

			for(auto vtHd : allVtHd)
			{
				YSSIZE_T indexInLayer;
				if(YSOK==srcVtKeyToMidPointIndex.FindElement(indexInLayer,shl.GetSearchKey(vtHd)))
				{
					auto &point=layerArray[layerIdx].pointArray[indexInLayer];
					point.pos=rot*shl.GetVertexPosition(vtHd)+stepOffset*(double)(layerIdx+1);
					if(point.pos==shl.GetVertexPosition(vtHd))  // Vertex on axis
					{
						point.vtHd=vtHd;
					}
					else
					{
						point.vtHd=NULL;
					}
				}
			}
		}
		if(YSTRUE==closeSolid)
		{
			for(auto vtHd : allVtHd)
			{
				YSSIZE_T indexInLayer;
				if(YSOK==srcVtKeyToMidPointIndex.FindElement(indexInLayer,shl.GetSearchKey(vtHd)))
				{
					auto &point=layerArray[nStep-1].pointArray[indexInLayer];
					point.vtHd=vtHd;
					point.pos=shl.GetVertexPosition(vtHd);
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

