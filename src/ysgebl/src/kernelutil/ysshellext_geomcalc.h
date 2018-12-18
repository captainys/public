/* ////////////////////////////////////////////////////////////

File Name: ysshellext_geomcalc.h
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

#ifndef YSSHELLEXT_GEOMCALC_IS_INCLUDED
#define YSSHELLEXT_GEOMCALC_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include <ysshellextmisc.h>

/*! Calculate dihedral angle between two polygons. 
    These functions calculate normals based on vertex locations, not cached normal vectors. */
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellPolygonHandle plHd[2]);
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,YsShellPolygonHandle plHd0,YsShellPolygonHandle plHd1);

/*! Calculate dihedral angle across an edge.
    If the edge is not used by two polygons, this function returns 0.0 */
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexHandle edVtHd[2]);
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1);
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellPolygonHandle plHd,int nThEdgeZeroBased);

/*! Calculate total dihedral angle of edges:
    This function internally uses YsShellExt_CalculateDihedralAngle function.
           edVtHd[0]
    +--(1)--+
    |      /|
    |     / |
   (2)   /  |
    |  (5)  |
    |  /   (4)
    | /     |
    |/      |
    +--(3)--+
  edVtHd[1] */
const double YsShellExt_CalculateTotalDihedralAngleAroundEdge(const class YsShellExt &shl,const YsShellVertexHandle edVtHd[2]);
const double YsShellExt_CalculateTotalDihedralAngleAroundEdge(const class YsShellExt &shl,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1);


const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellPolygonHandle plHd[2]);
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,YsShellPolygonHandle plHd0,YsShellPolygonHandle plHd1);

const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellVertexHandle edVtHd[2]);
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1);
const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellPolygonHandle plHd,int nThEdgeZeroBased);


/*! Calculates maximum dihedral angle within a face group.
    It is based on the actual dihedral angle calculated from the vertex positions, not the normal assigned to the polygons.
*/
const double YsShellExt_CalculateMaxDihedralAngleInFaceGroup(const class YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd);  // Haven't I written this function before?  2016/02/09
const double YsShellExt_CalculateMaxDihedralAngleInFaceGroup(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,YsShellExt::FaceGroupHandle fgHd);  // Haven't I written this function before?  2016/02/09


/*! This function calculates average normal of the given polygons.  It does not use cached normal.
    It only rely on vertex positions. 
    It is not orientation sensitive.  The returned normal may be reverse of the normal that can be calculated based on the polygon orientation. */
const YsVec3 YsShell_CalculatePolygonAverageNormal(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);
template <const int N>
const YsVec3 YsShell_CalculatePolygonAverageNormal(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	return YsShell_CalculatePolygonAverageNormal(shl,plHdArray.GetN(),plHdArray);
}



/*! This function calculates polygon area times polygon normal.
    This function does not use cached normal vectors and calculates normal vectors based on vertex positions and polygon orientation for each polygon.
    The returned normal vector is NOT normalized.
*/
const YsVec3 YsShell_CalculateAreaWeightedPolygonNormal(const YsShell &shl,YsShell::PolygonHandle plHd);

/*! This function calculates area-weighted total of polygon normal vectors.
    This function does not use cached normal vectors and calculates normal vectors based on vertex positions and polygon orientation for each polygon.
    The returned normal vector is NOT normalized.  So that it can be updated later when a polygon is added or deleted.
*/
const YsVec3 YsShell_CalculateAreaWeightedSumOfPolygonNormal(const YsShell &shl,YSSIZE_T nPl,const YsShell::PolygonHandle plHdArray[]);

template <const int N>
const YsVec3 YsShell_CalculateAreaWeightedSumOfPolygonNormal(const YsShell &shl,const YsArray <YsShell::PolygonHandle,N> &plHdArray)
{
	return YsShell_CalculateAreaWeightedSumOfPolygonNormal(shl,plHdArray.GetN(),plHdArray);
}


/*! This function calculates the polygon normal from the vertex positions and the orientation.
*/
const YsVec3 YsShell_CalculateNormal(const YsShell &shl,YsShell::PolygonHandle plHd);




class YsShellExt_NormalCalculator
{
public:
	enum VERTEX_NORMAL_CALCULATION_WEIGHT
	{
		NO_WEIGHT
	};

