/* ////////////////////////////////////////////////////////////

File Name: fsgui3d.cpp
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

#include <fsgui.h>
#include <fsguiclipboard.h>
#include "fsgui3d.h"



FsGui3DInputCoordDialog1::FsGui3DInputCoordDialog1(class FsGui3DInterface *parent)
{
	parentInterface=parent;
}

void FsGui3DInputCoordDialog1::MakeDialog(void)
{
	SetIdent("inputPoint");
	SetTopLeftCorner(0,32);

	xCoord=AddTextBox(MakeIdent("x"),FSKEY_NULL,"","0.0",8,YSFALSE);
	xCoord->SetTextType(FSGUI_ASCIISTRING);
	yCoord=AddTextBox(MakeIdent("y"),FSKEY_NULL,"","0.0",8,YSFALSE);
	yCoord->SetTextType(FSGUI_ASCIISTRING);
	zCoord=AddTextBox(MakeIdent("z"),FSKEY_NULL,"","0.0",8,YSFALSE);
	zCoord->SetTextType(FSGUI_ASCIISTRING);

	absoluteBtn=AddTextButton(MakeIdent("absolute"),FSKEY_NULL,FSGUI_RADIOBUTTON,"Abs",YSFALSE);
	relativeBtn=AddTextButton(MakeIdent("relative"),FSKEY_NULL,FSGUI_RADIOBUTTON,"Rel",YSFALSE);

	absoluteBtn->SetCheck(YSTRUE);

	setBtn=AddTextButton(MakeIdent("set"),FSKEY_NULL,FSGUI_PUSHBUTTON,"Set",YSFALSE);

	xyPln=AddTextButton(MakeIdent("xyPlane"),FSKEY_NULL,FSGUI_RADIOBUTTON,"XY-Pln",YSFALSE);
	xzPln=AddTextButton(MakeIdent("xzPlane"),FSKEY_NULL,FSGUI_RADIOBUTTON,"XZ-Pln",YSFALSE);
	yzPln=AddTextButton(MakeIdent("yzPlane"),FSKEY_NULL,FSGUI_RADIOBUTTON,"YZ-Pln",YSFALSE);

	copyBtn=AddTextButton(MakeIdent("copy"),FSKEY_NULL,FSGUI_PUSHBUTTON,"Copy",YSFALSE);
	pasteBtn=AddTextButton(MakeIdent("paste"),FSKEY_NULL,FSGUI_PUSHBUTTON,"Paste",YSFALSE);

	verticalHandleBtn=AddTextButton(MakeIdent("vertHandle"),FSKEY_NULL,FSGUI_CHECKBOX,"Vertical Handle",YSFALSE);

	xzPln->SetCheck(YSTRUE);

	SetArrangeType(FSDIALOG_ARRANGE_BOTTOM_LEFT);

	FsGuiButton *radio[3];
	radio[0]=xyPln;
	radio[1]=xzPln;
	radio[2]=yzPln;
	SetRadioButtonGroup(3,radio);

	radio[0]=absoluteBtn;
	radio[1]=relativeBtn;
	SetRadioButtonGroup(2,radio);

	Fit();
}

void FsGui3DInputCoordDialog1::SetCoord(const double &x,const double &y,const double &z)
{
	if(ShowRelativeCoord()!=YSTRUE)
	{
		xCoord->SetRealNumber(x,6);
		yCoord->SetRealNumber(y,6);
		zCoord->SetRealNumber(z,6);
	}
	else
	{
		xCoord->SetRealNumber(x-parentInterface->point_pos_org.x(),6);
		yCoord->SetRealNumber(y-parentInterface->point_pos_org.y(),6);
		zCoord->SetRealNumber(z-parentInterface->point_pos_org.z(),6);
	}
}

int FsGui3DInputCoordDialog1::GetBasePlane(void) const
{
	if(YSTRUE==xyPln->GetCheck())
	{
		return 0;
	}
	else if(YSTRUE==xzPln->GetCheck())
	{
		return 1;
	}
	else if(YSTRUE==yzPln->GetCheck())
	{
		return 2;
	}
	return 0;
}

void FsGui3DInputCoordDialog1::SetBasePlane(int pln)
{
	if(nullptr!=this->xyPln)
	{
		this->xyPln->SetCheck(YSFALSE);
	}
	if(nullptr!=this->xzPln)
	{
		this->xzPln->SetCheck(YSFALSE);
	}
	if(nullptr!=this->yzPln)
	{
		this->yzPln->SetCheck(YSFALSE);
	}
	switch(pln)
	{
	case 0:
		this->xyPln->SetCheck(YSTRUE);
		break;
	case 1:
		this->xzPln->SetCheck(YSTRUE);
		break;
	case 2:
		this->yzPln->SetCheck(YSTRUE);
		break;
	}
}

void FsGui3DInputCoordDialog1::DisableBasePlaneButton(void)
{
	if(nullptr!=this->xyPln)
	{
		this->xyPln->Disable();
	}
	if(nullptr!=this->xzPln)
	{
		this->xzPln->Disable();
	}
	if(nullptr!=this->yzPln)
	{
		this->yzPln->Disable();
	}
}
void FsGui3DInputCoordDialog1::EnableBasePlaneButton(void)
{
	if(nullptr!=this->xyPln)
	{
		this->xyPln->Enable();
	}
	if(nullptr!=this->xzPln)
	{
		this->xzPln->Enable();
	}
	if(nullptr!=this->yzPln)
	{
		this->yzPln->Enable();
	}
}

YsVec3 FsGui3DInputCoordDialog1::GetNormalDirection(void) const
{
	switch(GetBasePlane())
	{
	case 0:
		return YsZVec();
	case 1:
		return YsYVec();
	default:
	case 2:
		return YsXVec();
	}
}

YSBOOL FsGui3DInputCoordDialog1::ShowRelativeCoord(void) const
{
	return relativeBtn->GetCheck();
}

void FsGui3DInputCoordDialog1::SetVerticalHandle(YSBOOL flg)
{
	verticalHandleBtn->SetCheck(flg);
}
YSBOOL FsGui3DInputCoordDialog1::ShowVerticalHandle(void) const
{
	return verticalHandleBtn->GetCheck();
}

void FsGui3DInputCoordDialog1::OnButtonClick(FsGuiButton *btn)
{
	if(btn==setBtn)
	{
		double x,y,z;
		x=xCoord->GetRealNumber();
		y=yCoord->GetRealNumber();
		z=zCoord->GetRealNumber();
		parentInterface->point_pos.Set(x,y,z);
		if(YSTRUE==ShowRelativeCoord())
		{
			parentInterface->point_pos+=parentInterface->point_pos_org;
		}
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==copyBtn)
	{
		YsString fmt;
		fmt.Printf("%.10lf, %.10lf, %.10lf",
		    parentInterface->point_pos.x(),
		    parentInterface->point_pos.y(),
		    parentInterface->point_pos.z());
		FsGuiCopyStringToClipBoard(fmt);
	}
	else if(btn==pasteBtn)
	{
		YsString clip;
		FsGuiCopyStringFromClipBoard(clip);

		for(YSSIZE_T i=0; i<clip.Strlen(); ++i)
		{
			if(clip[i]==',' || clip[i]=='(' || clip[i]==')')
			{
				clip.Set(i,' ');
			}
		}

		YsArray <YsString> args;
		clip.Arguments(args);
		if(3<=args.GetN())
		{
			const YsVec3 newPos(args[0],args[1],args[2]);
			if(newPos!=parentInterface->point_pos)
			{
				parentInterface->point_pos=newPos;
				parentInterface->SetNeedRedraw(YSTRUE);
				parentInterface->InvokeCallBack();
				SetCoord(newPos.x(),newPos.y(),newPos.z());
			}
		}
	}
	else if(btn==absoluteBtn)
	{
		SetCoord(parentInterface->point_pos.x(),parentInterface->point_pos.y(),parentInterface->point_pos.z());
	}
	else if(btn==relativeBtn)
	{
		SetCoord(parentInterface->point_pos.x(),parentInterface->point_pos.y(),parentInterface->point_pos.z());
	}
}

////////////////////////////////////////////////////////////

FsGui3DInputCoordDialog2::FsGui3DInputCoordDialog2(class FsGui3DInterface *parent)
{
	parentInterface=parent;
}

void FsGui3DInputCoordDialog2::MakeDialog(void)
{
	SetIdent("inputPoint");
	SetTopLeftCorner(0,32);

	xCoord=AddTextBox(MakeIdent("x"),FSKEY_NULL,"","0.0",8,YSFALSE);
	xCoord->SetTextType(FSGUI_ASCIISTRING);
	yCoord=AddTextBox(MakeIdent("y"),FSKEY_NULL,"","0.0",8,YSFALSE);
	yCoord->SetTextType(FSGUI_ASCIISTRING);
	zCoord=AddTextBox(MakeIdent("z"),FSKEY_NULL,"","0.0",8,YSFALSE);
	zCoord->SetTextType(FSGUI_ASCIISTRING);

	absoluteBtn=AddTextButton(MakeIdent("absolute"),FSKEY_NULL,FSGUI_RADIOBUTTON,"Abs",YSFALSE);
	relativeBtn=AddTextButton(MakeIdent("relative"),FSKEY_NULL,FSGUI_RADIOBUTTON,"Rel",YSFALSE);

	absoluteBtn->SetCheck(YSTRUE);

	setBtn=AddTextButton(MakeIdent("set"),FSKEY_NULL,FSGUI_PUSHBUTTON,"Set",YSFALSE);

	customOrientationBtn=AddTextButton(MakeIdent("customOrientation"),FSKEY_NULL,FSGUI_CHECKBOX,"Custon Orientation",YSFALSE);

	copyBtn=AddTextButton(MakeIdent("copy"),FSKEY_NULL,FSGUI_PUSHBUTTON,"Copy",YSFALSE);
	pasteBtn=AddTextButton(MakeIdent("paste"),FSKEY_NULL,FSGUI_PUSHBUTTON,"Paste",YSFALSE);

	SetArrangeType(FSDIALOG_ARRANGE_BOTTOM_LEFT);

	FsGuiButton *radio[2];
	radio[0]=absoluteBtn;
	radio[1]=relativeBtn;
	SetRadioButtonGroup(2,radio);

	Fit();
}

void FsGui3DInputCoordDialog2::SetCoord(const double &x,const double &y,const double &z)
{
	if(ShowRelativeCoord()!=YSTRUE)
	{
		xCoord->SetRealNumber(x,6);
		yCoord->SetRealNumber(y,6);
		zCoord->SetRealNumber(z,6);
	}
	else
	{
		xCoord->SetRealNumber(x-parentInterface->point_pos_org.x(),6);
		yCoord->SetRealNumber(y-parentInterface->point_pos_org.y(),6);
		zCoord->SetRealNumber(z-parentInterface->point_pos_org.z(),6);
	}
}

YSBOOL FsGui3DInputCoordDialog2::UseCustomOrientation(void)
{
	return customOrientationBtn->GetCheck();
}

YSBOOL FsGui3DInputCoordDialog2::ShowRelativeCoord(void) const
{
	return relativeBtn->GetCheck();
}

void FsGui3DInputCoordDialog2::SetUseCustomOrientation(YSBOOL sw)
{
	customOrientationBtn->SetCheck(sw);
}

void FsGui3DInputCoordDialog2::EnableCustomOrientationButton(YSBOOL sw)
{
	switch(sw)
	{
	default:
	case YSTRUE:
		customOrientationBtn->Enable();
		break;
	case YSFALSE:
		customOrientationBtn->Disable();
		break;
	}
}

void FsGui3DInputCoordDialog2::OnButtonClick(FsGuiButton *btn)
{
	if(btn==setBtn)
	{
		double x,y,z;
		x=xCoord->GetRealNumber();
		y=yCoord->GetRealNumber();
		z=zCoord->GetRealNumber();
		parentInterface->point_pos.Set(x,y,z);
		if(YSTRUE==ShowRelativeCoord())
		{
			parentInterface->point_pos+=parentInterface->point_pos_org;
		}
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==copyBtn)
	{
		YsString fmt;
		fmt.Printf("%.10lf, %.10lf, %.10lf",
		    parentInterface->point_pos.x(),
		    parentInterface->point_pos.y(),
		    parentInterface->point_pos.z());
		FsGuiCopyStringToClipBoard(fmt);
	}
	else if(btn==pasteBtn)
	{
		YsString clip;
		FsGuiCopyStringFromClipBoard(clip);

		for(YSSIZE_T i=0; i<clip.Strlen(); ++i)
		{
			if(clip[i]==',' || clip[i]=='(' || clip[i]==')')
			{
				clip.Set(i,' ');
			}
		}

		YsArray <YsString> args;
		clip.Arguments(args);
		if(3<=args.GetN())
		{
			const YsVec3 newPos(args[0],args[1],args[2]);
			if(newPos!=parentInterface->point_pos)
			{
				parentInterface->point_pos=newPos;
				parentInterface->SetNeedRedraw(YSTRUE);
				parentInterface->InvokeCallBack();
				SetCoord(newPos.x(),newPos.y(),newPos.z());
			}
		}
	}
	else if(btn==absoluteBtn)
	{
		SetCoord(parentInterface->point_pos.x(),parentInterface->point_pos.y(),parentInterface->point_pos.z());
	}
	else if(btn==relativeBtn)
	{
		SetCoord(parentInterface->point_pos.x(),parentInterface->point_pos.y(),parentInterface->point_pos.z());
	}
}

////////////////////////////////////////////////////////////

FsGui3DInputBoxDialog::FsGui3DInputBoxDialog(class FsGui3DInterface *parent)
{
	parentInterface=parent;
}

void FsGui3DInputBoxDialog::MakeDialog(void)
{
	SetTopLeftCorner(0,32);

	xCoord1=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	xCoord1->SetTextType(FSGUI_ASCIISTRING);
	yCoord1=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	yCoord1->SetTextType(FSGUI_ASCIISTRING);
	zCoord1=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	zCoord1->SetTextType(FSGUI_ASCIISTRING);
	centerBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Center",YSFALSE);
	cornerBtn1=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Corner",YSFALSE);

	cornerBtn1->SetCheck(YSTRUE);

	maintainAspectBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,"Maintain Aspect Ration",YSFALSE);
	maintainAspectBtn->Disable();

	xCoord2=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSTRUE);
	xCoord2->SetTextType(FSGUI_ASCIISTRING);
	yCoord2=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	yCoord2->SetTextType(FSGUI_ASCIISTRING);
	zCoord2=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	zCoord2->SetTextType(FSGUI_ASCIISTRING);
	sizeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Size",YSFALSE);
	cornerBtn2=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Corner",YSFALSE);

	cornerBtn2->SetCheck(YSTRUE);

	setBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Set",YSFALSE);

	SetArrangeType(FSDIALOG_ARRANGE_BOTTOM_LEFT);

	FsGuiButton *radio[2];
	radio[0]=centerBtn;
	radio[1]=cornerBtn1;
	SetRadioButtonGroup(2,radio);

	radio[0]=sizeBtn;
	radio[1]=cornerBtn2;
	SetRadioButtonGroup(2,radio);

	Fit();
}

void FsGui3DInputBoxDialog::SetCoord(const YsVec3 box[2])
{
	YsVec3 p[2];
	YsBoundingBoxMaker3 bbx;

	bbx.Begin(box[0]);
	bbx.Add(box[1]);
	bbx.Get(p[0],p[1]);

	if(YSTRUE==cornerBtn1->GetCheck() && YSTRUE==cornerBtn2->GetCheck())
	{
		// As it is
	}
	else if(YSTRUE!=cornerBtn1->GetCheck() && YSTRUE==cornerBtn2->GetCheck())
	{
		p[0]=(p[0]+p[1])/2.0;
	}
	else if(YSTRUE==cornerBtn1->GetCheck() && YSTRUE!=cornerBtn2->GetCheck())
	{
		p[1]=(p[1]-p[0]);
	}
	else
	{
		p[0]=(p[0]+p[1])/2.0;
		p[1]=(p[1]-p[0])*2.0;
	}

	char xstr[256],ystr[256],zstr[256];
	YsOptimizeDoubleString(xstr,p[0].x());
	YsOptimizeDoubleString(ystr,p[0].y());
	YsOptimizeDoubleString(zstr,p[0].z());
	xCoord1->SetText(xstr);
	yCoord1->SetText(ystr);
	zCoord1->SetText(zstr);

	YsOptimizeDoubleString(xstr,p[1].x());
	YsOptimizeDoubleString(ystr,p[1].y());
	YsOptimizeDoubleString(zstr,p[1].z());
	xCoord2->SetText(xstr);
	yCoord2->SetText(ystr);
	zCoord2->SetText(zstr);
}

void FsGui3DInputBoxDialog::SetMaintainAspectRatio(YSBOOL maintainAspect)
{
	maintainAspectBtn->SetCheck(maintainAspect);
}

void FsGui3DInputBoxDialog::EnableMaintainAspectRatioButton(void)
{
	maintainAspectBtn->Enable();
}
void FsGui3DInputBoxDialog::DisableMaintainAspectRatioButton(void)
{
	maintainAspectBtn->Disable();
}

void FsGui3DInputBoxDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==centerBtn || btn==cornerBtn1 || btn==sizeBtn || btn==cornerBtn2)
	{
		SetCoord(parentInterface->box_pos);
		parentInterface->InvokeCallBack();
	}
	else if(btn==maintainAspectBtn)
	{
		parentInterface->box_maintainAspectRatio=btn->GetCheck();
	}
	else if(btn==setBtn)
	{
		YsBoundingBoxMaker3 bbx;
		YsVec3 p[2];

		p[0].Set(xCoord1->GetRealNumber(),yCoord1->GetRealNumber(),zCoord1->GetRealNumber());
		p[1].Set(xCoord2->GetRealNumber(),yCoord2->GetRealNumber(),zCoord2->GetRealNumber());

		if(YSTRUE==cornerBtn1->GetCheck() && YSTRUE==cornerBtn2->GetCheck())
		{
			// As it is
		}
		else if(YSTRUE!=cornerBtn1->GetCheck() && YSTRUE==cornerBtn2->GetCheck())
		{
			p[0]=p[0]+(p[0]-p[1]);
		}
		else if(YSTRUE==cornerBtn1->GetCheck() && YSTRUE!=cornerBtn2->GetCheck())
		{
			p[1]=p[0]+p[1];
		}
		else
		{
			p[0]=p[0]-p[1]/2.0;
			p[1]=p[0]+p[1];
		}

		bbx.Begin(p[0]);
		bbx.Add(p[1]);
		bbx.Get(p[0],p[1]);

		if(parentInterface->box_maintainAspectRatio==YSTRUE)
		{
			int i,element;
			YsVec3 cen,orgSize,p_org[2];
			double resizeRatio,num,denom;

			element=0;
			for(i=1; i<3; i++)
			{
				if(p[1].GetValue()[i]-p[0].GetValue()[i]>
				   p[1].GetValue()[element]-p[0].GetValue()[element])
				{
					element=i;
				}
			}

			p_org[0]=parentInterface->box_pos_org[0];
			p_org[1]=parentInterface->box_pos_org[1];

			num=p[1].GetValue()[element]-p[0].GetValue()[element];
			denom=p_org[1].GetValue()[element]-p_org[0].GetValue()[element];
			resizeRatio=YsAbs(num/denom);

			cen=(p[1]+p[0])/2.0;
			orgSize=p_org[1]-p_org[0];

			double newValue[2][3];
			for(i=0; i<3; i++)
			{
				if(i!=element)
				{
					newValue[0][i]=cen.GetValue()[i]-(orgSize.GetValue()[i]/2.0)*resizeRatio;
					newValue[1][i]=cen.GetValue()[i]+(orgSize.GetValue()[i]/2.0)*resizeRatio;
				}
				else
				{
					newValue[0][i]=p[0].GetValue()[i];
					newValue[1][i]=p[1].GetValue()[i];
				}
			}
			p[0].Set(newValue[0]);
			p[1].Set(newValue[1]);
		}

		parentInterface->box_pos[0]=p[0];
		parentInterface->box_pos[1]=p[1];

		parentInterface->InvokeCallBack();
		parentInterface->SetNeedRedraw(YSTRUE);
	}
}

////////////////////////////////////////////////////////////

FsGui3DRotationDialog::FsGui3DRotationDialog(class FsGui3DInterface *parent)
{
	parentInterface=parent;
}

void FsGui3DRotationDialog::MakeDialog(void)
{
	SetTopLeftCorner(0,32);

	axisX=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	axisX->SetTextType(FSGUI_ASCIISTRING);
	axisY=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	axisY->SetTextType(FSGUI_ASCIISTRING);
	axisZ=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	axisZ->SetTextType(FSGUI_ASCIISTRING);
	setXAxisBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"XAxis",YSFALSE);
	setYAxisBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"YAxis",YSFALSE);
	setZAxisBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"ZAxis",YSFALSE);
	setAxisBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Set Axis",YSFALSE);

	centerX=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSTRUE);
	centerX->SetTextType(FSGUI_ASCIISTRING);
	centerY=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	centerY->SetTextType(FSGUI_ASCIISTRING);
	centerZ=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSFALSE);
	centerZ->SetTextType(FSGUI_ASCIISTRING);
	setCenterBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Set Center",YSFALSE);

	angle=AddTextBox(0,FSKEY_NULL,"","0.0",8,YSTRUE);
	angle->SetTextType(FSGUI_ASCIISTRING);
	setAngleBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Set Angle",YSFALSE);
	degreeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Degree",YSFALSE);
	radianBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,"Radian",YSFALSE);
	mirrorBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Mirror",YSFALSE);

	degreeBtn->SetCheck(YSTRUE);

	deg90Btn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"90 Deg",YSTRUE);
	deg180Btn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"180 Deg",YSFALSE);
	deg270Btn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"270 Deg",YSFALSE);
	deg0Btn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"0 Deg",YSFALSE);
	plus1DegBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"+1 Deg",YSFALSE);
	minus1DegBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"-1 Deg",YSFALSE);
	plus5DegBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"+5 Deg",YSFALSE);
	minus5DegBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"-5 Deg",YSFALSE);

	FsGuiButton *radio[2];
	radio[0]=degreeBtn;
	radio[1]=radianBtn;
	SetRadioButtonGroup(2,radio);

	SetArrangeType(FSDIALOG_ARRANGE_BOTTOM_LEFT);

	Fit();
}

void FsGui3DRotationDialog::SetAngle(const double &newAngle)
{
	char str[256];
	parentInterface->rot_angle=newAngle;
	if(YSTRUE==degreeBtn->GetCheck())
	{
		YsOptimizeDoubleString(str,YsRadToDeg(newAngle));
	}
	else
	{
		YsOptimizeDoubleString(str,newAngle);
	}
	angle->SetText(str);
}

void FsGui3DRotationDialog::SetRotation(const YsVec3 &axis,const double &angle,const YsVec3 &center)
{
	char str[256];

	YsOptimizeDoubleString(str,axis.x());
	axisX->SetText(str);
	YsOptimizeDoubleString(str,axis.y());
	axisY->SetText(str);
	YsOptimizeDoubleString(str,axis.z());
	axisZ->SetText(str);

	YsOptimizeDoubleString(str,center.x());
	centerX->SetText(str);
	YsOptimizeDoubleString(str,center.y());
	centerY->SetText(str);
	YsOptimizeDoubleString(str,center.z());
	centerZ->SetText(str);

	if(YSTRUE==degreeBtn->GetCheck())
	{
		YsOptimizeDoubleString(str,YsRadToDeg(angle));
	}
	else
	{
		YsOptimizeDoubleString(str,angle/YsPi);
	}
	this->angle->SetText(str);
}

void FsGui3DRotationDialog::AllowChangeAxis(YSBOOL allowMoveChangeAxis)
{
	switch(allowMoveChangeAxis)
	{
	default:
	case YSTRUE:
		axisX->Enable();
		axisY->Enable();
		axisZ->Enable();
		setXAxisBtn->Enable();
		setYAxisBtn->Enable();
		setZAxisBtn->Enable();
		setAxisBtn->Enable();
		break;
	case YSFALSE:
		axisX->Disable();
		axisY->Disable();
		axisZ->Disable();
		setXAxisBtn->Disable();
		setYAxisBtn->Disable();
		setZAxisBtn->Disable();
		setAxisBtn->Disable();
		break;
	}
}

void FsGui3DRotationDialog::AllowMoveCenter(YSBOOL allowMoveCenter)
{
	switch(allowMoveCenter)
	{
	default:
	case YSTRUE:
		setCenterBtn->Enable();
		centerX->Enable();
		centerY->Enable();
		centerZ->Enable();
		break;
	case YSFALSE:
		setCenterBtn->Disable();
		centerX->Disable();
		centerY->Disable();
		centerZ->Disable();
		break;
	}
}

void FsGui3DRotationDialog::AllowMirror(YSBOOL allowMirror)
{
	switch(allowMirror)
	{
	default:
	case YSTRUE:
		mirrorBtn->Enable();
		break;
	case YSFALSE:
		mirrorBtn->Disable();
		break;
	}
}

void FsGui3DRotationDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==setXAxisBtn)
	{
		parentInterface->SetRotationAxis(YsXVec());
		parentInterface->rot_drag_dir=YsYVec();
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==setYAxisBtn)
	{
		parentInterface->SetRotationAxis(YsYVec());
		parentInterface->rot_drag_dir=YsXVec();
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==setZAxisBtn)
	{
		parentInterface->SetRotationAxis(YsZVec());
		parentInterface->rot_drag_dir=YsXVec();
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==setAxisBtn)
	{
		YsVec3 axis;
		double x,y,z;
		x=axisX->GetRealNumber();
		y=axisY->GetRealNumber();
		z=axisZ->GetRealNumber();
		axis.Set(x,y,z);
		axis.Normalize();
		parentInterface->SetRotationAxis(axis);
		parentInterface->rot_drag_dir=axis.GetArbitraryPerpendicularVector();
		parentInterface->rot_drag_dir.Normalize();
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==setCenterBtn)
	{
		double x,y,z;
		x=centerX->GetRealNumber();
		y=centerY->GetRealNumber();
		z=centerZ->GetRealNumber();
		parentInterface->rot_center.Set(x,y,z);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==mirrorBtn)
	{
		parentInterface->rot_mirror*=-1.0;
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==degreeBtn)
	{
		SetAngle(parentInterface->rot_angle);
		parentInterface->InvokeCallBack();
	}
	else if(btn==radianBtn)
	{
		SetAngle(parentInterface->rot_angle);
		parentInterface->InvokeCallBack();
	}
	else if(btn==setAngleBtn)
	{
		double angle;
		angle=this->angle->GetRealNumber();
		if(YSTRUE==degreeBtn->GetCheck())
		{
			angle=YsDegToRad(angle);
		}
		else
		{
			angle=angle*YsPi;
		}
		parentInterface->rot_angle=angle;
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==deg90Btn)
	{
		SetAngle(YsPi/2.0);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==deg180Btn)
	{
		SetAngle(YsPi);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==deg270Btn)
	{
		SetAngle(YsPi*3.0/2.0);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==deg0Btn)
	{
		SetAngle(0.0);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==plus1DegBtn)
	{
		SetAngle(parentInterface->rot_angle+YsPi/180.0);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==minus1DegBtn)
	{
		SetAngle(parentInterface->rot_angle-YsPi/180.0);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==plus5DegBtn)
	{
		SetAngle(parentInterface->rot_angle+YsPi/36.0);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
	else if(btn==minus5DegBtn)
	{
		SetAngle(parentInterface->rot_angle-YsPi/36.0);
		parentInterface->SetNeedRedraw(YSTRUE);
		parentInterface->InvokeCallBack();
	}
}

////////////////////////////////////////////////////////////


FsGui3DInterface::FsGui3DInterface(class FsGuiCanvas *canvas)
{
	type=IFTYPE_NONE;
	dragging=YSFALSE;
	point_pos.Set(0.0,0.0,0.0);
	box_pos[0].Set(0.0,0.0,0.0);
	box_pos[1].Set(0.0,0.0,0.0);

	infiniteLongLine=1000.0;
	ofstX=0;
	ofstY=0;

	linePickingTolerance=3.0;
	pickerLength=32.0;
	markerSize=8.0;

	this->canvas=canvas;
	viewport[0]=0;
	viewport[1]=0;
	viewport[1]=10;
	viewport[1]=10;

	point_dlg1=new FsGui3DInputCoordDialog1(this);
	point_dlg1->MakeDialog();

	point_dlg2=new FsGui3DInputCoordDialog2(this);
	point_dlg2->MakeDialog();

	box_dlg=new FsGui3DInputBoxDialog(this);
	box_dlg->MakeDialog();

	rot_dlg=new FsGui3DRotationDialog(this);
	rot_dlg->MakeDialog();

	ChangeCallBack=NULL;
	contextPtr=NULL;
}

FsGui3DInterface::~FsGui3DInterface()
{
	delete point_dlg1;
	delete point_dlg2;
	delete box_dlg;
	delete rot_dlg;
}

void FsGui3DInterface::Initialize(void)
{
	dragging=YSFALSE;
	type=IFTYPE_NONE;
	canvas->RemoveDialog(point_dlg1);
	canvas->RemoveDialog(point_dlg2);
	canvas->RemoveDialog(box_dlg);
	canvas->RemoveDialog(rot_dlg);
	canvas->ArrangeDialog();

	ClearCallBack();
}

FsGui3DInterface::INTERFACETYPE FsGui3DInterface::GetInterfaceType(void) const
{
	return type;
}

YSRESULT FsGui3DInterface::CalculateLineEndPointForSpecificPixelLength(
    YsVec3 &p1,YsVec3 &p2,YsVec3 &scrnP1,YsVec3 &scrnP2,
    const YsVec3 &o,const YsVec3 &v,const double lng) const
{
	YsTransform3DCoordToScreenCoord(scrnP1,o  ,viewport,projViewModelTfm);
	YsTransform3DCoordToScreenCoord(scrnP2,o+v,viewport,projViewModelTfm);

	const YsVec3 scrnD=scrnP2-scrnP1;

	const double scrnLng=sqrt(scrnD.x()*scrnD.x()+scrnD.y()*scrnD.y());
	if(YsTolerance<scrnLng)
	{
		scrnP2=scrnP1+scrnD*lng/scrnLng;

		p1=o;
		YsTransformScreenCoordTo3DCoord(p2,scrnP2,viewport,projViewModelTfm);
		return YSOK;
	}
	return YSERR;
}

YSBOOL FsGui3DInterface::Is3DLinePicked(const YsVec3 &o,const YsVec3 &v,const double scrnLng,int glMx,int glMy) const
{
	YsVec3 p1,p2,scrnP1,scrnP2;
	if(YSOK==CalculateLineEndPointForSpecificPixelLength(p1,p2,scrnP1,scrnP2,o,v,scrnLng))
	{
		YsVec2 a(scrnP1.x(),scrnP1.y()),b(scrnP2.x(),scrnP2.y());
		YsVec2 m((double)glMx,(double)glMy);

		if(YSTRUE==YsCheckInBetween2(m,a,b))
		{
			const double d=YsGetPointLineDistance2(a,b,m);
			if(d<linePickingTolerance)
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}

YSBOOL FsGui3DInterface::Is3DLinePickedAlsoGetNearestPoint(YsVec3 &nearPos,const YsVec3 &p1,const YsVec3 &p2,int glMx,int glMy) const
{
	YsVec3 mouseP1((double)glMx,(double)glMy,-1.0),mouseP2((double)glMx,(double)glMy,1.0);
	YsTransformScreenCoordTo3DCoord(mouseP1,mouseP1,viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(mouseP2,mouseP2,viewport,projViewModelTfm);

	YsVec3 nearP[2];
	if(YSOK==YsGetNearestPointOfTwoLine(nearP[0],nearP[1],p1,p2,mouseP1,mouseP2) &&
	   YSTRUE==nearP[0].IsInBetween(p1,p2) &&
	   YSTRUE==nearP[1].IsInBetween(mouseP1,mouseP2))
	{
		nearPos=nearP[0];

		YsVec3 scrnNearP,scrnMos;
		YsTransform3DCoordToScreenCoord(scrnNearP,nearP[0],viewport,projViewModelTfm);
		scrnNearP.SetZ(0.0);
		scrnMos.Set((double)glMx,(double)glMy,0.0);

		if((scrnNearP-scrnMos).GetLength()<linePickingTolerance)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

void FsGui3DInterface::SetViewport(const int viewport[4])
{
	this->viewport[0]=viewport[0];
	this->viewport[1]=viewport[1];
	this->viewport[2]=viewport[2];
	this->viewport[3]=viewport[3];
}

void FsGui3DInterface::SetProjViewModelMatrix(const YsMatrix4x4 &mat)
{
	projViewModelTfm=mat;
}

void FsGui3DInterface::SetNeedRedraw(YSBOOL needRedraw)
{
	this->needRedraw=needRedraw;
}

void FsGui3DInterface::SetCallBack(void (*CallBackFunc)(void *,FsGui3DInterface &),void *contextPtr)
{
	ClearCallBack();
	this->ChangeCallBack=CallBackFunc;
	this->contextPtr=contextPtr;
}

void FsGui3DInterface::ClearCallBack(void)
{
	ChangeCallBack=NULL;
	stdChangeCallBack=nullptr;
}

void FsGui3DInterface::InvokeCallBack(void)
{
	if(true==(bool)stdChangeCallBack)
	{
		stdChangeCallBack();
	}

	if(NULL!=ChangeCallBack)
	{
		(*ChangeCallBack)(contextPtr,*this);
	}
}

YSBOOL FsGui3DInterface::NeedRedraw(void) const
{
	return needRedraw;
}

void FsGui3DInterface::BeginInputBox(
    const YsVec3 &box1,const YsVec3 &box2,YSBOOL allowResize,YSBOOL maintainAspectRatio)
{
	YsBoundingBoxMaker3 bbx;
	bbx.Begin(box1);
	bbx.Add(box2);

	Initialize();
	type=IFTYPE_BOX;
	bbx.Get(box_pos[0],box_pos[1]);
	bbx.Get(box_pos_org[0],box_pos_org[1]);
	box_maintainAspectRatio=maintainAspectRatio;
	box_allowResize=allowResize;

	canvas->RemoveDialog(point_dlg1);
	canvas->RemoveDialog(point_dlg2);
	canvas->AddDialog(box_dlg);
	canvas->RemoveDialog(rot_dlg);
	canvas->ArrangeDialog();

	box_dlg->SetCoord(box_pos);
	box_dlg->DisableMaintainAspectRatioButton();
	box_dlg->SetMaintainAspectRatio(maintainAspectRatio);
}

void FsGui3DInterface::GetBox(YsVec3 bbx[2]) const
{
	bbx[0]=box_pos[0];
	bbx[1]=box_pos[1];
}

void FsGui3DInterface::GetOriginalBox(YsVec3 bbx[2]) const
{
	bbx[0]=box_pos_org[0];
	bbx[1]=box_pos_org[1];
}

void FsGui3DInterface::SetBox(const YsVec3 bbx[2])
{
	box_pos[0]=bbx[0];
	box_pos[1]=bbx[1];
	box_dlg->SetCoord(box_pos);
}

void FsGui3DInterface::EnableMaintainAspectRatioButton(void)
{
	box_dlg->EnableMaintainAspectRatioButton();
}
void FsGui3DInterface::DisableMaintainAspectRatioButton(void)
{
	box_dlg->DisableMaintainAspectRatioButton();
}

void FsGui3DInterface::BeginInputPoint1(const YsVec3 &pnt)
{
	Initialize();
	type=IFTYPE_POINT;
	point_dragging=NONE; // 2015/03/19 Allow left-drag to move Y
	point_pos=pnt;
	point_pos_org=pnt;

	canvas->AddDialog(point_dlg1);
	canvas->RemoveDialog(point_dlg2);
	canvas->RemoveDialog(box_dlg);
	canvas->RemoveDialog(rot_dlg);
	canvas->ArrangeDialog();

	point_dlg1->SetCoord(pnt.x(),pnt.y(),pnt.z());
}

void FsGui3DInterface::SetPoint1BasePlane(int pln)
{
	if(nullptr!=point_dlg1)
	{
		point_dlg1->SetBasePlane(pln);
	}
}
void FsGui3DInterface::EnablePoint1BasePlaneButton(void)
{
	if(nullptr!=point_dlg1)
	{
		point_dlg1->EnableBasePlaneButton();
	}
}
void FsGui3DInterface::DisablePoint1BasePlaneButton(void)
{
	if(nullptr!=point_dlg1)
	{
		point_dlg1->DisableBasePlaneButton();
	}
}

void FsGui3DInterface::BeginInputPoint2(const YsVec3 &pnt,const YSBOOL dirSwitch[3],const YsVec3 dir[3])
{
	Initialize();
	type=IFTYPE_DRAGPOINT;
	point_pos=pnt;
	point_pos_org=pnt;
	point_dragging=NONE;

	if(NULL==dirSwitch)
	{
		point_dragDirSwitch[0]=YSTRUE;
		point_dragDirSwitch[1]=YSTRUE;
		point_dragDirSwitch[2]=YSTRUE;
	}
	else
	{
		for(int i=0; i<3; i++)
		{
			point_dragDirSwitch[i]=dirSwitch[i];
		}
	}

	if(NULL==dir)
	{
		point_dragDir[0]=YsXVec();
		point_dragDir[1]=YsYVec();
		point_dragDir[2]=YsZVec();
	}
	else
	{
		for(int i=0; i<3; i++)
		{
			if(YSTRUE==point_dragDirSwitch[i])
			{
				point_dragDir[i]=dir[i];
			}
		}
	}

	canvas->RemoveDialog(point_dlg1);
	canvas->AddDialog(point_dlg2);
	canvas->RemoveDialog(box_dlg);
	canvas->RemoveDialog(rot_dlg);
	canvas->ArrangeDialog();

	point_dlg2->SetCoord(pnt.x(),pnt.y(),pnt.z());
}

YSRESULT FsGui3DInterface::ChangeInputPoint1to2(YSBOOL dirSwitch[3],const YsVec3 dir[3])
{
	if(IFTYPE_POINT==type)
	{
		type=IFTYPE_DRAGPOINT;
		point_dragging=NONE;

		if(NULL==dirSwitch)
		{
			point_dragDirSwitch[0]=YSTRUE;
			point_dragDirSwitch[1]=YSTRUE;
			point_dragDirSwitch[2]=YSTRUE;
		}
		else
		{
			for(int i=0; i<3; i++)
			{
				point_dragDirSwitch[i]=dirSwitch[i];
			}
		}

		if(NULL==dir)
		{
			point_dragDir[0]=YsXVec();
			point_dragDir[1]=YsYVec();
			point_dragDir[2]=YsZVec();
		}
		else
		{
			for(int i=0; i<3; i++)
			{
				if(YSTRUE==point_dragDirSwitch[i])
				{
					point_dragDir[i]=dir[i];
				}
			}
		}

		canvas->RemoveDialog(point_dlg1);
		canvas->AddDialog(point_dlg2);
		canvas->ArrangeDialog();
		point_dlg2->SetCoord(point_pos.x(),point_pos.y(),point_pos.z());

		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGui3DInterface::ChangeInputPoint2to1(void)
{
	if(IFTYPE_DRAGPOINT==type)
	{
		type=IFTYPE_POINT;
		point_dragging=NONE;

		canvas->RemoveDialog(point_dlg2);
		canvas->AddDialog(point_dlg1);
		canvas->ArrangeDialog();
		point_dlg1->SetCoord(point_pos.x(),point_pos.y(),point_pos.z());

		return YSOK;
	}
	return YSERR;
}

void FsGui3DInterface::SetInputPoint2UseCustomOrientation(YSBOOL sw)
{
	point_dlg2->SetUseCustomOrientation(sw);
}

void FsGui3DInterface::EnableInputPoint2CustomOrientationButton(YSBOOL sw)
{
	point_dlg2->EnableCustomOrientationButton(sw);
}

const YsVec3 &FsGui3DInterface::GetPoint(void) const
{
	return point_pos;
}

const YsVec3 &FsGui3DInterface::GetOriginalPoint(void) const
{
	return point_pos_org;
}

void FsGui3DInterface::BeginInputRotation(
    const YsVec3 &axis,const double &angle,const YsVec3 &center,const double &refSize,
    YSBOOL allowChangeAxis,YSBOOL allowMoveCenter,YSBOOL allowMirror)
{
	Initialize();
	type=IFTYPE_ROTATION;
	SetRotationAxis(axis);
	rot_angle=angle;
	rot_center=center;
	rot_ref_size=refSize;
	rot_mirror=1.0;

	canvas->RemoveDialog(point_dlg1);
	canvas->RemoveDialog(point_dlg2);
	canvas->RemoveDialog(box_dlg);
	canvas->AddDialog(rot_dlg);
	canvas->ArrangeDialog();

	rot_dlg->AllowChangeAxis(allowChangeAxis);
	rot_dlg->AllowMoveCenter(allowMoveCenter);
	rot_dlg->AllowMirror(allowMirror);
	rot_dlg->SetRotation(axis,angle,center);
}

void FsGui3DInterface::SetRotationAxis(const YsVec3 &axis)
{
	rot_axis=axis;
	rot_drag_dir=rot_axis.GetArbitraryPerpendicularVector();

	rot_axis.Normalize();
	rot_drag_dir.Normalize();

	rot_iVec=rot_axis.GetArbitraryPerpendicularVector();
	rot_jVec=rot_axis;
	rot_kVec=rot_iVec^rot_jVec;

	rot_iVec.Normalize();
	rot_jVec.Normalize();
	rot_kVec.Normalize();

}

void FsGui3DInterface::GetRotationMatrix(YsMatrix4x4 &mat) const
{
	YsMatrix4x4 axisMat;
	YsAtt3 att;

	mat.Initialize();

	att.SetForwardVector(rot_axis);
	att.GetMatrix4x4(axisMat);
	mat.Rotate(rot_axis.x(),rot_axis.y(),rot_axis.z(),rot_angle);
	mat=mat*axisMat;
	mat.Scale(1.0,1.0,rot_mirror);
	axisMat.Invert();
	mat=mat*axisMat;
}

const double FsGui3DInterface::GetRotationAngle(void) const
{
	return rot_angle;
}

const YsVec3 &FsGui3DInterface::GetRotationAxis(void) const
{
	return rot_axis;
}

const YsVec3 &FsGui3DInterface::GetRotationCenter(void) const
{
	return rot_center;
}

void FsGui3DInterface::End3DInterface(void)
{
	type=IFTYPE_NONE;
	canvas->RemoveDialog(point_dlg1);
	canvas->RemoveDialog(point_dlg2);
	canvas->RemoveDialog(box_dlg);
	canvas->RemoveDialog(rot_dlg);
	canvas->ArrangeDialog();
}

YSRESULT FsGui3DInterface::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	YSRESULT res=YSERR;

	if(YSTRUE!=prevLb && YSTRUE==lb && YSOK==OnLButtonDown(lb,mb,rb,mx,my))
	{
		res=YSOK;
	}
	if(YSTRUE==prevLb && YSTRUE!=lb && YSOK==OnLButtonUp(lb,mb,rb,mx,my))
	{
		res=YSOK;
	}

	if((mx!=prevMx || my!=prevMy) && YSOK==OnMouseMove(lb,mb,rb,mx,my))
	{
		res=YSOK;
	}

	if(YSTRUE!=prevRb && YSTRUE==rb && YSOK==OnRButtonDown(lb,mb,rb,mx,my))
	{
		res=YSOK;
	}
	if(YSTRUE==prevRb && YSTRUE!=rb && YSOK==OnRButtonUp(lb,mb,rb,mx,my))
	{
		res=YSOK;
	}

	prevLb=lb;
	prevMb=mb;
	prevRb=rb;
	prevMx=mx;
	prevMy=my;

	return res;
}

YSRESULT FsGui3DInterface::SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[])
{
	return YSERR;
}

YSRESULT FsGui3DInterface::KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	return YSERR;
}

template <const int N>
void FsGui3DInterface::MakeRotationHandle(YsArray <YsVec3,N> &handlePnt) const
{
	const YsVec3 sideX=rot_iVec*rot_ref_size*1.1;
	const YsVec3 sideY=rot_kVec*rot_ref_size*1.1;

	handlePnt.Clear();
	for(int i=0; i<360; i+=10)
	{
		const double a=YsPi*2.0*double(i)/360.0;
		YsVec3 p;
		handlePnt.Append(rot_center+sideX*cos(a)+sideY*sin(a));
	}
}

YSRESULT FsGui3DInterface::OnLButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int rawMx,int rawMy)
{
	if(YSTRUE!=mb)
	{
		YSBOOL sft,ctrl;
		sft=(YSBOOL)FsGetKeyState(FSKEY_SHIFT);
		ctrl=(YSBOOL)FsGetKeyState(FSKEY_CTRL);

		int winWid,winHei;
		FsGetWindowSize(winWid,winHei);

		const int glMx=rawMx,glMy=winHei-rawMy;

		if(type==IFTYPE_BOX)
		{
			const YsVec3 cen=(box_pos[0]+box_pos[1])/2.0;
			YsVec3 o;

			o.Set(box_pos[0].x(),cen.y(),cen.z());
			if(YSTRUE==Is3DLinePicked(o, YsXVec(),pickerLength,glMx,glMy) || 
			   YSTRUE==Is3DLinePicked(o,-YsXVec(),pickerLength,glMx,glMy))
			{
				box_dragging=BOX_XMIN;
				dragging=YSTRUE;
				SetNeedRedraw(YSTRUE);
			}

			o.Set(box_pos[1].x(),cen.y(),cen.z());
			if(YSTRUE==Is3DLinePicked(o, YsXVec(),pickerLength,glMx,glMy) || 
			   YSTRUE==Is3DLinePicked(o,-YsXVec(),pickerLength,glMx,glMy))
			{
				box_dragging=BOX_XMAX;
				dragging=YSTRUE;
				SetNeedRedraw(YSTRUE);
			}

			o.Set(cen.x(),box_pos[0].y(),cen.z());
			if(YSTRUE==Is3DLinePicked(o, YsYVec(),pickerLength,glMx,glMy) || 
			   YSTRUE==Is3DLinePicked(o,-YsYVec(),pickerLength,glMx,glMy))
			{
				box_dragging=BOX_YMIN;
				dragging=YSTRUE;
				SetNeedRedraw(YSTRUE);
			}

			o.Set(cen.x(),box_pos[1].y(),cen.z());
			if(YSTRUE==Is3DLinePicked(o, YsYVec(),pickerLength,glMx,glMy) || 
			   YSTRUE==Is3DLinePicked(o,-YsYVec(),pickerLength,glMx,glMy))
			{
				box_dragging=BOX_YMAX;
				dragging=YSTRUE;
				SetNeedRedraw(YSTRUE);
			}

			o.Set(cen.x(),cen.y(),box_pos[0].z());
			if(YSTRUE==Is3DLinePicked(o, YsZVec(),pickerLength,glMx,glMy) || 
			   YSTRUE==Is3DLinePicked(o,-YsZVec(),pickerLength,glMx,glMy))
			{
				box_dragging=BOX_ZMIN;
				dragging=YSTRUE;
				SetNeedRedraw(YSTRUE);
			}

			o.Set(cen.x(),cen.y(),box_pos[1].z());
			if(YSTRUE==Is3DLinePicked(o, YsZVec(),pickerLength,glMx,glMy) || 
			   YSTRUE==Is3DLinePicked(o,-YsZVec(),pickerLength,glMx,glMy))
			{
				box_dragging=BOX_ZMAX;
				dragging=YSTRUE;
				SetNeedRedraw(YSTRUE);
			}
		}
		else if(type==IFTYPE_DRAGPOINT)
		{
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

			for(int i=0; i<3; i++)
			{
				if(point_dragDirSwitch[i]==YSTRUE)
				{
					if(YSTRUE==Is3DLinePicked(point_pos, dragDir[i],pickerLength,glMx,glMy) ||
					   YSTRUE==Is3DLinePicked(point_pos,-dragDir[i],pickerLength,glMx,glMy))
					{
						point_dragging=pointDragTarget[i];
						dragging=YSTRUE;
						SetNeedRedraw(YSTRUE);
					}
				}
			}
		}
		else if(type==IFTYPE_SLIDER)
		{
		}
        else if(type==IFTYPE_ROTATION)
		{
			YsArray <YsVec3,37> hoop;
			MakeRotationHandle(hoop);
			const YsVec3 hoop0=hoop[0];
			hoop.Append(hoop0);

			YSBOOL hoopPicked=YSFALSE;
			YsVec3 pickedHoopPos;
			double pickedHoopNearZ=YsInfinity;
			for(int i=0; i<hoop.GetN()-1; ++i)
			{
				YsVec3 nearPos;
				if(YSTRUE==Is3DLinePickedAlsoGetNearestPoint(nearPos,hoop[i],hoop[i+1],glMx,glMy))
				{
					YsVec3 nearPosScrn;
					YsTransform3DCoordToScreenCoord(nearPosScrn,nearPos,viewport,projViewModelTfm);
					if(YSTRUE!=hoopPicked)
					{
						hoopPicked=YSTRUE;
						pickedHoopPos=nearPos;
						pickedHoopNearZ=nearPosScrn.z();
					}
					else if(nearPosScrn.z()<pickedHoopNearZ)
					{
						pickedHoopPos=nearPos;
						pickedHoopNearZ=nearPosScrn.z();
					}
				}
			}

			if(YSTRUE==hoopPicked)
			{
				rot_dragging=ROTATION_HANDLE;
				rot_picked_angle=YsCalculateCounterClockwiseAngle(pickedHoopPos-rot_center,rot_iVec,rot_jVec);
				rot_drag_angle=rot_picked_angle;

				dragging=YSTRUE;
				SetNeedRedraw(YSTRUE);
			}
		}
		else if(IFTYPE_POINT==type && YSTRUE!=sft)
		{
			dragging=YSTRUE;
			auto nom=point_dlg1->GetNormalDirection();

			YsVec3 m1((double)glMx,(double)glMy,-1.0);
			YsVec3 m2((double)glMx,(double)glMy, 1.0);
			YsTransformScreenCoordTo3DCoord(m1,m1,viewport,projViewModelTfm);
			YsTransformScreenCoordTo3DCoord(m2,m2,viewport,projViewModelTfm);
			auto viewDir=YsUnitVector(m2-m1);

			if(YSTRUE==point_dlg1->ShowVerticalHandle() &&
			   (YSTRUE==Is3DLinePicked(point_pos, nom,pickerLength,glMx,glMy) ||
			    YSTRUE==Is3DLinePicked(point_pos,-nom,pickerLength,glMx,glMy)) &&
			   viewDir*nom<YsCos3deg)
			{
				point_dragging=POINT_Y;
			}
			else
			{
				point_dragging=NONE;
			}
		}
	}
	return YSERR;
}

YSRESULT FsGui3DInterface::OnRButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	return OnLButtonDown(lb,mb,rb,mx,my);
}

YSRESULT FsGui3DInterface::OnLButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(dragging==YSTRUE)
	{
		SetNeedRedraw(YSTRUE);
		dragging=YSFALSE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT FsGui3DInterface::OnRButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	return OnLButtonUp(lb,mb,rb,mx,my);
}

YSRESULT FsGui3DInterface::OnMouseMove(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(dragging==YSTRUE)
	{
		YSRESULT res;

		YSBOOL sft,ctrl;
		sft=(YSBOOL)FsGetKeyState(FSKEY_SHIFT);
		ctrl=(YSBOOL)FsGetKeyState(FSKEY_CTRL);

		res=YSOK;
		if(type==IFTYPE_BOX)
		{
			res=DragBox(lb,rb,sft,ctrl,mx,my);
		}
		else if(type==IFTYPE_POINT)
		{
			if(DragPoint(lb,rb,sft,ctrl,mx,my)==YSOK)
			{
				point_dlg1->SetCoord(point_pos.x(),point_pos.y(),point_pos.z());
			}
		}
		else if(type==IFTYPE_DRAGPOINT)
		{
			if(DragPoint2(lb,rb,sft,ctrl,mx+ofstX,my+ofstY)==YSOK)
			{
				point_dlg2->SetCoord(point_pos.x(),point_pos.y(),point_pos.z());
			}
		}
		else if(type==IFTYPE_ROTATION)
		{
			if(DragRotation(lb,rb,sft,ctrl,mx,my)==YSOK)
			{
				rot_dlg->SetRotation(rot_axis,rot_angle,rot_center);
			}
		}
		prevMx=mx;
		prevMy=my;
		if(res==YSOK)
		{
			SetNeedRedraw(YSTRUE);
		}
		return res;
	}
	return YSERR;
}

YSRESULT FsGui3DInterface::DragBox(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int rawMx,int rawMy)
{
	YsVec3 nea1,nea2,cen,axis,dif;
	double mov[3];
	int target,element;

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	const int glMx=rawMx;
	const int glMy=winHei-rawMy;


	cen=(box_pos[0]+box_pos[1])/2.0;

	YsVec3 org((double)glMx,(double)glMy,-1.0);
	YsVec3 vec((double)glMx,(double)glMy, 1.0);
	YsTransformScreenCoordTo3DCoord(org,org,viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(vec,vec,viewport,projViewModelTfm);
	vec-=org;
	vec.Normalize();

	if(box_dragging==BOX_XMIN)
	{
		target=0;
		element=0;
	}
	else if(box_dragging==BOX_XMAX)
	{
		target=1;
		element=0;
	}
	else if(box_dragging==BOX_YMIN)
	{
		target=0;
		element=1;
	}
	else if(box_dragging==BOX_YMAX)
	{
		target=1;
		element=1;
	}
	else if(box_dragging==BOX_ZMIN)
	{
		target=0;
		element=2;
	}
	else if(box_dragging==BOX_ZMAX)
	{
		target=1;
		element=2;
	}
	else
	{
		return YSERR;
	}

	mov[0]=0.0;
	mov[1]=0.0;
	mov[2]=0.0;
	mov[element]=1.0;
	axis.Set(mov[0],mov[1],mov[2]);


	if(YsGetNearestPointOfTwoLine(nea1,nea2,org,org+vec,cen,cen+axis)==YSOK)
	{
		mov[0]=0.0;
		mov[1]=0.0;
		mov[2]=0.0;
		mov[element]=nea2.GetValue()[element]-box_pos[target].GetValue()[element];

		dif.Set(mov[0],mov[1],mov[2]);
		if(rb==YSTRUE || box_allowResize!=YSTRUE)
		{
			box_pos[0]+=dif;
			box_pos[1]+=dif;
			box_dlg->SetCoord(box_pos);
			InvokeCallBack();
			return YSOK;
		}
		else if(lb==YSTRUE)
		{
			box_pos[target]+=dif;

			if(box_maintainAspectRatio==YSTRUE)
			{
				int i;
				YsVec3 cen,orgSize;
				double resizeRatio;
				resizeRatio=YsAbs((box_pos[1].GetValue()[element]-box_pos[0].GetValue()[element])/
				                  (box_pos_org[1].GetValue()[element]-box_pos_org[0].GetValue()[element]));

				cen=(box_pos[1]+box_pos[0])/2.0;
				orgSize=box_pos_org[1]-box_pos_org[0];

				double newValue[2][3];
				for(i=0; i<3; i++)
				{
					if(i!=element)
					{
						newValue[0][i]=cen.GetValue()[i]-(orgSize.GetValue()[i]/2.0)*resizeRatio;
						newValue[1][i]=cen.GetValue()[i]+(orgSize.GetValue()[i]/2.0)*resizeRatio;
					}
					else
					{
						newValue[0][i]=box_pos[0].GetValue()[i];
						newValue[1][i]=box_pos[1].GetValue()[i];
					}
				}
				box_pos[0].Set(newValue[0]);
				box_pos[1].Set(newValue[1]);
			}

			box_dlg->SetCoord(box_pos);
			InvokeCallBack();
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGui3DInterface::DragPoint(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int rawMx,int rawMy)
{
	YsVec3 nea1,nea2,cen,axis,dif;
	int hor1,hor2,vert;

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	const int glMx=rawMx;
	const int glMy=winHei-rawMy;

	cen=(box_pos[0]+box_pos[1])/2.0;
	YsVec3 org((double)glMx,(double)glMy,-1.0);
	YsVec3 vec((double)glMx,(double)glMy, 1.0);
	YsTransformScreenCoordTo3DCoord(org,org,viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(vec,vec,viewport,projViewModelTfm);
	vec-=org;
	vec.Normalize();



	if(point_dlg1->GetBasePlane()==0)
	{
		axis=YsZVec();
		hor1=0;
		hor2=1;
		vert=2;
	}
	else if(point_dlg1->GetBasePlane()==1)
	{
		axis=YsYVec();
		hor1=0;
		hor2=2;
		vert=1;
	}
	else
	{
		axis=YsXVec();
		hor1=1;
		hor2=2;
		vert=0;
	}


	if(rb==YSTRUE || POINT_Y==point_dragging)
	{
		YsVec3 nea1,nea2;
		if(YsAbs(vec*axis)<0.996195)  // 5 degree
		{
			if(YsGetNearestPointOfTwoLine(nea1,nea2,org,org+vec,point_pos,point_pos+axis)==YSOK)
			{
				point_pos[vert]=nea2[vert];
				SetNeedRedraw(YSTRUE);
				InvokeCallBack();
				return YSOK;
			}
		}
	}
	else if(lb==YSTRUE)
	{
		YsVec3 crs;
		if(YsAbs(vec*axis)>=0.087156)  // 85 degree
		{
			YsPlane pln;
			pln.Set(point_pos,axis);
			if(pln.GetIntersection(crs,org,vec)==YSOK)
			{
				point_pos[hor1]=crs[hor1];
				point_pos[hor2]=crs[hor2];
				SetNeedRedraw(YSTRUE);
				InvokeCallBack();
				return YSOK;
			}
		}
		else
		{
			YsVec3 laxis;
			laxis=axis^vec;

			if(YsGetNearestPointOfTwoLine(nea1,nea2,org,org+vec,point_pos,point_pos+laxis)==YSOK)
			{
				point_pos[hor1]=nea2[hor1];
				point_pos[hor2]=nea2[hor2];
				SetNeedRedraw(YSTRUE);
				InvokeCallBack();
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT FsGui3DInterface::DragPoint2(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int rawMx,int rawMy)
{
	YsVec3 nea1,nea2;

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	const int glMx=rawMx;
	const int glMy=winHei-rawMy;

	YsVec3 mOrg((double)glMx,(double)glMy,-1.0);
	YsVec3 mVec((double)glMx,(double)glMy, 1.0);
	YsTransformScreenCoordTo3DCoord(mOrg,mOrg,viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(mVec,mVec,viewport,projViewModelTfm);
	mVec-=mOrg;


	if(POINT_X==point_dragging || POINT_Y==point_dragging || POINT_Z==point_dragging)
	{
		YsVec3 lOrg,lVec;
		lOrg=point_pos;

		if(YSTRUE==point_dlg2->UseCustomOrientation())
		{
			switch(point_dragging)
			{
			case POINT_X:
				lVec=point_dragDir[0];
				break;
			case POINT_Y:
				lVec=point_dragDir[1];
				break;
			case POINT_Z:
				lVec=point_dragDir[2];
				break;
			default:
				return YSERR;
			}
		}
		else
		{
			switch(point_dragging)
			{
			case POINT_X:
				lVec=YsXVec();
				break;
			case POINT_Y:
				lVec=YsYVec();
				break;
			case POINT_Z:
				lVec=YsZVec();
				break;
			default:
				return YSERR;
			}
		}
		if(YsGetNearestPointOfTwoLine(nea1,nea2,mOrg,mOrg+mVec,lOrg,lOrg+lVec)==YSOK)
		{
			point_pos=nea2;
			SetNeedRedraw(YSTRUE);
			InvokeCallBack();
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGui3DInterface::DragRotation(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int rawMx,int rawMy)
{
	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	const int winD=YsGreater(winWid,winHei);

	const int prevGlMx=prevMx;
	const int prevGlMy=winHei-prevMy;

	const int glMx=rawMx;
	const int glMy=winHei-rawMy;



	YsVec3 p1,p2;
	p1=rot_center;
	YsTransform3DCoordToScreenCoord(p1,rot_center,viewport,projViewModelTfm);
	p2=p1;
	p1.SetZ(-1.0);

	YsTransformScreenCoordTo3DCoord(p1,p1,viewport,projViewModelTfm);
	YsTransformScreenCoordTo3DCoord(p2,p2,viewport,projViewModelTfm);

	const YsVec3 v=YsUnitVector(p2-p1);
	if(YsCos85deg>fabs(rot_jVec*v))
	{
		p1=rot_center;
		p2=rot_center+rot_jVec;
		YsTransform3DCoordToScreenCoord(p1,p1,viewport,projViewModelTfm);
		YsTransform3DCoordToScreenCoord(p2,p2,viewport,projViewModelTfm);

		const YsVec3 p1p2=p2-p1;
		YsVec2 jVecScrn(p1p2.x(),p1p2.y());
		jVecScrn.Normalize();

		YsVec2 mouseMove((double)(glMx-prevGlMx),(double)(glMy-prevGlMy));

		const double sign=(0.0<(jVecScrn^mouseMove) ? -1.0 : 1.0);
		const double deltaAngle=YsPi*sign*mouseMove.GetLength()/(double)winD;  // Moving the size of the window will cause 180 degree turn.

		rot_angle+=deltaAngle;

		SetNeedRedraw(YSTRUE);
		InvokeCallBack();

		return YSOK;
	}
	else
	{
		p1.Set((double)glMx,(double)glMy,-1.0);
		p2.Set((double)glMx,(double)glMy, 1.0);
		YsTransformScreenCoordTo3DCoord(p1,p1,viewport,projViewModelTfm);
		YsTransformScreenCoordTo3DCoord(p2,p2,viewport,projViewModelTfm);

		const YsVec3 v=YsUnitVector(p2-p1);
		const YsPlane pln(rot_center,rot_jVec);

		YsVec3 itsc;
		pln.GetIntersection(itsc,p1,v);

		const YsVec3 radial=itsc-rot_center;

		rot_drag_angle=YsCalculateCounterClockwiseAngle(radial,rot_iVec,rot_jVec);

		const double deltaAngle=rot_drag_angle-rot_picked_angle;
		rot_angle+=deltaAngle;

		rot_picked_angle=rot_drag_angle;

		SetNeedRedraw(YSTRUE);
		InvokeCallBack();

		return YSOK;
	}


//	double dx,dy,motion,dir;
//	YsVec3 axisInViewCoord,movInScrnCoord;
//	YsAtt3 viewAtt;
//	YsMatrix4x4 viewMat;
//
//	dx= double(mx-prevMx);
//	dy=-double(my-prevMy);
//	movInScrnCoord.Set(dx,dy,0.0);
//	if(movInViewCoord.Normalize()==YSOK)
//	{
//		viewAtt.Set(0.0,0.0,0.0); // <= supposed to be the attitude.
//
//		if(rot_dragging==0xffff0000)
//		{
//			viewAtt.MulInverse(axisInViewCoord,rot_axis);
//			axisInViewCoord.SetZ(0.0);
//			if(axisInViewCoord.Normalize()==YSOK)
//			{
//				motion=sqrt(dx*dx+dy*dy);
//				dir=-(axisInViewCoord^movInViewCoord).z();
//				motion=(motion*dir/900.0)*YsPi;  // <- 1pixel -> 0.2 degree
//				rot_angle+=motion;
//				SetNeedRedraw(YSTRUE);
//				return YSOK;
//			}
//		}
//		else if(rot_dragging==0xffff0001)
//		{
//			YsVec3 iVec;
//			iVec=rot_drag_dir;
//			viewAtt.MulInverse(axisInViewCoord,rot_drag_dir);
//			axisInViewCoord.SetZ(0.0);
//			if(axisInViewCoord.Normalize()==YSOK)
//			{
//				motion=sqrt(dx*dx+dy*dy);
//				dir=axisInViewCoord*movInViewCoord;
//				motion=(motion*dir/900.0)*YsPi;
//				rot_angle+=motion;
//				SetNeedRedraw(YSTRUE);
//				return YSOK;
//			}
//		}
//	}
	return YSERR;
}
