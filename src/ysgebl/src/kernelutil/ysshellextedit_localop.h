/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_localop.h
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

#ifndef YSSHELLEXTEDIT_LOCALOP_IS_INCLUDED
#define YSSHELLEXTEDIT_LOCALOP_IS_INCLUDED
/* { */

#include <ysshellextedit.h>
#include "ysshellext_localop.h"

YSRESULT YsShellExtEdit_InsertVertexOnEdge(YsShellExtEdit &shl,const YsShellVertexHandle edVtHd[2],YsShellVertexHandle vtHd);

/*! Collapses vertex vtHdToDelete into vtHdToSurvive, and deletes vtHdToDelete. */
YSRESULT YsShellExtEdit_CollapseEdge(YsShellExtEdit &shl,YsShellVertexHandle vtHdToDelete,YsShellVertexHandle vtHdToSurvive);

YSRESULT YsShellExtEdit_TriangulatePolygon(YsShellExtEdit &shl,YsShellPolygonHandle plHd);

/*! This function splits a constraint edge ceHd at vertex vtHd, and returns a handle to the new const edge.
    If constraint edge ceHd does not include vertex vtHd, this function will do nothing and returns NULL.
    If constraint edge ceHd is an open constraint edge, and vtHd is the first or the last of its vertices, this functil will do nothing and returns NULL.
    If constraint edge ceHd is a loop, it will be made an open constraint edge that has the same first and the last vertex, which is vtHd, and the
    return value will also be ceHd. */
YsShellExt::ConstEdgeHandle YsShellExtEdit_SplitConstEdge(YsShellExtEdit &shl,YsShellExt::ConstEdgeHandle ceHd,YsShellVertexHandle vtHd);

/*! This function splits all constedge using vertex vtHd at the vertex. */
void YsShellExtEdit_SplitConstEdgeAtVertex(YsShellExtEdit &shl,YsShellVertexHandle vtHd);

/*! This function merges given constraint edges into one continuous constraint edge.
    If successful, this function will return the handle to the newly assembled const-edge, which will be one of the given handles.
    If makeLoopIfClosed is YSTRUE, and if chained const-edges form a loop, const-edges will become a loop.
    If it cannot make a continuous vertex chain without a branch from the given const edges, this function will fail (do nothing) and return NULL. */
YsShellExt::ConstEdgeHandle YsShellExtEdit_MergeConstEdge(YsShellExtEdit &shl,YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[],YSBOOL makeLoopIfClosed);

template <const int N>
YsShellExt::ConstEdgeHandle YsShellExtEdit_MergeConstEdge(YsShellExtEdit &shl,const YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,YSBOOL makeLoopIfClosed)
{
	return YsShellExtEdit_MergeConstEdge(shl,ceHdArray.GetN(),ceHdArray,makeLoopIfClosed);
}


/*! This function applies the changes desribed in YsShell_LocalOperation::PolygonSplit. */
YSRESULT YsShellExtEdit_ApplyPolygonSplit(YsShellExtEdit &shl,const YsShell_LocalOperation::PolygonSplit &plgSplit);

/*! This function applies the changes desribed in YsShell_LocalOperation::ConstEdgeSplit. */
YSRESULT YsShellExtEdit_ApplyConstEdgeSplit(YsShellExtEdit &shl,const YsShell_LocalOperation::ConstEdgeSplit &ceSplit);

/*! This function applies the changes described in YsShell_LocalOperation::FaceGroupSplit. */
YSRESULT YsShellExtEdit_ApplyFaceGroupSplit(YsShellExtEdit &shl,const YsShell_LocalOperation::FaceGroupSplit &ceSplit);

/* } */
#endif
