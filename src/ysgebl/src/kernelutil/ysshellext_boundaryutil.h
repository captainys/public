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

#ifndef YSSHELLEXT_BOUNDARYUTIL_IS_INCLUDED
#define YSSHELLEXT_BOUNDARYUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_BoundaryInfo
{
private:
	class BoundaryContour
	{
	public:
		YsArray <YsShellVertexHandle> vtHdArray;
		void Initialize(void){}
	};

protected:
	YsArray <YsShellVertexHandle> srcEdVtHd;
	YsFixedLengthToMultiHashTable <YsShellPolygonHandle,2,1> edgeToPlgHash;
	YsFixedLengthHashTable <YsShell::Edge,2> edgeDirectionHash;
	YsShellEdgeStore boundaryEdgeHash;

	YsArray <YsShellVertexHandle> nonDuplicateVtHdArray;
	YsArray <BoundaryContour> contourCache;

public:
	YsShellExt_BoundaryInfo();
	~YsShellExt_BoundaryInfo();
	void CleanUp(void);

	void MakeInfo(
	    const YsShell &shl,
	    YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	template <class PlgStore>
	void MakeInfo(
	    const YsShell &shl,
	    const PlgStore &plHdStore);

	/*! This function makes boundary information from multiple vertex-sequences.
	    Second parameter, vtSeqStore, must be a storage of an array class.
	    Since the information created by this function does not have a connection with the polygons,
	    some functionality of the class will not be available.
	*/
	template <class VtSeqStore>
	void MakeInfoFromVertexSequenceArray(
		const YsShell &shl,
		const VtSeqStore &vtSeqStore);

	/*! This function makes a hash-table for vertex connections. */
	void MakeVertexConnectionTable(
		YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &vtKeyToConnVtHd,
		const YsShell &shl) const;

	/*! This function caches the vertex handles that are included in the boundary. */
	void CacheNonDuplicateVtHdArray(void);

	/*! This function must be called after vertices are cached by MakeNonDuplicateVtHdArray. */
	YsVec3 GetCenter(const YsShellExt &shl) const;

	/*! Returns the center of the bounding box.  Must be called after vertices are cached by MakeNonDuplicateVtHdArray. */
	YsVec3 GetBoundingBoxCenter(const YsShellExt &shl) const;

	/*! This function must be called after vertices are cached by MakeNonDuplicateVtHdArray. */
	template <const int N>
	void GetVertexAll(YsArray <YsShellVertexHandle,N> &vtHdArray) const;

	/*! This function must be called after vertices are cached by MakeNonDuplicateVtHdArray. */
	const YsArray <YsShellVertexHandle> &GetVertexAll(void) const;

	/*! This function caches countours.  If the contours won't make unique loops, this function will return YSERR. */
	YSRESULT CacheContour(const YsShell &shl);

private:
	YSRESULT TrackAndAddLoop(
	    YsTwoKeyStore &usedEdgePiece,
	    const YsShell &shl,YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,
	    const YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &connection);

public:
	/*! Fix orientations of the boundary contours.  
	    The boundary contour will be re-oriented so that the source polygons are using pieces of edges in the
	    reverse order of the cached boundary contours.
	    If the polygons are not oriented correctly, the outcome is undefined.
	    Call this function after CacheContour.
	*/
	void ReorientContour(const YsShell &shl);

	/*! This function returns number of contours.  If contours are not cached, CacheContour function failed, or there is no contour, this function will return zero. */
	YSSIZE_T GetNumContour(void) const;

	/*! This function returns a contour.  idx must be 0<=N<GetNumContour().  Returns YSERR if N is out of bound.  Contours must be cached before this function. */
	YSRESULT GetContour(YSSIZE_T &nVt,const YsShellVertexHandle *&vtHdArray,YSSIZE_T idx) const;

	/*! This function returns a contour.  idx must be 0<=N<GetNumContour().  Returns YSERR if N is out of bound.  Contours must be cached before this function. */
	const YsConstArrayMask <YsShell::VertexHandle> GetContour(YSSIZE_T idx) const;

	/*! This function returns a contour.  idx must be 0<=N<GetNumContour().  Returns YSERR and the array will be empty if N is out of bound.  Contours must be cached before this function. 
	    First and last vertex handle in the array will be the same. */
	template <const int N>
	YSRESULT GetContour(YsArray <YsShellVertexHandle,N> &vtHdArray,YSSIZE_T idx) const;

	/*! This function returns arrays of the vertices of the boundary contours. 
	    Can be used after CacheContour.  
	    First and last vertex handles in each array will be the same. */
	YsArray <YsArray <YsShellVertexHandle> > GetContourAll(void) const;

	/*! This function returns YSTRUE if the edge is part of the boundary.
	    Can be used after CacheContour.
	*/
	YSBOOL IsBoundaryEdge(const YsShell &shl,const YsShell::VertexHandle edVtHd[2]) const;
	YSBOOL IsBoundaryEdge(const YsShell &shl,YsShell::VertexHandle edVtHd0,YsShell::VertexHandle edVtHd1) const;

	void GetEdgeUsageMinMax(int &min,int &max,const YsShellExt &shl) const;

	YSSIZE_T GetNumEdgePiece(void) const;
	YSRESULT GetEdge(YsShellVertexHandle edVtHd[2],YSSIZE_T edIdx) const;
};

template <class PlgStore>
void YsShellExt_BoundaryInfo::MakeInfo(
    const YsShell &shl,
    const PlgStore &plHdStore)
{
	CleanUp();
	boundaryEdgeHash.SetShell(shl);

	for(auto plHd : plHdStore)
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetVertexListOfPolygon(plVtHd,plHd);

		for(int edIdx=0; edIdx<plVtHd.GetN(); ++edIdx)
		{
			const YSHASHKEY edVtKey[2]=
			{
				shl.GetSearchKey(plVtHd[edIdx]),
				shl.GetSearchKey(plVtHd.GetCyclic(edIdx+1))
			};
			edgeToPlgHash.AddElement(2,edVtKey,plHd);
		}
	}

	for(auto plHd : plHdStore)
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetVertexListOfPolygon(plVtHd,plHd);

		for(int edIdx=0; edIdx<plVtHd.GetN(); ++edIdx)
		{
			YsShell::Edge edge(plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1));
			const YSHASHKEY edVtKey[2]=
			{
				shl.GetSearchKey(edge[0]),
				shl.GetSearchKey(edge[1])
			};

			int nEdPl;
			const YsShellPolygonHandle *edPlHd;
			if(YSOK==edgeToPlgHash.FindElement(nEdPl,edPlHd,2,edVtKey) && 1==nEdPl)
			{
				const YsShellVertexHandle edVtHd[2]=
				{
					plVtHd[edIdx],
                    plVtHd.GetCyclic(edIdx+1)
				};

				srcEdVtHd.Append(edVtHd[0]);
				srcEdVtHd.Append(edVtHd[1]);
				boundaryEdgeHash.AddEdge(edVtHd[0],edVtHd[1]);
				edgeDirectionHash.AddElement(2,edVtKey,edge);
			}
		}
	}
}

