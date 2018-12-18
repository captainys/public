/* ////////////////////////////////////////////////////////////

File Name: ysshellext_topologyutil.h
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

#ifndef YSSHELLEXT_TOPOLOGYUTIL_IS_INCLUDED
#define YSSHELLEXT_TOPOLOGYUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include "ysshellext_localop.h"
#include "ysshellext_topologyutil.h"

class YsShellExt_TopologyUtil : private YsShell_LocalOperation
{
public:
	static int GetNumSingleUseEdgeConnectedToVertex(const YsShellExt &shl,YsShellVertexHandle vtHd);
	static YSBOOL IsSingleUseEdge(const YsShellExt &shl,YsShellEdgeEnumHandle edHd);
	static YSBOOL IsSingleUseEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1);
	static YSBOOL IsSingleUseEdge(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2]);

	/*! Returns YSTRUE if vertex vtHd is used by at least one boundary (or single-use) edge.
	*/
	static YSBOOL IsVertexUsedBySingleUseEdge(const YsShellExt &shl,const YsShellVertexHandle vtHd);

public:
	YsArray <YsShell::PolygonHandle> modifiedPlHdArray;
	YsArray <YsShellExt::ConstEdgeHandle> modifiedCeHdArray;


	void CleanUp(void);

	template <class SHLCLASS>
	YSRESULT InsertVertexOnEdge(SHLCLASS &shl,const YsShellVertexHandle edVtHd[2],YsShellVertexHandle midVtHd);

	template <class SHLCLASS>
	YSRESULT InsertVertexOnEdge(SHLCLASS &shl,YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[]);
	template <class SHLCLASS>
	YSRESULT InsertVertexOnEdge(SHLCLASS &shl,const YsShellVertexHandle edVtHd[2],YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[]);

	template <class SHLCLASS>
	YSRESULT InsertVertexOnEdge(SHLCLASS &shl,YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,const YsConstArrayMask <YsShellVertexHandle> &vtHdArray);

	template <class SHLCLASS>
	YSRESULT InsertVertexOnEdge(SHLCLASS &shl,const YsShellVertexHandle edVtHd[2],const YsConstArrayMask <YsShellVertexHandle> &vtHdArray);

protected:
	YsArray <YsShell::VertexHandle,16> InsertVertexOnPolygonEdge(
	    const YsConstArrayMask <YsShell::VertexHandle> &plVtHd,
	    YsShell::VertexHandle fromVtHd,
	    YsShell::VertexHandle toVtHd,
	    YSSIZE_T nVt,
	    const YsShell::VertexHandle vtHdArray[]) const;
	YsArray <YsShell::VertexHandle,16> InsertVertexOnConstEdge(
	    const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,
	    YSBOOL isLoop,
	    YsShell::VertexHandle fromVtHd,
	    YsShell::VertexHandle toVtHd,
	    YSSIZE_T nVt,
	    const YsShell::VertexHandle vtHdArray[]) const;
	void FindSplitLocation(YSSIZE_T &vtIdx0,YSSIZE_T &vtIdx1,const YsConstArrayMask <YsShell::VertexHandle> &plVtHd,YsShell::VertexHandle vtHd0,YsShell::VertexHandle vtHd1) const;

public:
	template <class SHLCLASS>
	YSRESULT SplitPolygon(SHLCLASS &shl,YsShellPolygonHandle plHd,YsShellVertexHandle vtHd0,YsShellVertexHandle vtHd1);
	template <class SHLCLASS>
	YSRESULT SplitPolygon(SHLCLASS &shl,YsShellPolygonHandle plHd,const YsShellVertexHandle vtHd[2]);

	template <class SHLCLASS>
	YSRESULT SplitPolygonWithMidNode(SHLCLASS &shl,YsShellPolygonHandle plHd,const YsConstArrayMask <YsShell::VertexHandle> &cutter);

	/*! Splits a degenerate polygon (using a vertex more than once) into non-degenerate polygons. 
	   Does nothing and returns YSERR if the polygon is not degenerate.  
	   Returns YSOK if successful.  
	   It could fail if the polygon cannot be split into a unique set of non-degenerate polygons. */
	template <class SHLCLASS>
	YSRESULT SplitDegeneratePolygon(SHLCLASS &shl,YsShellPolygonHandle plHd);

	/*! Splits all degenerate polygons in the shell. */
	template <class SHLCLASS>
	YSRESULT SplitAllDegeneratePolygon(SHLCLASS &shl);
};

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::InsertVertexOnEdge(SHLCLASS &shl,YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,const YsConstArrayMask <YsShellVertexHandle> &vtHdArray)
{
	return InsertVertexOnEdge(shl,fromVtHd,toVtHd,vtHdArray.GetN(),vtHdArray);
}

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::InsertVertexOnEdge(SHLCLASS &shl,const YsShellVertexHandle edVtHd[2],const YsConstArrayMask <YsShellVertexHandle> &vtHdArray)
{
	return InsertVertexOnEdge(shl,edVtHd[0],edVtHd[1],vtHdArray.GetN(),vtHdArray);
}


