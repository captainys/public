/* ////////////////////////////////////////////////////////////

File Name: ysshellextgl_gl2.cpp
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

#include <ysgl.h>
#include <ysglslcpp.h>
#include <ysglbuffermanager_gl2.h>
#include "ysshellextgl.h"



void YsHasShellExtVboSet::Render(const YsMatrix4x4 &viewModelTfm,RenderingOption opt) const
{
	int prevFrontFace;
	glGetIntegerv(GL_FRONT_FACE,&prevFrontFace);

	float mat[16],push[16];
	if(YSLEFT_ZPLUS_YPLUS==YsCoordSysModel)
	{
		YsMatrix4x4 total;
		total.Scale(1.0,1.0,-1.0);
		total*=viewModelTfm;
		total.GetOpenGlCompatibleMatrix(mat);
		glFrontFace(GL_CW);
	}
	else if(YSRIGHT_ZMINUS_YPLUS==YsCoordSysModel)
	{
		viewModelTfm.GetOpenGlCompatibleMatrix(mat);
		glFrontFace(GL_CCW);
	}

	// YsGLBufferManager::Handle solidShadedPosNomColHd;
	// YsGLBufferManager::Handle solidUnshadedPosColHd;
	// YsGLBufferManager::Handle trspShadedPosNomColHd;
	// YsGLBufferManager::Handle trspUnshadedPosColHd;
	// YsGLBufferManager::Handle lightPosColPointSizeHd;

	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();
	if(YSTRUE==opt.solidPolygon)
	{
		if(NULL!=YsGLSLSharedVariColorShaded3DRenderer())
		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.solidShadedPosNomColHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				YsGLSLRenderer renderer(YsGLSLSharedVariColorShaded3DRenderer());
				renderer.GetModelView(push);
				renderer.SetModelView(mat);
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(renderer,GL_TRIANGLES);
				renderer.SetModelView(push);
			}
		}
		if(NULL!=YsGLSLSharedVariColor3DRenderer())
		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.solidUnshadedPosColHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				YsGLSLRenderer renderer(YsGLSLSharedVariColor3DRenderer());
				renderer.GetModelView(push);
				renderer.SetModelView(mat);
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(renderer,GL_TRIANGLES);
				renderer.SetModelView(push);
			}
		}
	}
	if(YSTRUE==opt.backFace)
	{
		if(NULL!=YsGLSLSharedVariColor3DRenderer())
		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.backFacePosColHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				YsGLSLRenderer renderer(YsGLSLSharedVariColor3DRenderer());
				renderer.GetModelView(push);
				renderer.SetModelView(mat);
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(renderer,GL_TRIANGLES);
				renderer.SetModelView(push);
			}
		}
	}
	if(YSTRUE==opt.transparentPolygon)
	{
		if(NULL!=YsGLSLSharedVariColorShaded3DRenderer())
		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.trspShadedPosNomColHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				YsGLSLRenderer renderer(YsGLSLSharedVariColorShaded3DRenderer());
				renderer.GetModelView(push);
				renderer.SetModelView(mat);
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(renderer,GL_TRIANGLES);
				renderer.SetModelView(push);
			}
		}
		if(NULL!=YsGLSLSharedVariColor3DRenderer())
		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.trspUnshadedPosColHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				YsGLSLRenderer renderer(YsGLSLSharedVariColor3DRenderer());
				renderer.GetModelView(push);
				renderer.SetModelView(mat);
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(renderer,GL_TRIANGLES);
				renderer.SetModelView(push);
			}
		}
	}
	if(YSTRUE==opt.light)
	{
		if(NULL!=YsGLSLSharedFlashByPointSprite3DRenderer())
		{
			auto unitPtr=bufManager.GetBufferUnit(vboSet.lightPosColPointSizeHd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				int viewport[4];
				glGetIntegerv(GL_VIEWPORT,viewport);
#ifdef GL_PROGRAM_POINT_SIZE
				glEnable(GL_PROGRAM_POINT_SIZE);  // Needed for enabling gl_PointSize in the vertex shader.
#endif
#ifdef GL_POINT_SPRITE
                glEnable(GL_POINT_SPRITE);        // Needed for enabling gl_PointCoord in the fragment shader.
#endif
				YsGLSLRenderer renderer(YsGLSLSharedFlashByPointSprite3DRenderer());

				YsGLSLSet3DRendererZOffsetEnabled(renderer,1);
				YsGLSLSet3DRendererZOffset(renderer,-0.0001f);

				YsGLSLSet3DRendererViewportDimensionf(renderer,(float)viewport[2],(float)viewport[3]);
				YsGLSLSet3DRendererPointSizeMode(renderer,YSGLSL_POINTSPRITE_SIZE_IN_3DSPACE);

				renderer.GetModelView(push);
				renderer.SetModelView(mat);
				unitPtr->GetActualBuffer()->DrawPrimitiveVtxColPointSize(renderer,GL_POINTS);
				renderer.SetModelView(push);

#ifdef GL_PROGRAM_POINT_SIZE
				glDisable(GL_PROGRAM_POINT_SIZE);
#endif
#ifdef GL_POINT_SPRITE
                glDisable(GL_POINT_SPRITE);
#endif
            }
		}
	}
	if(YSTRUE==opt.shadow)
	{
		if(YSLEFT_ZPLUS_YPLUS==YsCoordSysModel)
		{
			glFrontFace(GL_CCW);
		}
		else
		{
			glFrontFace(GL_CW);
		}

		if(NULL!=YsGLSLSharedFlat3DRenderer())
		{
			for(auto hd : vboSet.plainPlgPosHd)
			{
				auto unitPtr=bufManager.GetBufferUnit(hd);
				if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
				{
					YsGLSLRenderer renderer(YsGLSLSharedFlat3DRenderer());
					renderer.SetUniformColor(0.0,0.0,0.0,1.0);
					renderer.GetModelView(push);
					renderer.SetModelView(mat);
					unitPtr->GetActualBuffer()->DrawPrimitiveVtx(renderer,GL_TRIANGLES);
					renderer.SetModelView(push);
				}
			}
		}
	}

	glFrontFace(prevFrontFace);
}

