/* ////////////////////////////////////////////////////////////

File Name: fsguidraw.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include "fsguicommondrawing.h"
#include "fsgui.h"
#include "fsguibitmaputil.h"


class FsGuiFont
{
public:
	static int GetWidth(void);
	static int GetHeight(void);
};

int FsGuiFont::GetWidth(void)
{
	int fontWid,fontHei;
	FsGuiObject::GetAsciiCharacterBitmapSize(fontWid,fontHei);
	return fontWid;
}

int FsGuiFont::GetHeight(void)
{
	int fontWid,fontHei;
	FsGuiObject::GetAsciiCharacterBitmapSize(fontWid,fontHei);
	return fontHei;
}

////////////////////////////////////////////////////////////

static void FsGetRoundRectCoord(double plg[28*2],int x1,int y1,int x2,int y2,const double roundRatio,const double roundPix,const YSBOOL round[4])
{
/*
      round[0]               round[1]
   (x1,y1)     ---------------   (x2,y1)
              /               \
             |                 |
             |                 |
              \               /
   (x1,y2)     ---------------   (x2,y2)
      round[2]               round[3]
*/
	const double roundCoeff[4]=
	{
		((NULL==round || YSTRUE==round[0]) ? 1.0 : 0.0),
		((NULL==round || YSTRUE==round[1]) ? 1.0 : 0.0),
		((NULL==round || YSTRUE==round[2]) ? 1.0 : 0.0),
		((NULL==round || YSTRUE==round[3]) ? 1.0 : 0.0)
	};

	if(x2<x1)
	{
		YsSwapSomething(x1,x2);
	}
	if(y2<y1)
	{
		YsSwapSomething(y1,y2);
	}

	const double dy=(float)(y2-y1);
	const double hy=dy/2.0f;
	const double rad=dy*roundRatio+roundPix;
	const int ym=(y1+y2)/2;

	// Top half
	plg[ 0*2  ]=(double)x1;
	plg[ 0*2+1]=(double)ym+(hy-rad);
	plg[ 1*2  ]=(double)x1+(rad   -YsCos15deg*rad)*roundCoeff[0];
	plg[ 1*2+1]=(double)ym+(hy-rad+YsSin15deg*rad);
	plg[ 2*2  ]=(double)x1+(rad   -YsCos30deg*rad)*roundCoeff[0];
	plg[ 2*2+1]=(double)ym+(hy-rad+YsSin30deg*rad);
	plg[ 3*2  ]=(double)x1+(rad   -YsCos45deg*rad)*roundCoeff[0];
	plg[ 3*2+1]=(double)ym+(hy-rad+YsSin45deg*rad);
	plg[ 4*2  ]=(double)x1+(rad   -YsCos60deg*rad)*roundCoeff[0];
	plg[ 4*2+1]=(double)ym+(hy-rad+YsSin60deg*rad);
	plg[ 5*2  ]=(double)x1+(rad   -YsCos75deg*rad)*roundCoeff[0];
	plg[ 5*2+1]=(double)ym+(hy-rad+YsSin75deg*rad);
	plg[ 6*2  ]=(double)x1+(rad                  )*roundCoeff[0];
	plg[ 6*2+1]=(double)ym+hy;

	plg[ 7*2  ]=(double)x2-(rad                  )*roundCoeff[1];
	plg[ 7*2+1]=(double)ym+hy;
	plg[ 8*2  ]=(double)x2-(rad   -YsCos75deg*rad)*roundCoeff[1];
	plg[ 8*2+1]=(double)ym+(hy-rad+YsSin75deg*rad);
	plg[ 9*2  ]=(double)x2-(rad   -YsCos60deg*rad)*roundCoeff[1];
	plg[ 9*2+1]=(double)ym+(hy-rad+YsSin60deg*rad);
	plg[10*2  ]=(double)x2-(rad   -YsCos45deg*rad)*roundCoeff[1];
	plg[10*2+1]=(double)ym+(hy-rad+YsSin45deg*rad);
	plg[11*2  ]=(double)x2-(rad   -YsCos30deg*rad)*roundCoeff[1];
	plg[11*2+1]=(double)ym+(hy-rad+YsSin30deg*rad);
	plg[12*2  ]=(double)x2-(rad   -YsCos15deg*rad)*roundCoeff[1];
	plg[12*2+1]=(double)ym+(hy-rad+YsSin15deg*rad);
	plg[13*2  ]=(double)x2;
	plg[13*2+1]=(double)ym+(hy-rad);

	// Bottom half
	plg[14*2  ]=(double)x2;
	plg[14*2+1]=(double)ym-(hy-rad);
	plg[15*2  ]=(double)x2-(rad   -YsCos15deg*rad)*roundCoeff[3];
	plg[15*2+1]=(double)ym-(hy-rad+YsSin15deg*rad);
	plg[16*2  ]=(double)x2-(rad   -YsCos30deg*rad)*roundCoeff[3];
	plg[16*2+1]=(double)ym-(hy-rad+YsSin30deg*rad);
	plg[17*2  ]=(double)x2-(rad   -YsCos45deg*rad)*roundCoeff[3];
	plg[17*2+1]=(double)ym-(hy-rad+YsSin45deg*rad);
	plg[18*2  ]=(double)x2-(rad   -YsCos60deg*rad)*roundCoeff[3];
	plg[18*2+1]=(double)ym-(hy-rad+YsSin60deg*rad);
	plg[19*2  ]=(double)x2-(rad   -YsCos75deg*rad)*roundCoeff[3];
	plg[19*2+1]=(double)ym-(hy-rad+YsSin75deg*rad);
	plg[20*2  ]=(double)x2-(rad                  )*roundCoeff[3];
	plg[20*2+1]=(double)ym-hy;

	plg[21*2  ]=(double)x1+(rad                  )*roundCoeff[2];
	plg[21*2+1]=(double)ym-hy;
	plg[22*2  ]=(double)x1+(rad   -YsCos75deg*rad)*roundCoeff[2];
	plg[22*2+1]=(double)ym-(hy-rad+YsSin75deg*rad);
	plg[23*2  ]=(double)x1+(rad   -YsCos60deg*rad)*roundCoeff[2];
	plg[23*2+1]=(double)ym-(hy-rad+YsSin60deg*rad);
	plg[24*2  ]=(double)x1+(rad   -YsCos45deg*rad)*roundCoeff[2];
	plg[24*2+1]=(double)ym-(hy-rad+YsSin45deg*rad);
	plg[25*2  ]=(double)x1+(rad   -YsCos30deg*rad)*roundCoeff[2];
	plg[25*2+1]=(double)ym-(hy-rad+YsSin30deg*rad);
	plg[26*2  ]=(double)x1+(rad   -YsCos15deg*rad)*roundCoeff[2];
	plg[26*2+1]=(double)ym-(hy-rad+YsSin15deg*rad);
	plg[27*2  ]=(double)x1;
	plg[27*2+1]=(double)ym-(hy-rad);
}

static void FsGuiDrawRoundRect(int x1,int y1,int x2,int y2,const YsColor &color,double roundRatio,double roundPix,const YSBOOL round[4]=NULL)
{
	double plg[28*2];
	FsGetRoundRectCoord(plg,x1,y1,x2,y2,roundRatio,roundPix,round);
	FsGuiCommonDrawing::DrawPolygon(28,plg,color);
}

static void FsGuiDrawEmptyRoundRect(int x1,int y1,int x2,int y2,const YsColor &color,double roundRatio,double roundPix,const YSBOOL round[4]=NULL)
{
	double plg[29*2];
	FsGetRoundRectCoord(plg,x1,y1,x2,y2,roundRatio,roundPix,round);
	plg[28*2  ]=plg[0];
	plg[28*2+1]=plg[1];
	FsGuiCommonDrawing::DrawLineStrip(29,plg,color);
}