template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::InsertVertexOnEdge(SHLCLASS &shl,const YsShellVertexHandle edVtHd[2],YsShellVertexHandle midVtHd)
{
	return InsertVertexOnEdge(shl,edVtHd[0],edVtHd[1],1,&midVtHd);
}

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::InsertVertexOnEdge(SHLCLASS &shl,YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[])
{
	CleanUp();

	typename SHLCLASS::StopIncUndo incUndo(&shl);

	for(auto plHd : shl.FindPolygonFromEdgePiece(fromVtHd,toVtHd))
	{
		YsArray <YsShellVertexHandle,16> plVtHd;
		shl.GetPolygon(plVtHd,plHd);

		auto newPlVtHd=InsertVertexOnPolygonEdge(plVtHd,fromVtHd,toVtHd,nVt,vtHdArray);

		if(plVtHd.GetN()<newPlVtHd.GetN())
		{
			shl.SetPolygonVertex(plHd,newPlVtHd);
			modifiedPlHdArray.Append(plHd);
		}
	}

	for(auto ceHd : shl.FindConstEdgeFromEdgePiece(fromVtHd,toVtHd))
	{
		YSBOOL isLoop;
		YsArray <YsShellVertexHandle,16> ceVtHd;

		shl.GetConstEdge(ceVtHd,isLoop,ceHd);

		auto newCeVtHd=InsertVertexOnConstEdge(ceVtHd,isLoop,fromVtHd,toVtHd,nVt,vtHdArray);

		if(ceVtHd.GetN()<newCeVtHd.GetN())
		{
			shl.ModifyConstEdge(ceHd,newCeVtHd,isLoop);
			modifiedCeHdArray.Append(ceHd);
		}
	}

	return YSOK;
}

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::InsertVertexOnEdge(SHLCLASS &shl,const YsShellVertexHandle edVtHd[2],YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[])
{
	return InsertVertexOnEdge(shl,edVtHd[0],edVtHd[1],nVt,vtHdArray);
}

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::SplitPolygon(SHLCLASS &shl,YsShellPolygonHandle plHd,YsShellVertexHandle vtHd0,YsShellVertexHandle vtHd1)
{
	CleanUp();

	YsArray <YsShellVertexHandle,16> plVtHd,newPlVtHd;

	shl.GetPolygon(plVtHd,plHd);
	YSSIZE_T vtIdx0=-1,vtIdx1=-1;
	FindSplitLocation(vtIdx0,vtIdx1,plVtHd,vtHd0,vtHd1);

	if(0<=vtIdx0 && 0<=vtIdx1 && YSOK==YsShellExt_SplitLoop(plVtHd,newPlVtHd,vtIdx0,vtIdx1) && 3<=plVtHd.GetN() && 3<=newPlVtHd.GetN())
	{
		typename SHLCLASS::StopIncUndo stopIncUndo(shl);

		auto fgHd=shl.FindFaceGroupFromPolygon(plHd);

		shl.SetPolygonVertex(plHd,plVtHd);

		const YsShellExt::PolygonAttrib *plAtt=shl.GetPolygonAttrib(plHd);
		const YsVec3 nom=shl.GetNormal(plHd);
		const YsColor col=shl.GetColor(plHd);

		YsShellPolygonHandle newPlHd=shl.AddPolygon(newPlVtHd);
		shl.SetPolygonAttrib(newPlHd,*plAtt);
		shl.SetPolygonNormal(newPlHd,nom);
		shl.SetPolygonColor(newPlHd,col);
		if(NULL!=fgHd)
		{
			shl.AddFaceGroupPolygon(fgHd,1,&newPlHd);
		}

		modifiedPlHdArray.Append(plHd);
		modifiedPlHdArray.Append(newPlHd);

		return YSOK;
	}
	return YSERR;
}

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::SplitPolygonWithMidNode(SHLCLASS &shl,YsShellPolygonHandle plHd,const YsConstArrayMask <YsShell::VertexHandle> &cutter)
{
	CleanUp();

	YsArray <YsShellVertexHandle,16> plVtHd,newPlVtHd;

	plVtHd=shl.GetPolygonVertex(plHd);
	if(YSOK==YsShellExt_SplitLoopByHandleAndMidNode(plVtHd,newPlVtHd,cutter) && 3<=plVtHd.GetN() && 3<=newPlVtHd.GetN())
	{
		typename SHLCLASS::StopIncUndo stopIncUndo(shl);

		auto fgHd=shl.FindFaceGroupFromPolygon(plHd);

		shl.SetPolygonVertex(plHd,plVtHd);

		const YsShellExt::PolygonAttrib *plAtt=shl.GetPolygonAttrib(plHd);
		const YsVec3 nom=shl.GetNormal(plHd);
		const YsColor col=shl.GetColor(plHd);

		YsShellPolygonHandle newPlHd=shl.AddPolygon(newPlVtHd);
		shl.SetPolygonAttrib(newPlHd,*plAtt);
		shl.SetPolygonNormal(newPlHd,nom);
		shl.SetPolygonColor(newPlHd,col);
		if(NULL!=fgHd)
		{
			shl.AddFaceGroupPolygon(fgHd,1,&newPlHd);
		}

		modifiedPlHdArray.Append(plHd);
		modifiedPlHdArray.Append(newPlHd);

		return YSOK;
	}
	return YSERR;
}

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::SplitPolygon(SHLCLASS &shl,YsShellPolygonHandle plHd,const YsShellVertexHandle vtHd[2])
{
	return SplitPolygon(shl,plHd,vtHd[0],vtHd[1]);
}

