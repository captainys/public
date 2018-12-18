/* ////////////////////////////////////////////////////////////

File Name: ysshellext_sweeputil.h
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

#ifndef YSSHELLEXT_SWEEPUTIL_IS_INCLUDED
#define YSSHELLEXT_SWEEPUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include "ysshellext_boundaryutil.h"
#include "ysshellext_mappingutil.h"

class YsShellExt_SweepInfo : public YsShellExt_BoundaryInfo, public YsShellExt_VertexToVertexMapping
{
public:
	/*! Vertices of source edges. There are N source edges, and srcEdVtHd[n*2]-srcEdVtHd[n*2+1] is a source edge piece. */
	using YsShellExt_BoundaryInfo::srcEdVtHd;

	/*! All vertices of the source polygon. */
	YsShellVertexStore allSrcVtHd;

public:
	static YsShellExt_SweepInfo *Create(void);
	static void Delete(YsShellExt_SweepInfo *ptr);

	YsShellExt_SweepInfo();
	~YsShellExt_SweepInfo();
	void CleanUp(void);

	void MakeInfo(
	    const YsShellExt &shl,
	    YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[],
	    YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[]);

	void MakeInfo(
	    const YsShellExt &shl,
	    const YsConstArrayMask <YsShellPolygonHandle> &plHdArray,
	    const YsConstArrayMask <YsShellExt::ConstEdgeHandle> &ceHdArray);

	YsArray <YsShellVertexHandle> GetVertexAll(void) const;

	template <const int N>
	void GetVertexAll(YsArray <YsShellVertexHandle,N> &vtHdArray) const;
};

template <const int N>
void YsShellExt_SweepInfo::GetVertexAll(YsArray <YsShellVertexHandle,N> &vtHdArray) const
{
	vtHdArray.CleanUp();
	for(auto vtHd : allSrcVtHd)
	{
		vtHdArray.Append(vtHd);
	}
}

////////////////////////////////////////////////////////////

class YsShellExt_SweepInfoMultiStep : public YsShellExt_SweepInfo
{
public:
	enum ORIENTATION_CONTROL_TYPE
	{
		ORICON_PREVIOUS_SEGMENT,
		ORICON_NEXT_SEGMENT,
		ORICON_AVERAGE_ANGLE,
		ORICON_FROMPREVPOINT_TO_NEXTPOINT,
		// ORICON_PREVNORMAL_TIMES_PREVROTATION
	};

	class Option
	{
	public:
		/*! Only one of pathPnt or pathVtHd must be populated.  The other must be empty.
		*/
		YsArray <YsVec3> pathPnt;
		/*! Only one of pathPnt or pathVtHd must be populated.  The other must be empty.
		*/
		YsArray <YsShell::VertexHandle> pathVtHd;

		/*! Must be empty or same size as pathPnt or pathVtHd.
		*/
		YsArray <double> scaling;    // 

		/*! Defines the twist axis with with respect to the first (source) layer.
		*/
		YsVec3 twistAxis;

		/*! Defines the twist center with respect to the first (source) layer.
		*/
		YsVec3 twistCenter;

		/*! If twistAngle is empty, twistAxis and twistCenter will not be used.
		    If not empty, twistAngle.size() must match the number of path points.
		*/
		YsArray <double> twistAngle;

		ORIENTATION_CONTROL_TYPE oriconTypeMid;
		ORIENTATION_CONTROL_TYPE oriconTypeLast;

		Option()
		{
			twistAxis=YsVec3::Origin();
			twistCenter=YsVec3::Origin();
			oriconTypeMid=ORICON_AVERAGE_ANGLE;
			oriconTypeLast=ORICON_PREVIOUS_SEGMENT;
		}
	};

	class MidPoint
	{
	public:
		YsShellVertexHandle vtHd;
		YsVec3 pos;

		void Initialize(void);
	};
	class Layer
	{
	public:
		YsArray <MidPoint> pointArray;

