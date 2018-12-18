/* ////////////////////////////////////////////////////////////

File Name: geblworkorder.h
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

#ifndef GEBLCMD_WORKORDER_IS_INCLUDED
#define GEBLCMD_WORKORDER_IS_INCLUDED
/* { */


#include <ysshellextedit.h>
#include <ysshelldnmtemplate.h>
#include <ysshellext_commandsupport.h>

class GeblCmd_WorkOrder
{
protected:
	YsShellDnmContainer <YsShellExtEdit> *shlGrpPtr;
	YsShellDnmContainer <YsShellExtEdit>::Node *slHd;

	YsShellExt_CommandSupport cmdSupport;

public:
	void SetCurrentShellGroup(YsShellDnmContainer <YsShellExtEdit> &shlGrp);
	void SetCurrentShell(YsShellDnmContainer <YsShellExtEdit>::Node *slHd);
	YSRESULT RunWorkOrder(const YsString &workOrder);

protected:
	YSRESULT RunConfigWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConfigTolerance(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConfigColorTolerance(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConfigNumThread(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunConstEdgeWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConstEdgeClear(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConstEdgeAdd(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConstEdgeDelete(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConstEdgeReconsider(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunConstEdgeMakeNoBubble(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunFaceGroupWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFaceGroupAssignElemSize(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFaceGroupAssignBoundaryLayerSpecification(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFaceGroupRemoveBoundaryLayerSpecification(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFaceGroupCreate(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFaceGroupMakeNoBubble(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFaceGroupExtract(const YsString &workOrder,const YsArray <YsString,16> &args);

protected:
	YSRESULT RunMergeVertexWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunMergeVertexAll(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunMergeVertexNonManifold(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunStitchWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunStitchFreeStitching(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunStitchLoopStitching(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunOrientationWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunOrientation_CalculateNormalFromOrientation(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunOrientation_CalculateOrientationClosedSolid(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunOrientation_ExpandNormal(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunGlobalWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunGlobalWorkOrder_DeleteUnusedVertex(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunFileIOWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFileIOWorkOrder_Save(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFileIOWorkOrder_Open(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFileIOWorkOrder_OpenSrf(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFileIOWorkOrder_OpenStl(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunFileIOWorkOrder_SaveVertexCurvature(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunControlWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);

	YSRESULT RunRepairWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunRepairWorkOrder_Delete(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunRepairWorkOrder_Delete_IdenticalPolygon(const YsString &workOrder,const YsArray <YsString,16> &args);
	YSRESULT RunRepairWorkOrder_Delete_FlatTriangle(const YsString &workOrder,const YsArray <YsString,16> &args);

	virtual YSRESULT RunCustomWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args);

	virtual void ShowError(const YsString &workOrder,const YsString &reason) const;

public:
	void ShowHelp(void) const;
protected:
	virtual void ShowCustomHelp(void) const;
};


/* } */
#endif
