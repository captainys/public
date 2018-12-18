#include <string.h>

#include <ysgl.h>
#include <ysglsldrawfontbitmap.h>

#include "gui.h"



/* static */ void CheapGUI::CreateBitmapFontRenderer(void)
{
	YsGLSLCreateSharedBitmapFontRenderer();
	YsGLSLCreateSharedRenderer();
	{
		auto renderer=YsGLSLSharedBitmapFontRenderer();
		YsGLSLUseBitmapFontRenderer(renderer);
		YsGLSLBitmapFontRendererRequestFontSize(renderer,24,20);
		YsGLSLEndUseBitmapFontRenderer(renderer);
	}
}
/* static */ void CheapGUI::SetBitmapFontRendererViewportSize(int wid,int hei)
{
	auto renderer=YsGLSLSharedBitmapFontRenderer();
	YsGLSLUseBitmapFontRenderer(renderer);
	YsGLSLSetBitmapFontRendererViewportSize(renderer,wid,hei);
	YsGLSLEndUseBitmapFontRenderer(renderer);
}
/* static */ void CheapGUI::DeleteBitmapFontRenderer(void)
{
	YsGLSLDeleteSharedBitmapFontRenderer();
}

/* virtual */ void CheapGUI::Text::Draw()
{
	auto *renderer=YsGLSLSharedBitmapFontRenderer();
	YsGLSLUseBitmapFontRenderer(renderer);
	auto fontH=YsGLSLGetBitmapFontRendererFontHeight(renderer);
	auto spaceH=(hei-fontH)/2;
	YsGLSLSetBitmapFontRendererColor4ub(renderer,this->r,this->g,this->b,this->a);
	YsGLSLRenderBitmapFontString2D(renderer,x0+8,y0+hei-spaceH,label.c_str());
	YsGLSLEndUseBitmapFontRenderer(renderer);
}

/* virtual */ void CheapGUI::PushButton::Draw()
{
	{
		auto renderer=YsGLSLSharedPlain2DRenderer();
		YsGLSLUsePlain2DRenderer(renderer);

		YsGLSLUseWindowCoordinateInPlain2DDrawing(renderer,1);

		const float col[4]={0,0,1,1};
		YsGLSLSetPlain2DRendererUniformColor(renderer,col);

		float vertex[8]=
		{
			(float)x0    ,(float)y0,
			(float)x0+wid,(float)y0,
			(float)x0+wid,(float)y0+hei,
			(float)x0    ,(float)y0+hei,
		};
		YsGLSLDrawPlain2DPrimitiveVtxfv(renderer,GL_LINE_LOOP,4,vertex);

		YsGLSLEndUsePlain2DRenderer(renderer);
	}
	{
		auto *renderer=YsGLSLSharedBitmapFontRenderer();
		YsGLSLUseBitmapFontRenderer(renderer);
		auto fontH=YsGLSLGetBitmapFontRendererFontHeight(renderer);
		auto spaceH=(hei-fontH)/2;
		auto textW=YsGLSLGetBitmapFontRendererFontWidth(renderer)*label.size();
		auto spaceW=(wid-textW)/2;
		YsGLSLSetBitmapFontRendererColor4ub(renderer,this->r,this->g,this->b,this->a);
		YsGLSLRenderBitmapFontString2D(renderer,x0+spaceW,y0+hei-spaceH,label.c_str());
		YsGLSLEndUseBitmapFontRenderer(renderer);
	}
}

/* virtual */ void CheapGUI::CheckBox::Draw()
{
	const float rectVertex[8]=
	{
		(float)x0    ,(float)y0,
		(float)x0+wid,(float)y0,
		(float)x0+wid,(float)y0+hei,
		(float)x0    ,(float)y0+hei,
	};

	{
		auto renderer=YsGLSLSharedPlain2DRenderer();
		YsGLSLUsePlain2DRenderer(renderer);

		YsGLSLUseWindowCoordinateInPlain2DDrawing(renderer,1);

		if(true==checked)
		{
			const float col[4]=
			{
				float(this->r)/255.0f,
				float(this->g)/255.0f,
				float(this->b)/255.0f,
				1.0f
			};
			YsGLSLSetPlain2DRendererUniformColor(renderer,col);
			YsGLSLDrawPlain2DPrimitiveVtxfv(renderer,GL_TRIANGLE_FAN,4,rectVertex);
		}

		const float col[4]={0,0,1,1};
		YsGLSLSetPlain2DRendererUniformColor(renderer,col);
		YsGLSLDrawPlain2DPrimitiveVtxfv(renderer,GL_LINE_LOOP,4,rectVertex);
		YsGLSLEndUsePlain2DRenderer(renderer);
	}
	{
		auto *renderer=YsGLSLSharedBitmapFontRenderer();
		YsGLSLUseBitmapFontRenderer(renderer);
		auto fontH=YsGLSLGetBitmapFontRendererFontHeight(renderer);
		auto spaceH=(hei-fontH)/2;
		auto textW=YsGLSLGetBitmapFontRendererFontWidth(renderer)*label.size();
		auto spaceW=(wid-textW)/2;
		if(true!=checked)
		{
			YsGLSLSetBitmapFontRendererColor4ub(renderer,this->r,this->g,this->b,this->a);
		}
		else
		{
			YsGLSLSetBitmapFontRendererColor4ub(renderer,255-this->r,255-this->g,255-this->b,this->a);
		}
		YsGLSLRenderBitmapFontString2D(renderer,x0+spaceW,y0+hei-spaceH,label.c_str());
		YsGLSLEndUseBitmapFontRenderer(renderer);
	}
}