		void Initialize(void);
	};
	YsHashTable <YSSIZE_T> srcVtKeyToMidPointIndex;
	YsArray <Layer> layerArray;

	class Quad
	{
	public:
		YsShellVertexHandle quadVtHd[4];
	};

	/*! This function cleans up the layer information. */
	void CleanUpLayer(void);

	/*! After MakeInfo (of the base class YsShellExt_SweepInfo), this function sets up a parallel sweeping with path. 
	    Scaling factor scaling[] is relative to the source. 
	    scaling[0] will no be used. */
	YSRESULT SetUpParallelSweepWithPath(const YsShellExt &shl,YSSIZE_T nPathVt,const YsShellVertexHandle pathVtHdArray[],const double scaling[]);

	/*! After MakeInfo (of the base class YsShellExt_SweepInfo), this function sets up a parallel sweeping with path. 
	    Scaling factor scaling[] is relative to the source. 
	    scaling[0] will no be used. */
	YSRESULT SetUpParallelSweepWithPath(const YsShellExt &shl,const YsConstArrayMask <YsShellVertexHandle> &pathVtHd,const double scaling[]);

	/*! After MakeInfo (of the base class YsShellExt_SweepInfo), this function sets up a parallel sweeping with path. 
	*/
	YSRESULT SetUpParallelSweepWithPath(const YsShellExt &shl,const Option &opt);

	/*! After MakeInfo, this function sets up a non-parallel sweeping with a path. */
	YSRESULT SetUpNonParallelSweepWithPath(const YsShellExt &shl,YSSIZE_T nPathVt,const YsShellVertexHandle pathVtHdArray[],YSBOOL isLoop,const double scaling[],ORIENTATION_CONTROL_TYPE oriconTypeMid,ORIENTATION_CONTROL_TYPE oriconTypeLast);

	/*! After MakeInfo, this function sets up a non-parallel sweeping with a path. */
	YSRESULT SetUpNonParallelSweepWithPath(const YsShellExt &shl,const YsConstArrayMask <YsShellVertexHandle> &pathVtHdArray,YSBOOL isLoop,const double scaling[],ORIENTATION_CONTROL_TYPE oriconType,ORIENTATION_CONTROL_TYPE );

	/*! After MakeInfo, this function sets up a solid-of revolution. */
	YSRESULT SetUpSolidOfRevolution(const YsShellExt &shl,const YsVec3 &axiso,const YsVec3 &axisv,int nStep,const double stepAngle,const YsVec3 &stepOffset,YSBOOL closeSolid);

private:  // Private for the time being
	YSRESULT SetUpNonParallelSweepWithPath(const YsShellExt &shl,YSSIZE_T nPathVt,const YsShellVertexHandle pathVtHdArray[],YSBOOL isLoop,const YsVec3 nom[],const double scaling[]);

public:
	/*! After populating vtHd members of points, call this function to make a list of quadrilaterals that fills the side faces. 
	    Since this modifies vertex-to-vertex mapping, this function is not const.  */
	YsArray <Quad> MakeSideFaceAndFirstToLastVertexMapping(const YsShellExt &shl);

private:
	void SetUpNLayer(const YsShellExt &shl,YSSIZE_T nLayer);

public:
	static YsArray <double> CalculateScalingForParallelSweepWithPathAndGuideLine(
	    const YsVec3 &sweepDir,const YsArray <YsVec3> &pathArray,const YsArray <YsVec3> &guideArray);

	static YsArray <double> CalculateScalingForParallelSweepWithPathAndGuideLine(
	    const YsShellExt &shl,const YsVec3 &sweepDir,const YsArray <YsShellVertexHandle> &pathVtHdArray,const YsArray <YsShellVertexHandle> &guideVtHdArray);

public:
	template <class SHLCLASS>
	YSRESULT MakeSweepPolygon(SHLCLASS &shl,YsColor col,YSBOOL deleteSrcPlg);
};

/* } */
#endif
