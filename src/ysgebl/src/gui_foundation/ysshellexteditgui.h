/* ////////////////////////////////////////////////////////////

File Name: ysshellexteditgui.h
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

#ifndef YSSHELLEXTEDITGUI_IS_INCLUDED
#define YSSHELLEXTEDITGUI_IS_INCLUDED
/* { */

#include <ysshellextedit.h>
#include <ysshellexttemporarymodification.h>
#include <ysshellext_clusteringutil.h>
#include <ysglbuffer.h>
#include <ysglbuffermanager_gl2.h>
#include <ysshellextgl.h>

class YsShellExtEditGui : public YsShellExtWithVbo <YsShellExtEdit>
{
public:
	class Highlight
	{
	public:
		// Why not just YsShellPolygonStore?
		// The user often want to flip through clusters of highlighted polygons.
		YSHASHKEY highlightedPolygonFocus;
		YsShellExt_PolygonClusteringUtil highlightedPolygon;
		YsShellExt_EdgeClusteringUtil highlightedEdge;
		YsGLVertexBuffer highlightedThickEdgeVertexBuffer;
		YsGLVertexBuffer highlightedEdgeVertexBuffer;
		YsGLVertexBuffer highlightedPolygonVertexBuffer;

		YsGLBufferManager::Handle highlightedThickEdgeVboHandle;
		YsGLBufferManager::Handle highlightedEdgeVboHandle;
		YsGLBufferManager::Handle highlightedPolygonVboHandle;

		Highlight();
		~Highlight();
		void CleanUp(void);
		void RemakeVertexBuffer(const YsShellExtEditGui &shl);  // Want to make it YsShellExt.  But, due to Visual C++'s bug, I cannot do it.  It wrongfully gives an error.
		void RemakeVboOnly(void);
	};

private:
	Highlight highlight;
	mutable TemporaryModification temporaryModification;
	YSBOOL needRemakeHighlightVertexBuffer;

public:
	YsShellExtEditGui();
	void CleanUp(void);

	void SetNeedRemakeHighlightVertexBuffer(YSBOOL needRemake);
	YSBOOL NeedRemakeHighlightVertexBuffer(void) const;
	Highlight &GetHighlight(void);
	const Highlight &GetHighlight(void) const;

	TemporaryModification &GetTemporaryModification(void) const;
};


/* } */
#endif
