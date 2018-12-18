/* ////////////////////////////////////////////////////////////

File Name: ysshellextgl_gl1.cpp
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

#include <ysglbuffermanager_gl1.h>
#include <ysshellextgl.h>



void YsHasShellExtVboSet::Render(const YsMatrix4x4 &viewModelTfm,RenderingOption opt) const
{
	glPushMatrix();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glLoadIdentity();
	glScaled(1.0,1.0,-1.0);

	double mat[16];
	viewModelTfm.GetOpenGlCompatibleMatrix(mat);
	glMultMatrixd(mat);

	auto &bufManager=YsGLBufferManager::GetSharedBufferManager();
	if(YSTRUE==opt.solidPolygon)
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.solidShadedPosNomColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			glEnable(GL_LIGHTING);
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(GL_TRIANGLES);
		}
		unitPtr=bufManager.GetBufferUnit(vboSet.solidUnshadedPosColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			glDisable(GL_LIGHTING);
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(GL_TRIANGLES);
		}
	}
	if(YSTRUE==opt.backFace)
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.backFacePosColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			glDisable(GL_LIGHTING);
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxCol(GL_TRIANGLES);
		}
	}
	if(YSTRUE==opt.transparentPolygon)
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.trspShadedPosNomColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			glEnable(GL_LIGHTING);
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(GL_TRIANGLES);
		}
		unitPtr=bufManager.GetBufferUnit(vboSet.trspUnshadedPosColHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			glDisable(GL_LIGHTING);
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxNomCol(GL_TRIANGLES);
		}
	}
	glDisable(GL_CULL_FACE);

	if(YSTRUE==opt.shadow)
	{
		glDisable(GL_LIGHTING);
		glColor3ub(0,0,0);
		for(auto hd : vboSet.plainPlgPosHd)
		{
			auto unitPtr=bufManager.GetBufferUnit(hd);
			if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
			{
				unitPtr->GetActualBuffer()->DrawPrimitiveVtx(GL_TRIANGLES);
			}
		}
	}

	if(YSTRUE==opt.light)
	{
		auto unitPtr=bufManager.GetBufferUnit(vboSet.lightPosColPointSizeHd);
		if(nullptr!=unitPtr && YsGLBufferManager::Unit::EMPTY!=unitPtr->GetState())
		{
			int viewport[4];
			glGetIntegerv(GL_VIEWPORT,viewport);

			float pointSize=(float)viewport[2]/800.0f;
			if(1.0f>pointSize)
			{
				pointSize=1.0f;
			}

			glPointSize(pointSize);
			glDisable(GL_LIGHTING);
			unitPtr->GetActualBuffer()->DrawPrimitiveVtxColPointSize(GL_POINTS);
			glPointSize(1);
		}
	}

	glPopMatrix();
}