static void FsGuiDrawFlashyRect(
    int x1,int y1,int x2,int y2,
    const YsColor &baseCol,const YsColor &frameCol,double roundRatio,double roundPix,YSBOOL frame,YSBOOL orientation,const YSBOOL round[4]=NULL)
{
/*
      round[0]               round[1]
   (x1,y1)     ---------------   (x2,y1)
              /               \
   (x1,yu)   |                 | (x2,yu)
             |                 |
   (x1,ym)   |-----------------| (x2,ym)
             |                 |
   (x1,yd)   |                 | (x2,yd)
              \               /
   (x1,y2)     ---------------   (x2,y2)
      round[2]               round[3]
*/

	if(x2<x1)
	{
		YsSwapSomething(x1,x2);
	}
	if(y2<y1)
	{
		YsSwapSomething(y1,y2);
	}

	const double dy=(float)(y2-y1);
	const double hy=dy/2.0f;
	const double rad=dy*roundRatio+roundPix;
	const int ym=(y1+y2)/2;

	const double vy=(orientation==YSTRUE ? -1.0f : 1.0f);

	const int maxNVtx=14;
	double plg[maxNVtx*2];
	YsColor col[maxNVtx];


	const double roundCoeff[4]=
	{
		((NULL==round ||YSTRUE==round[0]) ? 1.0 : 0.0),
		((NULL==round ||YSTRUE==round[1]) ? 1.0 : 0.0),
		((NULL==round ||YSTRUE==round[2]) ? 1.0 : 0.0),
		((NULL==round ||YSTRUE==round[3]) ? 1.0 : 0.0)
	};


	// Top quarter to bottom quarter
	plg[0*2  ]=(double)x1;
	plg[0*2+1]=(double)ym+(vy*(hy-rad));
	plg[1*2  ]=(double)x1+(rad       -YsCos15deg*rad)*roundCoeff[0];
	plg[1*2+1]=(double)ym+(vy*(hy-rad+YsSin15deg*rad));
	plg[2*2  ]=(double)x1+(rad       -YsCos30deg*rad)*roundCoeff[0];
	plg[2*2+1]=(double)ym+(vy*(hy-rad+YsSin30deg*rad));
	plg[3*2  ]=(double)x1+(rad       -YsCos45deg*rad)*roundCoeff[0];
	plg[3*2+1]=(double)ym+(vy*(hy-rad+YsSin45deg*rad));
	plg[4*2  ]=(double)x1+(rad       -YsCos60deg*rad)*roundCoeff[0];
	plg[4*2+1]=(double)ym+(vy*(hy-rad+YsSin60deg*rad));
	plg[5*2  ]=(double)x1+(rad       -YsCos75deg*rad)*roundCoeff[0];
	plg[5*2+1]=(double)ym+(vy*(hy-rad+YsSin75deg*rad));
	plg[6*2  ]=(double)x1+(rad                      )*roundCoeff[0];
	plg[6*2+1]=(double)ym+(vy*hy);

	plg[ 7*2  ]=(double)x2-(rad                      )*roundCoeff[1];
	plg[ 7*2+1]=(double)ym+(vy*hy);
	plg[ 8*2  ]=(double)x2-(rad       -YsCos75deg*rad)*roundCoeff[1];
	plg[ 8*2+1]=(double)ym+(vy*(hy-rad+YsSin75deg*rad));
	plg[ 9*2  ]=(double)x2-(rad       -YsCos60deg*rad)*roundCoeff[1];
	plg[ 9*2+1]=(double)ym+(vy*(hy-rad+YsSin60deg*rad));
	plg[10*2  ]=(double)x2-(rad       -YsCos45deg*rad)*roundCoeff[1];
	plg[10*2+1]=(double)ym+(vy*(hy-rad+YsSin45deg*rad));
	plg[11*2  ]=(double)x2-(rad       -YsCos30deg*rad)*roundCoeff[1];
	plg[11*2+1]=(double)ym+(vy*(hy-rad+YsSin30deg*rad));
	plg[12*2  ]=(double)x2-(rad       -YsCos15deg*rad)*roundCoeff[1];
	plg[12*2+1]=(double)ym+(vy*(hy-rad+YsSin15deg*rad));
	plg[13*2  ]=(double)x2;
	plg[13*2+1]=(double)ym+(vy*(hy-rad));

	for(int i=0; i<14; ++i)
	{
		const double t=fabs(plg[i*2+1]-(double)ym)/hy;
		const double addition=YsSmaller(0.1+t*0.15,0.2);
		col[i].SetDoubleRGB(
		    YsBound(baseCol.Rd()+addition,0.0,1.0),
		    YsBound(baseCol.Gd()+addition,0.0,1.0),
		    YsBound(baseCol.Bd()+addition,0.0,1.0));
	}

	FsGuiCommonDrawing::DrawGradationPolygon(14,plg,col);
	if(YSTRUE==frame)
	{
		FsGuiCommonDrawing::DrawLineStrip(14,plg,frameCol);
	}

	plg[0*2  ]=(double)x1;
	plg[0*2+1]=(double)ym;
	plg[1*2  ]=(double)x1;
	plg[1*2+1]=(double)ym+(vy*(hy-rad));
	plg[2*2  ]=(double)x2;
	plg[2*2+1]=(double)ym+(vy*(hy-rad));
	plg[3*2  ]=(double)x2;
	plg[3*2+1]=(double)ym;

	for(int i=0; i<4; ++i)
	{
		const double t=fabs(plg[i*2+1]-(double)ym)/hy;
		const double addition=YsSmaller(0.1+t*0.15,0.2);
		col[i].SetDoubleRGB(
		    YsBound(baseCol.Rd()+addition,0.0,1.0),
		    YsBound(baseCol.Gd()+addition,0.0,1.0),
		    YsBound(baseCol.Bd()+addition,0.0,1.0));
	}

	FsGuiCommonDrawing::DrawGradationPolygon(4,plg,col);
	if(YSTRUE==frame)
	{
		FsGuiCommonDrawing::DrawLine(plg[0],plg[1],plg[2],plg[3],frameCol);
		FsGuiCommonDrawing::DrawLine(plg[4],plg[5],plg[6],plg[7],frameCol);
	}


	plg[0*2  ]=(double)x1;
	plg[0*2+1]=(double)ym;
	plg[1*2  ]=(double)x1;
	plg[1*2+1]=(double)ym-(vy*(hy-rad));
	plg[2*2  ]=(double)x2;
	plg[2*2+1]=(double)ym-(vy*(hy-rad));
	plg[3*2  ]=(double)x2;
	plg[3*2+1]=(double)ym;

	FsGuiCommonDrawing::DrawPolygon(4,plg,baseCol);
	if(YSTRUE==frame)
	{
		FsGuiCommonDrawing::DrawLine(plg[0],plg[1],plg[2],plg[3],frameCol);
		FsGuiCommonDrawing::DrawLine(plg[4],plg[5],plg[6],plg[7],frameCol);
	}

	plg[0*2  ]=(double)x1;
	plg[0*2+1]=(double)ym-(vy*(hy-rad));
	plg[1*2  ]=(double)x1+(rad       -YsCos15deg*rad)*roundCoeff[2];
	plg[1*2+1]=(double)ym-(vy*(hy-rad+YsSin15deg*rad));
	plg[2*2  ]=(double)x1+(rad       -YsCos30deg*rad)*roundCoeff[2];
	plg[2*2+1]=(double)ym-(vy*(hy-rad+YsSin30deg*rad));
	plg[3*2  ]=(double)x1+(rad       -YsCos45deg*rad)*roundCoeff[2];
	plg[3*2+1]=(double)ym-(vy*(hy-rad+YsSin45deg*rad));
	plg[4*2  ]=(double)x1+(rad       -YsCos60deg*rad)*roundCoeff[2];
	plg[4*2+1]=(double)ym-(vy*(hy-rad+YsSin60deg*rad));
	plg[5*2  ]=(double)x1+(rad       -YsCos75deg*rad)*roundCoeff[2];
	plg[5*2+1]=(double)ym-(vy*(hy-rad+YsSin75deg*rad));
	plg[6*2  ]=(double)x1+(rad                      )*roundCoeff[2];
	plg[6*2+1]=(double)ym-(vy*hy);

	plg[ 7*2  ]=(double)x2-(rad                      )*roundCoeff[3];
	plg[ 7*2+1]=(double)ym-(vy*hy);
	plg[ 8*2  ]=(double)x2-(rad       -YsCos75deg*rad)*roundCoeff[3];
	plg[ 8*2+1]=(double)ym-(vy*(hy-rad+YsSin75deg*rad));
	plg[ 9*2  ]=(double)x2-(rad       -YsCos60deg*rad)*roundCoeff[3];
	plg[ 9*2+1]=(double)ym-(vy*(hy-rad+YsSin60deg*rad));
	plg[10*2  ]=(double)x2-(rad       -YsCos45deg*rad)*roundCoeff[3];
	plg[10*2+1]=(double)ym-(vy*(hy-rad+YsSin45deg*rad));
	plg[11*2  ]=(double)x2-(rad       -YsCos30deg*rad)*roundCoeff[3];
	plg[11*2+1]=(double)ym-(vy*(hy-rad+YsSin30deg*rad));
	plg[12*2  ]=(double)x2-(rad       -YsCos15deg*rad)*roundCoeff[3];
	plg[12*2+1]=(double)ym-(vy*(hy-rad+YsSin15deg*rad));
	plg[13*2  ]=(double)x2;
	plg[13*2+1]=(double)ym-(vy*(hy-rad));

	FsGuiCommonDrawing::DrawPolygon(14,plg,baseCol);
	if(YSTRUE==frame)
	{
		FsGuiCommonDrawing::DrawLineStrip(14,plg,frameCol);
	}
}

////////////////////////////////////////////////////////////

void FsGuiStatusBar::Show(int y0) const
{
	for(int c=0; c<columnArray.GetN(); ++c)
	{
		const int x0=widthInPixel*c/(int)columnArray.GetN();
		const int x1=widthInPixel*(c+1)/(int)columnArray.GetN();
		const int y=y0+(heightInPixel-columnArray[c].bmpCache.GetHeight())/2;

		FsGuiCommonDrawing::DrawRect(x0,y0,x1,y0+heightInPixel,bgColor,YSTRUE);
		FsGuiCommonDrawing::DrawBmpArray(columnArray[c].bmpCache,x0,y);
		FsGuiCommonDrawing::DrawLine(x1,y0,x1,y0+heightInPixel,defFgCol);
	}
	needRedraw=YSFALSE;
}

////////////////////////////////////////////////////////////

