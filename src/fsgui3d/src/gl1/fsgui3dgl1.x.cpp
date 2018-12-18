/* ////////////////////////////////////////////////////////////

File Name: fsgui3dgl1.x.cpp
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
#include <ysglfontdata.h>
#include <ysbitmap.h>
#include <fsgui.h>
#include "fsgui3d.h"



void FsGui3DInterface::Draw(void) const
{
	needRedraw=YSFALSE;

	if(type==IFTYPE_BOX)
	{
		YsVec3 cen,o;

		glColor3d(0.0,1.0,0.0);

		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,0xf0f0);

		glBegin(GL_LINES);

		glVertex3d(box_pos[0].x(),box_pos[0].y(),box_pos[0].z());
		glVertex3d(box_pos[1].x(),box_pos[0].y(),box_pos[0].z());

		glVertex3d(box_pos[1].x(),box_pos[0].y(),box_pos[0].z());
		glVertex3d(box_pos[1].x(),box_pos[1].y(),box_pos[0].z());

		glVertex3d(box_pos[1].x(),box_pos[1].y(),box_pos[0].z());
		glVertex3d(box_pos[0].x(),box_pos[1].y(),box_pos[0].z());


		glVertex3d(box_pos[0].x(),box_pos[1].y(),box_pos[0].z());
		glVertex3d(box_pos[0].x(),box_pos[0].y(),box_pos[0].z());

		glVertex3d(box_pos[0].x(),box_pos[0].y(),box_pos[1].z());
		glVertex3d(box_pos[1].x(),box_pos[0].y(),box_pos[1].z());

		glVertex3d(box_pos[1].x(),box_pos[0].y(),box_pos[1].z());
		glVertex3d(box_pos[1].x(),box_pos[1].y(),box_pos[1].z());

		glVertex3d(box_pos[1].x(),box_pos[1].y(),box_pos[1].z());
		glVertex3d(box_pos[0].x(),box_pos[1].y(),box_pos[1].z());

		glVertex3d(box_pos[0].x(),box_pos[1].y(),box_pos[1].z());
		glVertex3d(box_pos[0].x(),box_pos[0].y(),box_pos[1].z());


		glVertex3d(box_pos[0].x(),box_pos[0].y(),box_pos[0].z());
		glVertex3d(box_pos[0].x(),box_pos[0].y(),box_pos[1].z());

		glVertex3d(box_pos[1].x(),box_pos[0].y(),box_pos[0].z());
		glVertex3d(box_pos[1].x(),box_pos[0].y(),box_pos[1].z());

		glVertex3d(box_pos[1].x(),box_pos[1].y(),box_pos[0].z());
		glVertex3d(box_pos[1].x(),box_pos[1].y(),box_pos[1].z());

		glVertex3d(box_pos[0].x(),box_pos[1].y(),box_pos[0].z());
		glVertex3d(box_pos[0].x(),box_pos[1].y(),box_pos[1].z());


		glEnd();

		glDisable(GL_LINE_STIPPLE);

		cen=(box_pos[0]+box_pos[1])/2.0;

		glPointSize(float(markerSize));
		glLineWidth(float(linePickingTolerance));

		o.Set(box_pos[0].x(),cen.y(),cen.z());
		glBegin(GL_POINTS);
		glVertex3dv(o.GetValue());
		glEnd();
		DrawLineByPixelLength(o,YsVec3( 1.0,0.0,0.0),pickerLength);
		DrawLineByPixelLength(o,YsVec3(-1.0,0.0,0.0),pickerLength);

		o.Set(box_pos[1].x(),cen.y(),cen.z());
		glBegin(GL_POINTS);
		glVertex3dv(o.GetValue());
		glEnd();
		DrawLineByPixelLength(o,YsVec3( 1.0,0.0,0.0),pickerLength);
		DrawLineByPixelLength(o,YsVec3(-1.0,0.0,0.0),pickerLength);

		o.Set(cen.x(),box_pos[0].y(),cen.z());
		glBegin(GL_POINTS);
		glVertex3dv(o.GetValue());
		glEnd();
		DrawLineByPixelLength(o,YsVec3(0.0, 1.0,0.0),pickerLength);
		DrawLineByPixelLength(o,YsVec3(0.0,-1.0,0.0),pickerLength);

		o.Set(cen.x(),box_pos[1].y(),cen.z());
		glBegin(GL_POINTS);
		glVertex3dv(o.GetValue());
		glEnd();
		DrawLineByPixelLength(o,YsVec3(0.0, 1.0,0.0),pickerLength);
		DrawLineByPixelLength(o,YsVec3(0.0,-1.0,0.0),pickerLength);

		o.Set(cen.x(),cen.y(),box_pos[0].z());
		glBegin(GL_POINTS);
		glVertex3dv(o.GetValue());
		glEnd();
		DrawLineByPixelLength(o,YsVec3(0.0,0.0, 1.0),pickerLength);
		DrawLineByPixelLength(o,YsVec3(0.0,0.0,-1.0),pickerLength);

		o.Set(cen.x(),cen.y(),box_pos[1].z());
		glBegin(GL_POINTS);
		glVertex3dv(o.GetValue());
		glEnd();
		DrawLineByPixelLength(o,YsVec3(0.0,0.0, 1.0),pickerLength);
		DrawLineByPixelLength(o,YsVec3(0.0,0.0,-1.0),pickerLength);


		glPointSize(1);
		glLineWidth(1);

		if(dragging==YSTRUE)
		{
			switch(box_dragging)
			{
			case BOX_XMIN:
				glBegin(GL_LINES);
				glVertex3d(box_pos[0].x()-infiniteLongLine,cen.y(),cen.z());
				glVertex3d(box_pos[0].x()+infiniteLongLine,cen.y(),cen.z());
				glEnd();
				break;
			case BOX_XMAX:
				glBegin(GL_LINES);
				glVertex3d(box_pos[1].x()-infiniteLongLine,cen.y(),cen.z());
				glVertex3d(box_pos[1].x()+infiniteLongLine,cen.y(),cen.z());
				glEnd();
				break;
			case BOX_YMIN:
				glBegin(GL_LINES);
				glVertex3d(cen.x(),box_pos[0].y()-infiniteLongLine,cen.z());
				glVertex3d(cen.x(),box_pos[0].y()+infiniteLongLine,cen.z());
				glEnd();
				break;
			case BOX_YMAX:
				glBegin(GL_LINES);
				glVertex3d(cen.x(),box_pos[1].y()-infiniteLongLine,cen.z());
				glVertex3d(cen.x(),box_pos[1].y()+infiniteLongLine,cen.z());
				glEnd();
				break;
			case BOX_ZMIN:
				glBegin(GL_LINES);
				glVertex3d(cen.x(),cen.y(),box_pos[0].z()-infiniteLongLine);
				glVertex3d(cen.x(),cen.y(),box_pos[0].z()+infiniteLongLine);
				glEnd();
				break;
			case BOX_ZMAX:
				glBegin(GL_LINES);
				glVertex3d(cen.x(),cen.y(),box_pos[1].z()-infiniteLongLine);
				glVertex3d(cen.x(),cen.y(),box_pos[1].z()+infiniteLongLine);
				glEnd();
				break;
			}
		}
	}
	else if(type==IFTYPE_POINT)
	{
		glColor3d(0.0,1.0,0.0);

		glBegin(GL_LINE_STRIP);
		switch(point_dlg1->GetBasePlane())
		{
		case 0:
			glVertex3d(point_pos.x(),point_pos.y(),point_pos.z());
			glVertex3d(point_pos.x(),point_pos.y(),0.0);
			glVertex3d(point_pos.x(),0.0          ,0.0);
			glVertex3d(0.0          ,0.0          ,0.0);
			glVertex3d(0.0          ,point_pos.y(),0.0);
			glVertex3d(point_pos.x(),point_pos.y(),0.0);
			break;
		case 1:
			glVertex3d(point_pos.x(),point_pos.y(),point_pos.z());
			glVertex3d(point_pos.x(),0.0          ,point_pos.z());
			glVertex3d(0.0          ,0.0          ,point_pos.z());
			glVertex3d(0.0          ,0.0          ,0.0);
			glVertex3d(point_pos.x(),0.0          ,0.0);
			glVertex3d(point_pos.x(),0.0          ,point_pos.z());
			break;
		case 2:
			glVertex3d(point_pos.x(),point_pos.y(),point_pos.z());
			glVertex3d(0.0          ,point_pos.y(),point_pos.z());
			glVertex3d(0.0          ,0.0          ,point_pos.z());
			glVertex3d(0.0          ,0.0          ,0.0);
			glVertex3d(0.0          ,point_pos.y(),0.0);
			glVertex3d(0.0          ,point_pos.y(),point_pos.z());
			break;
		}

		glEnd();


		auto nom=point_dlg1->GetNormalDirection();

		if(POINT_Y==point_dragging)
		{
			glPushAttrib(GL_ENABLE_BIT);
			glLineWidth(1);
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1,0x0003);
			glBegin(GL_LINES);
			glVertex3dv((point_pos-nom*infiniteLongLine).GetValue());
			glVertex3dv((point_pos+nom*infiniteLongLine).GetValue());
			glEnd();
			glPopAttrib();
		}

		if(YSTRUE==point_dlg1->ShowVerticalHandle())
		{
			glLineWidth(float(linePickingTolerance));
			DrawLineByPixelLength(point_pos, nom,pickerLength);
			DrawLineByPixelLength(point_pos,-nom,pickerLength);
			glLineWidth(float(1));
		}

		DrawRectMarker(point_pos,markerSize);
	}
	else if(type==IFTYPE_DRAGPOINT)
	{
		glColor3d(0.0,1.0,0.0);

		glPointSize(float(markerSize));
		glBegin(GL_POINTS);
		glVertex3dv(point_pos.GetValue());
		glEnd();
		glPointSize(1);


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


		int i;
		for(i=0; i<3; i++)
		{
			if(point_dragDirSwitch[i]==YSTRUE)
			{
				glLineWidth(float(linePickingTolerance));
				DrawLineByPixelLength(point_pos, dragDir[i],pickerLength);
				DrawLineByPixelLength(point_pos,-dragDir[i],pickerLength);
			}
			if(dragging==YSTRUE && point_dragging==pointDragTarget[i])
			{
				glPushAttrib(GL_ENABLE_BIT);
				glLineWidth(1);
				glEnable(GL_LINE_STIPPLE);
				glLineStipple(1,0x0003);
				glBegin(GL_LINES);
				glVertex3dv((point_pos-dragDir[i]*infiniteLongLine).GetValue());
				glVertex3dv((point_pos+dragDir[i]*infiniteLongLine).GetValue());
				glEnd();
				glPopAttrib();
			}
		}

		glLineWidth(1);
	}
	else if(type==IFTYPE_SLIDER)
	{
	}
	else if(type==IFTYPE_ROTATION)
	{
		if(dragging==YSTRUE)
		{
			glColor3d(1.0,0.0,5.0);
		}
		else
		{
			glColor3d(0.0,1.0,0.0);
		}


		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex3dv((rot_center+rot_axis*infiniteLongLine).GetValue());
		glVertex3dv((rot_center-rot_axis*infiniteLongLine).GetValue());
		glEnd();
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

		glBegin(GL_LINES);
		for(int i=0; i<360; i+=30)
		{
			YsVec3 p[2];
			double a;
			a=YsPi*2.0*double(i)/360.0;
			p[0]=rot_center+iVec*cos(a)+jVec*1.0+kVec*sin(a);
			p[1]=rot_center+iVec*cos(a)-jVec*1.0+kVec*sin(a);

			glVertex3dv(p[0].GetValue());
			glVertex3dv(p[1].GetValue());
		}
		glEnd();

		glBegin(GL_LINE_LOOP);
		for(int i=0; i<360; i+=10)
		{
			double a;
			YsVec3 p;
			a=YsPi*2.0*double(i)/360.0;
			p=rot_center+iVec*cos(a)+jVec*1.0+kVec*sin(a);
			glVertex3dv(p.GetValue());
		}
		glEnd();

		glBegin(GL_LINE_LOOP);
		for(int i=0; i<360; i+=10)
		{
			double a;
			YsVec3 p;
			a=YsPi*2.0*double(i)/360.0;
			p=rot_center+iVec*cos(a)-jVec*1.0+kVec*sin(a);
			glVertex3dv(p.GetValue());
		}
		glEnd();


		// Side drag marker

		const YsVec3 sideX=rot_drag_dir*rot_ref_size*1.1;
		const YsVec3 sideY=(rot_drag_dir^rot_axis)*rot_ref_size*1.1;

		glLineWidth(3);

		glBegin(GL_LINE_LOOP);
		for(int i=0; i<360; i+=10)
		{
			const double a=YsPi*2.0*double(i)/360.0;
			YsVec3 p;
			p=rot_center+sideX*cos(a)+sideY*sin(a);
			glVertex3dv(p);
		}
		glEnd();


		if(ROTATION_HANDLE==rot_dragging)
		{
			glBegin(GL_LINES);
			glVertex3dv(rot_center);

			const YsVec3 radial=rot_ref_size*1.1*(rot_iVec*cos(rot_drag_angle)-rot_kVec*sin(rot_drag_angle));
			glVertex3dv(rot_center+radial);

			glEnd();
		}

		glLineWidth(1);
	}
}

void FsGui3DInterface::DrawLineByPixelLength(const YsVec3 &o,const YsVec3 &v,const double lng) const
{
	YsVec3 p1,p2,scrnP1,scrnP2;
	CalculateLineEndPointForSpecificPixelLength(p1,p2,scrnP1,scrnP2,o,v,lng);

	glBegin(GL_LINES);
	glVertex3dv(p1);
	glVertex3dv(p2);
	glEnd();
}

void FsGui3DInterface::DrawRectMarker(const YsVec3 &o,const double lng) const
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

	glBegin(GL_LINE_LOOP);
	glVertex3dv(quad[0]);
	glVertex3dv(quad[1]);
	glVertex3dv(quad[2]);
	glVertex3dv(quad[3]);
	glEnd();
}

