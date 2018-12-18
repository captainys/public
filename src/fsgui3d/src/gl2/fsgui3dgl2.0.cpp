/* ////////////////////////////////////////////////////////////

File Name: fsgui3dgl2.0.cpp
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
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <ysclass.h>

#include <fssimplewindow.h>

#include <ysbitmap.h>
#include <fsgui.h>
#include "fsgui3d.h"

#include <ysgl.h>



void FsGui3DInterface::Draw(void) const
{
	needRedraw=YSFALSE;

	if(type==IFTYPE_BOX)
	{
		YsGLSLUse3DRenderer(YsGLSLSharedFlat3DRenderer());

		const GLfloat colBuf[4]={0.0f,1.0f,0.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(YsGLSLSharedFlat3DRenderer(),colBuf);
		YsGLSLSet3DRendererTextureType(YsGLSLSharedFlat3DRenderer(),YSGLSL_TEX_TYPE_NONE);

#ifdef GL_LINE_STIPPLE
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,0xf0f0);
#endif
		GLfloat vtxBuf[72];

		vtxBuf[ 0]=(GLfloat)box_pos[0].x();
		vtxBuf[ 1]=(GLfloat)box_pos[0].y();
		vtxBuf[ 2]=(GLfloat)box_pos[0].z();
		vtxBuf[ 3]=(GLfloat)box_pos[1].x();
		vtxBuf[ 4]=(GLfloat)box_pos[0].y();
		vtxBuf[ 5]=(GLfloat)box_pos[0].z();

		vtxBuf[ 6]=(GLfloat)box_pos[1].x();
		vtxBuf[ 7]=(GLfloat)box_pos[0].y();
		vtxBuf[ 8]=(GLfloat)box_pos[0].z();
		vtxBuf[ 9]=(GLfloat)box_pos[1].x();
		vtxBuf[10]=(GLfloat)box_pos[1].y();
		vtxBuf[11]=(GLfloat)box_pos[0].z();

		vtxBuf[12]=(GLfloat)box_pos[1].x();
		vtxBuf[13]=(GLfloat)box_pos[1].y();
		vtxBuf[14]=(GLfloat)box_pos[0].z();
		vtxBuf[15]=(GLfloat)box_pos[0].x();
		vtxBuf[16]=(GLfloat)box_pos[1].y();
		vtxBuf[17]=(GLfloat)box_pos[0].z();


		vtxBuf[18]=(GLfloat)box_pos[0].x();
		vtxBuf[19]=(GLfloat)box_pos[1].y();
		vtxBuf[20]=(GLfloat)box_pos[0].z();
		vtxBuf[21]=(GLfloat)box_pos[0].x();
		vtxBuf[22]=(GLfloat)box_pos[0].y();
		vtxBuf[23]=(GLfloat)box_pos[0].z();

		vtxBuf[24]=(GLfloat)box_pos[0].x();
		vtxBuf[25]=(GLfloat)box_pos[0].y();
		vtxBuf[26]=(GLfloat)box_pos[1].z();
		vtxBuf[27]=(GLfloat)box_pos[1].x();
		vtxBuf[28]=(GLfloat)box_pos[0].y();
		vtxBuf[29]=(GLfloat)box_pos[1].z();

		vtxBuf[30]=(GLfloat)box_pos[1].x();
		vtxBuf[31]=(GLfloat)box_pos[0].y();
		vtxBuf[32]=(GLfloat)box_pos[1].z();
		vtxBuf[33]=(GLfloat)box_pos[1].x();
		vtxBuf[34]=(GLfloat)box_pos[1].y();
		vtxBuf[35]=(GLfloat)box_pos[1].z();

		vtxBuf[36]=(GLfloat)box_pos[1].x();
		vtxBuf[37]=(GLfloat)box_pos[1].y();
		vtxBuf[38]=(GLfloat)box_pos[1].z();
		vtxBuf[39]=(GLfloat)box_pos[0].x();
		vtxBuf[40]=(GLfloat)box_pos[1].y();
		vtxBuf[41]=(GLfloat)box_pos[1].z();

		vtxBuf[42]=(GLfloat)box_pos[0].x();
		vtxBuf[43]=(GLfloat)box_pos[1].y();
		vtxBuf[44]=(GLfloat)box_pos[1].z();
		vtxBuf[45]=(GLfloat)box_pos[0].x();
		vtxBuf[46]=(GLfloat)box_pos[0].y();
		vtxBuf[47]=(GLfloat)box_pos[1].z();


		vtxBuf[48]=(GLfloat)box_pos[0].x();
		vtxBuf[49]=(GLfloat)box_pos[0].y();
		vtxBuf[50]=(GLfloat)box_pos[0].z();
		vtxBuf[51]=(GLfloat)box_pos[0].x();
		vtxBuf[52]=(GLfloat)box_pos[0].y();
		vtxBuf[53]=(GLfloat)box_pos[1].z();

		vtxBuf[54]=(GLfloat)box_pos[1].x();
		vtxBuf[55]=(GLfloat)box_pos[0].y();
		vtxBuf[56]=(GLfloat)box_pos[0].z();
		vtxBuf[57]=(GLfloat)box_pos[1].x();
		vtxBuf[58]=(GLfloat)box_pos[0].y();
		vtxBuf[59]=(GLfloat)box_pos[1].z();

		vtxBuf[60]=(GLfloat)box_pos[1].x();
		vtxBuf[61]=(GLfloat)box_pos[1].y();
		vtxBuf[62]=(GLfloat)box_pos[0].z();
		vtxBuf[63]=(GLfloat)box_pos[1].x();
		vtxBuf[64]=(GLfloat)box_pos[1].y();
		vtxBuf[65]=(GLfloat)box_pos[1].z();

		vtxBuf[66]=(GLfloat)box_pos[0].x();
		vtxBuf[67]=(GLfloat)box_pos[1].y();
		vtxBuf[68]=(GLfloat)box_pos[0].z();
		vtxBuf[69]=(GLfloat)box_pos[0].x();
		vtxBuf[70]=(GLfloat)box_pos[1].y();
		vtxBuf[71]=(GLfloat)box_pos[1].z();

		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,24,vtxBuf);

#ifdef GL_LINE_STIPPLE
		glDisable(GL_LINE_STIPPLE);
#endif
		const YsVec3 cen=(box_pos[0]+box_pos[1])/2.0;
		YsVec3 faceCen[6];

		faceCen[0].Set(box_pos[0].x(),cen.y(),cen.z());

		vtxBuf[ 0]=(GLfloat)faceCen[0].x();
		vtxBuf[ 1]=(GLfloat)faceCen[0].y();
		vtxBuf[ 2]=(GLfloat)faceCen[0].z();

		faceCen[1].Set(box_pos[1].x(),cen.y(),cen.z());

		vtxBuf[ 3]=(GLfloat)faceCen[1].x();
		vtxBuf[ 4]=(GLfloat)faceCen[1].y();
		vtxBuf[ 5]=(GLfloat)faceCen[1].z();

		faceCen[2].Set(cen.x(),box_pos[0].y(),cen.z());

		vtxBuf[ 6]=(GLfloat)faceCen[2].x();
		vtxBuf[ 7]=(GLfloat)faceCen[2].y();
		vtxBuf[ 8]=(GLfloat)faceCen[2].z();

		faceCen[3].Set(cen.x(),box_pos[1].y(),cen.z());

		vtxBuf[ 9]=(GLfloat)faceCen[3].x();
		vtxBuf[10]=(GLfloat)faceCen[3].y();
		vtxBuf[11]=(GLfloat)faceCen[3].z();

		faceCen[4].Set(cen.x(),cen.y(),box_pos[0].z());

		vtxBuf[12]=(GLfloat)faceCen[4].x();
		vtxBuf[13]=(GLfloat)faceCen[4].y();
		vtxBuf[14]=(GLfloat)faceCen[4].z();

		faceCen[5].Set(cen.x(),cen.y(),box_pos[1].z());

		vtxBuf[15]=(GLfloat)faceCen[5].x();
		vtxBuf[16]=(GLfloat)faceCen[5].y();
		vtxBuf[17]=(GLfloat)faceCen[5].z();


	#ifdef GL_PROGRAM_POINT_SIZE
		glEnable(GL_PROGRAM_POINT_SIZE);
	#endif
		YsGLSLSet3DRendererUniformPointSize(YsGLSLSharedFlat3DRenderer(),(float)markerSize);
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_POINTS,6,vtxBuf);
	#ifdef GL_PROGRAM_POINT_SIZE
		glDisable(GL_PROGRAM_POINT_SIZE);
	#endif


		if(dragging==YSTRUE)
		{
			switch(box_dragging)
			{
			default:
				break;
			case BOX_XMIN:
				vtxBuf[0]=(GLfloat)(box_pos[0].x()-infiniteLongLine);
				vtxBuf[1]=(GLfloat)(cen.y());
				vtxBuf[2]=(GLfloat)(cen.z());
				vtxBuf[3]=(GLfloat)(box_pos[0].x()+infiniteLongLine);
				vtxBuf[4]=(GLfloat)(cen.y());
				vtxBuf[5]=(GLfloat)(cen.z());
				YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
				break;
			case BOX_XMAX:
				vtxBuf[0]=(GLfloat)(box_pos[1].x()-infiniteLongLine);
				vtxBuf[1]=(GLfloat)(cen.y());
				vtxBuf[2]=(GLfloat)(cen.z());
				vtxBuf[3]=(GLfloat)(box_pos[1].x()+infiniteLongLine);
				vtxBuf[4]=(GLfloat)(cen.y());
				vtxBuf[5]=(GLfloat)(cen.z());
				YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
				break;
			case BOX_YMIN:
				vtxBuf[0]=(GLfloat)(cen.x());
				vtxBuf[1]=(GLfloat)(box_pos[0].y()-infiniteLongLine);
				vtxBuf[2]=(GLfloat)(cen.z());
				vtxBuf[3]=(GLfloat)(cen.x());
				vtxBuf[4]=(GLfloat)(box_pos[0].y()+infiniteLongLine);
				vtxBuf[5]=(GLfloat)(cen.z());
				YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
				break;
			case BOX_YMAX:
				vtxBuf[0]=(GLfloat)(cen.x());
				vtxBuf[1]=(GLfloat)(box_pos[1].y()-infiniteLongLine);
				vtxBuf[2]=(GLfloat)(cen.z());
				vtxBuf[3]=(GLfloat)(cen.x());
				vtxBuf[4]=(GLfloat)(box_pos[1].y()+infiniteLongLine);
				vtxBuf[5]=(GLfloat)(cen.z());
				YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
				break;
			case BOX_ZMIN:
				vtxBuf[0]=(GLfloat)(cen.x());
				vtxBuf[1]=(GLfloat)(cen.y());
				vtxBuf[2]=(GLfloat)(box_pos[0].z()-infiniteLongLine);
				vtxBuf[3]=(GLfloat)(cen.x());
				vtxBuf[4]=(GLfloat)(cen.y());
				vtxBuf[5]=(GLfloat)(box_pos[0].z()+infiniteLongLine);
				YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
				break;
			case BOX_ZMAX:
				vtxBuf[0]=(GLfloat)(cen.x());
				vtxBuf[1]=(GLfloat)(cen.y());
				vtxBuf[2]=(GLfloat)(box_pos[1].z()-infiniteLongLine);
				vtxBuf[3]=(GLfloat)(cen.x());
				vtxBuf[4]=(GLfloat)(cen.y());
				vtxBuf[5]=(GLfloat)(box_pos[1].z()+infiniteLongLine);
				YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
				break;
			}
		}

		YsGLSLEndUse3DRenderer(YsGLSLSharedFlat3DRenderer());

		glLineWidth(float(linePickingTolerance));

		DrawLineByPixelLength(faceCen[0],YsVec3( 1.0,0.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[0],YsVec3(-1.0,0.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[1],YsVec3( 1.0,0.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[1],YsVec3(-1.0,0.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[2],YsVec3(0.0, 1.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[2],YsVec3(0.0,-1.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[3],YsVec3(0.0, 1.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[3],YsVec3(0.0,-1.0,0.0),pickerLength);
		DrawLineByPixelLength(faceCen[4],YsVec3(0.0,0.0, 1.0),pickerLength);
		DrawLineByPixelLength(faceCen[4],YsVec3(0.0,0.0,-1.0),pickerLength);
		DrawLineByPixelLength(faceCen[5],YsVec3(0.0,0.0, 1.0),pickerLength);
		DrawLineByPixelLength(faceCen[5],YsVec3(0.0,0.0,-1.0),pickerLength);

		glLineWidth(1);
	}
	else if(type==IFTYPE_POINT)
	{
		auto nom=point_dlg1->GetNormalDirection();

		YsGLSLUse3DRenderer(YsGLSLSharedFlat3DRenderer());

		const GLfloat colBuf[4]={0.0f,1.0f,0.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(YsGLSLSharedFlat3DRenderer(),colBuf);

		GLfloat vtxBuf[18];

		switch(point_dlg1->GetBasePlane())
		{
		case 0:
			vtxBuf[ 0]=(GLfloat)(point_pos.x());
			vtxBuf[ 1]=(GLfloat)(point_pos.y());
			vtxBuf[ 2]=(GLfloat)(point_pos.z());
			vtxBuf[ 3]=(GLfloat)(point_pos.x());
			vtxBuf[ 4]=(GLfloat)(point_pos.y());
			vtxBuf[ 5]=0.0f;
			vtxBuf[ 6]=(GLfloat)(point_pos.x());
			vtxBuf[ 7]=0.0f;
			vtxBuf[ 8]=0.0f;
			vtxBuf[ 9]=0.0f;
			vtxBuf[10]=0.0f;
			vtxBuf[11]=0.0f;
			vtxBuf[12]=0.0f;
			vtxBuf[13]=(GLfloat)(point_pos.y());
			vtxBuf[14]=0.0f;
			vtxBuf[15]=(GLfloat)(point_pos.x());
			vtxBuf[16]=(GLfloat)(point_pos.y());
			vtxBuf[17]=0.0f;
			break;
		case 1:
			vtxBuf[ 0]=(GLfloat)(point_pos.x());
			vtxBuf[ 1]=(GLfloat)(point_pos.y());
			vtxBuf[ 2]=(GLfloat)(point_pos.z());
			vtxBuf[ 3]=(GLfloat)(point_pos.x());
			vtxBuf[ 4]=0.0f;
			vtxBuf[ 5]=(GLfloat)(point_pos.z());
			vtxBuf[ 6]=0.0f;
			vtxBuf[ 7]=0.0f;
			vtxBuf[ 8]=(GLfloat)(point_pos.z());
			vtxBuf[ 9]=0.0f;
			vtxBuf[10]=0.0f;
			vtxBuf[11]=0.0f;
			vtxBuf[12]=(GLfloat)(point_pos.x());
			vtxBuf[13]=0.0f;
			vtxBuf[14]=0.0f;
			vtxBuf[15]=(GLfloat)(point_pos.x());
			vtxBuf[16]=0.0f;
			vtxBuf[17]=(GLfloat)(point_pos.z());
			break;
		case 2:
			vtxBuf[ 0]=(GLfloat)(point_pos.x());
			vtxBuf[ 1]=(GLfloat)(point_pos.y());
			vtxBuf[ 2]=(GLfloat)(point_pos.z());
			vtxBuf[ 3]=0.0f;
			vtxBuf[ 4]=(GLfloat)(point_pos.y());
			vtxBuf[ 5]=(GLfloat)(point_pos.z());
			vtxBuf[ 6]=0.0f;
			vtxBuf[ 7]=0.0f;
			vtxBuf[ 8]=(GLfloat)(point_pos.z());
			vtxBuf[ 9]=0.0f;
			vtxBuf[10]=0.0f;
			vtxBuf[11]=0.0f;
			vtxBuf[12]=0.0f;
			vtxBuf[13]=(GLfloat)(point_pos.y());
			vtxBuf[14]=0.0f;
			vtxBuf[15]=0.0f;
			vtxBuf[16]=(GLfloat)(point_pos.y());
			vtxBuf[17]=(GLfloat)(point_pos.z());
			break;
		}

		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINE_STRIP,6,vtxBuf);

		if(POINT_Y==point_dragging)
		{
			glLineWidth(1);
#ifdef GL_LINE_STIPPLE
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1,0x0003);
#endif
			GLfloat vtxBuf[6];
			vtxBuf[0]=(GLfloat)(point_pos-nom*infiniteLongLine).x();
			vtxBuf[1]=(GLfloat)(point_pos-nom*infiniteLongLine).y();
			vtxBuf[2]=(GLfloat)(point_pos-nom*infiniteLongLine).z();
			vtxBuf[3]=(GLfloat)(point_pos+nom*infiniteLongLine).x();
			vtxBuf[4]=(GLfloat)(point_pos+nom*infiniteLongLine).y();
			vtxBuf[5]=(GLfloat)(point_pos+nom*infiniteLongLine).z();
			YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
#ifdef GL_LINE_STIPPLE
			glDisable(GL_LINE_STIPPLE);
#endif
        }

		YsGLSLEndUse3DRenderer(YsGLSLSharedFlat3DRenderer());

		DrawRectMarker(point_pos,markerSize);

		if(YSTRUE==point_dlg1->ShowVerticalHandle())
		{
			glLineWidth(float(linePickingTolerance));
			DrawLineByPixelLength(point_pos, nom,pickerLength);
			DrawLineByPixelLength(point_pos,-nom,pickerLength);
			glLineWidth(float(1));
		}
	}
	else if(type==IFTYPE_DRAGPOINT)
	{
		YsGLSLUse3DRenderer(YsGLSLSharedFlat3DRenderer());

		const GLfloat colBuf[4]={0.0f,1.0f,0.0f,1.0f};
		YsGLSLSet3DRendererUniformColorfv(YsGLSLSharedFlat3DRenderer(),colBuf);

		GLfloat vtxBuf[18];

		vtxBuf[0]=(GLfloat)point_pos.x();
		vtxBuf[1]=(GLfloat)point_pos.y();
		vtxBuf[2]=(GLfloat)point_pos.z();

	#ifdef GL_PROGRAM_POINT_SIZE
		glEnable(GL_PROGRAM_POINT_SIZE);
	#endif
		YsGLSLSet3DRendererUniformPointSize(YsGLSLSharedFlat3DRenderer(),(float)markerSize);
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_POINTS,1,vtxBuf);
	#ifdef GL_PROGRAM_POINT_SIZE
		glDisable(GL_PROGRAM_POINT_SIZE);
	#endif


		YsVec3 dragDir[3];
		if(YSTRUE==point_dlg2->UseCustomOrientation())
		{
			dragDir[0]=point_dragDir[0];
			dragDir[1]=point_dragDir[1];
			dragDir[2]=point_dragDir[2];
		}
		else
		{
			dragDir[0]=YsXVec();
			dragDir[1]=YsYVec();
			dragDir[2]=YsZVec();
		}

		const DRAGTARGET pointDragTarget[3]=
		{
			POINT_X,
			POINT_Y,
			POINT_Z
		};

		glLineWidth(1);
#ifdef GL_LINE_STIPPLE
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,0x0003);
#endif
        int nVtx=0;
		for(int i=0; i<3; i++)
		{
			if(dragging==YSTRUE && point_dragging==pointDragTarget[i])
			{
				vtxBuf[nVtx*3  ]=(GLfloat)(point_pos-dragDir[i]*infiniteLongLine).x();
				vtxBuf[nVtx*3+1]=(GLfloat)(point_pos-dragDir[i]*infiniteLongLine).y();
				vtxBuf[nVtx*3+2]=(GLfloat)(point_pos-dragDir[i]*infiniteLongLine).z();
				vtxBuf[nVtx*3+3]=(GLfloat)(point_pos+dragDir[i]*infiniteLongLine).x();
				vtxBuf[nVtx*3+4]=(GLfloat)(point_pos+dragDir[i]*infiniteLongLine).y();
				vtxBuf[nVtx*3+5]=(GLfloat)(point_pos+dragDir[i]*infiniteLongLine).z();
				nVtx+=2;
			}
		}
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,nVtx,vtxBuf);
#ifdef GL_LINE_STIPPLE
		glDisable(GL_LINE_STIPPLE);
#endif
		YsGLSLEndUse3DRenderer(YsGLSLSharedFlat3DRenderer());

		glLineWidth(float(linePickingTolerance));
		for(int i=0; i<3; i++)
		{
			if(point_dragDirSwitch[i]==YSTRUE)
			{
				DrawLineByPixelLength(point_pos, dragDir[i],pickerLength);
				DrawLineByPixelLength(point_pos,-dragDir[i],pickerLength);
			}
		}
		glLineWidth(1);
	}
	else if(type==IFTYPE_SLIDER)
	{
	}
	else if(type==IFTYPE_ROTATION)
	{
		YsGLSLUse3DRenderer(YsGLSLSharedFlat3DRenderer());

		if(dragging==YSTRUE)
		{
			const GLfloat colBuf[4]={1.0f,0.0f,1.0f,1.0f};
			YsGLSLSet3DRendererUniformColorfv(YsGLSLSharedFlat3DRenderer(),colBuf);
		}
		else
		{
			const GLfloat colBuf[4]={0.0f,1.0f,0.0f,1.0f};
			YsGLSLSet3DRendererUniformColorfv(YsGLSLSharedFlat3DRenderer(),colBuf);
		}

		GLfloat vtxBuf[36*3];

		vtxBuf[0]=(GLfloat)(rot_center+rot_axis*infiniteLongLine).x();
		vtxBuf[1]=(GLfloat)(rot_center+rot_axis*infiniteLongLine).y();
		vtxBuf[2]=(GLfloat)(rot_center+rot_axis*infiniteLongLine).z();
		vtxBuf[3]=(GLfloat)(rot_center-rot_axis*infiniteLongLine).x();
		vtxBuf[4]=(GLfloat)(rot_center-rot_axis*infiniteLongLine).y();
		vtxBuf[5]=(GLfloat)(rot_center-rot_axis*infiniteLongLine).z();

		glLineWidth(3);
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
		glLineWidth(1);


		YsVec3 iVec=rot_iVec;
		YsVec3 jVec=rot_jVec;
		YsVec3 kVec=rot_kVec;

		YsMatrix4x4 mat,axisMat;
		YsAtt3 att;
		att.SetForwardVector(rot_axis);
		att.GetMatrix4x4(axisMat);
		mat.Initialize();
		mat.Rotate(rot_axis.x(),rot_axis.y(),rot_axis.z(),rot_angle);
		mat=mat*axisMat;
		mat.Scale(1.0,1.0,rot_mirror);
		mat.Scale(rot_ref_size,rot_ref_size,rot_ref_size);
		axisMat.Invert();
		mat=mat*axisMat;

		iVec=mat*iVec;
		jVec=mat*jVec;
		kVec=mat*kVec;

		for(int i=0; i<12; ++i)  // 24 vtxs
		{
			const double a=YsPi*2.0*double(i)/12.0;
			const YsVec3 p[2]=
			{
				rot_center+iVec*cos(a)+jVec*1.0+kVec*sin(a),
				rot_center+iVec*cos(a)-jVec*1.0+kVec*sin(a)
			};

			vtxBuf[i*6  ]=(GLfloat)p[0].x();
			vtxBuf[i*6+1]=(GLfloat)p[0].y();
			vtxBuf[i*6+2]=(GLfloat)p[0].z();
			vtxBuf[i*6+3]=(GLfloat)p[1].x();
			vtxBuf[i*6+4]=(GLfloat)p[1].y();
			vtxBuf[i*6+5]=(GLfloat)p[1].z();
		}
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,24,vtxBuf);

		for(int i=0; i<36; ++i)
		{
			const double a=YsPi*2.0*double(i)/36.0;
			const YsVec3 p=rot_center+iVec*cos(a)+jVec*1.0+kVec*sin(a);
			vtxBuf[i*3  ]=(GLfloat)p.x();
			vtxBuf[i*3+1]=(GLfloat)p.y();
			vtxBuf[i*3+2]=(GLfloat)p.z();
		}
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINE_LOOP,36,vtxBuf);

		for(int i=0; i<36; ++i)
		{
			const double a=YsPi*2.0*double(i)/36.0;
			const YsVec3 p=rot_center+iVec*cos(a)-jVec*1.0+kVec*sin(a);
			vtxBuf[i*3  ]=(GLfloat)p.x();
			vtxBuf[i*3+1]=(GLfloat)p.y();
			vtxBuf[i*3+2]=(GLfloat)p.z();
		}
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINE_LOOP,36,vtxBuf);


		// Side drag marker

		const YsVec3 sideX=rot_drag_dir*rot_ref_size*1.1;
		const YsVec3 sideY=(rot_drag_dir^rot_axis)*rot_ref_size*1.1;


		for(int i=0; i<36; ++i)
		{
			const double a=YsPi*2.0*double(i)/36.0;
			const YsVec3 p=rot_center+sideX*cos(a)+sideY*sin(a);
			vtxBuf[i*3  ]=(GLfloat)p.x();
			vtxBuf[i*3+1]=(GLfloat)p.y();
			vtxBuf[i*3+2]=(GLfloat)p.z();
		}
		glLineWidth(3);
		YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINE_LOOP,36,vtxBuf);


		if(ROTATION_HANDLE==rot_dragging)
		{
			vtxBuf[0]=(GLfloat)rot_center.x();
			vtxBuf[1]=(GLfloat)rot_center.y();
			vtxBuf[2]=(GLfloat)rot_center.z();

			const YsVec3 radial=rot_ref_size*1.1*(rot_iVec*cos(rot_drag_angle)-rot_kVec*sin(rot_drag_angle));
			vtxBuf[3]=(GLfloat)(rot_center.x()+radial.x());
			vtxBuf[4]=(GLfloat)(rot_center.y()+radial.y());
			vtxBuf[5]=(GLfloat)(rot_center.z()+radial.z());

			YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
		}

		glLineWidth(1);

		YsGLSLEndUse3DRenderer(YsGLSLSharedFlat3DRenderer());
	}
}

void FsGui3DInterface::DrawLineByPixelLength(const YsVec3 &o,const YsVec3 &v,const double lng) const
{
	YsVec3 p1,p2,scrnP1,scrnP2;
	CalculateLineEndPointForSpecificPixelLength(p1,p2,scrnP1,scrnP2,o,v,lng);

	const GLfloat vtxBuf[6]=
	{
		p1.xf(),
		p1.yf(),
		p1.zf(),
		p2.xf(),
		p2.yf(),
		p2.zf()
	};
	YsGLSLUse3DRenderer(YsGLSLSharedFlat3DRenderer());
	YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINES,2,vtxBuf);
	YsGLSLEndUse3DRenderer(YsGLSLSharedFlat3DRenderer());
}

void FsGui3DInterface::DrawRectMarker(const YsVec3 &o,const double markerSize) const
{
	YsVec3 scrnO;
	YsTransform3DCoordToScreenCoord(scrnO,o,viewport,projViewModelTfm);

	YsVec3 quad[4]=
	{
		YsVec3(scrnO.x()-markerSize,scrnO.y()-markerSize,scrnO.z()),
		YsVec3(scrnO.x()+markerSize,scrnO.y()-markerSize,scrnO.z()),
		YsVec3(scrnO.x()+markerSize,scrnO.y()+markerSize,scrnO.z()),
		YsVec3(scrnO.x()-markerSize,scrnO.y()+markerSize,scrnO.z())
	};
	YsTransformScreenCoordTo3DCoord(quad[0],quad[0],viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(quad[1],quad[1],viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(quad[2],quad[2],viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(quad[3],quad[3],viewport,projViewModelTfm);

	const GLfloat vtxBuf[12]=
	{
		quad[0].xf(),
		quad[0].yf(),
		quad[0].zf(),
		quad[1].xf(),
		quad[1].yf(),
		quad[1].zf(),
		quad[2].xf(),
		quad[2].yf(),
		quad[2].zf(),
		quad[3].xf(),
		quad[3].yf(),
		quad[3].zf()
	};
	YsGLSLUse3DRenderer(YsGLSLSharedFlat3DRenderer());
	YsGLSLDrawPrimitiveVtxfv(YsGLSLSharedFlat3DRenderer(),GL_LINE_LOOP,4,vtxBuf);
	YsGLSLEndUse3DRenderer(YsGLSLSharedFlat3DRenderer());
}

