/* ////////////////////////////////////////////////////////////

File Name: ysshellext_orientationutil.h
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

#ifndef YSSHELLEXT_ORIENTATIONUTIL_IS_INCLUDED
#define YSSHELLEXT_ORIENTATIONUTIL_IS_INCLUDED
/* { */

#include <ysshellextedit.h>

class YsShellExt_OrientationUtil
{
friend class YsShellExtEdit_OrientationUtil;

public:
	class PolygonNormalPair
	{
	public:
		YsShellPolygonHandle plHd;
		YsVec3 nom;
	};

private:
	const YsShellExt *shl;
	YsShellPolygonStore visitedPlHd;
	YsArray <YsShellPolygonHandle> plHdNeedFlipArray;
	YsArray <PolygonNormalPair> plHdNeedNormalChangeArray;

	YSBOOL abortFlag;
	YSBOOL fixOrientetionFromReliablePolygonEnded;
	YSBOOL fixOrientationOfClosedSolidEnded;
	YSBOOL normalRecalculationEnded;

	YSBOOL staticCreate;

public:
	YsShellExt_OrientationUtil();
	~YsShellExt_OrientationUtil();

	/*! This sets abort flag.  In multi-threaded environment, this will terminate a process running on this object. */
	void Abort(void);

	/*! Cleans up the internal data structure. */
	void CleanUp(void);

	/*! This function assumes that seedPlHd's orientation is reliable, and makes a list of polygons that need to be flipped starting from a seedPlHd. 
	    This function will not clear the list of polygons if it already exists. */
	YSRESULT FixOrientationFromReliablePolygon(const YsShellExt &shl,YsShellPolygonHandle seedPlHd,YSBOOL seedPlgNeedFlip);

	/*! This function assumes that seedPlHd's orientation is reliable, and makes a list of polygons that need to be flipped starting from a seedPlHd. 
	    This function will not clear the list of polygons if it already exists. 
	    The number of polygons covered in this function will be returned in nPlgCovered. */
	YSRESULT FixOrientationFromReliablePolygon(YSSIZE_T &nPlgCovered,const YsShellExt &shl,YsShellPolygonHandle seedPlHd,YSBOOL seedPlgNeedFlip);

	/*! This function assumes that the normal vectors assigned to polygons are reliable, and flips a polygon if
	    the orientation is inverted with respect to the assigned normal vector. */
	YSRESULT FixOrientationBasedOnAssignedNormal(const YsShellExt &shl);

	/*! This function makes a list of polygons that need to be flipped.  
	    Call CleanUp function before calling this function. */
	YSRESULT FixOrientationOfClosedSolid(const YsShellExt &shl);

	/*! This function re-calculate normal based on the polygon orientation and
	    makes list of polygon-normal array that stores updated normal. */
	YSRESULT RecalculateNormalFromCurrentOrientation(const YsShell &shl);

	/*! This function re-calculate normal of the polygons plHd based on the polygon orientation and
	    makes list of polygon-normal array that stores updated normal. */
	YSRESULT RecalculateNormalFromCurrentOrientation(const YsShell &shl,YsShellPolygonHandle plHd);

	/*! This function re-calculate normal of the polygons plHd[0] ... plHd[nPl-1] based on the polygon orientation and
	    makes list of polygon-normal array that stores updated normal. */
	YSRESULT RecalculateNormalFromCurrentOrientation(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! This function re-calculate normal of the polygons plHd[0] ... plHd[nPl-1] based on the polygon orientation and
	    makes list of polygon-normal array that stores updated normal. */
	template <const int N>
	YSRESULT RecalculateNormalFromCurrentOrientation(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray);

	/*! This function re-calculate normal of the polygons plHd assuming that the current normal direction is almost correct.
	    It is useful after moving vertices by small distance and the polygon normal needs to be re-calculated. */
	YSRESULT RecalculateNormalRespectingCurrentNormal(const YsShell &shl,YsShellPolygonHandle plHd);

	/*! This function re-calculate normal of the polygons plHd[0]...plHd[nPl-1] assuming that the current normal direction is almost correct.
	    It is useful after moving vertices by small distance and the polygon normal needs to be re-calculated. */
	YSRESULT RecalculateNormalRespectingCurrentNormal(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! This function re-calculate normal of the polygons plHd[0]...plHd[nPl-1] assuming that the current normal direction is almost correct.
	    It is useful after moving vertices by small distance and the polygon normal needs to be re-calculated. */
	template <const int N>
	YSRESULT RecalculateNormalRespectingCurrentNormal(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray);

	/*! This function will assign normals to the polygons that does not have an assigned normal vector by
	    taking average from the neighboring polygons.
	    It expands the normal-vector assignments as much as possible to reduce polygons without assigned normal.
	    Normal vectors will be assigned to the polygons in the subsequent Commit function call.
	*/
	YSRESULT ExpandNormal(const YsShellExt &shl);

	/*! This function returns a pointer to the array of polygons that needs to be flipped. */
	const YsArray <YsShellPolygonHandle> &GetPolygonNeedFlip(void) const;

	/*! This function returns polygon-normal pairs of the polygons that the normal vector needs to be reset. */
	const YsArray <PolygonNormalPair> &GetPolygonNormalPair(void) const;

	/*! This function inverts all polygons in YsShellExt */
	static void InvertPolygonAll(YsShellExt &shl);

	/*! */
	template <class SHLCLASS>
	void Commit(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo incUndo(shl);
		for(auto needFlip : plHdNeedFlipArray)
		{
			auto plVtHd=shl.GetPolygonVertex(needFlip);
			plVtHd.Invert();
			shl.SetPolygonVertex(needFlip,plVtHd);
		}
		for(auto nomChg : plHdNeedNormalChangeArray)
		{
			shl.SetPolygonNormal(nomChg.plHd,nomChg.nom);
		}
	}
};

template <const int N>
YSRESULT YsShellExt_OrientationUtil::RecalculateNormalFromCurrentOrientation(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	return RecalculateNormalFromCurrentOrientation(shl,plHdArray.GetN(),plHdArray);
}

template <const int N>
YSRESULT YsShellExt_OrientationUtil::RecalculateNormalRespectingCurrentNormal(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	return RecalculateNormalRespectingCurrentNormal(shl,plHdArray.GetN(),plHdArray);
}

/* } */
#endif
