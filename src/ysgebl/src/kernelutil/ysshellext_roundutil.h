/* ////////////////////////////////////////////////////////////

File Name: ysshellext_roundutil.h
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

#ifndef YSSHELLEXT_ROUNDUTIL_IS_INCLUDED
#define YSSHELLEXT_ROUNDUTIL_IS_INCLUDED
/* { */

#include "ysshellext.h"
#include "ysshellext_offsetutil.h"

class YsShellExt_RoundUtil
{
public:
	class VertexPositionPair
	{
	public:
		YsShellVertexHandle vtHd;
		YsVec3 pos;
	};
	class RoundCorner
	{
	public:
		double radius;
		YsVec3 center;
		YsShellVertexHandle fromVtHd;
		YsVec3 roundedCornerPos;
		YsShellVertexHandle toVtHd[2];
		YsArray <VertexPositionPair> subDiv[2];  // subDivVtHd[x][0] will be toVtHd[x] side from fromVtHd.
	};

	double angleThr; // If the corner make less than this angle, it cannot be rounded.
	YsArray <RoundCorner> cornerArray;
	YsArray <YSHASHKEY> targetPlKeyArray;
	YsArray <YSHASHKEY> targetCeKeyArray;

	YsShellExt_RoundUtil();
	void CleanUp(void);

	/*! Set up rounding of all corners of a polygon.  
	    The radius will automatically be limited by the length of the edges.  
	    When roundVtx is given (non-NULL), only vertices included in *roundVtx will be rounded. */
	YSRESULT SetUpRoundPolygon(const YsShell &shl,YsShellPolygonHandle plHd,const YsShellVertexStore *roundVtx);

	/*! Set up rounding of all corners of a constraint edge. 
	    The radius will automatically be limited by the length of the edges.  
	    When roundVtx is given (non-NULL), only vertices included in *roundVtx will be rounded. */
	YSRESULT SetUpRoundConstEdge(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,const YsShellVertexStore *roundVtx);

	/*! Adds a rounded corner.  A rounded corner is a corner of a polygon or a const-edge, to be rounded into given radius.
	    nDiv must be at least 1.  Or, this function will take it as 1.  
	    nDiv vertices needs to be added on each of the edges fromVtHd-toVtHd[0] and fromVtHd-toVtHd[1]. */
	YSRESULT AddRoundCorner(const YsShell &shl,YsShellVertexHandle fromVtHd,const YsShellVertexHandle toVtHd[2]);

	/*! Calculates the maximum radius.  Returns 0.0 if the current setting cannot be rounded. */
	const double CalculateMaximumRadius(const YsShell &shl) const;

	/*! This function calculates rounding for all corners. */
	YSRESULT CalculateRoundingAll(const YsShell &shl,const double radius,const int nDiv);

	/*! Makes a vertex sequence.  If the vertex needs to be created, vtHd will be NULL.
	    The pos member will be the new vertex location. */
	YsArray <VertexPositionPair> MakeRoundedVertexSequence(const YsShell &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[],YSBOOL isLoop) const;

	/*! Makes a vertex sequence.  If the vertex needs to be created, vtHd will be NULL.
	    The pos member will be the new vertex location. */
	template <const int N>
	YsArray <VertexPositionPair> MakeRoundedVertexSequence(const YsShell &shl,const YsArray <YsShellVertexHandle,N> &plVtHd,YSBOOL isLoop) const;

	/*! Calculate radius from the length from the corner to the tangent point. */
	static double CalculateRadiusFromTangentDistance(const YsShell &shl,YsShellVertexHandle fromVtHd,const YsShellVertexHandle toVtHd[2],const double dist);
};

template <const int N>
YsArray <YsShellExt_RoundUtil::VertexPositionPair> YsShellExt_RoundUtil::MakeRoundedVertexSequence(const YsShell &shl,const YsArray <YsShellVertexHandle,N> &plVtHd,YSBOOL isLoop) const
{
	return MakeRoundedVertexSequence(shl,plVtHd.GetN(),plVtHd,isLoop);
}

////////////////////////////////////////////////////////////

class YsShellExt_RoundUtil3d
{
private:
	YSBOOL alwaysUseOffset;

public:
	class VertexPositionPair
	{
	public:
		YsShellVertexHandle vtHd;
		YsVec3 pos;
	};

	/*! This class defines a rounding at one vertex per edge. */
	class HalfRoundCorner
	{
	public:
		double radius;
		YsVec3 center,foot;
		YsShellVertexHandle fromVtHd,toVtHd;
		YsVec3 toPos;  // If toVtHd is NULL, it means that toVtHd is to be created.  (eg. Polygons need to be offsetted).  In such a case, toPos should be populated and toVtHd should be NULL.
		YsVec3 roundedCornerPos,roundDir; // roundDir is the direction fromVtHd will be moved.
		YsArray <VertexPositionPair> subDiv;

		YSBOOL notReallyDistanceLimited;  // Added 20141016. If the vertex is at the corner of a triangle, all of whose vertices are also offsetted, the triangle is not really limiting the offset.

		// YsShellExt_RoundUtil3d may be used together with YsShellExt_OffsetUtil2d.
		class YsShellExt_OffsetUtil2d *offsetUtil;
		YSSIZE_T offsetUtil_newVtxIdx;

		HalfRoundCorner();

		/*! Initializes the object. */
		void Initialize(void);

		/*! This calculates a subdivision of the circular arc by the number of subdivision.
		    subDiv[0] will be foot, and subDiv[nDiv-1] will be the last step before roundedCornerPos. */
		YSRESULT SubdivideByNumberOfSubdivision(int nDiv);

