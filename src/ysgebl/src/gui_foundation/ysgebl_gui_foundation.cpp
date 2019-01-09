/* ////////////////////////////////////////////////////////////

File Name: ysgebl_gui_foundation.cpp
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

#include "ysgebl_gui_foundation.h"

GeblGuiFoundation::GeblGuiFoundation()
{
	vertexPickRadius=5.0f;
	normalVertexMarkerRadius=5.0f;
	roundVertexMarkerRadius=7.0f;
	selectedVertexMarkerRadius=10.0;
	polygonShrinkRatioForPicking=0.7f;

	slHd=NULL;

	actuallyDrawVertex=YSTRUE;
	actuallyDrawConstEdge=YSTRUE;
	actuallyDrawPolygonEdge=YSTRUE;
	actuallyDrawPolygon=YSTRUE;
	actuallyDrawShrunkPolygon=YSTRUE;
	actuallyDrawOtherShell=YSTRUE;
}


YsShellExtEditGui *GeblGuiFoundation::Slhd(void)
{
	return slHd;
}

const YsShellExtEditGui *GeblGuiFoundation::Slhd(void) const
{
	return slHd;
}

YsShellDnmContainer <YsShellExtEditGui> &GeblGuiFoundation::ShlGrp(void)
{
	return shlGrp;
}
const YsShellDnmContainer <YsShellExtEditGui> &GeblGuiFoundation::ShlGrp(void) const
{
	return shlGrp;
}

YsShellVertexHandle GeblGuiFoundation::PickedVtHd(int mx,int my,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[],const YsMatrix4x4 &modelTfm) const
{
	YsShellVertexHandle candidateVtHd=NULL;
	if(NULL!=slHd)
	{
		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		YsKeyStore exclusion;
		for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		{
			exclusion.AddKey(slHd->GetSearchKey(excludeVtHd[idx]));
		}

		double candidateDepth=0.0;

		for(int iter=0; iter<2; ++iter)
		{
			const YsGLVertexBuffer &vtxBuf=(0==iter ? drawBuf.normalVertexPosBuffer : drawBuf.roundVertexPosBuffer);
			const YsArray <int> &idxBuf=(0==iter ? drawBuf.normalVertexIdxBuffer : drawBuf.roundVertexIdxBuffer);

			double z;
			const int pickedIdx=PickedIndexInPointBuffer(z,mx,my,vtxBuf,idxBuf,exclusion,modelTfm);
			if(0<=pickedIdx)
			{
				YsShellVertexHandle vtHd=slHd->FindVertex(pickedIdx);
				if(NULL!=vtHd && (NULL==candidateVtHd || z<candidateDepth))
				{
					candidateVtHd=vtHd;
					candidateDepth=z;
				}
			}
		}
	}
	return candidateVtHd;
}

YsShellVertexHandle GeblGuiFoundation::PickedVtHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const
{
	if(NULL!=slHd)
	{
		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		YsKeyStore exclusion;

		double z;
		const int pickedIdx=PickedIndexInPointBuffer(z,mx,my,drawBuf.selectedVertexPosBuffer,drawBuf.selectedVertexIdxBuffer,exclusion,modelTfm);
		if(0<=pickedIdx)
		{
			YsShellVertexHandle vtHd=slHd->FindVertex(pickedIdx);
			return vtHd;
		}
	}
	return nullptr;
}

YsArray <YsPair <YsShellVertexHandle,double> > GeblGuiFoundation::BoxedVtHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[],const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <YsShellVertexHandle,double> > vtHdDepthPair;
	if(NULL!=slHd)
	{
		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		YsKeyStore exclusion;
		for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		{
			exclusion.AddKey(slHd->GetSearchKey(excludeVtHd[idx]));
		}

		YsArray <YsPair <int,double> > vtKeyDepthPair[2]=
		{
			BoxedIndexInPointBuffer(x0,y0,x1,y1,drawBuf.normalVertexPosBuffer,drawBuf.normalVertexIdxBuffer,exclusion,modelTfm),
			BoxedIndexInPointBuffer(x0,y0,x1,y1,drawBuf.roundVertexPosBuffer,drawBuf.roundVertexIdxBuffer,exclusion,modelTfm)
		};
		for(auto &pairArray : vtKeyDepthPair)
		{
			for(auto pair : pairArray)
			{
				YsShellVertexHandle vtHd=slHd->FindVertex(pair.a);
				if(NULL!=vtHd)
				{
					vtHdDepthPair.Increment();
					vtHdDepthPair.Last().a=vtHd;
					vtHdDepthPair.Last().b=pair.b;
				}
			}
		}
	}
	return vtHdDepthPair;
}

YsArray <YsPair <YsShellVertexHandle,double> > GeblGuiFoundation::BoxedVtHdAmongSelection(int x0,int y0,int x1,int y1,const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <YsShellVertexHandle,double> > vtHdDepthPair;
	if(NULL!=slHd)
	{
		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		YsKeyStore exclusion;

		YsArray <YsPair <int,double> > vtKeyDepthPair=
			BoxedIndexInPointBuffer(x0,y0,x1,y1,drawBuf.selectedVertexPosBuffer,drawBuf.selectedVertexIdxBuffer,exclusion,modelTfm);
		for(auto pair : vtKeyDepthPair)
		{
			YsShellVertexHandle vtHd=slHd->FindVertex(pair.a);
			if(NULL!=vtHd)
			{
				vtHdDepthPair.Increment();
				vtHdDepthPair.Last().a=vtHd;
				vtHdDepthPair.Last().b=pair.b;
			}
		}
	}
	return vtHdDepthPair;
}

YsShellPolygonHandle GeblGuiFoundation::PickedPlHd(int mx,int my,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[],const YsMatrix4x4 &modelTfm) const
{
	YsShellPolygonHandle candidatePlHd=NULL;
	if(NULL!=slHd)
	{
		YsKeyStore exclusion;
		for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		{
			exclusion.AddKey(slHd->GetSearchKey(excludePlHd[idx]));
		}

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();
		double candidateDepth=0.0;

		if(YSTRUE==actuallyDrawShrunkPolygon)
		{
			double depth;
			const int idx=PickedIndexInEdgeBuffer(depth,mx,my,drawBuf.shrunkEdgePosBuffer,drawBuf.shrunkEdgeIdxBuffer,exclusion,modelTfm);
			YsShellPolygonHandle plHd;
			if(0<=idx && NULL!=(plHd=slHd->FindPolygon(idx)) && (NULL==candidatePlHd || depth<candidateDepth))
			{
				candidateDepth=depth;
				candidatePlHd=plHd;
			}
		}

		if(YSTRUE==actuallyDrawPolygon)
		{
			for(int i=0; i<6; ++i)
			{
				const YsGLVertexBuffer *vtxBufPtr;
				const YsArray <int> *idxBufPtr;

				switch(i)
				{
				case 0:
					vtxBufPtr=&drawBuf.solidShadedVtxBuffer;
					idxBufPtr=&drawBuf.solidShadedIdxBuffer;
					break;
				case 1:
					vtxBufPtr=&drawBuf.solidUnshadedVtxBuffer;
					idxBufPtr=&drawBuf.solidUnshadedIdxBuffer;
					break;
				case 2:
					vtxBufPtr=&drawBuf.trspShadedVtxBuffer;
					idxBufPtr=&drawBuf.trspShadedIdxBuffer;
					break;
				case 3:
					vtxBufPtr=&drawBuf.backFaceVtxBuffer;
					idxBufPtr=&drawBuf.backFaceIdxBuffer;
					break;
				case 4:
					vtxBufPtr=&drawBuf.trspUnshadedVtxBuffer;
					idxBufPtr=&drawBuf.trspUnshadedIdxBuffer;
					break;
				case 5:
					vtxBufPtr=&drawBuf.invisibleButPickablePolygonVtxBuffer;
					idxBufPtr=&drawBuf.invisibleButPickablePolygonIdxBuffer;
					break;
				default:
					continue;
				}

				double depth;
				const int idx=PickedIndexInTriangleBuffer(depth,mx,my,*vtxBufPtr,*idxBufPtr,NULL,&exclusion,modelTfm);
				YsShellPolygonHandle plHd;
				if(0<=idx && NULL!=(plHd=slHd->FindPolygon(idx)) && (NULL==candidatePlHd || depth<candidateDepth))
				{
					candidateDepth=depth;
					candidatePlHd=plHd;
				}
			}
		}
	}
	return candidatePlHd;
}

YsShellPolygonHandle GeblGuiFoundation::PickedPlHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const
{
	YsShellPolygonHandle candidatePlHd=NULL;
	if(NULL!=slHd)
	{
		YsKeyStore exclusion;

		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);
		YsKeyStore selected;
		for(auto plHd : selPlHd)
		{
			selected.AddKey(slHd->GetSearchKey(plHd));
		}

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();
		double candidateDepth=0.0;

		{
			double depth;
			const int idx=PickedIndexInEdgeBuffer(depth,mx,my,drawBuf.selectedPolygonPosBuffer,drawBuf.selectedPolygonIdxBuffer,exclusion,modelTfm);
			YsShellPolygonHandle plHd;
			if(0<=idx && NULL!=(plHd=slHd->FindPolygon(idx)) && (NULL==candidatePlHd || depth<candidateDepth))
			{
				candidateDepth=depth;
				candidatePlHd=plHd;
			}
		}

		if(YSTRUE==actuallyDrawPolygon)
		{
			for(int i=0; i<5; ++i)
			{
				const YsGLVertexBuffer &vtxBuf=
				   (i==0 ? drawBuf.solidShadedVtxBuffer :
				   (i==1 ? drawBuf.solidUnshadedVtxBuffer :
				   (i==2 ? drawBuf.trspShadedVtxBuffer :
				   (i==3 ? drawBuf.backFaceVtxBuffer :
				          (drawBuf.trspUnshadedVtxBuffer)))));
				const YsArray <int> &idxBuf=
				   (i==0 ? drawBuf.solidShadedIdxBuffer :
				   (i==1 ? drawBuf.solidUnshadedIdxBuffer :
				   (i==2 ? drawBuf.trspShadedIdxBuffer :
				   (i==3 ? drawBuf.backFaceIdxBuffer :
				          (drawBuf.trspUnshadedIdxBuffer)))));

				double depth;
				const int idx=PickedIndexInTriangleBuffer(depth,mx,my,vtxBuf,idxBuf,&selected,nullptr,modelTfm);
				YsShellPolygonHandle plHd;
				if(0<=idx && NULL!=(plHd=slHd->FindPolygon(idx)) && (NULL==candidatePlHd || depth<candidateDepth))
				{
					candidateDepth=depth;
					candidatePlHd=plHd;
				}
			}
		}
	}
	return candidatePlHd;
}

YsArray <YsPair <YsShellPolygonHandle,double> > GeblGuiFoundation::EnclosedPlHd(YsConstArrayMask <YsVec2> strokePnt,const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <YsShellPolygonHandle,double> > plHdDepthPairArray;

	YsShell2d strokeShl;
	{
		YsArray <YsShell2dVertexHandle> allVtHd;
		for(auto p : strokePnt)
		{
			allVtHd.push_back(strokeShl.AddVertexH(p));
		}
		for(YSSIZE_T i=0; i<allVtHd.size(); ++i)
		{
			strokeShl.AddEdgeH(allVtHd[i],allVtHd.GetCyclic(i+1));
		}
	}

	YsShell2dLattice strokeLtc;
	strokeLtc.SetDomain(strokeShl,strokePnt.size()+1);

	if(NULL!=slHd)
	{
		YsKeyStore exclusion;
		// for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		// {
		// 	exclusion.AddKey(slHd->GetSearchKey(excludePlHd[idx]));
		// }

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		if(YSTRUE==actuallyDrawShrunkPolygon)
		{
			for(auto pair : EnclosedIndexInEdgeBuffer(strokeShl,strokeLtc,drawBuf.shrunkEdgePosBuffer,drawBuf.shrunkEdgeIdxBuffer,NULL,&exclusion,modelTfm))
			{
				YsShellPolygonHandle plHd=slHd->FindPolygon(pair.a);
				if(NULL!=plHd)
				{
					plHdDepthPairArray.Increment();
					plHdDepthPairArray.Last().a=plHd;
					plHdDepthPairArray.Last().b=pair.b;
					exclusion.AddKey(pair.a);
				}
			}
		}

		for(auto pair : plHdDepthPairArray)
		{
			exclusion.AddKey(slHd->GetSearchKey(pair.a));
		}

		if(YSTRUE==actuallyDrawPolygon)
		{
			for(int i=0; i<5; ++i)
			{
				const YsGLVertexBuffer &vtxBuf=
				   (i==0 ? drawBuf.solidShadedVtxBuffer :
				   (i==1 ? drawBuf.solidUnshadedVtxBuffer :
				   (i==2 ? drawBuf.trspShadedVtxBuffer :
				   (i==3 ? drawBuf.backFaceVtxBuffer :
				          (drawBuf.trspUnshadedVtxBuffer)))));
				const YsArray <int> &idxBuf=
				   (i==0 ? drawBuf.solidShadedIdxBuffer :
				   (i==1 ? drawBuf.solidUnshadedIdxBuffer :
				   (i==2 ? drawBuf.trspShadedIdxBuffer :
				   (i==3 ? drawBuf.backFaceIdxBuffer :
				          (drawBuf.trspUnshadedIdxBuffer)))));


				for(auto pair : EnclosedIndexInTriangleBuffer(strokeShl,strokeLtc,vtxBuf,idxBuf,NULL,&exclusion,modelTfm))
				{
					YsShellPolygonHandle plHd=slHd->FindPolygon(pair.a);
					if(NULL!=plHd)
					{
						plHdDepthPairArray.Increment();
						plHdDepthPairArray.Last().a=plHd;
						plHdDepthPairArray.Last().b=pair.b;
						exclusion.AddKey(pair.a);
					}
				}
			}
		}
	}

	return plHdDepthPairArray;
}

YsArray <YsPair <YsShellPolygonHandle,double> > GeblGuiFoundation::BoxedPlHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[],const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <YsShellPolygonHandle,double> > plHdDepthPairArray;
	if(NULL!=slHd)
	{
		YsKeyStore exclusion;
		for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		{
			exclusion.AddKey(slHd->GetSearchKey(excludePlHd[idx]));
		}

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		if(YSTRUE==actuallyDrawShrunkPolygon)
		{
			YsArray <YsPair <int,double> > boxed=BoxedIndexInEdgeBuffer(x0,y0,x1,y1,drawBuf.shrunkEdgePosBuffer,drawBuf.shrunkEdgeIdxBuffer,NULL,&exclusion,modelTfm);
			for(auto pair : boxed)
			{
				YsShellPolygonHandle plHd=slHd->FindPolygon(pair.a);
				if(NULL!=plHd)
				{
					plHdDepthPairArray.Increment();
					plHdDepthPairArray.Last().a=plHd;
					plHdDepthPairArray.Last().b=pair.b;
					exclusion.AddKey(pair.a);
				}
			}
		}

		for(auto pair : plHdDepthPairArray)
		{
			exclusion.AddKey(slHd->GetSearchKey(pair.a));
		}

		if(YSTRUE==actuallyDrawPolygon)
		{
			for(int i=0; i<5; ++i)
			{
				const YsGLVertexBuffer &vtxBuf=
				   (i==0 ? drawBuf.solidShadedVtxBuffer :
				   (i==1 ? drawBuf.solidUnshadedVtxBuffer :
				   (i==2 ? drawBuf.trspShadedVtxBuffer :
				   (i==3 ? drawBuf.backFaceVtxBuffer :
				          (drawBuf.trspUnshadedVtxBuffer)))));
				const YsArray <int> &idxBuf=
				   (i==0 ? drawBuf.solidShadedIdxBuffer :
				   (i==1 ? drawBuf.solidUnshadedIdxBuffer :
				   (i==2 ? drawBuf.trspShadedIdxBuffer :
				   (i==3 ? drawBuf.backFaceIdxBuffer :
				          (drawBuf.trspUnshadedIdxBuffer)))));


				YsArray <YsPair <int,double> > boxed=BoxedIndexInTriangleBuffer(x0,y0,x1,y1,vtxBuf,idxBuf,NULL,&exclusion,modelTfm);

				for(auto pair : boxed)
				{
					YsShellPolygonHandle plHd=slHd->FindPolygon(pair.a);
					if(NULL!=plHd)
					{
						plHdDepthPairArray.Increment();
						plHdDepthPairArray.Last().a=plHd;
						plHdDepthPairArray.Last().b=pair.b;
						exclusion.AddKey(pair.a);
					}
				}
			}
		}
	}
	return plHdDepthPairArray;
}

YsArray <YsPair <YsShellPolygonHandle,double> > GeblGuiFoundation::BoxedPlHdAmongSelection(int x0,int y0,int x1,int y1,const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <YsShellPolygonHandle,double> > plHdDepthPairArray;
	if(NULL!=slHd)
	{
		YsArray <YsShellPolygonHandle> selPlHd;
		slHd->GetSelectedPolygon(selPlHd);

		YsKeyStore selected,exclusion;
		for(auto plHd : selPlHd)
		{
			selected.AddKey(slHd->GetSearchKey(plHd));
		}

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		{
			YsArray <YsPair <int,double> > boxed=BoxedIndexInEdgeBuffer(x0,y0,x1,y1,drawBuf.selectedPolygonPosBuffer,drawBuf.selectedPolygonIdxBuffer,&selected,&exclusion,modelTfm);
			for(auto pair : boxed)
			{
				YsShellPolygonHandle plHd=slHd->FindPolygon(pair.a);
				if(NULL!=plHd)
				{
					plHdDepthPairArray.Increment();
					plHdDepthPairArray.Last().a=plHd;
					plHdDepthPairArray.Last().b=pair.b;
					exclusion.AddKey(pair.a);
				}
			}
		}

		for(auto pair : plHdDepthPairArray)
		{
			exclusion.AddKey(slHd->GetSearchKey(pair.a));
		}

		if(YSTRUE==actuallyDrawPolygon)
		{
			for(int i=0; i<5; ++i)
			{
				const YsGLVertexBuffer &vtxBuf=
				   (i==0 ? drawBuf.solidShadedVtxBuffer :
				   (i==1 ? drawBuf.solidUnshadedVtxBuffer :
				   (i==2 ? drawBuf.trspShadedVtxBuffer :
				   (i==3 ? drawBuf.backFaceVtxBuffer :
				          (drawBuf.trspUnshadedVtxBuffer)))));
				const YsArray <int> &idxBuf=
				   (i==0 ? drawBuf.solidShadedIdxBuffer :
				   (i==1 ? drawBuf.solidUnshadedIdxBuffer :
				   (i==2 ? drawBuf.trspShadedIdxBuffer :
				   (i==3 ? drawBuf.backFaceIdxBuffer :
				          (drawBuf.trspUnshadedIdxBuffer)))));


				YsArray <YsPair <int,double> > boxed=BoxedIndexInTriangleBuffer(x0,y0,x1,y1,vtxBuf,idxBuf,&selected,&exclusion,modelTfm);

				for(auto pair : boxed)
				{
					YsShellPolygonHandle plHd=slHd->FindPolygon(pair.a);
					if(NULL!=plHd)
					{
						plHdDepthPairArray.Increment();
						plHdDepthPairArray.Last().a=plHd;
						plHdDepthPairArray.Last().b=pair.b;
						exclusion.AddKey(pair.a);
					}
				}
			}
		}
	}
	return plHdDepthPairArray;
}

YsShellExt::ConstEdgeHandle GeblGuiFoundation::PickedCeHd(int mx,int my,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[],const YsMatrix4x4 &modelTfm) const
{
	YsShellExt::ConstEdgeHandle candidateCeHd=NULL;
	if(NULL!=slHd)
	{
		YsKeyStore exclusion;
		for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		{
			exclusion.AddKey(slHd->GetSearchKey(excludeCeHd[idx]));
		}

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();


		double depth;
		const int idx=PickedIndexInEdgeBuffer(depth,mx,my,drawBuf.constEdgeVtxBuffer,drawBuf.constEdgeIdxBuffer,exclusion,modelTfm);
		if(0<=idx)
		{
			candidateCeHd=slHd->FindConstEdge(idx);
			printf("PickedCeKey=%d\n",slHd->GetSearchKey(candidateCeHd));
		}
	}
	return candidateCeHd;
}

YsShellExt::ConstEdgeHandle GeblGuiFoundation::PickedCeHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const
{
	if(NULL!=slHd)
	{
		YsKeyStore exclusion;

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		double depth;
		const int idx=PickedIndexInEdgeBuffer(depth,mx,my,drawBuf.selectedConstEdgeVtxBuffer,drawBuf.selectedConstEdgeIdxBuffer,exclusion,modelTfm);
		if(0<=idx)
		{
			return slHd->FindConstEdge(idx);
		}
	}
	return NULL;
}

YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > GeblGuiFoundation::BoxedCeHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[],const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > ceHdDepthPairArray;
	if(NULL!=slHd)
	{
		YsKeyStore exclusion;
		for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		{
			exclusion.AddKey(slHd->GetSearchKey(excludeCeHd[idx]));
		}

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		YsArray <YsPair <int,double> > boxed=BoxedIndexInEdgeBuffer(x0,y0,x1,y1,drawBuf.constEdgeVtxBuffer,drawBuf.constEdgeIdxBuffer,NULL,&exclusion,modelTfm);
		for(auto pair : boxed)
		{
			auto ceHd=slHd->FindConstEdge(pair.a);
			if(NULL!=ceHd)
			{
				ceHdDepthPairArray.Increment();
				ceHdDepthPairArray.Last().a=ceHd;
				ceHdDepthPairArray.Last().b=pair.b;
			}
		}
	}
	return ceHdDepthPairArray;
}

YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > GeblGuiFoundation::BoxedCeHdAmongSelection(int x0,int y0,int x1,int y1,const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > ceHdDepthPairArray;
	if(NULL!=slHd)
	{
		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();

		YsArray <YsPair <int,double> > boxed=BoxedIndexInEdgeBuffer(x0,y0,x1,y1,drawBuf.selectedConstEdgeVtxBuffer,drawBuf.selectedConstEdgeIdxBuffer,NULL,NULL,modelTfm);
		for(auto pair : boxed)
		{
			auto ceHd=slHd->FindConstEdge(pair.a);
			if(NULL!=ceHd)
			{
				ceHdDepthPairArray.Increment();
				ceHdDepthPairArray.Last().a=ceHd;
				ceHdDepthPairArray.Last().b=pair.b;
			}
		}
	}
	return ceHdDepthPairArray;
}

YsShellExt::FaceGroupHandle GeblGuiFoundation::PickedFgHd(int mx,int my,int nExclude,const YsShellExt::FaceGroupHandle excludeFgHd[],const YsMatrix4x4 &modelTfm) const
{
	YsShellExt::FaceGroupHandle candidateFgHd=NULL;
	if(NULL!=slHd)
	{
		YsKeyStore excludeFgKey;
		for(YSSIZE_T idx=0; idx<nExclude; ++idx)
		{
			excludeFgKey.AddKey(slHd->GetSearchKey(excludeFgHd[idx]));
		}



		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();
		double candidateDepth=0.0;

		if(YSTRUE==actuallyDrawShrunkPolygon)
		{
			for(YSSIZE_T edIdx=0; edIdx<=drawBuf.shrunkEdgePosBuffer.GetNumVertex()-2; edIdx+=2)
			{
				YsVec3 edVtPos[2]=
				{
					YsVec3(drawBuf.shrunkEdgePosBuffer[edIdx  ][0],drawBuf.shrunkEdgePosBuffer[edIdx  ][1],drawBuf.shrunkEdgePosBuffer[edIdx  ][2]),
					YsVec3(drawBuf.shrunkEdgePosBuffer[edIdx+1][0],drawBuf.shrunkEdgePosBuffer[edIdx+1][1],drawBuf.shrunkEdgePosBuffer[edIdx+1][2]),
				};

				edVtPos[0]=modelTfm*edVtPos[0];
				edVtPos[1]=modelTfm*edVtPos[1];

				double depth;
				if(YSTRUE==drawEnv.IsLinePicked(depth,mx,my,10,edVtPos[0],edVtPos[1]))
				{
					YsShellPolygonHandle plHd=slHd->FindPolygon(drawBuf.shrunkEdgeIdxBuffer[edIdx/2]);
					YsShellExt::FaceGroupHandle fgHd=slHd->FindFaceGroupFromPolygon(plHd);

					if(NULL==candidateFgHd || depth<candidateDepth)
					{
						if(NULL!=fgHd && YSTRUE!=excludeFgKey.IsIncluded(slHd->GetSearchKey(fgHd)))
						{
							candidateFgHd=fgHd;
							candidateDepth=depth;
						}
					}
				}
			}
		}

		if(YSTRUE==actuallyDrawPolygon)
		{
			for(int i=0; i<5; ++i)
			{
				const YsGLVertexBuffer &vtxBuf=
				   (i==0 ? drawBuf.solidShadedVtxBuffer :
				   (i==1 ? drawBuf.solidUnshadedVtxBuffer :
				   (i==2 ? drawBuf.trspShadedVtxBuffer :
				   (i==3 ? drawBuf.backFaceVtxBuffer :
				          (drawBuf.trspUnshadedVtxBuffer)))));
				const YsArray <int> &idxBuf=
				   (i==0 ? drawBuf.solidShadedIdxBuffer :
				   (i==1 ? drawBuf.solidUnshadedIdxBuffer :
				   (i==2 ? drawBuf.trspShadedIdxBuffer :
				   (i==3 ? drawBuf.backFaceIdxBuffer :
				          (drawBuf.trspUnshadedIdxBuffer)))));

				for(YSSIZE_T plIdx=0; plIdx<=vtxBuf.GetNumVertex()-3; plIdx+=3)
				{
					YsVec3 triVtPos[3]=
					{
						YsVec3(vtxBuf[plIdx  ][0],vtxBuf[plIdx  ][1],vtxBuf[plIdx  ][2]),
						YsVec3(vtxBuf[plIdx+1][0],vtxBuf[plIdx+1][1],vtxBuf[plIdx+1][2]),
						YsVec3(vtxBuf[plIdx+2][0],vtxBuf[plIdx+2][1],vtxBuf[plIdx+2][2])
					};

					triVtPos[0]=modelTfm*triVtPos[0];
					triVtPos[1]=modelTfm*triVtPos[1];
					triVtPos[2]=modelTfm*triVtPos[2];

					double depth;
					if(YSTRUE==drawEnv.IsPolygonPicked(depth,mx,my,3,triVtPos))
					{
						if(NULL==candidateFgHd || depth<candidateDepth)
						{
							YsShellPolygonHandle plHd=slHd->FindPolygon(idxBuf[plIdx/3]);
							YsShellExt::FaceGroupHandle fgHd=slHd->FindFaceGroupFromPolygon(plHd);
							if(NULL!=fgHd && YSTRUE!=excludeFgKey.IsIncluded(slHd->GetSearchKey(fgHd)))
							{
								candidateFgHd=fgHd;
								candidateDepth=depth;
							}
						}
					}
				}
			}
		}
	}
	return candidateFgHd;
}

YsShellExt::FaceGroupHandle GeblGuiFoundation::PickedFgHdAmongSelection(int mx,int my,const YsMatrix4x4 &modelTfm) const
{
	YsShellExt::FaceGroupHandle candidateFgHd=NULL;
	if(NULL!=slHd)
	{
		YsKeyStore excludeFgKey;

		YsShellExtDrawingBuffer &drawBuf=slHd->GetDrawingBuffer();
		double candidateDepth=0.0;

		auto index=PickedIndexInTriangleBuffer(candidateDepth,mx,my,drawBuf.selectedFaceGroupVtxBuffer,drawBuf.selectedFaceGroupIdxBuffer,NULL,NULL,modelTfm);
		candidateFgHd=slHd->FindFaceGroup(index);
	}
	return candidateFgHd;
}

const YsShellDnmContainer <YsShellExtEditGui>::Node *GeblGuiFoundation::PickedSlHd(
	int mx,int my,
	YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[],
	const YsShellDnmContainer <YsShellExtEditGui>::DnmState *dnmState) const
{
	YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> allNode;
	shlGrp.GetNodePointerAll(allNode);

	if(0<allNode.GetN())
	{
		double candidateDepth=0.0;
		const YsShellDnmContainer <YsShellExtEditGui>::Node *candidateSlHd=NULL;

		for(YSSIZE_T slIdx=0; slIdx<allNode.GetN(); ++slIdx)
		{
			if(YSTRUE==YsIsIncluded(nExclude,excludeSlHd,allNode[slIdx]))
			{
				continue;
			}

			YsShellExtDrawingBuffer &drawBuf=allNode[slIdx]->GetDrawingBuffer();
			YsGLVertexBuffer &vtxBuf=drawBuf.normalEdgePosBuffer;

			YsMatrix4x4 modelTfm;
			if(nullptr!=dnmState)
			{
				modelTfm=dnmState->GetNodeToRootTransformation(allNode[slIdx]);
			}

			for(YSSIZE_T edIdx=0; edIdx<=vtxBuf.GetNumVertex()-2; edIdx+=2)
			{
				YsVec3 edVtPos[2]=
				{
					YsVec3(vtxBuf[edIdx  ][0],vtxBuf[edIdx  ][1],vtxBuf[edIdx  ][2]),
					YsVec3(vtxBuf[edIdx+1][0],vtxBuf[edIdx+1][1],vtxBuf[edIdx+1][2]),
				};

				edVtPos[0]=modelTfm*edVtPos[0];
				edVtPos[1]=modelTfm*edVtPos[1];

				double depth;
				if(YSTRUE==drawEnv.IsLinePicked(depth,mx,my,10,edVtPos[0],edVtPos[1]))
				{
					if(NULL==candidateSlHd || depth<candidateDepth)
					{
						candidateSlHd=allNode[slIdx];
						candidateDepth=depth;
					}
				}
			}
		}

		return candidateSlHd;
	}

	return NULL;
}

YsShellDnmContainer <YsShellExtEditGui>::Node *GeblGuiFoundation::PickedSlHd(
	int mx,int my,
	YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[],
	const YsShellDnmContainer <YsShellExtEditGui>::DnmState *dnmState)
{
	const auto *constPtr=this;
	const YsShellDnmContainer <YsShellExtEditGui>::Node *pickedSlHd=constPtr->PickedSlHd(mx,my,nExclude,excludeSlHd,dnmState);
	return (YsShellDnmContainer <YsShellExtEditGui>::Node *)pickedSlHd;
}

int GeblGuiFoundation::PickedIndexInPointBuffer(double &z,int mx,int my,const YsGLVertexBuffer &pointVtxBuffer,const YsArray <int> &pointIdxBuffer,const YsKeyStore &exclusion,const YsMatrix4x4 &modelTfm) const
{
	int candidateIdx=-1;
	double candidateDepth=0.0;

	for(YSSIZE_T vtIdx=0; vtIdx<pointVtxBuffer.GetNumVertex(); ++vtIdx)
	{
		if(YSTRUE!=exclusion.IsIncluded(pointIdxBuffer[vtIdx]))
		{
			YsVec3 vtPos(pointVtxBuffer[vtIdx][0],pointVtxBuffer[vtIdx][1],pointVtxBuffer[vtIdx][2]);

			vtPos=modelTfm*vtPos;

			double z;
			if(YSTRUE==drawEnv.IsPointPicked(z,mx,my,(int)vertexPickRadius,vtPos))
			{
				if(0>candidateIdx || candidateDepth>z)
				{
					candidateIdx=pointIdxBuffer[vtIdx];
					candidateDepth=z;
				}
			}
		}
	}

	z=candidateDepth;
	return candidateIdx;
}

int GeblGuiFoundation::PickedIndexInEdgeBuffer(double &z,int mx,int my,const YsGLVertexBuffer &edgeVtxBuffer,const YsArray <int> &edgeIdxBuffer,const YsKeyStore &exclusion,const YsMatrix4x4 &modelTfm) const
{
	int candidateIndex=-1;
	double candidateDepth=0.0;

	for(YSSIZE_T edIdx=0; edIdx<=edgeVtxBuffer.GetNumVertex()-2; edIdx+=2)
	{
		if(YSTRUE!=exclusion.IsIncluded((YSHASHKEY)edgeIdxBuffer[edIdx/2]))
		{
			YsVec3 edVtPos[2]=
			{
				YsVec3(edgeVtxBuffer[edIdx  ][0],edgeVtxBuffer[edIdx  ][1],edgeVtxBuffer[edIdx  ][2]),
				YsVec3(edgeVtxBuffer[edIdx+1][0],edgeVtxBuffer[edIdx+1][1],edgeVtxBuffer[edIdx+1][2]),
			};

			edVtPos[0]=modelTfm*edVtPos[0];
			edVtPos[1]=modelTfm*edVtPos[1];

			double depth;
			if(YSTRUE==drawEnv.IsLinePicked(depth,mx,my,10,edVtPos[0],edVtPos[1]))
			{
				if(candidateIndex<0 || depth<candidateDepth)
				{
					candidateIndex=edgeIdxBuffer[edIdx/2];
					candidateDepth=depth;
				}
			}
		}
	}

	z=candidateDepth;
	return candidateIndex;
}

int GeblGuiFoundation::PickedIndexInTriangleBuffer(
    double &z,int mx,int my,const YsGLVertexBuffer &triVtxBuffer,const YsArray <int> &triIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const
{
	int candidateIndex=-1;
	double candidateDepth=YsInfinity;

	for(YSSIZE_T plIdx=0; plIdx<=triVtxBuffer.GetNumVertex()-3; plIdx+=3)
	{
		if(NULL!=limit && YSTRUE!=limit->IsIncluded(triIdxBuffer[plIdx/3]))
		{
			continue;
		}

		if(NULL==exclusion || YSTRUE!=exclusion->IsIncluded(triIdxBuffer[plIdx/3]))
		{
			YsVec3 triVtPos[3]=
			{
				YsVec3(triVtxBuffer[plIdx  ][0],triVtxBuffer[plIdx  ][1],triVtxBuffer[plIdx  ][2]),
				YsVec3(triVtxBuffer[plIdx+1][0],triVtxBuffer[plIdx+1][1],triVtxBuffer[plIdx+1][2]),
				YsVec3(triVtxBuffer[plIdx+2][0],triVtxBuffer[plIdx+2][1],triVtxBuffer[plIdx+2][2])
			};

			triVtPos[0]=modelTfm*triVtPos[0];
			triVtPos[1]=modelTfm*triVtPos[1];
			triVtPos[2]=modelTfm*triVtPos[2];

			double depth;
			if(YSTRUE==drawEnv.IsPolygonPicked(depth,mx,my,3,triVtPos))
			{
				if(candidateIndex<0 || depth<candidateDepth)
				{
					candidateIndex=triIdxBuffer[plIdx/3];
					candidateDepth=depth;
				}
			}
		}
	}

	z=candidateDepth;
	return candidateIndex;
}

YsArray <YsPair <int,double> >  GeblGuiFoundation::BoxedIndexInPointBuffer(
    int x0,int y0,int x1,int y1,
    const YsGLVertexBuffer &pointVtxBuffer,
    const YsArray <int> &pointIdxBuffer,
    const YsKeyStore &exclusion,
    const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <int,double> >  indexDepthPairArray;

	if(NULL!=slHd)
	{
		for(YSSIZE_T vtIdx=0; vtIdx<pointVtxBuffer.GetNumVertex(); ++vtIdx)
		{
			if(YSTRUE!=exclusion.IsIncluded(pointIdxBuffer[vtIdx]))
			{
				YsVec3 vtPos(pointVtxBuffer[vtIdx][0],pointVtxBuffer[vtIdx][1],pointVtxBuffer[vtIdx][2]);

				vtPos=modelTfm*vtPos;

				double z;
				if(YSTRUE==drawEnv.IsPointWithinRect(z,x0,y0,x1,y1,vtPos))
				{
					indexDepthPairArray.Increment();
					indexDepthPairArray.Last().a=pointIdxBuffer[vtIdx];
					indexDepthPairArray.Last().b=z;
				}
			}
		}
	}
	return indexDepthPairArray;
}

YsArray <YsPair <int,double> > GeblGuiFoundation::BoxedIndexInEdgeBuffer(
    int x0,int y0,int x1,int y1,const YsGLVertexBuffer &edgeVtxBuffer,const YsArray <int> &edgeIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,
    const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <int,double> > indexDepthPairArray;

	if(NULL!=slHd)
	{
		for(YSSIZE_T edIdx=0; edIdx<=edgeVtxBuffer.GetNumVertex()-2; edIdx+=2)
		{
			if(NULL!=limit && YSTRUE!=limit->IsIncluded((YSHASHKEY)edgeIdxBuffer[edIdx/2]))
			{
				continue;
			}

			if(NULL==exclusion || YSTRUE!=exclusion->IsIncluded((YSHASHKEY)edgeIdxBuffer[edIdx/2]))
			{
				YsVec3 edVtPos[2]=
				{
					YsVec3(edgeVtxBuffer[edIdx  ][0],edgeVtxBuffer[edIdx  ][1],edgeVtxBuffer[edIdx  ][2]),
					YsVec3(edgeVtxBuffer[edIdx+1][0],edgeVtxBuffer[edIdx+1][1],edgeVtxBuffer[edIdx+1][2]),
				};

				edVtPos[0]=modelTfm*edVtPos[0];
				edVtPos[1]=modelTfm*edVtPos[1];

				double depth;
				if(YSTRUE==drawEnv.IsLineWithinRect(depth,x0,y0,x1,y1,edVtPos))
				{
					indexDepthPairArray.Increment();
					indexDepthPairArray.Last().a=edgeIdxBuffer[edIdx/2];
					indexDepthPairArray.Last().b=depth;
				}
			}
		}


		YsKeyStore mayBeIncluded;
		for(auto pair : indexDepthPairArray)
		{
			mayBeIncluded.AddKey(pair.a);
		}


		// When identifying a constraint edge, one piece may be within rect, but other pieces
		// maybe extending outside of the rect.  The following loop filters out const edges
		// that are extending outside of the rect and not fitting within the rect.
		YsKeyStore actuallyExtendOutside;
		for(YSSIZE_T edIdx=0; edIdx<=edgeVtxBuffer.GetNumVertex()-2; edIdx+=2)
		{
			if(YSTRUE==mayBeIncluded.IsIncluded(edgeIdxBuffer[edIdx/2]))
			{
				YsVec3 edVtPos[2]=
				{
					YsVec3(edgeVtxBuffer[edIdx  ][0],edgeVtxBuffer[edIdx  ][1],edgeVtxBuffer[edIdx  ][2]),
					YsVec3(edgeVtxBuffer[edIdx+1][0],edgeVtxBuffer[edIdx+1][1],edgeVtxBuffer[edIdx+1][2]),
				};

				edVtPos[0]=modelTfm*edVtPos[0];
				edVtPos[1]=modelTfm*edVtPos[1];

				double depth;
				if(YSTRUE!=drawEnv.IsLineWithinRect(depth,x0,y0,x1,y1,edVtPos))
				{
					actuallyExtendOutside.AddKey(edgeIdxBuffer[edIdx/2]);
				}
			}
		}


		for(YSSIZE_T idx=indexDepthPairArray.GetN()-1; 0<=idx; --idx)
		{
			if(YSTRUE==actuallyExtendOutside.IsIncluded(indexDepthPairArray[idx].a))
			{
				indexDepthPairArray.DeleteBySwapping(idx);
			}
		}


		YsHashTable <double> idxToDepth;
		for(auto pair : indexDepthPairArray)
		{
			double depth;
			if(YSOK!=idxToDepth.FindElement(depth,pair.a))
			{
				idxToDepth.AddElement(pair.a,pair.b);
			}
			else if(depth>pair.b)
			{
				idxToDepth.UpdateElement(pair.a,pair.b);
			}
		}


		indexDepthPairArray.CleanUp();

		YsHashElementEnumHandle hd;
		if(YSOK==idxToDepth.RewindElementEnumHandle(hd))
		{
			for(;;)
			{
				YSHASHKEY key;
				double elem;
				idxToDepth.GetKey(key,hd);
				idxToDepth.GetElement(elem,hd);

				indexDepthPairArray.Increment();
				indexDepthPairArray.Last().a=(int)key;
				indexDepthPairArray.Last().b=elem;

				if(YSOK!=idxToDepth.FindNextElement(hd))
				{
					break;
				}
			}
		}
	}

	return indexDepthPairArray;
}

YsArray <YsPair <int,double> > GeblGuiFoundation::BoxedIndexInTriangleBuffer(
    int x0,int y0,int x1,int y1,const YsGLVertexBuffer &triVtxBuffer,const YsArray <int> &triIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,
    const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <int,double> > indexDepthPairArray;

	if(NULL!=slHd)
	{
		for(YSSIZE_T plIdx=0; plIdx<=triVtxBuffer.GetNumVertex()-3; plIdx+=3)
		{
			if(NULL!=limit && YSTRUE!=limit->IsIncluded(triIdxBuffer[plIdx/3]))
			{
				continue;
			}

			if(NULL==exclusion || YSTRUE!=exclusion->IsIncluded(triIdxBuffer[plIdx/3]))
			{
				YsVec3 triVtPos[3]=
				{
					YsVec3(triVtxBuffer[plIdx  ][0],triVtxBuffer[plIdx  ][1],triVtxBuffer[plIdx  ][2]),
					YsVec3(triVtxBuffer[plIdx+1][0],triVtxBuffer[plIdx+1][1],triVtxBuffer[plIdx+1][2]),
					YsVec3(triVtxBuffer[plIdx+2][0],triVtxBuffer[plIdx+2][1],triVtxBuffer[plIdx+2][2])
				};

				triVtPos[0]=modelTfm*triVtPos[0];
				triVtPos[1]=modelTfm*triVtPos[1];
				triVtPos[2]=modelTfm*triVtPos[2];

				double depth;
				if(YSTRUE==drawEnv.IsPolygonWithinRect(depth,x0,y0,x1,y1,3,triVtPos))
				{
					indexDepthPairArray.Increment();
					indexDepthPairArray.Last().a=triIdxBuffer[plIdx/3];
					indexDepthPairArray.Last().b=depth;
				}
			}
		}


		YsKeyStore mayBeIncluded;
		for(auto pair : indexDepthPairArray)
		{
			mayBeIncluded.AddKey(pair.a);
		}


		// The following loop is for identifying polygons that are actually not fitting inside the rect.
		// Non-triangular polygons will be divided into multiple triangles.  Therefore, one triangle fitting within rect
		// does not mean it is completely inside the rect.  The following loop filters out a polygon that are extending
		// outside the rect.
		YsKeyStore actuallyExtendOutside;
		for(YSSIZE_T plIdx=0; plIdx<=triVtxBuffer.GetNumVertex()-3; plIdx+=3)
		{
			if(YSTRUE==mayBeIncluded.IsIncluded(triIdxBuffer[plIdx/3]))
			{
				YsVec3 triVtPos[3]=
				{
					YsVec3(triVtxBuffer[plIdx  ][0],triVtxBuffer[plIdx  ][1],triVtxBuffer[plIdx  ][2]),
					YsVec3(triVtxBuffer[plIdx+1][0],triVtxBuffer[plIdx+1][1],triVtxBuffer[plIdx+1][2]),
					YsVec3(triVtxBuffer[plIdx+2][0],triVtxBuffer[plIdx+2][1],triVtxBuffer[plIdx+2][2])
				};

				triVtPos[0]=modelTfm*triVtPos[0];
				triVtPos[1]=modelTfm*triVtPos[1];
				triVtPos[2]=modelTfm*triVtPos[2];

				double depth;
				if(YSTRUE!=drawEnv.IsPolygonWithinRect(depth,x0,y0,x1,y1,3,triVtPos))
				{
					actuallyExtendOutside.AddKey(triIdxBuffer[plIdx/3]);
				}
			}
		}


		for(YSSIZE_T idx=indexDepthPairArray.GetN()-1; 0<=idx; --idx)
		{
			if(YSTRUE==actuallyExtendOutside.IsIncluded(indexDepthPairArray[idx].a))
			{
				indexDepthPairArray.DeleteBySwapping(idx);
			}
		}


		YsHashTable <double> idxToDepth;
		for(auto pair : indexDepthPairArray)
		{
			double depth;
			if(YSOK!=idxToDepth.FindElement(depth,pair.a))
			{
				idxToDepth.AddElement(pair.a,pair.b);
			}
			else if(depth>pair.b)
			{
				idxToDepth.UpdateElement(pair.a,pair.b);
			}
		}


		indexDepthPairArray.CleanUp();

		YsHashElementEnumHandle hd;
		if(YSOK==idxToDepth.RewindElementEnumHandle(hd))
		{
			for(;;)
			{
				YSHASHKEY key;
				double elem;
				idxToDepth.GetKey(key,hd);
				idxToDepth.GetElement(elem,hd);

				indexDepthPairArray.Increment();
				indexDepthPairArray.Last().a=(int)key;
				indexDepthPairArray.Last().b=elem;

				if(YSOK!=idxToDepth.FindNextElement(hd))
				{
					break;
				}
			}
		}
	}

	return indexDepthPairArray;
}

YsArray <YsPair <int,double> > GeblGuiFoundation::EnclosedIndexInEdgeBuffer(
	    const YsShell2d &shl,const YsShell2dLattice &ltc,
	    const YsGLVertexBuffer &edgeVtxBuffer,const YsArray <int> &edgeIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <int,double> > indexDepthPairArray;

	if(NULL!=slHd)
	{
		for(YSSIZE_T edIdx=0; edIdx<=edgeVtxBuffer.GetNumVertex()-2; edIdx+=2)
		{
			if(NULL!=limit && YSTRUE!=limit->IsIncluded((YSHASHKEY)edgeIdxBuffer[edIdx/2]))
			{
				continue;
			}

			if(NULL==exclusion || YSTRUE!=exclusion->IsIncluded((YSHASHKEY)edgeIdxBuffer[edIdx/2]))
			{
				YsVec3 edVtPos[2]=
				{
					YsVec3(edgeVtxBuffer[edIdx  ][0],edgeVtxBuffer[edIdx  ][1],edgeVtxBuffer[edIdx  ][2]),
					YsVec3(edgeVtxBuffer[edIdx+1][0],edgeVtxBuffer[edIdx+1][1],edgeVtxBuffer[edIdx+1][2]),
				};

				edVtPos[0]=modelTfm*edVtPos[0];
				edVtPos[1]=modelTfm*edVtPos[1];

				double depth;
				if(YSTRUE==drawEnv.IsLineWithinPolygon(depth,shl,ltc,edVtPos))
				{
					indexDepthPairArray.Increment();
					indexDepthPairArray.Last().a=edgeIdxBuffer[edIdx/2];
					indexDepthPairArray.Last().b=depth;
				}
			}
		}


		YsKeyStore mayBeIncluded;
		for(auto pair : indexDepthPairArray)
		{
			mayBeIncluded.AddKey(pair.a);
		}


		// When identifying a constraint edge, one piece may be within rect, but other pieces
		// maybe extending outside of the rect.  The following loop filters out const edges
		// that are extending outside of the rect and not fitting within the rect.
		YsKeyStore actuallyExtendOutside;
		for(YSSIZE_T edIdx=0; edIdx<=edgeVtxBuffer.GetNumVertex()-2; edIdx+=2)
		{
			if(YSTRUE==mayBeIncluded.IsIncluded(edgeIdxBuffer[edIdx/2]))
			{
				YsVec3 edVtPos[2]=
				{
					YsVec3(edgeVtxBuffer[edIdx  ][0],edgeVtxBuffer[edIdx  ][1],edgeVtxBuffer[edIdx  ][2]),
					YsVec3(edgeVtxBuffer[edIdx+1][0],edgeVtxBuffer[edIdx+1][1],edgeVtxBuffer[edIdx+1][2]),
				};

				edVtPos[0]=modelTfm*edVtPos[0];
				edVtPos[1]=modelTfm*edVtPos[1];

				double depth;
				if(YSTRUE!=drawEnv.IsLineWithinPolygon(depth,shl,ltc,edVtPos))
				{
					actuallyExtendOutside.AddKey(edgeIdxBuffer[edIdx/2]);
				}
			}
		}


		for(YSSIZE_T idx=indexDepthPairArray.GetN()-1; 0<=idx; --idx)
		{
			if(YSTRUE==actuallyExtendOutside.IsIncluded(indexDepthPairArray[idx].a))
			{
				indexDepthPairArray.DeleteBySwapping(idx);
			}
		}


		YsHashTable <double> idxToDepth;
		for(auto pair : indexDepthPairArray)
		{
			double depth;
			if(YSOK!=idxToDepth.FindElement(depth,pair.a))
			{
				idxToDepth.AddElement(pair.a,pair.b);
			}
			else if(depth>pair.b)
			{
				idxToDepth.UpdateElement(pair.a,pair.b);
			}
		}


		indexDepthPairArray.CleanUp();

		YsHashElementEnumHandle hd;
		if(YSOK==idxToDepth.RewindElementEnumHandle(hd))
		{
			for(;;)
			{
				YSHASHKEY key;
				double elem;
				idxToDepth.GetKey(key,hd);
				idxToDepth.GetElement(elem,hd);

				indexDepthPairArray.Increment();
				indexDepthPairArray.Last().a=(int)key;
				indexDepthPairArray.Last().b=elem;

				if(YSOK!=idxToDepth.FindNextElement(hd))
				{
					break;
				}
			}
		}
	}

	return indexDepthPairArray;
}
YsArray <YsPair <int,double> > GeblGuiFoundation::EnclosedIndexInTriangleBuffer(
	    const YsShell2d &shl,const YsShell2dLattice &ltc,
	    const YsGLVertexBuffer &triVtxBuffer,const YsArray <int> &triIdxBuffer,const YsKeyStore *limit,const YsKeyStore *exclusion,const YsMatrix4x4 &modelTfm) const
{
	YsArray <YsPair <int,double> > indexDepthPairArray;

	if(NULL!=slHd)
	{
		for(YSSIZE_T plIdx=0; plIdx<=triVtxBuffer.GetNumVertex()-3; plIdx+=3)
		{
			if(NULL!=limit && YSTRUE!=limit->IsIncluded(triIdxBuffer[plIdx/3]))
			{
				continue;
			}

			if(NULL==exclusion || YSTRUE!=exclusion->IsIncluded(triIdxBuffer[plIdx/3]))
			{
				YsVec3 triVtPos[3]=
				{
					YsVec3(triVtxBuffer[plIdx  ][0],triVtxBuffer[plIdx  ][1],triVtxBuffer[plIdx  ][2]),
					YsVec3(triVtxBuffer[plIdx+1][0],triVtxBuffer[plIdx+1][1],triVtxBuffer[plIdx+1][2]),
					YsVec3(triVtxBuffer[plIdx+2][0],triVtxBuffer[plIdx+2][1],triVtxBuffer[plIdx+2][2])
				};

				triVtPos[0]=modelTfm*triVtPos[0];
				triVtPos[1]=modelTfm*triVtPos[1];
				triVtPos[2]=modelTfm*triVtPos[2];

				double depth;
				if(YSTRUE==drawEnv.IsPolygonWithinPolygon(depth,shl,ltc,3,triVtPos))
				{
					indexDepthPairArray.Increment();
					indexDepthPairArray.Last().a=triIdxBuffer[plIdx/3];
					indexDepthPairArray.Last().b=depth;
				}
			}
		}


		YsKeyStore mayBeIncluded;
		for(auto pair : indexDepthPairArray)
		{
			mayBeIncluded.AddKey(pair.a);
		}


		// The following loop is for identifying polygons that are actually not fitting inside the rect.
		// Non-triangular polygons will be divided into multiple triangles.  Therefore, one triangle fitting within rect
		// does not mean it is completely inside the rect.  The following loop filters out a polygon that are extending
		// outside the rect.
		YsKeyStore actuallyExtendOutside;
		for(YSSIZE_T plIdx=0; plIdx<=triVtxBuffer.GetNumVertex()-3; plIdx+=3)
		{
			if(YSTRUE==mayBeIncluded.IsIncluded(triIdxBuffer[plIdx/3]))
			{
				YsVec3 triVtPos[3]=
				{
					YsVec3(triVtxBuffer[plIdx  ][0],triVtxBuffer[plIdx  ][1],triVtxBuffer[plIdx  ][2]),
					YsVec3(triVtxBuffer[plIdx+1][0],triVtxBuffer[plIdx+1][1],triVtxBuffer[plIdx+1][2]),
					YsVec3(triVtxBuffer[plIdx+2][0],triVtxBuffer[plIdx+2][1],triVtxBuffer[plIdx+2][2])
				};

				triVtPos[0]=modelTfm*triVtPos[0];
				triVtPos[1]=modelTfm*triVtPos[1];
				triVtPos[2]=modelTfm*triVtPos[2];

				double depth;
				if(YSTRUE!=drawEnv.IsPolygonWithinPolygon(depth,shl,ltc,3,triVtPos))
				{
					actuallyExtendOutside.AddKey(triIdxBuffer[plIdx/3]);
				}
			}
		}


		for(YSSIZE_T idx=indexDepthPairArray.GetN()-1; 0<=idx; --idx)
		{
			if(YSTRUE==actuallyExtendOutside.IsIncluded(indexDepthPairArray[idx].a))
			{
				indexDepthPairArray.DeleteBySwapping(idx);
			}
		}


		YsHashTable <double> idxToDepth;
		for(auto pair : indexDepthPairArray)
		{
			double depth;
			if(YSOK!=idxToDepth.FindElement(depth,pair.a))
			{
				idxToDepth.AddElement(pair.a,pair.b);
			}
			else if(depth>pair.b)
			{
				idxToDepth.UpdateElement(pair.a,pair.b);
			}
		}


		indexDepthPairArray.CleanUp();

		YsHashElementEnumHandle hd;
		if(YSOK==idxToDepth.RewindElementEnumHandle(hd))
		{
			for(;;)
			{
				YSHASHKEY key;
				double elem;
				idxToDepth.GetKey(key,hd);
				idxToDepth.GetElement(elem,hd);

				indexDepthPairArray.Increment();
				indexDepthPairArray.Last().a=(int)key;
				indexDepthPairArray.Last().b=elem;

				if(YSOK!=idxToDepth.FindNextElement(hd))
				{
					break;
				}
			}
		}
	}

	return indexDepthPairArray;
}