template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::SplitDegeneratePolygon(SHLCLASS &shl,YsShellPolygonHandle plHd)
{
	auto plVtHd=shl.GetPolygonVertex(plHd);
	YsArray <YsArray <YsShellVertexHandle> > newPlVtHd;
	if(YSOK==YsSplitDegeneratePolygon(newPlVtHd,plVtHd))
	{
		modifiedPlHdArray.CleanUp();

		if(0==newPlVtHd.GetN()) // Means the polygon shrunk to a point or a line.
		{
			shl.ForceDeletePolygon(plHd);
		}
		else
		{
			typename SHLCLASS::StopIncUndo stopIncUndo(shl);
			for(int idx=0; idx<newPlVtHd.GetN(); ++idx)
			{
				if(0==idx)
				{
					shl.SetPolygonVertex(plHd,newPlVtHd[0]);
				}
				else
				{
					auto newPlHd=shl.AddPolygon(newPlVtHd[idx]);
					auto col=shl.GetColor(plHd);
					auto nom=shl.GetNormal(plHd);
					auto attrib=shl.GetPolygonAttrib(plHd);
					shl.SetPolygonColor(newPlHd,col);
					shl.SetPolygonNormal(newPlHd,nom);
					shl.SetPolygonAttrib(newPlHd,*attrib);
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}


template <class SHLCLASS>
YSRESULT YsShellExt_TopologyUtil::SplitAllDegeneratePolygon(SHLCLASS &shl)
{
	YsArray <YsShellPolygonHandle> accum;
	for(auto plHd : shl.AllPolygon())
	{
		if(YSOK==SplitDegeneratePolygon(shl,plHd))
		{
			accum.Append(modifiedPlHdArray);
		}
	}
	modifiedPlHdArray=accum;
	return YSOK;
}


/* } */
#endif