	VERTEX_NORMAL_CALCULATION_WEIGHT weight;
	YsShellExt::FaceGroupHandle limitFgHd;

	YsShellExt_NormalCalculator();
	YsVec3 CalculateVertexNormal(const YsShellExt &shl,YsShellVertexHandle vtHd) const;
	YsVec3 CalculateEdgeNormal(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2]) const;
};


/*! See Garland and Heckbert 1997. 
    The following calculation will give total of square distances from the planes converging at the vertex.

                (x)
     [x y z 1]Kp(y)
                (z)
                (1)

*/
void YsShellExt_CalculateQuadricErrorMetric(YsMatrix4x4 &Kp,const YsShellExt &shl,YsShellVertexHandle vtHd);


/*! Calculate error from the quadric error metric. 
    It measures an error if the vertex used for calculating Kp is moved to the given location. */
double YsShellExt_CalculateQuadricError(const YsMatrix4x4 &Kp,const YsVec3 &pos);


/*! Calculate total angle taken by polygons around the vertex. */
double YsShellExt_CalculateTotalFaceAngle(const YsShellExt &shl,YsShellVertexHandle vtHd);

/*! Calculate total fan angle of the polygon at vertex. */
double YsShellExt_CalculatePolygonFaceAngle(const YsShellExt &shl,YsShellPolygonHandle plHd,YsShellVertexHandle vtHd);

/*! Calculate total fan angle of the polygon defined by plVtHd[0]...plVtHd[nVt-1] at vertex. */
double YsShellExt_CalculatePolygonFaceAngle(const YsShellExt &shl,YSSIZE_T nVt,const YsShellVertexHandle plVtHd[],YsShellVertexHandle vtHd);

/*! Calculate total fan angle of the polygon defined by plVtHd[0]...plVtHd[nVt-1] at vertex. */
template <const int N>
double YsShellExt_CalculatePolygonFaceAngle(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &plVtHd,YsShellVertexHandle vtHd)
{
	return YsShellExt_CalculatePolygonFaceAngle(shl,plVtHd.GetN(),plVtHd,vtHd);
}

YSBOOL YsShellExt_IsFaceGroupPlanar(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const double cosineTolerance=1.0-YsTolerance);

/*! Returns the longest edge of the polygon and the height measured from the longest edge. */
void YsShellExt_CalculateAspectRatio(double &length,double &height,YSSIZE_T np,const YsVec3 p[]);

/*! Returns the longest edge of the polygon and the height measured from the longest edge. */
template <const int N>
void YsShellExt_CalculateAspectRatio(double &length,double &height,const YsArray <YsVec3,N> &plg)
{
	YsShellExt_CalculateAspectRatio(length,height,plg.GetN(),plg);
}

/*! Returns the longest edge of the polygon and the height measured from the longest edge. */
void YsShellExt_CalculateAspectRatio(double &length,double &height,const YsShell &shl,YsShellPolygonHandle plHd);

/*! Returns the longest edge of the polygon and the height measured from the longest edge. */
void YsShellExt_CalculateAspectRatio(double &length,double &height,const YsShell &shl,YSSIZE_T nv,const YsShellVertexHandle v[]);

/*! Returns the longest edge of the polygon and the height measured from the longest edge. */
template <const int N>
void YsShellExt_CalculateAspectRatio(double &length,double &height,const YsShell &shl,const YsArray <YsShellVertexHandle,N> &v)
{
	YsShellExt_CalculateAspectRatio(length,height,shl,v.GetN(),v);
}

/*! Returns the intersection between a const edge and a plane. */
YsArray <YsShell::EdgeAndPos> YsShellExt_GetConstEdgeAndPlaneIntersection(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,const YsPlane &pln);

// class YsShellExt_PolygonProjection has been moved to ysshellext_projectionutil.h

double YsShellExt_CalculateConstEdgeLength(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd);

/*! Returns the maximum angle between normal vectors of the face groups divided by the const edge. */
double YsShellExt_CalculateMaxNormalDeviationAcrossConstEdge(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd);
double YsShellExt_CalculateMaxNormalDeviationAcrossConstEdge(YsShellPolygonHandle plHd[2],const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd);