template <class VtSeqStore>
void YsShellExt_BoundaryInfo::MakeInfoFromVertexSequenceArray(
	const YsShell &shl,
	const VtSeqStore &vtSeqStore)
{
	CleanUp();
	boundaryEdgeHash.SetShell(shl);

	YsFixedLengthToMultiHashTable <YSSIZE_T,2,1> edgeToPlIdxHash;
	YSSIZE_T plHd=0;
	for(auto &plVtHd : vtSeqStore)
	{
		for(int edIdx=0; edIdx<plVtHd.GetN(); ++edIdx)
		{
			const YSHASHKEY edVtKey[2]=
			{
				shl.GetSearchKey(plVtHd[edIdx]),
				shl.GetSearchKey(plVtHd.GetCyclic(edIdx+1))
			};
			edgeToPlIdxHash.AddElement(2,edVtKey,plHd);
		}
		++plHd;
	}

	for(auto &plVtHd : vtSeqStore)
	{
		for(int edIdx=0; edIdx<plVtHd.GetN(); ++edIdx)
		{
			YsShell::Edge edge(plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1));
			const YSHASHKEY edVtKey[2]=
			{
				shl.GetSearchKey(edge[0]),
				shl.GetSearchKey(edge[1])
			};

			int nEdPl;
			const YSSIZE_T *edPlHd;
			if(YSOK==edgeToPlIdxHash.FindElement(nEdPl,edPlHd,2,edVtKey) && 1==nEdPl)
			{
				srcEdVtHd.Append(edge[0]);
				srcEdVtHd.Append(edge[1]);
				boundaryEdgeHash.Add(edge[0],edge[1]);
				edgeDirectionHash.AddElement(2,edVtKey,edge);
			}
		}
	}
}

template <const int N>
void YsShellExt_BoundaryInfo::GetVertexAll(YsArray <YsShellVertexHandle,N> &vtHdArray) const
{
	vtHdArray=nonDuplicateVtHdArray;
}

template <const int N>
YSRESULT YsShellExt_BoundaryInfo::GetContour(YsArray <YsShellVertexHandle,N> &vtHdArray,YSSIZE_T idx) const
{
	YSSIZE_T n;
	const YsShellVertexHandle *arrayPtr;
	if(YSOK==GetContour(n,arrayPtr,idx))
	{
		vtHdArray.Set(n,arrayPtr);
		return YSOK;
	}
	vtHdArray.CleanUp();
	return YSERR;
}

/* } */
#endif