void FsGuiPopUpMenu::Show(const FsGuiDialog *excludeFromDrawing) const
{
	double tri[6];
	if(YSTRUE==pullDown)
	{
		FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei,bgCol,YSTRUE);

		YsListItem <FsGuiPopUpMenuItem> *ptr;
		ptr=NULL;
		while(NULL!=(ptr=itemList.FindNext(ptr)))
		{
			const int hSpace=ptr->dat.wid-ptr->dat.msgWid;
			const int vSpace=ptr->dat.hei-ptr->dat.msgHei;

			const int rectX0=ptr->dat.x0+hSpace/2;
			const int rectY0=ptr->dat.y0+vSpace/2;
			const int rectX1=ptr->dat.x0+ptr->dat.wid-hSpace/2;
			const int rectY1=ptr->dat.y0+ptr->dat.hei-vSpace/2;

			ShowItem(&ptr->dat,rectX0,rectY0,rectX1,rectY1);
		}
	}
	else
	{
		FsGuiCommonDrawing::DrawRect(x0-1,y0-1,x0+wid,y0+hei,bgCol,YSTRUE);
		FsGuiCommonDrawing::DrawRect(x0-1,y0-1,x0+wid,y0+hei,frameCol,YSFALSE);

		const int rectX0=x0+defHSpaceUnit+defHAnnotation;
		const int rectX1=x0+wid-(defHSpaceUnit+defHAnnotation);
		YsListItem <FsGuiPopUpMenuItem> *ptr;
		ptr=NULL;
		while(NULL!=(ptr=itemList.FindNext(ptr)))
		{
			const YsColor annotationCol=(&ptr->dat==mouseOn ? activeFgCol : fgCol);

			const int vSpace=ptr->dat.hei-ptr->dat.msgHei;

			const int rectY0=ptr->dat.y0+vSpace/2;
			const int rectY1=ptr->dat.y0+ptr->dat.hei-vSpace/2;

			ShowItem(&ptr->dat,rectX0,rectY0,rectX1,rectY1);

			if(ptr->dat.GetSubMenu()!=NULL)
			{
				tri[0]=(double)(x0+wid)-(double)defHSpaceUnit/2.0-(double)defHAnnotation;
				tri[1]=(double)ptr->dat.y0+(double)defVSpaceUnit/4.0;
				tri[2]=(double)(x0+wid)-(double)defHSpaceUnit/2.0;
				tri[3]=(double)ptr->dat.y0+(double)ptr->dat.hei/2.0;
				tri[4]=(double)(x0+wid)-(double)defHSpaceUnit/2-defHAnnotation;
				tri[5]=(double)ptr->dat.y0+(double)ptr->dat.hei-(double)defVSpaceUnit/4.0;
				FsGuiCommonDrawing::DrawPolygon(3,tri,annotationCol);
			}

			if(ptr->dat.checked==YSTRUE)
			{
				double tri[6];
				tri[0]=(double)x0+(double)defHSpaceUnit/2.0;
				tri[1]=(double)ptr->dat.y0+(double)ptr->dat.hei/2.0;
				tri[2]=(double)x0+(double)defHSpaceUnit/2.0+(double)defHAnnotation/2.0;
				tri[3]=(double)ptr->dat.y0+(double)ptr->dat.hei-(double)defVSpaceUnit/4.0;
				tri[4]=(double)x0+(double)defHSpaceUnit/2.0+(double)defHAnnotation;
				tri[5]=(double)ptr->dat.y0+(double)defVSpaceUnit/4.0;
				FsGuiCommonDrawing::DrawLine(tri[0],tri[1],tri[2],tri[3],annotationCol);
				FsGuiCommonDrawing::DrawLine(tri[2],tri[3],tri[4],tri[5],annotationCol);
				FsGuiCommonDrawing::DrawLine(tri[0]+1,tri[1],tri[2]+1,tri[3],annotationCol);
				FsGuiCommonDrawing::DrawLine(tri[2]+1,tri[3],tri[4]+1,tri[5],annotationCol);
				FsGuiCommonDrawing::DrawLine(tri[0],tri[1]-1,tri[2],tri[3]-1,annotationCol);
				FsGuiCommonDrawing::DrawLine(tri[2],tri[3]-1,tri[4],tri[5]-1,annotationCol);
			}
		}
	}

	if(openSubMenu!=NULL && openSubMenu->GetSubMenu()!=NULL)
	{
		openSubMenu->GetSubMenu()->Show();
	}

	if(NULL!=GetModalDialog())
	{
		GetModalDialog()->Show(excludeFromDrawing);
	}

	needRedraw=YSFALSE;
}

void FsGuiPopUpMenu::ShowItem(const FsGuiPopUpMenuItem *item,int rectX0,int rectY0,int rectX1,int /*rectY1*/) const
{
	FSGUI_DRAWINGTYPE newDrawingType=FSGUI_DRAWINGTYPE_NORMAL;

	if(item==mouseOn || openSubMenu==item)
	{
		newDrawingType=FSGUI_DRAWINGTYPE_ACTIVE;
		if(YSTRUE==pullDown)
		{
			FsGuiCommonDrawing::DrawRect(item->x0,item->y0-1,item->x0+item->wid,item->y0+item->hei,activeBgCol,YSTRUE);
		}
		else
		{
			FsGuiCommonDrawing::DrawRect(x0,item->y0-1,x0+wid,item->y0+item->hei,activeBgCol,YSTRUE);
		}
	}

	if(newDrawingType!=item->drawingType && YSTRUE==item->isText)
	{
		const YsColor col=(FSGUI_DRAWINGTYPE_NORMAL==newDrawingType ? fgCol : activeFgCol);
		FsGuiChangeBmpNonTransparentColor(item->bmp,col);
		item->drawingType=newDrawingType;
	}

	if(item->bmp.GetTotalWidth()>0 && item->bmp.GetTotalHeight()>0)
	{
		FsGuiCommonDrawing::DrawBmpArray(item->bmp,rectX0,rectY0);
	}

	int rightX=rectX1-item->rightMsgWid;
	if(item->rightAlignedBmp.GetTotalWidth()>0 && item->rightAlignedBmp.GetTotalHeight()>0)
	{
		FsGuiCommonDrawing::DrawBmpArray(item->rightAlignedBmp,rightX,rectY0);
	}
}



////////////////////////////////////////////////////////////


void FsGuiDialogItem::DrawVerticalScrollBar(int x1,int y1,int x2,int y2,const YsColor &bgCol,const YsColor &fgCol) const
{
	double tri[6];

	switch(scheme)
	{
	default:
	case PLAIN:
		FsGuiCommonDrawing::DrawRect((double)x1,(double)y1,(double)x2,(double)y2,bgCol,YSTRUE);
		FsGuiCommonDrawing::DrawRect((double)x1,(double)y1,(double)x2,(double)y2,fgCol,YSFALSE);
		break;
	case MODERN:
		{
			const double rad=fabs((double)(x2-x1))/2.0;
			FsGuiDrawRoundRect(x1,y1,x2,y2,bgCol,0.0,rad);
			FsGuiDrawEmptyRoundRect(x1,y1,x2,y2,fgCol,0.0,rad);
		}
		break;
	}

	tri[0]=(double)x1+2.0;
	tri[1]=(double)y1+(double)FsGuiObject::defHScrollBar-2.0;
	tri[2]=(double)x2-3.0;
	tri[3]=(double)y1+(double)FsGuiObject::defHScrollBar-2.0;
	tri[4]=(double)(x1+x2)/2.0;
	tri[5]=(double)y1+2.0;
	FsGuiCommonDrawing::DrawPolygon(3,tri,fgCol);
	if(scheme!=MODERN)
	{
		FsGuiCommonDrawing::DrawRect(x1,y1,x2,y1+FsGuiObject::defHScrollBar,fgCol,YSFALSE);
	}

	tri[0]=(double)x1+2.0;
	tri[1]=(double)y2-(double)FsGuiObject::defHScrollBar+2.0;
	tri[2]=(double)x2-3.0;
	tri[3]=(double)y2-(double)FsGuiObject::defHScrollBar+2.0;
	tri[4]=(double)(x1+x2)/2.0;
	tri[5]=(double)y2-2.0;
	FsGuiCommonDrawing::DrawPolygon(3,tri,defFgCol);
	if(scheme!=MODERN)
	{
		FsGuiCommonDrawing::DrawRect((double)x1,(double)y2,(double)x2,(double)(y2-FsGuiObject::defHScrollBar),fgCol,YSFALSE);
	}
}

void FsGuiDialogItem::DrawVerticalScrollBarSlider(int x1,int y1,int x2,int y2,int totalHeight,int showTop,int showHeight,const YsColor &/*bgCol*/,const YsColor &fgCol) const
{
//	int sliderHei,sliderBugHei,sliderBugPos;
//	sliderHei=y2-y1+1;
//
//	sliderBugHei=sliderHei*showHeight/totalHeight;
//
//	sliderBugPos=sliderHei*showTop/totalHeight;
//
//	int sliderY2=YsSmaller(y1+sliderBugPos+sliderBugHei,y2);

	int bugY1,bugY2;
	GetScrollBarBugPosition(bugY1,bugY2,y1,y2,totalHeight,showTop,showHeight);
	if(MODERN!=scheme)
	{
		FsGuiCommonDrawing::DrawRect(x1,bugY1,x2,bugY2,fgCol,YSTRUE);
	}
	else
	{
		FsGuiCommonDrawing::DrawRect(x1+2,bugY1,x2-2,bugY2,fgCol,YSTRUE);
	}
}


////////////////////////////////////////////////////////////


void FsGuiDialogItem::DrawPopUp(const YsColor & /*defBgCol*/,const YsColor & /*defFgCol*/,const YsColor & /*activeBgCol*/,const YsColor & /*activeFgCol*/,const YsColor & /*frameCol*/) const
{
}

void FsGuiDialogItem::Draw(YSBOOL  /*focus*/,YSBOOL  /*mouseOver*/,const YsColor & /*defBgCol*/,const YsColor & /*defFgCol*/,const YsColor & /*activeBgCol*/,const YsColor & /*activeFgCol*/,const YsColor & /*frameCol*/) const
{
}

void FsGuiDialogItem::DrawRect(YSBOOL focus,const YsColor &col) const
{
	FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,col,YSFALSE);
	if(focus==YSTRUE)
	{
		FsGuiCommonDrawing::DrawRect(x0+2,y0+2,x0+wid-3,y0+hei-3,col,YSFALSE);
		FsGuiCommonDrawing::DrawRect(x0-2,y0-2,x0+wid+1,y0+hei+1,col,YSFALSE);
	}
}

void FsGuiDialogItem::DrawRightSideArrow(int  /*x0*/,int y0,int x1,int y1,YSBOOL fill1,YSBOOL fill2,const YsColor &fillCol,const YsColor & /*bgCol*/,const YsColor &fgCol) const
{
	const double w=(double)(y1-y0+1)/2.0;
	double tri[6];

	if(fill1==YSTRUE)
	{
		FsGuiCommonDrawing::DrawRect((double)(x1-w),(double)y0  ,(double)x1  ,(double)(y0+w),fillCol,YSTRUE);
	}
	FsGuiCommonDrawing::DrawRect((double)(x1-w),(double)y0  ,(double)x1  ,(double)(y0+w),fgCol,YSFALSE);
	tri[0]=(double)x1-(double)w/2.0;
	tri[1]=(double)y0+2.0;
	tri[2]=(double)x1-(double)w+2.0;
	tri[3]=(double)y0+(double)w/2.0-2.0;
	tri[4]=(double)x1-2.0;
	tri[5]=(double)y0+(double)w/2.0-2.0;
	FsGuiCommonDrawing::DrawPolygon(3,tri,fgCol);

	if(fill2==YSTRUE)
	{
		FsGuiCommonDrawing::DrawRect((double)(x1-w),(double)(y0+w),(double)x1,(double)y1,fillCol,YSTRUE);
	}
	FsGuiCommonDrawing::DrawRect((double)(x1-w),(double)(y0+w),(double)x1,(double)y1,fgCol,YSFALSE);
	tri[0]=(double)x1-(double)w/2.0;
	tri[1]=(double)y1-2.0;
	tri[2]=(double)x1-(double)w+2.0;
	tri[3]=(double)y1-(double)w/2.0+2.0;
	tri[4]=(double)x1-2.0;
	tri[5]=(double)y1-(double)w/2.0+2.0;
	FsGuiCommonDrawing::DrawPolygon(3,tri,fgCol);
}

