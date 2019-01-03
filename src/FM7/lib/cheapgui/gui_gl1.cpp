#include <string.h>

#include <ysglheader.h>
#include <ysglfontdata.h>

#include "gui.h"


// For Windows XP compatibility, do everything within OpenGL 1.1

/* static */ void CheapGUI::CreateBitmapFontRenderer(void)
{
}
/* static */ void CheapGUI::SetBitmapFontRendererViewportSize(int wid,int hei)
{
}
/* static */ void CheapGUI::DeleteBitmapFontRenderer(void)
{
}

/* virtual */ void CheapGUI::Text::Draw()
{
	auto fontH=24;
	auto spaceH=(hei-fontH)/2;
	glColor4ub(this->r,this->g,this->b,this->a);
	glRasterPos2d((double)(x0+8),(double)(y0+hei-spaceH));
	YsGlDrawFontBitmap16x24(label.c_str());
}

/* virtual */ void CheapGUI::PushButton::Draw()
{
	{
		glColor4f(0,0,1,1);

		float vertex[8]=
		{
			(float)x0    ,(float)y0,
			(float)x0+wid,(float)y0,
			(float)x0+wid,(float)y0+hei,
			(float)x0    ,(float)y0+hei,
		};
		glBegin(GL_LINE_LOOP);
		glVertex2f(vertex[0],vertex[1]);
		glVertex2f(vertex[2],vertex[3]);
		glVertex2f(vertex[4],vertex[5]);
		glVertex2f(vertex[6],vertex[7]);
		glEnd();
	}
	{
		auto fontH=24;
		auto fontW=16;
		auto spaceH=(hei-fontH)/2;
		auto textW=fontW*label.size();
		auto spaceW=(wid-textW)/2;

		glColor4ub(this->r,this->g,this->b,this->a);
		glRasterPos2d((double)(x0+spaceW),(double)(y0+hei-spaceH));
		YsGlDrawFontBitmap16x24(label.c_str());
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
		if(true==checked)
		{
			const float col[4]=
			{
				float(this->r)/255.0f,
				float(this->g)/255.0f,
				float(this->b)/255.0f,
				1.0f
			};
			glColor4fv(col);
			glBegin(GL_TRIANGLE_FAN);
			glVertex2f(rectVertex[0],rectVertex[1]);
			glVertex2f(rectVertex[2],rectVertex[3]);
			glVertex2f(rectVertex[4],rectVertex[5]);
			glVertex2f(rectVertex[6],rectVertex[7]);
			glEnd();
		}

		const float col[4]={0,0,1,1};
		glColor4fv(col);
		glBegin(GL_LINE_LOOP);
		glVertex2f(rectVertex[0],rectVertex[1]);
		glVertex2f(rectVertex[2],rectVertex[3]);
		glVertex2f(rectVertex[4],rectVertex[5]);
		glVertex2f(rectVertex[6],rectVertex[7]);
		glEnd();
	}
	{
		auto fontH=24;
		auto fontW=16;
		auto spaceH=(hei-fontH)/2;
		auto textW=fontW*label.size();
		auto spaceW=(wid-textW)/2;
		if(true!=checked)
		{
			glColor4ub(this->r,this->g,this->b,this->a);
		}
		else
		{
			glColor4ub(255-this->r,255-this->g,255-this->b,this->a);
		}
		glRasterPos2d((double)(x0+spaceW),(double)(y0+hei-spaceH));
		YsGlDrawFontBitmap16x24(label.c_str());
	}
}