		/*! This calculates a subdivision of the circular arc by the step angle.
		    subDiv[0] will be foot, and subDiv[nDiv-1] will be the last step before roundedCornerPos. */
		YSRESULT SubdivideByStepAngle(const double stepAngle);
	};

	class RoundEdge
	{
	public:
		YsShellVertexHandle edVtHd[2];

		/*! Initializes the object. */
		void Initialize(void);
	};

public:
	YsArray <HalfRoundCorner> cornerArray;
	YsArray <RoundEdge> roundEdgeArray;
	YsShellExt_OffsetUtil2d offsetUtil[2];


public:
	YsShellExt_RoundUtil3d();
	void CleanUp(void);
	void SetAlwaysUseOffset(YSBOOL alwaysUseOffset);

private:
	YsArray <HalfRoundCorner *> MakeSortedHalfRoundCorner(const YsShell &shl);
	YsArray <const HalfRoundCorner *> MakeSortedHalfRoundCorner(const YsShell &shl) const;

	YSRESULT CalculateRoundingDirectionAll(const YsShell &shl);

	YsVec3 CalculateRoundingDirction(const YsShell &shl,YsShellVertexHandle fromVtHd,YSSIZE_T nTo,const YsShellVertexHandle toVtHd[]) const;

	// fromVtHd and toPos of each member must be populated before this function.
	YsVec3 CalculateRoundingDirction(const YsShell &shl,const YsArray <HalfRoundCorner *> &roundCornerPerVertex) const;

	YSRESULT CalculateCenterFromRadius(YsVec3 &center,const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double raduis) const;
	const double CalculateRadiusFromLimit(const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double L) const;
	const double CalculateDisplacementFromRadius(const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double radius) const;
	const double CalculateRadiusFromDisplacement(const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double t) const;

public:
	/*! This function makes an array of HalfRoundCorner to prepare for rounding around a vertex. */
	YSRESULT SetUpRoundVertex(const YsShell &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[]);

	/*! This function makes an array of HalfRoundCorner to prepare for rounding around a vertex. */
	template <const int N>
	YSRESULT SetUpRoundVertex(const YsShell &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray);

	/*! This function makes an array of HalfRoundCorner to prepare for rounding around a group of polygons. */
	YSRESULT SetUpForAroundPolygonGroup(
	    const YsShellExt &shl,
	    YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! Templated version of SetUpForAroundPolygonGroup. */
	template <const int N>
	YSRESULT SetUpForAroundPolygonGroup(const class YsShellExt &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray);

	/*! This function sets up this object to prepare for rounding along the given vertex sequence. */
	YSRESULT SetUpForVertexSequence(
		const YsShellExt &shl,
		YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[]);

	/*! Templated version of SetUpForVertexSequence. */
	template <const int N>
	YSRESULT SetUpForVertexSequence(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray);

private:
	YSRESULT SetUpForVertexSequenceAndPolygonArray(
	    const class YsShellExt &shl,
	    const YsArray <YsArray <YsShellVertexHandle> > &allBoundary,
	    const YsArray <YsShellPolygonHandle> &sideAPolygonArray,
	    const YsArray <YsShellPolygonHandle> &sideBPolygonArray);

	YSBOOL IsPolygonOnTheSameContour(const YsShellExt &shl,YsShellPolygonHandle plHd,const YsHashTable <YSSIZE_T> &vtKeyToBoundaryIdx);

public:

public:
	/*! This function returns maximum radius that the corner can be rounded. 
	    SetUp* function must be called before this function. 
	    Returns 0.0 if rounding is impossible. */
	const double CalculateMaximumRadius(const YsShell &shl) const;

	/*! This function calculates rounding for an array of HalfRoundCorner returned by SetUpFor* functions. 
	    Returns YSOK if successful, YSERR otherwise. */
	YSRESULT CalculateRoundingAll(const YsShell &shl,const double radius);

	/*! Subdivide half round corners by the given subdivision count. */
	YSRESULT SubdivideByNumberOfSubdivision(int nDiv);

	/*! Subdivide half round corners by the given subdivision angle. */
	YSRESULT SubdivideByStepAngle(const double stepAngle);

private:
	YSRESULT CalculateRoundingPerVertex(const YsShell &shl,const YsArray <HalfRoundCorner *> &roundCornerPerVertex,const double radius) const;
	YSBOOL IsEdgeUsing(const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1,const YsShell &shl,const YsShellPolygonStore &polygonStore) const;

public:
	/*! This function finds connected half round corner indices of a polygon. */
	YsArray <YSSIZE_T> FindHalfRoundCornerFromPolygon(const YsShell &shl,YsShellPolygonHandle plHd) const;
};

template <const int N>
YSRESULT YsShellExt_RoundUtil3d::SetUpRoundVertex(const YsShell &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray)
{
	return SetUpRoundVertex(shl,vtHdArray.GetN(),vtHdArray);
}

template <const int N>
YSRESULT YsShellExt_RoundUtil3d::SetUpForAroundPolygonGroup(const YsShellExt &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	return SetUpForAroundPolygonGroup(shl,plHdArray.GetN(),plHdArray);
}

template <const int N>
YSRESULT YsShellExt_RoundUtil3d::SetUpForVertexSequence(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray)
{
	return SetUpForVertexSequence(shl,vtHdArray.GetN(),vtHdArray);
}

/* } */
#endif
