/* ////////////////////////////////////////////////////////////

File Name: ysshellexteditgui.cpp
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

#include <ysglbuffermanager_gl2.h>

#include "ysshellexteditgui.h"


YsShellExtEditGui::Highlight::Highlight()
{
	highlightedThickEdgeVboHandle=nullptr;
	highlightedEdgeVboHandle=nullptr;
	highlightedPolygonVboHandle=nullptr;

	CleanUp();
}
YsShellExtEditGui::Highlight::~Highlight()
{
	CleanUp();
}
void YsShellExtEditGui::Highlight::CleanUp(void)
{
	highlightedPolygonFocus=YSNULLHASHKEY;
	highlightedPolygon.CleanUp();
	highlightedThickEdgeVertexBuffer.CleanUp();
	highlightedEdgeVertexBuffer.CleanUp();
	highlightedPolygonVertexBuffer.CleanUp();

	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();
	bufManager.Delete(highlightedThickEdgeVboHandle);
	bufManager.Delete(highlightedEdgeVboHandle);
	bufManager.Delete(highlightedPolygonVboHandle);

	highlightedThickEdgeVboHandle=nullptr;
	highlightedEdgeVboHandle=nullptr;
	highlightedPolygonVboHandle=nullptr;
}

void YsShellExtEditGui::Highlight::RemakeVertexBuffer(const YsShellExtEditGui &shl)
{
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	highlightedThickEdgeVertexBuffer.CleanUp();
	highlightedEdgeVertexBuffer.CleanUp();
	highlightedPolygonVertexBuffer.CleanUp();

	for(auto &cluster : highlightedEdge.AllCluster())
	{
		for(auto edge : cluster.edgInCluster)
		{
			highlightedThickEdgeVertexBuffer.AddVertex(shl.GetVertexPosition(edge[0]));
			highlightedThickEdgeVertexBuffer.AddVertex(shl.GetVertexPosition(edge[1]));
		}
	}

	for(auto &cluster : highlightedPolygon.AllCluster())
	{
		for(auto plHd : cluster.plgInCluster)
		{
			YsArray <YsVec3,4> plVtPos;
			shl.GetVertexListOfPolygon(plVtPos,plHd);

			for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
			{
				highlightedEdgeVertexBuffer.AddVertex(plVtPos[vtIdx]);
				highlightedEdgeVertexBuffer.AddVertex(plVtPos.GetCyclic(vtIdx+1));
			}

			if(3==plVtPos.GetN())
			{
				highlightedPolygonVertexBuffer.AddVertex(plVtPos[0]);
				highlightedPolygonVertexBuffer.AddVertex(plVtPos[1]);
				highlightedPolygonVertexBuffer.AddVertex(plVtPos[2]);
			}
			else
			{
				YsSword swd;

				swd.SetInitialPolygon(plVtPos.GetN(),plVtPos,NULL);
				swd.Triangulate();
				for(int i=0; i<swd.GetNumPolygon(); i++)
				{
					const YsArray <YsVec3> *tri=swd.GetPolygon(i);
					if(tri->GetN()>=3)
					{
						const YsVec3 triVtPos[3]={(*tri)[0],(*tri)[1],(*tri)[2]};
						highlightedPolygonVertexBuffer.AddVertex(triVtPos[0]);
						highlightedPolygonVertexBuffer.AddVertex(triVtPos[1]);
						highlightedPolygonVertexBuffer.AddVertex(triVtPos[2]);
					}
				}
			}
		}
	}

	RemakeVboOnly();

}

void YsShellExtEditGui::Highlight::RemakeVboOnly(void)
{
	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();

	if(nullptr==highlightedThickEdgeVboHandle)
	{
		highlightedThickEdgeVboHandle=bufManager.Create();
	}
	bufManager.MakeVtx(highlightedThickEdgeVboHandle,YsGL::LINES,highlightedThickEdgeVertexBuffer);

	if(nullptr==highlightedEdgeVboHandle)
	{
		highlightedEdgeVboHandle=bufManager.Create();
	}
	bufManager.MakeVtx(highlightedEdgeVboHandle,YsGL::LINES,highlightedEdgeVertexBuffer);

	if(nullptr==highlightedPolygonVboHandle)
	{
		highlightedPolygonVboHandle=bufManager.Create();
	}
	bufManager.MakeVtx(highlightedPolygonVboHandle,YsGL::TRIANGLES,highlightedPolygonVertexBuffer);
}

////////////////////////////////////////////////////////////

YsShellExtEditGui::YsShellExtEditGui()
{
	CleanUp();
	temporaryModification.SetShell(this->Conv());
}
void YsShellExtEditGui::CleanUp(void)
{
	YsShellExtEdit::CleanUp();
	highlight.CleanUp();
	temporaryModification.CleanUp();
	needRemakeHighlightVertexBuffer=YSFALSE;
}
void YsShellExtEditGui::SetNeedRemakeHighlightVertexBuffer(YSBOOL needRemake)
{
	needRemakeHighlightVertexBuffer=needRemake;
}
YSBOOL YsShellExtEditGui::NeedRemakeHighlightVertexBuffer(void) const
{
	return needRemakeHighlightVertexBuffer;
}

YsShellExtEditGui::Highlight &YsShellExtEditGui::GetHighlight(void)
{
	return highlight;
}
const YsShellExtEditGui::Highlight &YsShellExtEditGui::GetHighlight(void) const
{
	return highlight;
}

YsShellExt::TemporaryModification &YsShellExtEditGui::GetTemporaryModification(void) const
{
	return temporaryModification;
}
