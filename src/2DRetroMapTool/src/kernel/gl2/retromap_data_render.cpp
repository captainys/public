#include <ysglcpp.h>
#include <ysglslcpp.h>
#include <ysglbuffer.h>

#include "../retromap_data.h"



void RetroMap_World::MapPiece::Render(void) const
{
	ReadyTextureIfNotReady();
	ReadyVboIfNotReady();

	auto &texMan=GetTexMan();
	auto texUnitPtr=texMan.GetTexture(texHd);
	if(nullptr!=texUnitPtr)
	{
		texUnitPtr->Bind(0);

		YsGLSLPlain3DRenderer renderer;
		renderer.SetTextureIdent(0);
		renderer.SetTextureType(YSGLSL_TEX_TYPE_ATTRIBUTE);
		renderer.DrawVtxTexCoordCol(GL_TRIANGLE_FAN,vtxBuf.GetN(),vtxBuf,texCoordBuf,colBuf);
	}
}

void RetroMap_World::MapPiece::RenderFrame(YsColor col) const
{
	YsGLSLPlain3DRenderer renderer;
	renderer.SetTextureIdent(0);
	renderer.SetTextureType(YSGLSL_TEX_TYPE_NONE);

	glLineWidth(10);
	renderer.SetUniformColor(col);
	renderer.DrawVtx(GL_LINE_LOOP,vtxBuf.GetN(),vtxBuf);
	glLineWidth(1);
}

void RetroMap_World::MarkUp::Render(void) const
{
	if(MARKUP_STRING==markUpType)
	{
		auto &texMan=GetTexMan();
		auto texUnitPtr=texMan.GetTexture(texHd);
		if(nullptr!=texUnitPtr)
		{
			texUnitPtr->Bind(0);

			YsGLSLPlain3DRenderer renderer;
			renderer.SetTextureIdent(0);
			renderer.SetTextureType(YSGLSL_TEX_TYPE_ATTRIBUTE);
			renderer.DrawVtxTexCoordCol(YsGL::TRIANGLE_FAN,vtxBuf.GetN(),vtxBuf,texCoordBuf,colBuf);

			renderer.SetTextureType(YSGLSL_TEX_TYPE_NONE);
			renderer.SetUniformColor(highlightCol);
			renderer.DrawVtx(YsGL::TRIANGLE_FAN,vtxBuf.GetN(),vtxBuf);
		}
	}
	else if(MARKUP_POINT_ARRAY==markUpType)
	{
		YsGLSLPlain3DRenderer renderer;
		renderer.SetTextureType(YSGLSL_TEX_TYPE_NONE);
		renderer.DrawVtxCol(YsGL::LINE_STRIP,vtxBuf.GetN(),vtxBuf,colBuf);

		if(0<markerSize)
		{
		#ifdef GL_PROGRAM_POINT_SIZE
			glEnable(GL_PROGRAM_POINT_SIZE);
		#endif
			renderer.SetUniformPointSize(markerSize);
			renderer.SetPointSizeMode(YSGLSL_POINTSPRITE_SIZE_IN_PIXEL);
			renderer.SetUniformColor(highlightCol);
			renderer.DrawVtx(YsGL::POINTS,vtxBuf.GetN(),vtxBuf);
		}

		renderer.DrawVtxCol(YsGL::TRIANGLES,arrowVtxBuf.GetN(),arrowVtxBuf,arrowColBuf);
	}
}
void RetroMap_World::MarkUp::RenderFrame(YsColor col) const
{
	YsGLSLPlain3DRenderer renderer;
	renderer.SetTextureIdent(0);
	renderer.SetTextureType(YSGLSL_TEX_TYPE_NONE);

	glLineWidth(10);
	renderer.SetUniformColor(col);
	renderer.DrawVtx(YsGL::LINE_LOOP,frameVtxBuf.GetN(),frameVtxBuf);
	glLineWidth(1);
}

void RetroMap_World::MapPiece::RenderOverlap(MapPiece::Shape shape,YsTextureManager::TexHandle texHd,YsColor col) const
{
	auto bmpSize=GetBitmapSize();
	auto visArea=shape.visibleArea;
	auto pos=shape.pos;

	auto realRect=shape.GetBoundingBox();

	YsVec3 quadVtPos[4];
	YsVec2 quadTexCoord[4];

	quadVtPos[0]=YsVec3(realRect.Min().x(),realRect.Min().y(),0.0);
	quadTexCoord[0]=YsVec2(0.0f,1.0f);

	quadVtPos[1]=YsVec3(realRect.Max().x(),realRect.Min().y(),0.0);
	quadTexCoord[1]=YsVec2(1.0f,1.0f);

	quadVtPos[2]=YsVec3(realRect.Max().x(),realRect.Max().y(),0.0);
	quadTexCoord[2]=YsVec2(1.0f,0.0f);

	quadVtPos[3]=YsVec3(realRect.Min().x(),realRect.Max().y(),0.0);
	quadTexCoord[3]=YsVec2(0.0f,0.0f);

	
	YsGLVertexBuffer vtxBuf;
	vtxBuf.Add(quadVtPos[0]);
	vtxBuf.Add(quadVtPos[1]);
	vtxBuf.Add(quadVtPos[2]);
	vtxBuf.Add(quadVtPos[3]);

	YsGLVertexBuffer2D texCoordBuf;
	texCoordBuf.Add(quadTexCoord[0]);
	texCoordBuf.Add(quadTexCoord[1]);
	texCoordBuf.Add(quadTexCoord[2]);
	texCoordBuf.Add(quadTexCoord[3]);

	YsGLColorBuffer colBuf;
	colBuf.Add(col);
	colBuf.Add(col);
	colBuf.Add(col);
	colBuf.Add(col);

	auto &texMan=GetTexMan();
	auto texUnitPtr=texMan.GetTexture(texHd);
	if(nullptr!=texUnitPtr)
	{
		texUnitPtr->Bind(0);

		YsGLSLPlain3DRenderer renderer;
		renderer.SetTextureIdent(0);
		renderer.SetTextureType(YSGLSL_TEX_TYPE_ATTRIBUTE);
		renderer.DrawVtxTexCoordCol(GL_TRIANGLE_FAN,vtxBuf.GetN(),vtxBuf,texCoordBuf,colBuf);
	}
}