/*! Returns the axis and aperture angle of the normal cone across the const edge. 
    (Normal cone of the polygons included in the face groups incident to the const edge.) 
 */
void YsShellExt_CalculateNormalConeAcrossConstEdge(YsVec3 &axis,double &aperture,const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd);



template <class STORAGECLASS>
YsShellExt::ConstEdgeHandle YsShellExt_FindLongestConstEdge(const YsShellExt &shl,const STORAGECLASS &ceHdStore)
{
	double longestLength=0.0;
	YsShellExt::ConstEdgeHandle longestCeHd=NULL;

	for(auto ceHd : ceHdStore)
	{
		const double l=YsShellExt_CalculateConstEdgeLength(shl,ceHd);
		if(NULL==longestCeHd || longestLength<l)
		{
			longestCeHd=ceHd;
			longestLength=l;
		}
	}

	return longestCeHd;
}


/*! This function returns a vector pointing into the const edge from one of the tips, tip==0 for the first vertex or
    tip==1 for the last vertex of the const edge. 
    The vector will NOT be normalized. */
YsVec3 YsShellExt_CalculateInboundConstEdgeVectorAtTip(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,int tip);


/*! This function returns a vector pointing into the const edge from one of its tip vertices.
    Vertex tipVtHd must be one of the first or the last vertex of the const edge.
    The vector will NOT be normalized. */
YsVec3 YsShellExt_CalculateInboundConstEdgeVectorAtTip(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,YsShellVertexHandle tipVtHd);



YsShellExt::VertexFaceGroupAttribTable <YsVec3> YsShellExt_MakeVertexFaceGroupNormalTable(const YsShellExt &shl);

/*! Make a global polygon-normal cache.  (Matching table from a polygon handle to geometrically re-calculated normal vector.) 
    A global polygon-normal cache can be useful when a shell is deformed in multiple steps.
    If the program checks incremental normal-vector change, the shell might be deformed too much after several iterations.
    Instead, the program can check normal-vector change from the original state.  A pointer to a normal cache can be passed to
    MakeInfoForEdgeCollapse function.  If non-NULL pointer is given, MakeInfoForEdgeCollapse function uses cached value rather than
    re-calculating normal-vectors on the fly.
*/
YsShellPolygonAttribTable <YsVec3> YsShellExt_MakePolygonNormalTable(const YsShell &shl);



/*! Returns the circum-circle radius divided by the in-circle radius in rr.
    The minimum value is two for an equilateral triangle, and goes infinity as the triangle becomes flatter.
    If the calculation is successful, it return YSOK.
    If polygon plHd is not a triangle, it returns YSERR, and rr will be 0.0.
    If the radius ratio cannot be calculated, it returns YSERR, and rr will be 0.0.
*/
YSRESULT YsShell_CalculateRawRadiusRatio(double &rr,const YsVec3 triVtPos[3]);
YSRESULT YsShell_CalculateRawRadiusRatio(double &rr,const YsShell &shl,YsShell::PolygonHandle plHd);
YSRESULT YsShell_CalculateRawRadiusRatio(double &rr,const YsShell &shl,const YsShell::VertexHandle triVtHd[3]);


