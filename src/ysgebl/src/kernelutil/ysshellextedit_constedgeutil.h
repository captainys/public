/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_constedgeutil.h
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

#ifndef YSSHELLEXTEDIT_CONSTEDGEUTIL_IS_INCLUDED
#define YSSHELLEXTEDIT_CONSTEDGEUTIL_IS_INCLUDED
/* { */

#include <ysshellextedit.h>

/*! This function adds constraint edges on high-dihedral angle edges. */
YSRESULT YsShellExtEdit_AddConstEdgeByHighDihedralAngleThreshold(YsShellExtEdit &shl,const double angleThr);

/*! This function adds constraint edges along non-manifold angle edges, which are the edges used by only one polygon or more than two polygons. */
YSRESULT YsShellExtEdit_AddConstEdgeAlongNonManifoldEdge(YsShellExtEdit &shl);

/*! This function adds constraint edges along face-group boundaries. */
YSRESULT YsShellExtEdit_AddConstEdgeAlongFaceGroupBoundary(YsShellExtEdit &shl);

/*! This function connects const edges at the vertices where two const edges are converging, 
    and splits const edges at the vertices where more than two vertices are const-edge connected. */
YSRESULT YsShellExtEdit_ReconsiderConstEdge(YsShellExtEdit &shl);

/* } */
#endif
