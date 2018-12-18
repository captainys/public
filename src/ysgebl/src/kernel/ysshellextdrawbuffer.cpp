/* ////////////////////////////////////////////////////////////

File Name: ysshellextdrawbuffer.cpp
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

#include "ysshellext.h"
#include "ysshellextdrawbuffer.h"
#include "ysshellexttemporarymodification.h"
#include <ysclass11.h>
#include <ystask.h>
#include <thread>

#define YS_RUN_PARALLEL

YsShellExtDrawingBuffer::YsShellExtDrawingBuffer()
{
	viewPortEnabled=YSFALSE;
	viewPort[0]=-YsXYZ();
	viewPort[1]=YsXYZ();

	crossSectionEnabled=YSFALSE;
	crossSection.Set(YsOrigin(),YsZVec());

	drawBackFaceInDifferentColor=YSTRUE;
	backFaceColor.SetIntRGB(128,16,16);
}

void YsShellExtDrawingBuffer::EnableViewPort(void)
{
	viewPortEnabled=YSTRUE;
}

void YsShellExtDrawingBuffer::DisableViewPort(void)
{
	viewPortEnabled=YSFALSE;
}

void YsShellExtDrawingBuffer::SetViewPort(const YsVec3 viewPort[2])
{
	this->viewPort[0]=viewPort[0];
	this->viewPort[1]=viewPort[1];
}

void YsShellExtDrawingBuffer::GetViewPort(YsVec3 viewPort[2]) const
{
	viewPort[0]=this->viewPort[0];
	viewPort[1]=this->viewPort[1];
}

void YsShellExtDrawingBuffer::EnableCrossSection(void)
{
	crossSectionEnabled=YSTRUE;
}
void YsShellExtDrawingBuffer::DisableCrossSection(void)
{
	crossSectionEnabled=YSFALSE;
}
void YsShellExtDrawingBuffer::ToggleCrossSection(void)
{
	YsFlip(crossSectionEnabled);
}
void YsShellExtDrawingBuffer::SetCrossSection(const YsPlane &pln)
{
	crossSection=pln;
}
YSBOOL YsShellExtDrawingBuffer::CrossSectionEnabled(void) const
{
	return crossSectionEnabled;
}
YsPlane YsShellExtDrawingBuffer::GetCrossSection(void) const
{
	return crossSection;
}

void YsShellExtDrawingBuffer::RemakeNormalVertexBuffer(const class YsShellExt &shl,const double normalVertexMarkerRadius,const double roundVertexMarkerRadius)
{
	normalVertexPosBuffer.CleanUp();
	normalVertexIdxBuffer.CleanUp();

	roundVertexPosBuffer.CleanUp();
	roundVertexIdxBuffer.CleanUp();

	YsShellVertexHandle vtHd=NULL;
	while(YSOK==shl.MoveToNextVertex(vtHd))
	{
		YsVec3 pos;
		shl.GetVertexPosition(pos,vtHd);

		if(YSTRUE==viewPortEnabled && YSTRUE!=YsCheckInsideBoundingBox3(pos,viewPort[0],viewPort[1]))
		{
			continue;
		}
		if(YSTRUE==crossSectionEnabled && 0>crossSection.GetSideOfPlane(pos))
		{
			continue;
		}

		const YsShellExt::VertexAttrib *vtAttr=shl.GetVertexAttrib(vtHd);
		if(YSTRUE!=vtAttr->IsRound())
		{
			normalVertexPosBuffer.AddVertex(pos);
			normalVertexIdxBuffer.Append((int)shl.GetSearchKey(vtHd));
		}
		else
		{
			roundVertexPosBuffer.AddVertex(pos);
			roundVertexIdxBuffer.Append((int)shl.GetSearchKey(vtHd));
		}
	}
}

void YsShellExtDrawingBuffer::RemakeSelectedVertexBuffer(const class YsShellExt &shl,const double selectedVertexMarkerRadius,YSSIZE_T nSel,const YsShellVertexHandle selVtHd[])
{
	selectedVertexPosBuffer.CleanUp();
	selectedVertexIdxBuffer.CleanUp();
	selectedVertexColBuffer.CleanUp();

	selectedVertexLineBuffer.CleanUp();
	selectedVertexLineColBuffer.CleanUp();

	for(int idx=0; idx<nSel; ++idx)
	{
		YsVec3 pos;
		shl.GetVertexPosition(pos,selVtHd[idx]);

		if(YSTRUE==viewPortEnabled && YSTRUE!=YsCheckInsideBoundingBox3(pos,viewPort[0],viewPort[1]))
		{
			continue;
		}
		if(YSTRUE==crossSectionEnabled && 0>crossSection.GetSideOfPlane(pos))
		{
			continue;
		}

		YsColor col=ColorGradation(idx,nSel);
		selectedVertexColBuffer.AddColor(col);
		selectedVertexPosBuffer.AddVertex(pos);
		selectedVertexIdxBuffer.Append(shl.GetSearchKey(selVtHd[idx]));
	}

	for(int idx=0; idx<nSel-1; ++idx)
	{
		YsVec3 edVtPos[2];
		shl.GetVertexPosition(edVtPos[0],selVtHd[idx]);
		shl.GetVertexPosition(edVtPos[1],selVtHd[idx+1]);

		const YsColor col[2]={ColorGradation(idx,nSel),ColorGradation(idx+1,nSel)};

		if(YSTRUE!=viewPortEnabled && YSTRUE!=crossSectionEnabled)
		{
			selectedVertexLineBuffer.AddVertex(edVtPos[0]);
			selectedVertexLineBuffer.AddVertex(edVtPos[1]);
			selectedVertexLineColBuffer.AddColor(col[0]);
			selectedVertexLineColBuffer.AddColor(col[1]);
		}
		else
		{
			YsVec3 clippedEdVtPos[2];
			if(YSOK==ClipLine(clippedEdVtPos,edVtPos))
			{
				selectedVertexLineBuffer.AddVertex(clippedEdVtPos[0]);
				selectedVertexLineBuffer.AddVertex(clippedEdVtPos[1]);
				selectedVertexLineColBuffer.AddColor(col[0]);
				selectedVertexLineColBuffer.AddColor(col[1]);
			}
		}
	}
}

////////////////////////////////////////////////////////////

class YsShellExtDrawingBuffer::RemakePolygonBufferTask : public YsTask
{
public:
	// Output
	YsGLVertexBuffer normalEdgePosBuffer;
	YsArray <int> normalEdgeIdxBuffer;
	YsGLVertexBuffer shrunkEdgePosBuffer;
	YsArray <int> shrunkEdgeIdxBuffer;

	YsGLVertexBuffer solidShadedVtxBuffer;
	YsGLNormalBuffer solidShadedNomBuffer;
	YsGLColorBuffer solidShadedColBuffer;
	YsArray <int> solidShadedIdxBuffer;

	YsGLVertexBuffer solidUnshadedVtxBuffer;
	YsGLColorBuffer solidUnshadedColBuffer;
	YsArray <int> solidUnshadedIdxBuffer;

	YsGLVertexBuffer trspShadedVtxBuffer;
	YsGLNormalBuffer trspShadedNomBuffer;
	YsGLColorBuffer trspShadedColBuffer;
	YsArray <int> trspShadedIdxBuffer;

	YsGLVertexBuffer trspUnshadedVtxBuffer;
	YsGLColorBuffer trspUnshadedColBuffer;
	YsArray <int> trspUnshadedIdxBuffer;

	YsGLVertexBuffer backFaceVtxBuffer;
	YsGLColorBuffer backFaceColBuffer;
	YsArray <int> backFaceIdxBuffer;

	YsGLVertexBuffer invisibleButPickablePolygonVtxBuffer;
	YsArray <int> invisibleButPickablePolygonIdxBuffer;

	YsGLVertexBuffer lightVtxBuffer;
	YsGLColorBuffer lightColBuffer;
	YsGLPointSizeBuffer lightSizeBuffer;

	// Input
	YsArray <YsShellVertexHandle> edVtHdArray;
	YsArray <YsShellPolygonHandle> plHdArray;
	const YsShellExtDrawingBuffer::ShapeInfo *shapeInfoPtr;
	const YsShellExtDrawingBuffer *drawingBuffer;
	double polygonShrinkRatioForPicking;
	YSBOOL viewPortEnabled;
	YsVec3 viewPort[2];
	YSBOOL crossSectionEnabled;
	YsPlane crossSection;
	YSBOOL drawBackFaceInDifferentColor;
	YsColor backFaceColor;
	const YsHashTable <YsVec3> *vtxNomHashPtr;

	virtual void Run(void);  // Implementation of the thread task.

	void AddPolygonFragment(
	    const YsShellExtDrawingBuffer::ShapeInfo &shapeInfo,
	    YsShellPolygonHandle plHd,const YsVec3 &defNom,const YsHashTable <YsVec3> &vtxNomHash,const YsShellVertexHandle triVtHd[3],YSBOOL flatShaded,
	    YSBOOL addAsBackFace);
	void AddPolygonFragment(
	    const YsVec3 plVtPos[3],const YsVec3 plVtNom[3],unsigned int index,const YsColor &col,YSBOOL lighting,
	    YSBOOL addAsBackFace,YSBOOL invisibleButPickable);
};

void YsShellExtDrawingBuffer::RemakePolygonBufferTask::Run(void)
{
	const auto &shl=*(shapeInfoPtr->shlExtPtr);
	// Must respect temporary modification... const auto modPtr=shapeInfoPtr->modPtr;

	for(YSSIZE_T edIdx=0; edIdx<=edVtHdArray.GetN()-2; edIdx+=2)
	{
		const YsShellVertexHandle edVtHd[2]={edVtHdArray[edIdx],edVtHdArray[edIdx+1]};

		YsVec3 edVtPos[2];
		shl.GetVertexPosition(edVtPos[0],edVtHd[0]);
		shl.GetVertexPosition(edVtPos[1],edVtHd[1]);

		if(YSTRUE==viewPortEnabled || YSTRUE==crossSectionEnabled)
		{
			YsVec3 clippedEdVtPos[2];
			if(YSOK==drawingBuffer->ClipLine(clippedEdVtPos,edVtPos))
			{
				normalEdgePosBuffer.AddVertex(clippedEdVtPos[0]);
				normalEdgeIdxBuffer.Append(shl.GetSearchKey(edVtHd[0]));
				normalEdgePosBuffer.AddVertex(clippedEdVtPos[1]);
				normalEdgeIdxBuffer.Append(shl.GetSearchKey(edVtHd[1]));
			}
		}
		else
		{
			normalEdgePosBuffer.AddVertex(edVtPos[0]);
			normalEdgeIdxBuffer.Append(shl.GetSearchKey(edVtHd[0]));
			normalEdgePosBuffer.AddVertex(edVtPos[1]);
			normalEdgeIdxBuffer.Append(shl.GetSearchKey(edVtHd[1]));
		}
	}

	for(YSSIZE_T plIdx=0; plIdx<plHdArray.GetN(); ++plIdx)
	{
		YsShellPolygonHandle plHd=plHdArray[plIdx];
		YsArray <YsVec3,4> plVtPos;
		shl.GetVertexListOfPolygon(plVtPos,plHd);

		YsVec3 cen;
		shl.GetCenterOfPolygon(cen,plHd);

		const auto plAttrib=shl.GetPolygonAttrib(plHd);
		if(YSTRUE==plAttrib->GetAsLight())
		{
			lightVtxBuffer.AddVertex(cen);
			lightColBuffer.AddColor(shl.GetColor(plHd));
			lightSizeBuffer.Add(1.0f);
		}

		for(YSSIZE_T idx=0; idx<plVtPos.GetN(); ++idx)
		{
			plVtPos[idx]=cen+(plVtPos[idx]-cen)*polygonShrinkRatioForPicking;
		}

		for(YSSIZE_T idx=0; idx<plVtPos.GetN(); ++idx)
		{
			if(YSTRUE==viewPortEnabled || YSTRUE==crossSectionEnabled)
			{
				YsVec3 clippedEdVtPos[2]={plVtPos[idx],plVtPos.GetCyclic(idx+1)};
				if(YSOK==drawingBuffer->ClipLine(clippedEdVtPos,clippedEdVtPos))
				{
					shrunkEdgePosBuffer.AddVertex(clippedEdVtPos[0]);
					shrunkEdgePosBuffer.AddVertex(clippedEdVtPos[1]);
					shrunkEdgeIdxBuffer.Append(shl.GetSearchKey(plHd));
				}
			}
			else
			{
				shrunkEdgePosBuffer.AddVertex(plVtPos[idx]);
				shrunkEdgePosBuffer.AddVertex(plVtPos.GetCyclic(idx+1));
				shrunkEdgeIdxBuffer.Append(shl.GetSearchKey(plHd));
			}
		}

		YsVec3 nom;
		shl.GetNormalOfPolygon(nom,plHd);

		const YSBOOL twoSide=(YsOrigin()==nom ? YSTRUE : YSFALSE);
		const YSBOOL flatShaded=twoSide;
		if(YSTRUE==twoSide)
		{
			YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos);
		}

		const YSBOOL backFaceSwitch=((YSTRUE!=twoSide && YSTRUE==drawBackFaceInDifferentColor) ? YSTRUE : YSFALSE);


		const YsHashTable <YsVec3> &vtxNomHash=*vtxNomHashPtr;

		const YsShellVertexHandle *const plVtHd=shl.GetVertexListOfPolygon(plHd);;
		if(3==plVtPos.GetN())
		{
			AddPolygonFragment(*shapeInfoPtr,plHd,nom,vtxNomHash,plVtHd,flatShaded,YSFALSE);

			const YsShellVertexHandle reverseTriVtHd[3]=
			{
				plVtHd[2],plVtHd[1],plVtHd[0]
			};
			AddPolygonFragment(*shapeInfoPtr,plHd,-nom,vtxNomHash,reverseTriVtHd,flatShaded,backFaceSwitch);
		}
		else if(YSTRUE==YsCheckConvex3(plVtPos.GetN(),plVtPos))
		{
			for(int i=1; i<plVtPos.GetN()-1; i++)
			{
				YsShellVertexHandle triVtHd[3]=
				{
					plVtHd[0],
					plVtHd[i],
					plVtHd[i+1]
				};
				AddPolygonFragment(*shapeInfoPtr,plHd,nom,vtxNomHash,triVtHd,flatShaded,YSFALSE);

				YsSwapSomething(triVtHd[0],triVtHd[1]);
				AddPolygonFragment(*shapeInfoPtr,plHd,-nom,vtxNomHash,triVtHd,flatShaded,backFaceSwitch);
			}
		}
		else
		{
			YsSword swd;

			swd.SetInitialPolygon(plVtPos.GetN(),plVtPos,nullptr,plVtHd);
			swd.Triangulate();
			for(int i=0; i<swd.GetNumPolygon(); i++)
			{
				auto triVtHdPtr=swd.GetVtHdList(i);
				if(triVtHdPtr->GetN()>=3)
				{
					for(int j=1; j<triVtHdPtr->GetN()-1; ++j)
					{
						YsShellVertexHandle triVtHd[3]=
						{
							(*triVtHdPtr)[0],
							(*triVtHdPtr)[j],
							(*triVtHdPtr)[j+1]
						};
						if(NULL!=triVtHd[0] && NULL!=triVtHd[1] && NULL!=triVtHd[2])
						{
							AddPolygonFragment(*shapeInfoPtr,plHd,nom,vtxNomHash,triVtHd,flatShaded,YSFALSE);
							YsSwapSomething(triVtHd[0],triVtHd[1]);
							AddPolygonFragment(*shapeInfoPtr,plHd,-nom,vtxNomHash,triVtHd,flatShaded,backFaceSwitch);
						}
					}
				}
			}
		}
	}
}

void YsShellExtDrawingBuffer::RemakePolygonBufferTask::AddPolygonFragment(
    const YsShellExtDrawingBuffer::ShapeInfo &shapeInfo,
    YsShellPolygonHandle plHd,const YsVec3 &defNom,const YsHashTable <YsVec3> &vtxNomHash,const YsShellVertexHandle triVtHd[3],YSBOOL flatShaded,
    YSBOOL addAsBackFace)
{
	const auto &shl=*shapeInfo.shlExtPtr;
	const auto modPtr=shapeInfo.modPtr;
	const YsShellExt::PolygonAttrib *plAttr=shl.GetPolygonAttrib(plHd);

	YsColor col=(nullptr!=modPtr ? modPtr->GetColor(plHd) : shl.GetColor(plHd));
	YSBOOL invisibleButPickable=plAttr->GetAsLight();

	const YSBOOL lighting=YsReverseBool(plAttr->GetNoShading());

	YsVec3 triVtPos[3];
	YsVec3 triNom[3];

	for(int i=0; i<3; i++)
	{
		const YsShellExt::VertexAttrib *vtAttr=shl.GetVertexAttrib(triVtHd[i]);

		triNom[i]=YsOrigin();

		shl.GetVertexPosition(triVtPos[i],triVtHd[i]);
		// 2015/04/07 Support crease const edge >>
		if(YSTRUE==shl.IsOnCreaseConstEdge(triVtHd[i]))
		{
			auto vtPlHd=shl.FindSmoothShadingPolygonGroup(plHd,triVtHd[i]);
			triNom[i]=YsOrigin();
			for(auto plHd : vtPlHd)
			{
				triNom[i]+=shl.GetNormal(plHd);
			}
			triNom[i].Normalize();
		}
		// 2015/04/07 Support crease const edge <<
		if(YsOrigin()==triNom[i])
		{
			if(YSTRUE!=flatShaded && YSTRUE==vtAttr->IsRound() && YSOK==vtxNomHash.FindElement(triNom[i],shl.GetSearchKey(triVtHd[i])))
			{
			}
			else
			{
				triNom[i]=defNom;
			}
		}
	}

	if(YSTRUE!=viewPortEnabled && YSTRUE!=crossSectionEnabled)
	{
		AddPolygonFragment(triVtPos,triNom,shl.GetSearchKey(plHd),col,lighting,addAsBackFace,invisibleButPickable);
	}
	else
	{
		int nPlVt;
		YsVec3 plVtPos[8];  // A triangle can be clipped into up to a quadrilateral by a plane.  But, just in case.
		if(YSTRUE==crossSectionEnabled)
		{
			YsClipPolygonByPlane(nPlVt,plVtPos,8,3,triVtPos,crossSection,1);
		}
		else
		{
			nPlVt=3;
			plVtPos[0]=triVtPos[0];
			plVtPos[1]=triVtPos[1];
			plVtPos[2]=triVtPos[2];
		}

		for(int idx=1; idx<=nPlVt-2; ++idx)
		{
			const YsVec3 triVtPos[3]={plVtPos[0],plVtPos[idx],plVtPos[idx+1]};
			if(YSTRUE!=viewPortEnabled)
			{
				AddPolygonFragment(triVtPos,triNom,shl.GetSearchKey(plHd),col,lighting,addAsBackFace,invisibleButPickable);
			}
			else
			{
				int newNp;
				YsVec3 newp[32]; // 15 should be good enough.  Just in case.
				if(YSOK==YsClipPolygon3(newNp,newp,32,3,triVtPos,viewPort[0],viewPort[1]) && 3<=newNp)
				{
					YsVec3 clpPos[3],clpNom[3];
					clpPos[0]=newp[0];
					clpNom[0]=defNom;
					for(int i=0; i<3; ++i)
					{
						if(triVtPos[i]==clpPos[0])
						{
							clpNom[0]=triNom[i];
							break;
						}
					}

					for(int i=1; i<newNp-1; ++i)
					{
						clpPos[1]=newp[i];
						clpNom[1]=defNom;
						clpPos[2]=newp[i+1];
						clpNom[2]=defNom;

						for(int j=1; j<3; ++j)
						{
							for(int k=0; k<3; ++k)
							{
								if(clpPos[j]==triVtPos[k])
								{
									clpNom[j]=triNom[k];
									break;
								}
							}
						}

						AddPolygonFragment(clpPos,clpNom,shl.GetSearchKey(plHd),col,lighting,addAsBackFace,invisibleButPickable);
					}
				}
			}
		}
	}
}

void YsShellExtDrawingBuffer::RemakePolygonBufferTask::AddPolygonFragment(
    const YsVec3 plVtPos[3],const YsVec3 plVtNom[3],unsigned int index,const YsColor &col,YSBOOL lighting,
    YSBOOL addAsBackFace,YSBOOL invisibleButPickable)
{
	for(int i=0; i<3; ++i)
	{
		if(YSTRUE==invisibleButPickable)
		{
			invisibleButPickablePolygonVtxBuffer.AddVertex(plVtPos[i]);
		}
		else if(YSTRUE==addAsBackFace)
		{
			backFaceVtxBuffer.AddVertex(plVtPos[i]);
			backFaceColBuffer.AddColor(backFaceColor);
		}
		else if(YSTRUE==lighting)
		{
			if(0.99f<=col.Af())
			{
				solidShadedVtxBuffer.AddVertex(plVtPos[i]);
				solidShadedNomBuffer.AddNormal(plVtNom[i]);
				solidShadedColBuffer.AddColor(col);
			}
			else
			{
				trspShadedVtxBuffer.AddVertex(plVtPos[i]);
				trspShadedNomBuffer.AddNormal(plVtNom[i]);
				trspShadedColBuffer.AddColor(col);
			}
		}
		else
		{
			if(0.99f<=col.Af())
			{
				solidUnshadedVtxBuffer.AddVertex(plVtPos[i]);
				solidUnshadedColBuffer.AddColor(col);
			}
			else
			{
				trspUnshadedVtxBuffer.AddVertex(plVtPos[i]);
				trspUnshadedColBuffer.AddColor(col);
			}
		}
	}

	if(YSTRUE==invisibleButPickable)
	{
		invisibleButPickablePolygonIdxBuffer.Add(index);
	}
	else if(YSTRUE==addAsBackFace)
	{
		backFaceIdxBuffer.Append(index);
	}
	else if(YSTRUE==lighting)
	{
		if(0.99f<=col.Af())
		{
			solidShadedIdxBuffer.Append(index);
		}
		else
		{
			trspShadedIdxBuffer.Append(index);
		}
	}
	else
	{
		if(0.99f<=col.Af())
		{
			solidUnshadedIdxBuffer.Append(index);
		}
		else
		{
			trspUnshadedIdxBuffer.Append(index);
		}
	}
}

void YsShellExtDrawingBuffer::RemakePolygonBuffer(const class YsShellExt &shl,const double polygonShrinkRatioForPicking)
{
	ShapeInfo shapeInfo;
	shapeInfo.shlExtPtr=&shl;
	shapeInfo.modPtr=nullptr;
	RemakePolygonBuffer(shapeInfo,polygonShrinkRatioForPicking);
}

void YsShellExtDrawingBuffer::RemakePolygonBuffer(const ShapeInfo &shapeInfo,const double polygonShrinkRatioForPicking)
{
	const auto &shl=*shapeInfo.shlExtPtr;

	normalEdgePosBuffer.CleanUp();
	normalEdgeIdxBuffer.CleanUp();
	shrunkEdgePosBuffer.CleanUp();
	shrunkEdgeIdxBuffer.CleanUp();

	// See ysdnmgl2.0.cpp
	solidShadedVtxBuffer.CleanUp();
	solidShadedNomBuffer.CleanUp();
	solidShadedColBuffer.CleanUp();
	solidShadedIdxBuffer.CleanUp();

	solidUnshadedVtxBuffer.CleanUp();
	solidUnshadedColBuffer.CleanUp();
	solidUnshadedIdxBuffer.CleanUp();

	trspShadedVtxBuffer.CleanUp();
	trspShadedNomBuffer.CleanUp();
	trspShadedColBuffer.CleanUp();
	trspShadedIdxBuffer.CleanUp();

	trspUnshadedVtxBuffer.CleanUp();
	trspUnshadedColBuffer.CleanUp();
	trspUnshadedIdxBuffer.CleanUp();

	backFaceVtxBuffer.CleanUp();
	backFaceColBuffer.CleanUp();
	backFaceIdxBuffer.CleanUp();

	invisibleButPickablePolygonVtxBuffer.CleanUp();
	invisibleButPickablePolygonIdxBuffer.CleanUp();

	lightVtxBuffer.CleanUp();
	lightColBuffer.CleanUp();
	lightSizeBuffer.CleanUp();


	int nThread=(int)std::thread::hardware_concurrency();
	if(0==nThread)
	{
		nThread=4;
	}
	YsArray <YsShellExtDrawingBuffer::RemakePolygonBufferTask,32> taskArray(nThread,NULL);
	YsArray <YsTask *,32> taskPtrArray(nThread,NULL);

	if(YSTRUE==shl.IsSearchEnabled())
	{
		int taskIdx=0;
		for(YsShellEdgeEnumHandle edHd=NULL; YSOK==shl.MoveToNextEdge(edHd); )
		{
			YsShellVertexHandle edVtHd[2];
			shl.GetEdge(edVtHd[0],edVtHd[1],edHd);

			taskArray[taskIdx].edVtHdArray.Append(2,edVtHd);
			++taskIdx;
			if(nThread<=taskIdx)
			{
				taskIdx=0;
			}
		}
	}
	else
	{
		int taskIdx=0;
		for(auto plHd : shl.AllPolygon())
		{
			auto plVtHd=shl.GetPolygonVertex(plHd);
			for(YSSIZE_T idx=0; idx<plVtHd.GetN(); ++idx)
			{
				YsShellVertexHandle edVtHd[2]={plVtHd[idx],plVtHd.GetCyclic(idx+1)};

				taskArray[taskIdx].edVtHdArray.Append(2,edVtHd);
				++taskIdx;
				if(nThread<=taskIdx)
				{
					taskIdx=0;
				}
			}
		}
	}

	YsHashTable <YsVec3> vtxNomHash;
	{
		MakeVertexNormalCache(vtxNomHash,shl);

		for(int i=0; i<nThread; ++i)
		{
			taskArray[i].shapeInfoPtr=&shapeInfo;
			taskArray[i].polygonShrinkRatioForPicking=polygonShrinkRatioForPicking;
			taskArray[i].viewPortEnabled=viewPortEnabled;
			taskArray[i].viewPort[0]=viewPort[0];
			taskArray[i].viewPort[1]=viewPort[1];
			taskArray[i].crossSectionEnabled=crossSectionEnabled;
			taskArray[i].crossSection=crossSection;
			taskArray[i].drawingBuffer=this;
			taskArray[i].drawBackFaceInDifferentColor=drawBackFaceInDifferentColor;
			taskArray[i].backFaceColor=backFaceColor;
			taskArray[i].vtxNomHashPtr=&vtxNomHash;
			taskPtrArray[i]=&taskArray[i];
		}

		int idx=0;
		for(YsShellPolygonHandle plHd=NULL; YSOK==shl.MoveToNextPolygon(plHd); )
		{
			taskArray[idx].plHdArray.Append(plHd);
			++idx;
			if(nThread<=idx)
			{
				idx=0;
			}
		}
	}

#ifdef YS_RUN_PARALLEL
	YsRunTaskParallel(taskPtrArray);
#else
	printf("RemakePolygonBuffer is running in sequential mode.\n");
	for(int i=0; i<nThread; ++i) // Will launch in parallel soon.
	{
		task[i].StartLocal();
	}
#endif

	for(int i=0; i<nThread; ++i)
	{
		normalEdgePosBuffer.AddAndCleanUpIncoming(taskArray[i].normalEdgePosBuffer);
		normalEdgeIdxBuffer.Append(taskArray[i].normalEdgeIdxBuffer);
		shrunkEdgePosBuffer.AddAndCleanUpIncoming(taskArray[i].shrunkEdgePosBuffer);
		shrunkEdgeIdxBuffer.Append(taskArray[i].shrunkEdgeIdxBuffer);

		solidShadedVtxBuffer.AddAndCleanUpIncoming(taskArray[i].solidShadedVtxBuffer);
		solidShadedNomBuffer.AddAndCleanUpIncoming(taskArray[i].solidShadedNomBuffer);
		solidShadedColBuffer.AddAndCleanUpIncoming(taskArray[i].solidShadedColBuffer);
		solidShadedIdxBuffer.Append(taskArray[i].solidShadedIdxBuffer);

		solidUnshadedVtxBuffer.AddAndCleanUpIncoming(taskArray[i].solidUnshadedVtxBuffer);
		solidUnshadedColBuffer.AddAndCleanUpIncoming(taskArray[i].solidUnshadedColBuffer);
		solidUnshadedIdxBuffer.Append(taskArray[i].solidUnshadedIdxBuffer);

		trspShadedVtxBuffer.AddAndCleanUpIncoming(taskArray[i].trspShadedVtxBuffer);
		trspShadedNomBuffer.AddAndCleanUpIncoming(taskArray[i].trspShadedNomBuffer);
		trspShadedColBuffer.AddAndCleanUpIncoming(taskArray[i].trspShadedColBuffer);
		trspShadedIdxBuffer.Append(taskArray[i].trspShadedIdxBuffer);

		backFaceVtxBuffer.AddAndCleanUpIncoming(taskArray[i].backFaceVtxBuffer);
		backFaceColBuffer.AddAndCleanUpIncoming(taskArray[i].backFaceColBuffer);
		backFaceIdxBuffer.Append(taskArray[i].backFaceIdxBuffer);

		trspUnshadedVtxBuffer.AddAndCleanUpIncoming(taskArray[i].trspUnshadedVtxBuffer);
		trspUnshadedColBuffer.AddAndCleanUpIncoming(taskArray[i].trspUnshadedColBuffer);
		trspUnshadedIdxBuffer.Append(taskArray[i].trspUnshadedIdxBuffer);

		invisibleButPickablePolygonVtxBuffer.AddAndCleanUpIncoming(taskArray[i].invisibleButPickablePolygonVtxBuffer);
		invisibleButPickablePolygonIdxBuffer.Append(taskArray[i].invisibleButPickablePolygonIdxBuffer);

		lightVtxBuffer.AddAndCleanUpIncoming(taskArray[i].lightVtxBuffer);
		lightColBuffer.AddAndCleanUpIncoming(taskArray[i].lightColBuffer);
		lightSizeBuffer.AddAndCleanUpIncoming(taskArray[i].lightSizeBuffer);
	}
}

void YsShellExtDrawingBuffer::AddPolygonAsLight(const class YsShellExt &shl,const double lightSize)
{
	for(auto plHd : shl.AllPolygon())
	{
		lightVtxBuffer.Add(shl.GetCenter(plHd));
		lightColBuffer.Add(shl.GetColor(plHd));
		lightSizeBuffer.Add((YSGLFLOAT)lightSize);
	}
}

////////////////////////////////////////////////////////////

class YsShellExtDrawingBuffer::RemakeSelectedPolygonBufferTask : public YsTask
{
public:
	// Output
	YsGLVertexBuffer selectedPolygonPosBuffer;
	YsGLColorBuffer selectedPolygonColBuffer;
	YsArray <int> selectedPolygonIdxBuffer;

	// Input
	YsArray <YsShellPolygonHandle> selPlHd;
	const YsShellExt *shlExtPtr;
	double polygonShrinkRatio;
	YSBOOL viewPortEnabled;
	YsVec3 viewPort[2];

	virtual void Run(void);  // Implementation of the thread task.
};

void YsShellExtDrawingBuffer::RemakeSelectedPolygonBufferTask::Run(void)
{
	const YsShellExt &shl=*shlExtPtr;
	for(YSSIZE_T plIdx=0; plIdx<selPlHd.GetN(); ++plIdx)
	{
		YsArray <YsVec3,4> plVtPos;
		shl.GetVertexListOfPolygon(plVtPos,selPlHd[plIdx]);

		YsVec3 cen;
		shl.GetCenterOfPolygon(cen,selPlHd[plIdx]);

		for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
		{
			plVtPos[vtIdx]=cen+(plVtPos[vtIdx]-cen)*polygonShrinkRatio;
		}

		for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
		{
			if(YSTRUE==viewPortEnabled)
			{
				YsVec3 clp[2];
				if(YSOK==YsClipLineSeg3(clp[0],clp[1],plVtPos[vtIdx],plVtPos.GetCyclic(vtIdx+1),viewPort[0],viewPort[1]))
				{
					selectedPolygonPosBuffer.AddVertex(clp[0]);
					selectedPolygonColBuffer.AddColor(ColorGradation(vtIdx,plVtPos.GetN()+1));
					selectedPolygonPosBuffer.AddVertex(clp[1]);
					selectedPolygonColBuffer.AddColor(ColorGradation(vtIdx+1,plVtPos.GetN()+1));
					selectedPolygonIdxBuffer.Append(shl.GetSearchKey(selPlHd[plIdx]));
				}
			}
			else
			{
				selectedPolygonPosBuffer.AddVertex(plVtPos[vtIdx]);
				selectedPolygonColBuffer.AddColor(ColorGradation(vtIdx,plVtPos.GetN()+1));
				selectedPolygonPosBuffer.AddVertex(plVtPos.GetCyclic(vtIdx+1));
				selectedPolygonColBuffer.AddColor(ColorGradation(vtIdx+1,plVtPos.GetN()+1));
				selectedPolygonIdxBuffer.Append(shl.GetSearchKey(selPlHd[plIdx]));
			}
		}
	}
}

void YsShellExtDrawingBuffer::RemakeSelectedPolygonBuffer(
    const class YsShellExt &shl,const double polygonShrinkRatio,YSSIZE_T nSel,const YsShellPolygonHandle selPlHd[])
{
	selectedPolygonPosBuffer.CleanUp();
	selectedPolygonColBuffer.CleanUp();
	selectedPolygonIdxBuffer.CleanUp();

	int nThread=(int)std::thread::hardware_concurrency();
	if(0==nThread)
	{
		nThread=4;
	}
	YsArray <YsShellExtDrawingBuffer::RemakeSelectedPolygonBufferTask,32> taskArray(nThread,NULL);
	YsArray <YsTask *,32> taskPtrArray(nThread,NULL);

	for(int taskIdx=0; taskIdx<nThread; ++taskIdx)
	{
		taskArray[taskIdx].shlExtPtr=&shl;
		taskArray[taskIdx].polygonShrinkRatio=polygonShrinkRatio;
		taskArray[taskIdx].viewPortEnabled=viewPortEnabled;
		taskArray[taskIdx].viewPort[0]=viewPort[0];
		taskArray[taskIdx].viewPort[1]=viewPort[1];
		taskPtrArray[taskIdx]=&taskArray[taskIdx];
	}

	int taskIdx=0;
	for(YSSIZE_T plIdx=0; plIdx<nSel; ++plIdx)
	{
		taskArray[taskIdx].selPlHd.Append(selPlHd[plIdx]);
		++taskIdx;
		if(nThread<=taskIdx)
		{
			taskIdx=0;
		}
	}

#ifdef YS_RUN_PARALLEL
	YsRunTaskParallel(taskPtrArray);
#else
	for(int i=0; i<nThread; ++i) // Will launch in parallel soon.
	{
		taskArray[i].StartLocal();
	}
#endif

	for(YSSIZE_T taskIdx=0; taskIdx<nThread; ++taskIdx)
	{
		selectedPolygonPosBuffer.AddAndCleanUpIncoming(taskArray[taskIdx].selectedPolygonPosBuffer);
		selectedPolygonColBuffer.AddAndCleanUpIncoming(taskArray[taskIdx].selectedPolygonColBuffer);
		selectedPolygonIdxBuffer.Append(taskArray[taskIdx].selectedPolygonIdxBuffer);
	}
}

void YsShellExtDrawingBuffer::RemakeConstEdgeBuffer(const class YsShellExt &shl,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[])
{
	YsKeyStore selCeKey;
	for(YSSIZE_T idx=0; idx<nSel; ++idx)
	{
		selCeKey.AddKey(shl.GetSearchKey(ceHdArray[idx]));
	}

	constEdgeVtxBuffer.CleanUp();
	constEdgeIdxBuffer.CleanUp();

	YsShellExt::ConstEdgeHandle ceHd=NULL;
	while(YSOK==shl.MoveToNextConstEdge(ceHd))
	{
		if(YSTRUE==selCeKey.IsIncluded(shl.GetSearchKey(ceHd)))
		{
			continue;
		}

		YSBOOL isLoop;
		YSSIZE_T nVt;
		const YsShellVertexHandle *ceVtHd;

		shl.GetConstEdge(nVt,ceVtHd,isLoop,ceHd);

		for(YSSIZE_T vtIdx=0; vtIdx<nVt-1; ++vtIdx)
		{
			YsVec3 edVtPos[2];

			shl.GetVertexPosition(edVtPos[0],ceVtHd[vtIdx]);
			shl.GetVertexPosition(edVtPos[1],ceVtHd[vtIdx+1]);

			if(YSTRUE!=viewPortEnabled && YSTRUE!=crossSectionEnabled)
			{
				constEdgeVtxBuffer.AddVertex(edVtPos[0]);
				constEdgeVtxBuffer.AddVertex(edVtPos[1]);

				constEdgeIdxBuffer.Append(shl.GetSearchKey(ceHd));
			}
			else
			{
				YsVec3 clippedEdVtPos[2];
				if(YSOK==ClipLine(clippedEdVtPos,edVtPos))
				{
					constEdgeVtxBuffer.AddVertex(clippedEdVtPos[0]);
					constEdgeVtxBuffer.AddVertex(clippedEdVtPos[1]);

					constEdgeIdxBuffer.Append(shl.GetSearchKey(ceHd));
				}
			}
		}

		if(YSTRUE==isLoop)
		{
			YsVec3 edVtPos[2];

			shl.GetVertexPosition(edVtPos[0],ceVtHd[nVt-1]);
			shl.GetVertexPosition(edVtPos[1],ceVtHd[0]);

			if(YSTRUE!=viewPortEnabled)
			{
				constEdgeVtxBuffer.AddVertex(edVtPos[0]);
				constEdgeVtxBuffer.AddVertex(edVtPos[1]);

				constEdgeIdxBuffer.Append(shl.GetSearchKey(ceHd));
			}
			else
			{
				YsVec3 clippedEdVtPos[2];
				if(YSOK==ClipLine(clippedEdVtPos,edVtPos))
				{
					constEdgeVtxBuffer.AddVertex(clippedEdVtPos[0]);
					constEdgeVtxBuffer.AddVertex(clippedEdVtPos[1]);

					constEdgeIdxBuffer.Append(shl.GetSearchKey(ceHd));
				}
			}
		}
	}
}

void YsShellExtDrawingBuffer::RemakeSelectedConstEdgeBuffer(const class YsShellExt &shl,YSSIZE_T nSel,const YsShellExt::ConstEdgeHandle ceHdArray[])
{
	selectedConstEdgeVtxBuffer.CleanUp();
	selectedConstEdgeIdxBuffer.CleanUp();
	selectedConstEdgeArrowHeadVtxBuffer.CleanUp();

	for(YSSIZE_T idx=0; idx<nSel; ++idx)
	{
		YSBOOL isLoop;
		YSSIZE_T nVt;
		const YsShellVertexHandle *ceVtHd;

		shl.GetConstEdge(nVt,ceVtHd,isLoop,ceHdArray[idx]);

		for(YSSIZE_T vtIdx=0; vtIdx<nVt-1; ++vtIdx)
		{
			YsVec3 edVtPos[2];

			shl.GetVertexPosition(edVtPos[0],ceVtHd[vtIdx]);
			shl.GetVertexPosition(edVtPos[1],ceVtHd[vtIdx+1]);

			if(YSTRUE!=viewPortEnabled)
			{
				selectedConstEdgeVtxBuffer.AddVertex(edVtPos[0]);
				selectedConstEdgeVtxBuffer.AddVertex(edVtPos[1]);

				selectedConstEdgeIdxBuffer.Append(shl.GetSearchKey(ceHdArray[idx]));

				selectedConstEdgeArrowHeadVtxBuffer.AddArrowHeadTriangle(edVtPos[0],edVtPos[1],0.2);
			}
			else
			{
				YsVec3 clippedEdVtPos[2];
				if(YSOK==ClipLine(clippedEdVtPos,edVtPos))
				{
					selectedConstEdgeVtxBuffer.AddVertex(clippedEdVtPos[0]);
					selectedConstEdgeVtxBuffer.AddVertex(clippedEdVtPos[1]);

					selectedConstEdgeIdxBuffer.Append(shl.GetSearchKey(ceHdArray[idx]));

					selectedConstEdgeArrowHeadVtxBuffer.AddArrowHeadTriangle(clippedEdVtPos[0],clippedEdVtPos[1],0.2);
				}
			}
		}

		if(YSTRUE==isLoop)
		{
			YsVec3 edVtPos[2];

			shl.GetVertexPosition(edVtPos[0],ceVtHd[nVt-1]);
			shl.GetVertexPosition(edVtPos[1],ceVtHd[0]);

			if(YSTRUE!=viewPortEnabled)
			{
				selectedConstEdgeVtxBuffer.AddVertex(edVtPos[0]);
				selectedConstEdgeVtxBuffer.AddVertex(edVtPos[1]);

				selectedConstEdgeIdxBuffer.Append(shl.GetSearchKey(ceHdArray[idx]));

				selectedConstEdgeArrowHeadVtxBuffer.AddArrowHeadTriangle(edVtPos[0],edVtPos[1],0.2);
			}
			else
			{
				YsVec3 clippedEdVtPos[2];
				if(YSOK==ClipLine(clippedEdVtPos,edVtPos))
				{
					selectedConstEdgeVtxBuffer.AddVertex(clippedEdVtPos[0]);
					selectedConstEdgeVtxBuffer.AddVertex(clippedEdVtPos[1]);

					selectedConstEdgeIdxBuffer.Append(shl.GetSearchKey(ceHdArray[idx]));

					selectedConstEdgeArrowHeadVtxBuffer.AddArrowHeadTriangle(clippedEdVtPos[0],clippedEdVtPos[1],0.2);
				}
			}
		}
	}
}

YSRESULT YsShellExtDrawingBuffer::ClipLine(YsVec3 clippedEdVtPos[2],const YsVec3 edVtPos[2]) const
{
	clippedEdVtPos[0]=edVtPos[0];
	clippedEdVtPos[1]=edVtPos[1];

	if(YSTRUE==viewPortEnabled &&
	   YSOK!=YsClipLineSeg3(clippedEdVtPos[0],clippedEdVtPos[1],edVtPos[0],edVtPos[1],viewPort[0],viewPort[1]))
	{
		return YSERR;
	}

	if(YSTRUE==crossSectionEnabled)
	{
		const int side[2]={crossSection.GetSideOfPlane(clippedEdVtPos[0]),crossSection.GetSideOfPlane(clippedEdVtPos[1])};
		if(side[0]>0 && side[1]>0)
		{
			return YSOK;
		}
		else if(0>=side[0] && 0>=side[1])
		{
			return YSERR;
		}
		else if(side[0]==-side[1])
		{
			YsVec3 itsc;
			crossSection.GetIntersection(itsc,clippedEdVtPos[0],clippedEdVtPos[1]-clippedEdVtPos[0]);
			if(side[0]<0)
			{
				clippedEdVtPos[0]=itsc;
			}
			else
			{
				clippedEdVtPos[1]=itsc;
			}
		}
	}

	return YSOK;
}

/* static */void YsShellExtDrawingBuffer::MakeVertexNormalCache(YsHashTable <YsVec3> &vtxNomHash,const YsShellExt &shl)
{
	for(YsShellPolygonHandle plHd=NULL; YSOK==shl.MoveToNextPolygon(plHd); )
	{
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		shl.GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

		YsVec3 nom;
		shl.GetNormalOfPolygon(nom,plHd);

		for(int idx=0; idx<nPlVt; ++idx)
		{
			const YsShellExt::VertexAttrib *vtAttr=shl.GetVertexAttrib(plVtHd[idx]);
			if(YSTRUE==vtAttr->IsRound())
			{
				YsVec3 nomSum;
				if(YSOK!=vtxNomHash.FindElement(nomSum,shl.GetSearchKey(plVtHd[idx])))
				{
					nomSum=YsOrigin();
				}
				nomSum+=nom;
				vtxNomHash.UpdateElement(shl.GetSearchKey(plVtHd[idx]),nomSum);
			}
		}
	}
	for(YsShellVertexHandle vtHd=NULL; YSOK==shl.MoveToNextVertex(vtHd); )
	{
		YsVec3 nomSum;
		if(YSOK==vtxNomHash.FindElement(nomSum,shl.GetSearchKey(vtHd)))
		{
			nomSum.Normalize();
			vtxNomHash.UpdateElement(shl.GetSearchKey(vtHd),nomSum);
		}
	}
}