void FsGuiStatic::Draw(YSBOOL focus,YSBOOL  /*mouseOver*/,const YsColor & /*defBgCol*/,const YsColor & /*defFgCol*/,const YsColor & /*activeBgCol*/,const YsColor & /*activeFgCol*/,const YsColor &frameCol) const
{
	if(fill==YSTRUE)
	{
		FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,YsGrayScale(0.5),YSTRUE);
	}
	if(frame==YSTRUE)
	{
		DrawRect(focus,frameCol);
	}

	const int msgX0=x0+FsGuiObject::defHSpaceUnit/2;
	if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
	{
		const int vSpace=hei-bmp.GetHeight();
		FsGuiCommonDrawing::DrawBmpArray(bmp,msgX0,y0+vSpace/2);
	}
}

void FsGuiButton::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const
{
	YSBOOL iOSishButtonOrientation=YSTRUE;
	FSGUI_DRAWINGTYPE newDrawingType=FSGUI_DRAWINGTYPE_NORMAL;

	switch(scheme)
	{
	default:
	case PLAIN:
		newDrawingType=(YSTRUE!=mouseOver ? FSGUI_DRAWINGTYPE_NORMAL : FSGUI_DRAWINGTYPE_ACTIVE);
		break;
	case MODERN:
		iOSishButtonOrientation=YsReverseBool(mouseOver);
		newDrawingType=(FSGUI_RADIOBUTTON==btnType && YSTRUE==checked ? FSGUI_DRAWINGTYPE_ACTIVE : FSGUI_DRAWINGTYPE_NORMAL);
		break;
	}

	const YsColor &fgCol=(newDrawingType==FSGUI_DRAWINGTYPE_ACTIVE ? activeFgCol : defFgCol);
	const YsColor &bgCol=(newDrawingType==FSGUI_DRAWINGTYPE_ACTIVE ? activeBgCol : defBgCol);

	if(YSTRUE==isText && prevDrawingType!=newDrawingType)
	{
		FsGuiChangeBmpNonTransparentColor(bmp,fgCol);
		prevDrawingType=newDrawingType;
	}

	if(YSTRUE==useDefaultFrame)
	{
		switch(btnType)
		{
		case FSGUI_PUSHBUTTON:
			{
				const int msgX=x0+FsGuiObject::defHSpaceUnit/2;

				switch(scheme)
				{
				default:
				case PLAIN:
					FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,bgCol,YSTRUE);
					DrawRect(focus,frameCol);
					break;
				case MODERN:
					if(YSTRUE==focus)
					{
						FsGuiDrawRoundRect(x0-3,y0-3,x0+wid+2,y0+hei+2,frameCol,0.0,defRoundRadius);
					}
					FsGuiDrawFlashyRect(x0,y0,x0+wid-1,y0+hei-1,bgCol,frameCol,0.0,defRoundRadius,YSTRUE,iOSishButtonOrientation);
					break;
				}

				if(YSTRUE==negativeBmp.IsLoaded() && FSGUI_DRAWINGTYPE_ACTIVE==newDrawingType)
				{
					FsGuiCommonDrawing::DrawBmpArray(negativeBmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
				else if(YSTRUE==bmp.IsLoaded())
				{
					FsGuiCommonDrawing::DrawBmpArray(bmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
			}
			break;
		case FSGUI_CHECKBOX:
			{
				const double cbX0=(double)x0+(double)FsGuiObject::defHSpaceUnit/2.0;
				const double cbY0=(double)y0+(double)hei/2.0-(double)FsGuiObject::defHAnnotation/2.0;
				const double cbX1=cbX0+FsGuiObject::defHAnnotation;
				const double cbY1=cbY0+FsGuiObject::defHAnnotation;

				switch(scheme)
				{
				default:
				case PLAIN:
					{
						FsGuiCommonDrawing::DrawRect(cbX0,cbY0,cbX1,cbY1,bgCol,YSTRUE);
						FsGuiCommonDrawing::DrawRect(cbX0,cbY0,cbX1,cbY1,defFgCol,YSFALSE);

						if(focus==YSTRUE)
						{
							DrawRect(focus,frameCol);
						}
						if(checked==YSTRUE)
						{
							FsGuiCommonDrawing::DrawLine(cbX0,cbY0,cbX1,cbY1,defFgCol);
							FsGuiCommonDrawing::DrawLine(cbX1,cbY0,cbX0,cbY1,defFgCol);
						}
						else if(checked==YSTFUNKNOWN)
						{
							FsGuiCommonDrawing::DrawRect(cbX0+2,cbY0+2,cbX1-2,cbY1-2,defFgCol,YSFALSE);
						}
					}
					break;
				case MODERN:
					if(YSTRUE==focus)
					{
						FsGuiDrawRoundRect((int)cbX0-4,(int)cbY0-4,(int)cbX1+4,(int)cbY1+4,frameCol,0.5,0.0);
					}
					else
					{
						FsGuiDrawRoundRect((int)cbX0-2,(int)cbY0-2,(int)cbX1+2,(int)cbY1+2,frameCol,0.5,0.0);
					}
					FsGuiDrawFlashyRect((int)cbX0,(int)cbY0,(int)cbX1,(int)cbY1,bgCol,frameCol,0.5,0.0,YSTRUE,iOSishButtonOrientation);

					const double dx=cbX1-cbX0;
					const double dy=cbY1-cbY0;

					if(YSTRUE==checked)
					{
						const double checkMark[6*2]=
						{
							cbX0+0.5*dx,    cbY0+0.7*dy,
							cbX0+0.7*dx,    cbY0+0.2*dy,
							cbX0+0.9*dx,    cbY0+0.2*dy,
							cbX0+0.5*dx,    cbY0+0.9*dy,
							cbX0+0.1*dx,    cbY0+0.5*dy,
							cbX0+0.3*dx,    cbY0+0.5*dy
						};
						FsGuiCommonDrawing::DrawPolygon(6,checkMark,fgCol);
						FsGuiCommonDrawing::DrawLineStrip(6,checkMark,fgCol);
					}
					else if(YSTFUNKNOWN==checked)
					{
						const double cx=(cbX0+cbX1)/2.0;
						const double cy=(cbY0+cbY1)/2.0;
						const double r=dx/4.0;
						FsGuiCommonDrawing::DrawCircle(cx,cy,r,YsBlack(),YSFALSE);
					}
					break;
				}

				const int msgX=x0+FsGuiObject::defHSpaceUnit+FsGuiObject::defHAnnotation;;
				if(YSTRUE==negativeBmp.IsLoaded() && YSTRUE==checked)
				{
					FsGuiCommonDrawing::DrawBmpArray(negativeBmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
				else if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
				{
					FsGuiCommonDrawing::DrawBmpArray(bmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
			}
			break;
		case FSGUI_RADIOBUTTON:
			{
				switch(scheme)
				{
				default:
				case PLAIN:
					{
						const int circleCx=x0+FsGuiObject::defHSpaceUnit/2+FsGuiObject::defHAnnotation/2;
						const int circleCy=y0+hei/2;
						const int circleR=FsGuiObject::defHAnnotation/2;

						if(focus==YSTRUE)
						{
							DrawRect(focus,frameCol);
						}
						if(checked==YSTRUE)
						{
							FsGuiCommonDrawing::DrawCircle(circleCx,circleCy,circleR,defFgCol,YSTRUE);
						}
						else
						{
							FsGuiCommonDrawing::DrawCircle(circleCx,circleCy,circleR,bgCol,YSTRUE);
							FsGuiCommonDrawing::DrawCircle(circleCx,circleCy,circleR,defFgCol,YSFALSE);
						}
					}
					break;
				case MODERN:
					{
						YSBOOL round[4]=
						{
							YSFALSE,
							YSFALSE,
							YSFALSE,
							YSFALSE
						};
						if(0<btnGroup.GetN() && btnGroup.GetTop()==this)
						{
							round[0]=YSTRUE;
							round[2]=YSTRUE;
						}
						if(0<btnGroup.GetN() && btnGroup.GetEnd()==this)
						{
							round[1]=YSTRUE;
							round[3]=YSTRUE;
						}

						int bx0=x0;
						int by0=y0;
						int bx1=x0+wid-1;
						int by1=y0+hei-1;
						for(int i=0; i<btnGroup.GetN(); ++i)
						{
							if(btnGroup[i]==this)
							{
								if(0<i)
								{
									const int leftBx1=btnGroup[i-1]->x0+btnGroup[i-1]->wid-1;
									if(leftBx1<bx0)
									{
										bx0=(leftBx1+bx0)/2;
									}
								}
								if(i+1<btnGroup.GetN())
								{
									const int rightBx0=btnGroup[i+1]->x0;
									if(bx1<rightBx0)
									{
										bx1=(rightBx0+bx1)/2;
									}
								}
							}
						}

						if(YSTRUE==focus)
						{
							FsGuiDrawRoundRect(bx0-2,by0-2,bx1+2,by1+2,frameCol,0.0,defRoundRadius,round);
						}

						FsGuiDrawFlashyRect(bx0,by0,bx1,by1,bgCol,frameCol,0.0,defRoundRadius,YSTRUE,iOSishButtonOrientation,round);
					}
					break;
				}

				const int checkBoxWid=(scheme==MODERN ? 0 : defHAnnotation+defHSpaceUnit/3);

				const int msgX=x0+defHSpaceUnit*2/3+checkBoxWid;
				if(YSTRUE==negativeBmp.IsLoaded() && YSTRUE==checked)
				{
					FsGuiCommonDrawing::DrawBmpArray(negativeBmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
				else if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
				{
					FsGuiCommonDrawing::DrawBmpArray(bmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
			}
			break;
		}
	}
	else
	{
		switch(btnType)
		{
		case FSGUI_PUSHBUTTON:
			{
				const int msgX=x0+FsGuiObject::defHSpaceUnit/2;
				if(YSTRUE==negativeBmp.IsLoaded() && YSTRUE==mouseOver)
				{
					FsGuiCommonDrawing::DrawBmpArray(negativeBmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
				else if(YSTRUE==bmp.IsLoaded())
				{
					FsGuiCommonDrawing::DrawBmpArray(bmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
			}
			break;
		case FSGUI_CHECKBOX:
			{
				const int msgX=x0+FsGuiObject::defHSpaceUnit+FsGuiObject::defHAnnotation;;
				if(YSTRUE==negativeBmp.IsLoaded() && YSTRUE==checked)
				{
					FsGuiCommonDrawing::DrawBmpArray(negativeBmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
				else if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
				{
					FsGuiCommonDrawing::DrawBmpArray(bmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
			}
			break;
		case FSGUI_RADIOBUTTON:
			{
				const int checkBoxWid=(scheme==MODERN ? 0 : defHAnnotation+defHSpaceUnit/3);

				const int msgX=x0+defHSpaceUnit*2/3+checkBoxWid;
				if(YSTRUE==negativeBmp.IsLoaded() && YSTRUE==checked)
				{
					FsGuiCommonDrawing::DrawBmpArray(negativeBmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
				else if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
				{
					FsGuiCommonDrawing::DrawBmpArray(bmp,msgX,y0+FsGuiObject::defVSpaceUnit/2);
				}
			}
			break;
		}
	}

	if(YSTRUE!=enabled)
	{
		if(YSTRUE==useDefaultFrame)
		{
			FsGuiCommonDrawing::DrawMask(fgCol,bgCol,x0,y0,wid,hei);
		}
		else
		{
			const int bmpX=x0+FsGuiObject::defHSpaceUnit/2;
			const int bmpY=y0+FsGuiObject::defVSpaceUnit/2;
			FsGuiCommonDrawing::DrawMask(fgCol,bgCol,bmpX,bmpY,bmp.GetWidth(),bmp.GetHeight());
		}
	}
}

void FsGuiTextBox::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor & frameCol) const
{
	// sx+fontWid,sy+fontHei*3/2  Label

	// sx+fontWid,sy-fontHei*3/2
	//                            Input area
	// sx+fontWid,sy-fontHei/2

	const YsColor &fgCol=((scheme!=MODERN && YSTRUE==mouseOver) ? activeFgCol : defFgCol);
	const YsColor &bgCol=((scheme!=MODERN && YSTRUE==mouseOver) ? activeBgCol : defBgCol);

	switch(scheme)
	{
	default:
	case PLAIN:
		FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,bgCol,YSTRUE);
		DrawRect(focus,defFgCol);
		break;
	case MODERN:
		FsGuiDrawRoundRect(x0,y0,x0+wid-1,y0+hei-1,bgCol,0.0,defRoundRadius);
		if(YSTRUE==focus)
		{
			FsGuiDrawEmptyRoundRect(x0,y0,x0+wid-1,y0+hei-1,frameCol,0.0,defRoundRadius);
		}
		break;
	}

	int x1,y1,x2,y2;
	GetInputRect(x1,y1,x2,y2);
	const int inputRectCY=(y1+y2)/2;

	FsGuiCommonDrawing::DrawRect(x1,y1,x2,y2,YsBlack(),YSTRUE);
	if(YSTRUE==mouseOver)
	{
		FsGuiCommonDrawing::DrawRect(x1-1,y1-1,x2+1,y2+1,YsWhite(),YSFALSE);
	}

	if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
	{
		FsGuiCommonDrawing::DrawBmpArray(bmp,x0+msgX0,y0+msgY0);
	}



	FsGuiCommonDrawing::SetClipRect(x1,y1,x2,y2);

	int fontWid,fontHei;
	FsGuiObject::GetAsciiCharacterBitmapSize(fontWid,fontHei);

	x1=x0+typeX0;
	y1=y0+typeY0+typeHei;
	// Highlight Selection >>
	if(YSTRUE==focus && 0<=selBegin)
	{
		FsGuiCommonDrawing::DrawRect(x1+selTopPixOffset,y0+typeY0,x1+selEndPixOffset,y0+typeY0+typeHei,YsBlue(),YSTRUE);
	}
	// Highlight Selection <<

	FsGuiCommonDrawing::DrawBmpArray(txtBmp,x1,inputRectCY-txtBmp.GetHeight()/2);

	int t;
	t=FsGuiClock()/500;
	if(focus==YSTRUE)
	{
		if((t&1)!=0)
		{
			FsGuiCommonDrawing::DrawLine(x1+cursorPixOffset,y1  ,x1+nextCursorPixOffset,y1  ,YsWhite());
			FsGuiCommonDrawing::DrawLine(x1+cursorPixOffset,y1-1,x1+nextCursorPixOffset,y1-1,YsWhite());
		}
		else
		{
			FsGuiCommonDrawing::DrawLine(x1+cursorPixOffset  ,y1,x1+cursorPixOffset  ,y1-fontHei+1,YsWhite());
			FsGuiCommonDrawing::DrawLine(x1+cursorPixOffset+1,y1,x1+cursorPixOffset+1,y1-fontHei+1,YsWhite());
		}
	}

	FsGuiCommonDrawing::ClearClipRect();

	if(YSTRUE!=enabled)
	{
		FsGuiCommonDrawing::DrawMask(fgCol,bgCol,x0,y0,wid,hei);
	}
}

void FsGuiListBox::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const
{
	int x1,y1,x2,y2;
	YsString str;
	FSGUI_DRAWINGTYPE newDrawingType=FSGUI_DRAWINGTYPE_NORMAL;
	YSBOOL iOSishButtonOrientation=YSTRUE;

	switch(scheme)
	{
	default:
	case PLAIN:
		newDrawingType=(YSTRUE==mouseOver ? FSGUI_DRAWINGTYPE_ACTIVE : FSGUI_DRAWINGTYPE_NORMAL);
		break;
	case MODERN:
		iOSishButtonOrientation=YsReverseBool(mouseOver);
		break;
	};


	const YsColor &fgCol=(FSGUI_DRAWINGTYPE_ACTIVE==newDrawingType ? activeFgCol : defFgCol);
	const YsColor &bgCol=(FSGUI_DRAWINGTYPE_ACTIVE==newDrawingType ? activeBgCol : defBgCol);


	switch(scheme)
	{
	default:
	case PLAIN:
		FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,bgCol,YSTRUE);
		DrawRect(focus,defFgCol);
		break;
	case MODERN:
		if(YSTRUE!=tight)
		{
			if(YSTRUE==focus)
			{
				FsGuiDrawRoundRect(x0-2,y0-2,x0+wid+1,y0+hei+1,frameCol,0.0,defRoundRadius);
			}
			FsGuiDrawFlashyRect(x0,y0,x0+wid-1,y0+hei-1,bgCol,frameCol,0.0,defRoundRadius,YSTRUE,iOSishButtonOrientation);
		}
		break;
	}


	if(YSTRUE!=tight)
	{
		if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
		{
			x1=x0+FsGuiObject::defHSpaceUnit;
			y1=y0+FsGuiObject::defVSpaceUnit/2;
			FsGuiCommonDrawing::DrawBmpArray(bmp,x1,y1);
		}
	}



	GetChoiceRect(x1,y1,x2,y2);

	FsGuiCommonDrawing::SetClipRect(x1,y1,x2-x1+1,y2-y1+1);

	FsGuiCommonDrawing::DrawRect(x1-2,y1-2,x2+2,y2+2,defListBgCol,YSTRUE);
	if(YSTRUE==mouseOver)
	{
		FsGuiCommonDrawing::DrawRect(x1-3,y1-3,x2+3,y2+3,defListFgCol,YSFALSE);
	}

	int fontWid,fontHei;
	FsGuiObject::GetAsciiCharacterBitmapSize(fontWid,fontHei);

	int y=0;
	switch(scheme)
	{
	default:
	case PLAIN:
		for(int i=0; i<item.GetN(); i++)
		{
			if(showTopPix-item[i].GetHeight()<=y && y<showTopPix+showHeightPix &&
			   item[i].IsSelected()==YSTRUE)
			{
				const int rx1=x1-2;
				const int ry1=YsBound(y1+y-showTopPix-1,y1,y2);
				const int rx2=x2+2;
				const int ry2=YsBound(y1+y-showTopPix+item[i].GetHeight()+1,y1,y2);

				FsGuiCommonDrawing::DrawRect(rx1,ry1,rx2,ry2,defListActiveBgCol,YSTRUE);
			}
			y+=item[i].GetHeight();
		}
		break;
	case MODERN:
		for(int i=0; i<item.GetN(); i++)
		{
			if(showTopPix-item[i].GetHeight()<=y && y<showTopPix+showHeightPix)
			{
				const int rx1=x1-2;
				const int ry1=YsBound(y1+y-showTopPix-1,y1,y2);
				const int rx2=x2+2;
				const int ry2=YsBound(y1+y-showTopPix+item[i].GetHeight()+1,y1,y2);
				if(item[i].IsSelected()==YSTRUE)
				{
					FsGuiCommonDrawing::DrawRect(rx1,ry1,rx2,ry2,defListActiveBgCol,YSTRUE);
				}
				else
				{
					const double rect[8]=
					{
						(double)rx1,(double)ry1,
						(double)rx2,(double)ry1,
						(double)rx2,(double)ry2,
						(double)rx1,(double)ry2
					};
					const YsColor brighter(
					   YsSmaller(defListBgCol.Rd()+0.2,1.0),
					   YsSmaller(defListBgCol.Gd()+0.2,1.0),
					   YsSmaller(defListBgCol.Bd()+0.2,1.0));
					const YsColor col[4]=
					{
						brighter,
						brighter,
						defListBgCol,
						defListBgCol
					};
					FsGuiCommonDrawing::DrawGradationPolygon(4,rect,col);
				}
			}
			y+=item[i].GetHeight();
		}
		break;
	}

	y=0;
	YsString itemStr;
	for(int i=0; i<item.GetN(); i++)
	{
		if(showTopPix-item[i].GetHeight()<=y && y<showTopPix+showHeightPix)
		{
			FsGuiCommonDrawing::DrawBmpArray(item[i].GetBitmap(),x1,y1+y-showTopPix);
		}
		y+=item[i].GetHeight();
	}



	if(showHeightPix<totalHeightPix)
	{
		GetScrollBarRect(x1,y1,x2,y2);

		DrawVerticalScrollBar(x1,y1,x2,y2,defBgCol,defFgCol);

		GetScrollBarSliderRect(x1,y1,x2,y2);

		DrawVerticalScrollBarSlider(x1,y1,x2,y2,totalHeightPix,showTopPix,showHeightPix,defBgCol,defFgCol);
	}

	FsGuiCommonDrawing::ClearClipRect();

	if(YSTRUE!=enabled)
	{
		FsGuiCommonDrawing::DrawMask(fgCol,bgCol,x0,y0,wid,hei);
	}
}

void FsGuiDropList::DrawPopUp(const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const
{
	lbx.Draw(YSFALSE,YSFALSE,defBgCol,defFgCol,activeBgCol,activeFgCol,frameCol);
}

void FsGuiDropList::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor & /*activeBgCol*/,const YsColor & /*activeFgCol*/,const YsColor &frameCol) const
{
	switch(scheme)
	{
	default:
	case PLAIN:
		{
			FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,defBgCol,YSTRUE);
			DrawRect(focus,defFgCol);

			if(lbx.item.IsInRange(sel)==YSTRUE)
			{
				const int x=x0+FsGuiFont::GetWidth();
				const int y=y0+(hei-bmp.GetHeight())/2;

				FsGuiCommonDrawing::DrawBmpArray(bmp,x,y);
			}

			FsGuiCommonDrawing::DrawRect(x0+wid-hei,y0,x0+wid-1,y0+hei-1,defFgCol,YSFALSE);

			const double tri[6]=
			{
				(double)x0+(double)wid-(double)hei+3.0,
				(double)y0+3.0,
				(double)x0+(double)wid-4.0,
				(double)y0+3.0,
				(double)x0+(double)wid-(double)hei/2.0,
				(double)y0+(double)hei-4.0
			};
			FsGuiCommonDrawing::DrawPolygon(3,tri,defFgCol);
		}
		break;
	case MODERN:
		{
			const double roundRadius=(double)hei/2.0;

			YSBOOL iOSishButtonOrientation=mouseOver;
			YsFlip(iOSishButtonOrientation);
			if(YSTRUE==focus)
			{
				FsGuiDrawRoundRect(x0-2,y0-2,x0+wid+1,y0+hei+1,frameCol,0.0,roundRadius);
			}
			FsGuiDrawFlashyRect(x0,y0,x0+wid-1,y0+hei-1,defBgCol,frameCol,0.0,roundRadius,YSTRUE,iOSishButtonOrientation);
			FsGuiCommonDrawing::DrawLine(x0+wid-hei,y0,x0+wid-hei,y0+hei-1,frameCol);

			if(lbx.item.IsInRange(sel)==YSTRUE)
			{
				const int x=x0+FsGuiFont::GetWidth();
				const int y=y0+(hei-bmp.GetHeight())/2;

				FsGuiCommonDrawing::DrawBmpArray(bmp,x,y);
			}

			const double tri[6]=
			{
				(double)x0+(double)wid-(double)hei+3.0,
				(double)y0+3.0,
				(double)x0+(double)wid-4.0,
				(double)y0+(double)hei/2.0,
				(double)x0+(double)wid-(double)hei+3.0,
				(double)y0+(double)hei-3.0
			};
			FsGuiCommonDrawing::DrawPolygon(3,tri,defFgCol);
		}
		break;
	}

	if(YSTRUE!=enabled)
	{
		FsGuiCommonDrawing::DrawMask(defFgCol,defBgCol,x0,y0,wid,hei);
	}
}

void FsGuiNumberBox::Draw(YSBOOL focus,YSBOOL /*mouseOver*/,const YsColor &defBgCol,const YsColor & /*defFgCol*/,const YsColor &activeBgCol,const YsColor & /*activeFgCol*/,const YsColor & frameCol) const
{
	switch(scheme)
	{
	default:
	case PLAIN:
		{
			FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,defBgCol,YSTRUE);
			DrawRect(focus,defFgCol);
		}
		break;
	case MODERN:
		{
			if(YSTRUE==focus)
			{
				FsGuiDrawRoundRect(x0-2,y0-2,x0+wid+1,y0+hei+1,frameCol,0.0,defRoundRadius);
			}
			FsGuiDrawEmptyRoundRect(x0-2,y0-2,x0+wid+1,y0+hei+1,frameCol,0.0,defRoundRadius);
			FsGuiDrawRoundRect(x0,y0,x0+wid-1,y0+hei-1,defBgCol,0.0,defRoundRadius);
		}
	}


	int midY=y0+hei/2;

	if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
	{
		const int x=x0+FsGuiObject::defHSpaceUnit;
		FsGuiCommonDrawing::DrawBmpArray(bmp,x,midY-msgHei/2);
	}

	YsString str;
	str.Printf("%d",num);

	const int digitX0=x0+FsGuiObject::defHSpaceUnit+(0<msgWid ? msgWid+FsGuiObject::defHSpaceUnit/2 : 0);
	const int digitY0=midY-digitHei/2;

	if(enabled==YSTRUE && min<max)
	{
		YsBitmap numberBmp;
		FsGuiObject::RenderAsciiString(numberBmp,str,defFgCol,defBgCol);
		FsGuiCommonDrawing::DrawBmp(numberBmp,digitX0,digitY0);

		int ix0,iy0,ix1,iy1;
		double bx0,by0,bx1,by1;
		double tri[6];

		GetIncreaseButtonArea(ix0,iy0,ix1,iy1);
		bx0=(double)ix0;
		by0=(double)iy0;
		bx1=(double)ix1;
		by1=(double)iy1;
		switch(scheme)
		{
		default:
		case PLAIN:
			if(mouseOnIncreaseButton==YSTRUE)
			{
				FsGuiCommonDrawing::DrawRect(bx0,by0,bx1,by1,activeBgCol,YSTRUE);
			}
			FsGuiCommonDrawing::DrawRect(bx0,by0,bx1,by1,defFgCol,YSFALSE);
			break;
		case MODERN:
			{
				YSBOOL iOSishButtonOrientation=mouseOnIncreaseButton;
				YsFlip(iOSishButtonOrientation);
				FsGuiDrawFlashyRect((int)bx0,(int)by0,(int)bx1,(int)by1,defBgCol,frameCol,0.0,defRoundRadius,YSTRUE,iOSishButtonOrientation);
			}
			break;
		}

		tri[0]=(bx0+bx1)/2.0;
		tri[1]=by0+3.0;
		tri[2]=bx0+3.0;
		tri[3]=by1-3.0;
		tri[4]=bx1-3.0;
		tri[5]=by1-3.0;
		FsGuiCommonDrawing::DrawPolygon(3,tri,defFgCol);

		GetDecreaseButtonArea(ix0,iy0,ix1,iy1);
		bx0=(double)ix0;
		by0=(double)iy0;
		bx1=(double)ix1;
		by1=(double)iy1;
		switch(scheme)
		{
		default:
		case PLAIN:
			if(mouseOnDecreaseButton==YSTRUE)
			{
				FsGuiCommonDrawing::DrawRect(bx0,by0,bx1,by1,activeBgCol,YSTRUE);
			}
			FsGuiCommonDrawing::DrawRect(bx0,by0,bx1,by1,defFgCol,YSFALSE);
			break;
		case MODERN:
			{
				YSBOOL iOSishButtonOrientation=mouseOnDecreaseButton;
				YsFlip(iOSishButtonOrientation);
				FsGuiDrawFlashyRect((int)bx0,(int)by0,(int)bx1,(int)by1,defBgCol,frameCol,0.0,defRoundRadius,YSTRUE,iOSishButtonOrientation);
			}
			break;
		}

		tri[0]=(bx0+bx1)/2.0;
		tri[1]=by1-3.0;
		tri[2]=bx0+3.0;
		tri[3]=by0+3.0;
		tri[4]=bx1-3.0;
		tri[5]=by0+3.0;
		FsGuiCommonDrawing::DrawPolygon(3,tri,defFgCol);
	}
	else
	{
		YsBitmap numberBmp;
		FsGuiObject::RenderAsciiString(numberBmp,str,YsRed(),defBgCol);
		FsGuiCommonDrawing::DrawBmp(numberBmp,digitX0,digitY0);
	}

	if(YSTRUE!=enabled)
	{
		FsGuiCommonDrawing::DrawMask(defFgCol,defBgCol,x0,y0,wid,hei);
	}
}

void FsGuiColorPalette::Draw(YSBOOL focus,YSBOOL  /*mouseOver*/,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor & /*activeBgCol*/,const YsColor &activeFgCol,const YsColor & /*frameCol*/) const
{
	int i,x,y;
	x=this->x0+FsGuiFont::GetWidth();
	y=this->y0+FsGuiFont::GetHeight()*3/2;


	switch(scheme)
	{
	default:
	case PLAIN:
		FsGuiCommonDrawing::DrawRect(this->x0,this->y0,this->x0+wid-1,this->y0+hei-1,defBgCol,YSTRUE);
		DrawRect(focus,defFgCol);
		break;
	case MODERN:
		{
			const double rad=fabs((double)hei)/4.0;
			FsGuiDrawRoundRect(this->x0,this->y0,this->x0+wid-1,this->y0+hei-1,defBgCol,0.0,rad);
			FsGuiDrawEmptyRoundRect(this->x0,this->y0,this->x0+wid-1,this->y0+hei-1,defFgCol,0.0,rad);
		}
		break;
	}

	if(bmp.GetWidth()>0 && bmp.GetHeight()>0)
	{
		FsGuiCommonDrawing::DrawBmpArray(bmp,x,y0+FsGuiFont::GetHeight()/2);
	}

	YsColor c;
	int x0,y0,x1,y1;
	GetColorRect(x0,y0,x1,y1);
	c.SetIntRGB(col[0],col[1],col[2]);
	switch(scheme)
	{
	default:
	case PLAIN:
		FsGuiCommonDrawing::DrawRect(x0,y0,x1,y1,c,YSTRUE);
		break;
	case MODERN:
		{
			const int hei=y1-y0+1;
			const double rad=fabs((double)hei)/4.0;
			FsGuiDrawRoundRect(x0,y0,x1,y1,c,0.0,rad);
		}
		break;
	}


	if(YSTRUE==hasComponentButton)
	{
		char cmpLabel[3]={'R','G','B'};
		char str[256];
		for(i=0; i<3; i++)
		{
			int x0,y0,x1,y1;
			GetComponentRect(x0,y0,x1,y1,i);
			sprintf(str,"%c:%03d",cmpLabel[i],col[i]);

			const int cy=(int)((y0+y1)/2.0);

			YsBitmap numberBmp;
			RenderAsciiString(numberBmp,str,defFgCol,defBgCol);
			FsGuiCommonDrawing::DrawBmp(numberBmp,x0,cy-numberBmp.GetHeight()/2);

			DrawRightSideArrow(x0,y0,x1,y1,arrow[i][0],arrow[i][1],activeFgCol,defFgCol,defFgCol);
			if(focus==YSTRUE && cursor==i)
			{
				FsGuiCommonDrawing::DrawRect(x0-2,y0-2,x1+2,y1+2,defFgCol,YSFALSE);
			}
		}
	}

	if(YSTRUE==hasColorButton)
	{
		GetPaletteRect(x0,y0,x1,y1);
		for(i=0; i<16; i++)
		{
			int x,y,px0,py0,px1,py1;
			x=i&7;
			y=i/8;

			px0=x0+x*(x1-x0)/8;
			py0=y0+y*(y1-y0)/2;

			px1=x0+(x+1)*(x1-x0)/8;
			py1=y0+(y+1)*(y1-y0)/2;

			c.SetIntRGB(palette[i][0],palette[i][1],palette[i][2]);
			FsGuiCommonDrawing::DrawRect(px0,py0,px1,py1,c,YSTRUE);
		}
	}

	if(YSTRUE!=enabled)
	{
		FsGuiCommonDrawing::DrawMask(defFgCol,defBgCol,this->x0,this->y0,this->wid,this->hei);
	}
}

void FsGuiSlider::Draw(YSBOOL focus,YSBOOL  /*mouseOver*/,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor & /*activeBgCol*/,const YsColor & /*activeFgCol*/,const YsColor &frameCol) const
{
	int sx0,sy0,sx1,sy1;
	GetSliderRect(sx0,sy0,sx1,sy1);

	if(YSTRUE==horizontal)
	{
		int cy=(sy0+sy1)/2;

		if(YSTRUE==focus)
		{
			FsGuiCommonDrawing::DrawLine(sx0,cy-3,sx1,cy-3,frameCol);
			FsGuiCommonDrawing::DrawLine(sx0,cy+3,sx1,cy+3,frameCol);

			FsGuiCommonDrawing::DrawLine(sx0-2,sy0,sx0-2,sy1,frameCol);
			FsGuiCommonDrawing::DrawLine(sx1+2,sy0,sx1+2,sy1,frameCol);
		}

		FsGuiCommonDrawing::DrawLine(sx0,cy-1,sx1,cy-1,frameCol);
		FsGuiCommonDrawing::DrawLine(sx0,cy  ,sx1,cy  ,defBgCol);
		FsGuiCommonDrawing::DrawLine(sx0,cy+1,sx1,cy+1,frameCol);

		FsGuiCommonDrawing::DrawLine(sx0,sy0,sx0,sy1,frameCol);
		FsGuiCommonDrawing::DrawLine(sx1,sy0,sx1,sy1,frameCol);
	}
	else
	{
		int cx=(sx0+sx1)/2;

		if(YSTRUE==focus)
		{
			FsGuiCommonDrawing::DrawLine(cx-3,sy0,cx-3,sy1,frameCol);
			FsGuiCommonDrawing::DrawLine(cx+3,sy0,cx+3,sy1,frameCol);

			FsGuiCommonDrawing::DrawLine(sx0,sy0-2,sx1,sy0-2,frameCol);
			FsGuiCommonDrawing::DrawLine(sx0,sy1+2,sx1,sy1+2,frameCol);
		}

		FsGuiCommonDrawing::DrawLine(cx-1,sy0,cx-1,sy1,frameCol);
		FsGuiCommonDrawing::DrawLine(cx  ,sy0,cx  ,sy1,defBgCol);
		FsGuiCommonDrawing::DrawLine(cx+1,sy0,cx+1,sy1,frameCol);

		FsGuiCommonDrawing::DrawLine(sx0,sy0,sx1,sy0,frameCol);
		FsGuiCommonDrawing::DrawLine(sx0,sy1,sx1,sy1,frameCol);
	}

	int rx0,ry0,rx1,ry1;
	GetCursorRect(rx0,ry0,rx1,ry1);
	FsGuiCommonDrawing::DrawRect(rx0,ry0,rx1,ry1,defBgCol,YSTRUE);
	FsGuiCommonDrawing::DrawRect(rx0,ry0,rx1,ry1,frameCol,YSFALSE);

	if(YSTRUE!=enabled)
	{
		FsGuiCommonDrawing::DrawMask(defFgCol,defBgCol,x0,y0,wid,hei);
	}
}

void FsGuiTabControl::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor & /*defFgCol*/,const YsColor &activeBgCol,const YsColor & /*activeFgCol*/,const YsColor &frameCol) const
{
	int x,y;
	int minLabelHeight=0;

	for(int i=0; i<tabList.GetN(); ++i)
	{
		minLabelHeight=(0==i ? tabList[i].labelHei : YsSmaller(minLabelHeight,tabList[i].labelHei));
	}

	switch(scheme)
	{
	default:
	case PLAIN:
		if(YSTRUE==focus)
		{
			FsGuiCommonDrawing::DrawRect(x0-4,y0-4,x0+wid+3,y0+fieldHei+3,frameCol,YSFALSE);
		}
		FsGuiCommonDrawing::DrawRect(x0,y0+minLabelHeight-1,x0+wid,y0+fieldHei-1,defTabBgCol,YSTRUE);
		FsGuiCommonDrawing::DrawLine(x0,y0+fieldHei-1,x0+wid,y0+fieldHei-1,frameCol);
		break;
	case MODERN:
		if(YSTRUE==focus)
		{
			FsGuiDrawRoundRect(x0-2,y0-2,x0+wid+1,y0+fieldHei+1,frameCol,0.0,defRoundRadius);
		}
		FsGuiDrawRoundRect(x0,y0,x0+wid-1,y0+fieldHei-1,defTabBgCol,0.0,defRoundRadius);
		break;
	}

	x=x0;
	y=y0;
	for(int i=0; i<tabList.GetN(); ++i)
	{
		const int labelWid=tabList[i].labelWid;
		const int labelHei=tabList[i].labelHei;

		switch(scheme)
		{
		default:
		case PLAIN:
			FsGuiCommonDrawing::DrawRect(x,y,x+labelWid-1,y+labelHei-1,((YSTRUE==mouseOver && i==mouseOnTab) ? activeBgCol : defTabBgCol),YSTRUE);

			FsGuiCommonDrawing::DrawLine(x           ,y+labelHei-1,x           ,y+3         ,frameCol);
			FsGuiCommonDrawing::DrawLine(x           ,y+3         ,x+3         ,y           ,frameCol);
			FsGuiCommonDrawing::DrawLine(x+3         ,y           ,x+labelWid-4,y           ,frameCol);
			FsGuiCommonDrawing::DrawLine(x+labelWid-4,y           ,x+labelWid-1,y+3         ,frameCol);
			FsGuiCommonDrawing::DrawLine(x+labelWid-1,y+3         ,x+labelWid-1,y+labelHei-1,frameCol);

			if(0==i)
			{
				FsGuiCommonDrawing::DrawLine(x,y+labelHei-1,x,y0+fieldHei,frameCol);
			}
			if(tabList.GetN()-1==i)
			{
				FsGuiCommonDrawing::DrawLine(x+labelWid-1,y+labelHei-1,x0+wid-1,y+labelHei-1,frameCol);
				FsGuiCommonDrawing::DrawLine(x0+wid,y+labelHei-1,x0+wid,y0+fieldHei-1,frameCol);
			}
			if(currentTab!=i)
			{
				FsGuiCommonDrawing::DrawLine(x,y+labelHei-1,x+labelWid-1,y+labelHei-1,frameCol);
			}
			break;
		case MODERN:
			{
				const YSBOOL round[4]=
				{
					(0==i ? YSTRUE : YSFALSE),
					(tabList.GetN()-1==i ? YSTRUE : YSFALSE),
					YSFALSE,
					YSFALSE
				};
				YSBOOL brightUp=((YSTRUE==mouseOver && i==mouseOnTab) ? YSFALSE : YSTRUE);
				const YsColor &bgCol=(currentTab==i ? defBgCol : defTabClosedBgCol);
				FsGuiDrawFlashyRect(x,y,x+labelWid-1,y+labelHei-1,bgCol,frameCol,0.0,defRoundRadius,YSFALSE,brightUp,round);
				if(0<i)
				{
					FsGuiCommonDrawing::DrawLine(x,y,x,y+labelHei-1,frameCol);
				}
				if(tabList.GetN()-1>i)
				{
					FsGuiCommonDrawing::DrawLine(x+labelWid-1,y,x+labelWid-1,y+labelHei-1,frameCol);
				}
			}
			break;
		}


		const int yOffset=((scheme!=MODERN && currentTab==i) ? 3 : -1);
		if(0<tabList[i].bmp.GetWidth() && 0<tabList[i].bmp.GetHeight())
		{
			FsGuiCommonDrawing::DrawBmpArray(tabList[i].bmp,x+3,y+3+yOffset);
		}
		if(YSTRUE!=tabList[i].enabled)
		{
			FsGuiCommonDrawing::DrawMask(defFgCol,defBgCol,x,y,labelWid-1,labelHei-1);
		}

		x+=labelWid;
	}
}

void FsGuiTreeControl::Draw(YSBOOL focus,YSBOOL /*mouseOver*/,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor & /*activeBgCol*/,const YsColor & /*activeFgCol*/,const YsColor &frameCol) const
{
	if(YSTRUE==needRePosition)
	{
		RePosition();
	}

	switch(scheme)
	{
	default:
	case PLAIN:
		FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,defListBgCol,YSTRUE);
		FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,frameCol,YSFALSE);
		DrawRect(focus,defFgCol);
		break;
	case MODERN:
		if(YSTRUE==focus)
		{
			FsGuiCommonDrawing::DrawRect(x0-2,y0-2,x0+wid+1,y0+hei+1,frameCol,YSTRUE);
		}
		FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid-1,y0+hei-1,defListBgCol,YSTRUE);
		break;
	}


	FsGuiCommonDrawing::SetClipRect(x0,y0,wid,hei);

	if(YSTRUE==ShowRoot())
	{
		DrawHighlightOneNode(&rootNode,defListActiveBgCol);
	}
	DrawHighlightRecursive(defListActiveBgCol);
	if(YSTRUE==ShowRoot())
	{
		DrawOneNode(&rootNode,defBgCol,defFgCol);
	}
	DrawRecursive(&rootNode,defBgCol,defFgCol);

	if(YSTRUE!=enabled)
	{
		FsGuiCommonDrawing::DrawMask(defFgCol,defBgCol,x0,y0,wid,hei);
	}

	if(this->hei<contentHei)
	{
		int x1,y1,x2,y2;
		GetVerticalScrollBarRect(x1,y1,x2,y2);
		DrawVerticalScrollBar(x1,y1,x2,y2,defBgCol,defFgCol);
		GetVerticalScrollBarSliderRect(x1,y1,x2,y2);
		DrawVerticalScrollBarSlider(x1,y1,x2,y2,contentHei,yScroll,this->hei,defBgCol,defFgCol);
	}

	FsGuiCommonDrawing::ClearClipRect();

	if(YSTRUE==CaptureDragNode() && (lastMx!=mx0 || lastMy!=my0))
	{
		YsArray <const FsGuiTreeControlNode *,8> selNode;
		GetAllSelectedNode(selNode);

		int y=lastMy;
		for(YSSIZE_T i=0; i<selNode.GetN(); ++i)
		{
			FsGuiCommonDrawing::DrawBmpArray(selNode[i]->GetBitmap(),lastMx,y);
			y+=selNode[i]->Height();
		}

		const FsGuiTreeControlNode *atCursor=FindNodeAtWindowCoord(lastMx,lastMy);
		if(NULL!=atCursor)
		{
			int x1,y1,x2,y2;
			GetNodeRect(x1,y1,x2,y2,atCursor);
			x1=x1+this->x0-xScroll;
			y1=y1+this->y0-yScroll;
			x2=x2+this->x0-xScroll;
			y2=y2+this->y0-yScroll;
			FsGuiCommonDrawing::DrawRect(x1,y1,x2,y2,YsWhite(),YSFALSE);

			YSBOOL infinite=YSFALSE;
			for(YSSIZE_T i=0; i<selNode.GetN(); ++i)
			{
				if(YSTRUE==atCursor->IsDescendantOf(selNode[i]))
				{
					infinite=YSTRUE;
					break;
				}
			}
			if(YSTRUE==infinite)
			{
				FsGuiCommonDrawing::DrawLine(x1,y1,x1+boxWid,y1+boxWid,YsRed());
				FsGuiCommonDrawing::DrawLine(x1+boxWid,y1,x1,y1+boxWid,YsRed());

				FsGuiCommonDrawing::DrawLine(x1+1,y1,x1+1+boxWid,y1+boxWid,YsRed());
				FsGuiCommonDrawing::DrawLine(x1+1+boxWid,y1,x1+1,y1+boxWid,YsRed());
			}
		}
	}
}

void FsGuiTreeControl::DrawHighlightRecursive(const YsColor &col) const
{
	YsArray <NodeAndIndex> stack;

	stack.Increment();
	stack.GetEnd().node=&rootNode;
	stack.GetEnd().childIndex=0;
	while(0<stack.GetN())
	{
		if(stack.GetEnd().node->GetNChild()<=stack.GetEnd().childIndex)
		{
			stack.DeleteLast();
		}
		else
		{
			const int childIndex=stack.GetEnd().childIndex++;
			const FsGuiTreeControlNode *child=stack.GetEnd().node->GetChild(childIndex);
			DrawHighlightOneNode(child,col);
			if(YSTRUE==child->IsOpen() && 0<child->GetNChild())
			{
				stack.Increment();
				stack.GetEnd().node=child;
				stack.GetEnd().childIndex=0;
			}
		}
	}
}

void FsGuiTreeControl::DrawHighlightOneNode(const FsGuiTreeControlNode *node,const YsColor &col) const
{
	switch(scheme)
	{
	default:
	case PLAIN:
		if(YSTRUE==node->IsSelected())
		{
			const int x0=this->x0+boxWid+defHSpaceUnit+node->x-xScroll-1;
			const int y0=this->y0+node->y-yScroll-1;
			const int x1=x0+boxWid+node->Width();
			const int y1=this->y0+node->LowerY()-yScroll;
			FsGuiCommonDrawing::DrawRect(x0,y0,x1,y1,col,YSTRUE);
		}
		break;
	case MODERN:
		{
			const int x0=this->x0;
			const int y0=this->y0+node->y-yScroll-1;
			const int x1=this->x0+this->wid-1;
			const int y1=this->y0+node->LowerY()-yScroll;
			if(YSTRUE==node->IsSelected())
			{
				FsGuiCommonDrawing::DrawRect(x0,y0,x1,y1,col,YSTRUE);
			}
			else
			{
				const double rect[8]=
				{
					(double)x0,(double)y0,
					(double)x1,(double)y0,
					(double)x1,(double)y1,
					(double)x0,(double)y1
				};
				const YsColor brighter(
				   YsSmaller(defListBgCol.Rd()+0.2,1.0),
				   YsSmaller(defListBgCol.Gd()+0.2,1.0),
				   YsSmaller(defListBgCol.Bd()+0.2,1.0));
				const YsColor col[4]=
				{
					brighter,
					brighter,
					defListBgCol,
					defListBgCol
				};
				FsGuiCommonDrawing::DrawGradationPolygon(4,rect,col);
			}
		}
		break;
	}
}

void FsGuiTreeControl::DrawRecursive(const FsGuiTreeControlNode *node,const YsColor &bgCol,const YsColor &fgCol) const
{
	for(YSSIZE_T i=0; i<node->GetNChild(); ++i)
	{
		const FsGuiTreeControlNode *child=node->GetChild(i);
		DrawOneNode(child,bgCol,fgCol);

		if(YSTRUE==child->IsOpen())
		{
			DrawRecursive(child,bgCol,fgCol);
		}
	}
}

void FsGuiTreeControl::DrawOneNode(const FsGuiTreeControlNode *node,const YsColor &/*bgCol*/ ,const YsColor &fgCol) const
{
	const int x0=this->x0+node->x-xScroll;
	const int y0=this->y0+node->y-yScroll;

	FsGuiCommonDrawing::DrawRect(x0,y0,x0+boxWid,y0+boxWid,fgCol,YSFALSE);
	FsGuiCommonDrawing::DrawBmpArray(node->GetBitmap(),x0+boxWid+defHSpaceUnit,y0);

	if(cursorPos==node)
	{
		const int x0=this->x0+boxWid+defHSpaceUnit+node->x-xScroll-1;
		const int y0=this->y0+node->y-yScroll-1;
		const int x1=x0+boxWid+node->Width();
		const int y1=this->y0+node->LowerY()-yScroll;
		FsGuiCommonDrawing::DrawRect(x0,y0,x1,y1,YsBlue(),YSFALSE);
	}

	if(0<node->GetNChild())
	{
		FsGuiCommonDrawing::DrawLine(x0,y0+boxWid/2,x0+boxWid,y0+boxWid/2,fgCol);
		if(YSTRUE!=node->IsOpen())
		{
			FsGuiCommonDrawing::DrawLine(x0+boxWid/2,y0,x0+boxWid/2,y0+boxWid,fgCol);
		}
	}
}

void FsGuiDialog::Show(const FsGuiDialog *excludeFromDrawing) const
{
	if(this==excludeFromDrawing)
	{
		return;
	}

	if(2<=dialogBgCol.Ai())
	{
		switch(scheme)
		{
		case PLAIN:
			FsGuiCommonDrawing::DrawRect(dlgX0,dlgY0,dlgX0+dlgWid-1,dlgY0+dlgHei-1,dialogBgCol,YSTRUE);
			FsGuiCommonDrawing::DrawRect(dlgX0,dlgY0,dlgX0+dlgWid-1,dlgY0+dlgHei-1,fgCol,YSFALSE);
			break;
		case MODERN:
			FsGuiDrawRoundRect(dlgX0,dlgY0,dlgX0+dlgWid-1,dlgY0+dlgHei-1,dialogBgCol,0.0,defRoundRadius);
			break;
		}
	}

	if(0<bmpMessage.GetWidth() && 0<bmpMessage.GetHeight())
	{
		FsGuiCommonDrawing::DrawBmpArray(bmpMessage,dlgX0+FsGuiObject::defHSpaceUnit,dlgY0+FsGuiObject::defVSpaceUnit);
	}

	YsListItem <FsGuiGroupBox> *grpBox;
	grpBox=NULL;
	while(NULL!=(grpBox=grpBoxList.FindNext(grpBox)))
	{
		if(YSTRUE==grpBox->dat.IsVisible())
		{
			FsGuiCommonDrawing::DrawRect(grpBox->dat.x0,grpBox->dat.y0,grpBox->dat.x1,grpBox->dat.y1,fgCol,YSFALSE);
		}
	}

	int i;
	forYsArray(i,itemList)
	{
		if(YSTRUE==itemList[i]->IsVisible())
		{
			YSBOOL f,m;
			f=(itemList[i]==focus ? YSTRUE : YSFALSE);
			m=(itemList[i]==mouseOver ? YSTRUE : YSFALSE);
			itemList[i]->Draw(f,m,bgCol,fgCol,activeBgCol,activeFgCol,frameCol);
		}
	}

	if(popUp!=NULL)
	{
		popUp->DrawPopUp(bgCol,fgCol,activeBgCol,activeFgCol,frameCol);
	}

	needRedraw=YSFALSE;

	if(NULL!=GetModalDialog())
	{
		FsGuiCommonDrawing::DrawMask(fgCol,bgCol,dlgX0,dlgY0,dlgX0+dlgWid-1,dlgY0+dlgHei-1);
		GetModalDialog()->Show(excludeFromDrawing);
	}
}

