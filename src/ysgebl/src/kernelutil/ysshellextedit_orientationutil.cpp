/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_orientationutil.cpp
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

#include "ysshellextedit_orientationutil.h"

YsShellExtEdit_OrientationUtil::YsShellExtEdit_OrientationUtil()
{
}

YsShellExtEdit_OrientationUtil::~YsShellExtEdit_OrientationUtil()
{
}

YSRESULT YsShellExtEdit_OrientationUtil::RecalculateNormal(YsShellExtEdit &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);

	YsVec3 nom;
	if(YSOK==YsGetAverageNormalVector(nom,plVtPos))
	{
		shl.SetPolygonNormal(plHd,nom);
		return YSOK;
	}
	return YSERR;
}

/*static*/ void YsShellExtEdit_OrientationUtil::FixOrientationOfClosedSolid(YsShellExtEdit &shl)
{
	YsShellExt_OrientationUtil orientationUtil;
	orientationUtil.CleanUp();
	orientationUtil.FixOrientationOfClosedSolid(shl.Conv());
	auto plHdNeedFip=orientationUtil.GetPolygonNeedFlip();

	for(auto plHd : plHdNeedFip)
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetPolygon(plVtHd,plHd);
		plVtHd.Invert();
		shl.SetPolygonVertex(plHd,plVtHd);
	}

	orientationUtil.RecalculateNormalFromCurrentOrientation(shl.Conv());
	auto plNomPairNeedNormalReset=orientationUtil.GetPolygonNormalPair();
	for(auto plNomPair : plNomPairNeedNormalReset)
	{
		shl.SetPolygonNormal(plNomPair.plHd,plNomPair.nom);
	}
}

YSRESULT YsShellExtEdit_OrientationUtil::RecalculateNormal(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);
	for(auto plHd : shl.AllPolygon())
	{
		RecalculateNormal(shl,plHd);
	}
	return YSOK;
}

YSRESULT YsShellExtEdit_OrientationUtil::FixOrientationBasedOnAssignedNormal(YsShellExtEdit &shl)
{
	YsShellExt_OrientationUtil::FixOrientationBasedOnAssignedNormal(shl.Conv());

	YsShellExtEdit::StopIncUndo incUndo(shl);
	for(auto plHd : plHdNeedFlipArray)
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		plVtHd.Invert();
		shl.SetPolygonVertex(plHd,plVtHd);
	}

	return YSOK;
}


YSRESULT YsShellExtEdit_OrientationUtil::RecalculateNormalFromCurrentOrientation(YsShellExtEdit &shl)
{
	YsShellExt_OrientationUtil::RecalculateNormalFromCurrentOrientation(shl.Conv());

	YsShellExtEdit::StopIncUndo incUndo(shl);
	for(auto nomChg : plHdNeedNormalChangeArray)
	{
		shl.SetPolygonNormal(nomChg.plHd,nomChg.nom);
	}

	return YSOK;
}