////////////////////////////////////////////////////////////

class YsShellExtDrawingBuffer::RemakeSelectedFaceGroupBufferTask : public YsTask
{
public:
	YsGLVertexBuffer selectedFaceGroupVtxBuffer;
	YsArray <int> selectedFaceGroupIdxBuffer;

	YsArray <YsShellPolygonHandle> plHdArray;
	YsArray <int> indexArray;

	const YsShellExt *shlExtPtr;
	YSBOOL viewPortEnabled;
	YsVec3 viewPort[2];
	YSBOOL crossSectionEnabled;
	YsPlane crossSection;

	virtual void Run(void);  // Implementation of the thread task.
	void AddTriangle(const YsVec3 tri[3],int fgKey);
};

void YsShellExtDrawingBuffer::RemakeSelectedFaceGroupBufferTask::Run(void)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		YsArray <YsVec3,16> plVtPos;
		shlExtPtr->GetVertexListOfPolygon(plVtPos,plHdArray[idx]);

		if(3==plVtPos.GetN())
		{
			YsVec3 triVtPos[3]={plVtPos[0],plVtPos[1],plVtPos[2]};
			AddTriangle(triVtPos,indexArray[idx]);
		}
		else if(YSTRUE==YsCheckConvex3(plVtPos.GetN(),plVtPos))
		{
			for(int i=1; i<plVtPos.GetN()-1; i++)
			{
				YsVec3 triVtPos[3]={plVtPos[0],plVtPos[i],plVtPos[i+1]};
				AddTriangle(triVtPos,indexArray[idx]);
			}
		}
		else
		{
			YsSword swd;

			swd.SetInitialPolygon(plVtPos.GetN(),plVtPos,NULL);
			swd.Triangulate();
			for(int i=0; i<swd.GetNumPolygon(); i++)
			{
				const YsArray <YsVec3> *tri=swd.GetPolygon(i);
				if(tri->GetN()>=3)
				{
					const YsVec3 triVtPos[3]={(*tri)[0],(*tri)[1],(*tri)[2]};
					AddTriangle(triVtPos,indexArray[idx]);
				}
			}
		}
	}
}