/*! Takes two sets of line strips or line loops, and returns non-shared parts of the two sets.

	If two sets share at least one edge, the returned line strips share the first and the last points.
	The second line segment will be inverted if it is necessary in this case.

	If two sets are not sharing a point, this function fails and returns YSERR.  It returns YSOK if successful.

	When it returns YSERR, values of outSeg0 and outSeg1 will not change.
*/
template <class T>
YSRESULT YsExtractNonSharedPart(
    YsArray <T> &outSeg0,YsArray <T> &outSeg1,
    const YsConstArrayMask <T> &inSeg0,YSBOOL inSeg0IsLoop,
    const YsConstArrayMask <T> &inSeg1,YSBOOL inSeg1IsLoop)
{
	// By the way, haven't I written this function before?  I cannot just find it!!!

	YsArray <YsShell::VertexHandle> newVtHdArray[2];
	newVtHdArray[0]=inSeg0;
	newVtHdArray[1]=inSeg1;
	if(YSTRUE==inSeg0IsLoop && YSTRUE==inSeg1IsLoop)
	{
		for(YSSIZE_T i0=0; i0<newVtHdArray[0].size(); ++i0)
		{
			for(YSSIZE_T i1=0; i1<newVtHdArray[1].size(); ++i1)
			{
				if(newVtHdArray[0][i0]==newVtHdArray[1][i1])
				{
					for(YSSIZE_T i=0; i<i0; ++i)
					{
						newVtHdArray[0].Append(newVtHdArray[0][i]);
					}
					newVtHdArray[0].Delete(0,i0);
					for(YSSIZE_T i=0; i<i1; ++i)
					{
						newVtHdArray[1].Append(newVtHdArray[1][i]);
					}
					newVtHdArray[1].Delete(0,i1);
					break;
				}
			}
		}
		if(newVtHdArray[0][0]==newVtHdArray[1][0])
		{
			if(newVtHdArray[0][1]==newVtHdArray[1].back())
			{
				// [0]a-b-c-d   [1] a-e-f-b
				newVtHdArray[0].push_back(newVtHdArray[0][0]);
				newVtHdArray[0].Invert();
				newVtHdArray[0].pop_back();
				// [0]a-d-c-b   [1] a-e-f-b
			}
			else if(newVtHdArray[0].back()==newVtHdArray[1][1])
			{
				// [0]a-c-d-b   [1] a-b-e-f
				newVtHdArray[1].push_back(newVtHdArray[1][0]);
				newVtHdArray[1].Invert();
				newVtHdArray[1].pop_back();
				// [0]a-c-d-b   [1] a-f-e-b
			}
			else
			{
				// [0]a-b-c-d [1] a-e-f-g
				newVtHdArray[0].push_back(newVtHdArray[0][0]);
				newVtHdArray[1].push_back(newVtHdArray[1][0]);
				// [0]a-b-c-d-a [1] a-e-f-g-a
			}
			inSeg0IsLoop=YSFALSE;
			inSeg1IsLoop=YSFALSE;
		}
	}
	else if(YSTRUE!=inSeg0IsLoop && YSTRUE!=inSeg1IsLoop)
	{
		// Not implemented yet.
	}
	{
		bool swapped=false;
		if(YSTRUE!=inSeg0IsLoop && YSTRUE==inSeg1IsLoop)
		{
			swapped=true;
			std::swap(inSeg0IsLoop,inSeg1IsLoop);
			std::swap(newVtHdArray[0],newVtHdArray[1]);
		}

		if(YSTRUE==inSeg0IsLoop && YSTRUE!=inSeg1IsLoop)
		{
			// Not implemented yet.
		}

		if(true==swapped)
		{
			std::swap(inSeg0IsLoop,inSeg1IsLoop);
			std::swap(newVtHdArray[0],newVtHdArray[1]);
		}
	}
	if(YSTRUE!=inSeg0IsLoop &&
	   YSTRUE!=inSeg1IsLoop &&
	   2<=newVtHdArray[0].size() &&
	   2<=newVtHdArray[1].size() &&
	   newVtHdArray[0][0]!=newVtHdArray[0].back() &&
	   newVtHdArray[1][0]!=newVtHdArray[1].back() &&
	   newVtHdArray[0][0]==newVtHdArray[1].back() &&
	   newVtHdArray[1][0]==newVtHdArray[0].back())
	{
		newVtHdArray[1].Invert();
	}

	while(2<=newVtHdArray[0].size() && 2<=newVtHdArray[1].size() &&
	      newVtHdArray[0].back()==newVtHdArray[1].back() &&
	      newVtHdArray[0][newVtHdArray[0].size()-2]==newVtHdArray[1][newVtHdArray[1].size()-2])
	{
		newVtHdArray[0].pop_back();
		newVtHdArray[1].pop_back();
	}
	while(2<=newVtHdArray[0].size() && 2<=newVtHdArray[1].size() &&
	      newVtHdArray[0][0]==newVtHdArray[1][0] &&
	      newVtHdArray[0][1]==newVtHdArray[1][1])
	{
		newVtHdArray[0].Delete(0);
		newVtHdArray[1].Delete(0);
	}
	

	if(newVtHdArray[0][0]==newVtHdArray[1][0])
	{
		outSeg0.MoveFrom(newVtHdArray[0]);
		outSeg1.MoveFrom(newVtHdArray[1]);
		return YSOK;
	}

	return YSERR;
}

/* } */
#endif