void YsShellExtDrawingBuffer::RemakeSelectedFaceGroupBufferTask::AddTriangle(const YsVec3 tri[3],int fgKey)
{
	if(YSTRUE!=viewPortEnabled && YSTRUE!=crossSectionEnabled)
	{
		selectedFaceGroupVtxBuffer.AddVertex(tri[0]);
		selectedFaceGroupVtxBuffer.AddVertex(tri[1]);
		selectedFaceGroupVtxBuffer.AddVertex(tri[2]);
		selectedFaceGroupIdxBuffer.Append(fgKey);
	}
	else
	{
		int nPlVt=3;
		const YsVec3 *plVt=tri;
		YsVec3 plVtAfterCrossSectionBuf[8];
		if(YSTRUE==crossSectionEnabled)
		{
			int newNp;
			YsClipPolygonByPlane(newNp,plVtAfterCrossSectionBuf,8,3,tri,crossSection,1);
			nPlVt=newNp;
			plVt=plVtAfterCrossSectionBuf;
		}

		YsVec3 plVtAfterViewPortBuf[32]; // 15 should be good enough.  Just in case.
		if(YSTRUE==viewPortEnabled)
		{
			int newNp;
			if(YSOK==YsClipPolygon3(newNp,plVtAfterViewPortBuf,32,nPlVt,plVt,viewPort[0],viewPort[1]))
			{
				nPlVt=newNp;
				plVt=plVtAfterViewPortBuf;
			}
			else
			{
				nPlVt=0;
			}
		}

		if(3<=nPlVt)
		{
			for(YSSIZE_T idx=1; idx<nPlVt-1; ++idx)
			{
				YsVec3 tri[3]={plVt[0],plVt[idx],plVt[idx+1]};
				selectedFaceGroupVtxBuffer.AddVertex(tri[0]);
				selectedFaceGroupVtxBuffer.AddVertex(tri[1]);
				selectedFaceGroupVtxBuffer.AddVertex(tri[2]);
				selectedFaceGroupIdxBuffer.Append(fgKey);
			}
		}
	}
}

void YsShellExtDrawingBuffer::RemakeSelectedFaceGroupBuffer(const class YsShellExt &shl,YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHdArray[])
{
	selectedFaceGroupVtxBuffer.CleanUp();
	selectedFaceGroupIdxBuffer.CleanUp();

	int nThread=(int)std::thread::hardware_concurrency();
	if(0==nThread)
	{
		nThread=4;
	}
	YsArray <YsShellExtDrawingBuffer::RemakeSelectedFaceGroupBufferTask,32> taskArray(nThread,NULL);
	YsArray <YsTask *,32> taskPtrArray(nThread,NULL);

	for(YSSIZE_T taskIdx=0; taskIdx<nThread; ++taskIdx)
	{
		taskArray[taskIdx].shlExtPtr=&shl;
		taskArray[taskIdx].viewPortEnabled=viewPortEnabled;
		taskArray[taskIdx].viewPort[0]=viewPort[0];
		taskArray[taskIdx].viewPort[1]=viewPort[1];
		taskArray[taskIdx].crossSectionEnabled=crossSectionEnabled;
		taskArray[taskIdx].crossSection=crossSection;
		taskPtrArray[taskIdx]=&taskArray[taskIdx];
	}

	int taskIdx=0;
	for(YSSIZE_T fgIdx=0; fgIdx<nFg; ++fgIdx)
	{
		YsArray <YsShellPolygonHandle,16> fgPlHd;
		shl.GetFaceGroup(fgPlHd,fgHdArray[fgIdx]);

		const int fgKey=shl.GetSearchKey(fgHdArray[fgIdx]);

		for(YSSIZE_T plIdx=0; plIdx<fgPlHd.GetN(); ++plIdx)
		{
			taskArray[taskIdx].plHdArray.Append(fgPlHd[plIdx]);
			taskArray[taskIdx].indexArray.Append(fgKey);
			++taskIdx;
			if(nThread<=taskIdx)
			{
				taskIdx=0;
			}
		}
	}

#ifdef YS_RUN_PARALLEL
	YsRunTaskParallel(taskPtrArray);
#else
	for(int i=0; i<nThread; ++i) // Will launch in parallel soon.
	{
		taskArray[i].StartLocal();
	}
#endif

	for(YSSIZE_T taskIdx=0; taskIdx<nThread; ++taskIdx)
	{
		selectedFaceGroupVtxBuffer.AddAndCleanUpIncoming(taskArray[taskIdx].selectedFaceGroupVtxBuffer);
		selectedFaceGroupIdxBuffer.Append(taskArray[taskIdx].selectedFaceGroupIdxBuffer);
	}
}

////////////////////////////////////////////////////////////

YsColor YsShellExtDrawingBuffer::ColorGradation(const double t)
{
	if(t<0.7f)
	{
		const double s=t/0.7;
		return YsColor(s,0.0,1.0,1.0);
	}
	else
	{
		const double s=(t-0.7f)/0.3f;
		return YsColor(1.0,0.0f,1.0-s,1.0);
	}

}

YsColor YsShellExtDrawingBuffer::ColorGradation(YSSIZE_T idx,YSSIZE_T n)
{
	if(1<n)
	{
		const double t=(double)idx/(double)(n-1);
		return ColorGradation(t);
	}
	else
	{
		return YsBlue();
	}
}
