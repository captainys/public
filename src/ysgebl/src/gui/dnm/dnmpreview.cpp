/* ////////////////////////////////////////////////////////////

File Name: dnmpreview.cpp
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

#include <ysclass.h>

#include <ysgl.h>
#include <ysglbuffer.h>

#include <ysshelldnmident.h>

#include "../ysgebl_gui_editor_base.h"
#include "../textresource.h"

#include "dnmpreviewdialog.h"



PolyCreDnmPreviewDialog::PolyCreDnmPreviewDialog()
{
	staRotorAngle=0.0;
	staWheelAngle=0.0;
	staRadarAngle=0.0;
	lastTimer=FsSubSecondTimer();
}

void PolyCreDnmPreviewDialog::Make(class GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	hideDlgBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Hide Dialog",YSTRUE);

	objTypeTab=AddTabControl(0,FSKEY_NULL,YSTRUE);

	airTabIdent=AddTab(objTypeTab,L"As Aircraft");
	BeginAddTabItem(objTypeTab,airTabIdent);
	{
		YsArray <FsGuiDialogItem *> sliderColumn;

		AddStaticText(0,FSKEY_NULL,L"Landing Gear",YSTRUE);
		sliderColumn.Append(landingGearSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Variable-Geometry Wing",YSTRUE);
		sliderColumn.Append(variableGeometryWingSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Throttle",YSTRUE);
		sliderColumn.Append(throttleSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Air Brake",YSTRUE);
		sliderColumn.Append(airBrakeSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Arresting Hook",YSTRUE);
		sliderColumn.Append(hookSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Flaps",YSTRUE);
		sliderColumn.Append(flapSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Elevator",YSTRUE);
		sliderColumn.Append(elevatorSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Aileron",YSTRUE);
		sliderColumn.Append(aileronSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Rudder",YSTRUE);
		sliderColumn.Append(rudderSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Bomb-Bay Door",YSTRUE);
		sliderColumn.Append(bombBaySlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"VTOL Nozzle",YSTRUE);
		sliderColumn.Append(vtolNozzleSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Thrust Reverser",YSTRUE);
		sliderColumn.Append(reverserSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Nose Visor",YSTRUE);
		sliderColumn.Append(concordeNoseSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Turret(H)",YSTRUE);
		sliderColumn.Append(turretHSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Turret(P)",YSTRUE);
		sliderColumn.Append(turretPSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		throttleSlider->SetPosition(0.5);
		elevatorSlider->SetPosition(0.5);
		aileronSlider->SetPosition(0.5);
		rudderSlider->SetPosition(0.5);

		AlignColumnRight(sliderColumn);

		afterBurnerBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Afterburner",YSTRUE);
		rotatePropellerBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Rotate Propeller",YSFALSE);
		navLightBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"NAV Light",YSTRUE);
		beaconBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Beacon",YSFALSE);
		strobeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Strobe",YSFALSE);
	}
	EndAddTabItem();

	gndTabIdent=AddTab(objTypeTab,L"As Ground");
	BeginAddTabItem(objTypeTab,gndTabIdent);
	{
		YsArray <FsGuiDialogItem *> sliderColumn;

		AddStaticText(0,FSKEY_NULL,L"AAA (H)",YSTRUE);
		sliderColumn.Append(aaaHSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"AAA (P)",YSTRUE);
		sliderColumn.Append(aaaPSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"SAM (H)",YSTRUE);
		sliderColumn.Append(samHSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"SAM (P)",YSTRUE);
		sliderColumn.Append(samPSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"CANNON (H)",YSTRUE);
		sliderColumn.Append(cannonHSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"CANNON (H)",YSTRUE);
		sliderColumn.Append(cannonPSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AlignColumnRight(sliderColumn);

		hdgRotationBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Radar",YSTRUE);
	}
	EndAddTabItem();

	{
		YsArray <FsGuiDialogItem *> sliderColumn;

		AddStaticText(0,FSKEY_NULL,L"Steering",YSTRUE);
		sliderColumn.Append(steeringSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Left Door",YSTRUE);
		sliderColumn.Append(leftDoorSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Right Door",YSTRUE);
		sliderColumn.Append(rightDoorSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AddStaticText(0,FSKEY_NULL,L"Rear Door",YSTRUE);
		sliderColumn.Append(rearDoorSlider=AddHorizontalSlider(0,FSKEY_NULL,20,0.0,1.0,YSFALSE));

		AlignColumnRight(sliderColumn);

		steeringSlider->SetPosition(0.5);
		rotateTireBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Rotate Tire",YSTRUE);
		interiorBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Interior",YSFALSE);
		landingLightBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Landing Light/Head Light",YSFALSE);
	}


	objTypeTab->SelectCurrentTab(airTabIdent);
	SetBackgroundAlpha(0.0);
	Fit();
}

void PolyCreDnmPreviewDialog::UpdateDnmState(void)
{
	canvasPtr->dnmEditMode=YSTRUE;
	canvasPtr->dnmCurrentState=-1;

	auto &shlGrp=canvasPtr->ShlGrp();

	canvasPtr->dnmState=shlGrp.EmptyDnmState();

	auto &state=canvasPtr->dnmState;

	const YSBOOL staLandingLight=landingLightBtn->GetCheck(); // Also head light of ground.
	const double staLeftDoor=leftDoorSlider->GetPosition();
	const double staRightDoor=rightDoorSlider->GetPosition();
	const double staRearDoor=rearDoorSlider->GetPosition();
	const YSBOOL staInterior=interiorBtn->GetCheck();
	const double ctlSteering=1.0-steeringSlider->GetPosition()*2.0;

	shlGrp.SetClassShow(state,0,YSTRUE);  // The reason why it is necessary is worth a look, but it is necessary.

	if(airTabIdent==objTypeTab->GetCurrentTab())
	{
		const double staThrottle=throttleSlider->GetPosition();
		const double staGear=landingGearSlider->GetPosition();
		const double staVgw=variableGeometryWingSlider->GetPosition();
		const double staSpoiler=airBrakeSlider->GetPosition();
		const double staFlap=flapSlider->GetPosition();
		const double staThrRev=reverserSlider->GetPosition();
		const double staBombBayDoor=bombBaySlider->GetPosition();
		const double elevatorPlusTrim=elevatorSlider->GetPosition()*2.0-1.0;
		const double ctlAileron=1.0-aileronSlider->GetPosition()*2.0;
		const double ctlRudder=1.0-rudderSlider->GetPosition()*2.0;
		const YSBOOL staAfterburner=afterBurnerBtn->GetCheck();
		const double staThrustVector=vtolNozzleSlider->GetPosition();
		const double staBrake=hookSlider->GetPosition();
		const double staConcordeNose=concordeNoseSlider->GetPosition();

		const YSBOOL staNavLight=navLightBtn->GetCheck();
		const YSBOOL staBeacon=beaconBtn->GetCheck();
		const YSBOOL staStrobe=strobeBtn->GetCheck();

		if(staGear<0.2)
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_LANDINGDEVICE,0);
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARDOOR,0,1,staGear/0.2);
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARDOOR_FAST,0,1,staGear/0.2);
		}
		else if(staGear<0.8)
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_LANDINGDEVICE,0,1,(staGear-0.2)/0.6);
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARDOOR,1);
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARDOOR_FAST,1);
		}
		else
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_LANDINGDEVICE,1);
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARDOOR,0,1,(1.0-staGear)/0.2);
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARDOOR_FAST,1);
		}

		if(staGear<0.6) //   Changed the gear room behavior 2006/04/22
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARROOMWALL,0,1,staGear/0.6);
		}
		else
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_GEARROOMWALL,0,1,1.0);
		}
		//

		shlGrp.SetClassState(state,YSDNM_CLASSID_VGWING,0,1,staVgw);
		shlGrp.SetClassState(state,YSDNM_CLASSID_AIRBRAKE,0,1,staSpoiler);
		shlGrp.SetClassState(state,YSDNM_CLASSID_FLAP,0,1,staFlap);
		shlGrp.SetClassState(state,YSDNM_CLASSID_REVERSER,0,1,staThrRev);
		shlGrp.SetClassState(state,YSDNM_CLASSID_BOMBBAY,0,1,staBombBayDoor);

		if(elevatorPlusTrim>=0.0)
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_ELEVATOR,0,1,elevatorPlusTrim);
		}
		else
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_ELEVATOR,0,2,-elevatorPlusTrim);
		}

		if(ctlAileron>=0.0)
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_AILERON,0,1,ctlAileron);
		}
		else
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_AILERON,0,2,-ctlAileron);
		}

		if(ctlRudder>=0.0)
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_RUDDER,0,1,ctlRudder);
		}
		else
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_RUDDER,0,2,-ctlRudder);
		}

		if(staAfterburner==YSTRUE)
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_AFTERBURNER,1);
		}
		else
		{
			shlGrp.SetClassState(state,YSDNM_CLASSID_AFTERBURNER,0);
		}

		shlGrp.SetClassState(state,YSDNM_CLASSID_VTOLNOZZLE,0,1,staThrustVector);

		if(staThrottle>0.6)
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_PROPFAST,YSTRUE);
			shlGrp.SetClassShow(state,YSDNM_CLASSID_PROPSLOW,YSFALSE);
		}
		else if(staThrottle>0.3)
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_PROPFAST,YSTRUE);
			shlGrp.SetClassShow(state,YSDNM_CLASSID_PROPSLOW,YSTRUE);
		}
		else
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_PROPFAST,YSFALSE);
			shlGrp.SetClassShow(state,YSDNM_CLASSID_PROPSLOW,YSTRUE);
		}

		shlGrp.SetClassState(state,YSDNM_CLASSID_CONCORDE_NOSE,0,1,staConcordeNose);
		shlGrp.SetClassState(state,YSDNM_CLASSID_CONCORDE_VISOR,0,1,YsBound(staConcordeNose*2.5,0.0,1.0));
		shlGrp.SetClassState(state,YSDNM_CLASSID_BRAKE,0,1,staBrake);

		shlGrp.SetClassHeading(state,YSDNM_CLASSID_ROTOR,staRotorAngle);
		shlGrp.SetClassRotation(state,YSDNM_CLASSID_ROTOR_CUSTOM_AXIS,staRotorAngle);

	//	int i;
	//	for(i=0; i<chTurret.GetN(); i++)
	//	{
	//		staTurret[i].SetupVisual(vis,chTurret[i]);
	//	}

		shlGrp.SetClassShow(state,YSDNM_CLASSID_NAVLIGHT,staNavLight);

		if(YSTRUE==staLandingLight && staGear>0.2)
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_LIGHTONLANDINGGEAR,YSTRUE);
		}
		else
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_LIGHTONLANDINGGEAR,YSFALSE);
		}

		if(staBeacon==YSTRUE && 1000>(FsSubSecondTimer()%2000))
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_BEACON,YSTRUE);
		}
		else
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_BEACON,YSFALSE);
		}

		if(staStrobe==YSTRUE && 100>(FsSubSecondTimer()%1200))
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_STROBE,YSTRUE);
		}
		else
		{
			shlGrp.SetClassShow(state,YSDNM_CLASSID_STROBE,YSFALSE);
		}
	}
	else if(gndTabIdent==objTypeTab->GetCurrentTab())
	{
		const double staAaaH=aaaHSlider->GetPosition()*YsPi*2.0;
		const double staAaaP=aaaPSlider->GetPosition()*YsPi/2.0;
		const double staSamH=samHSlider->GetPosition()*YsPi*2.0;
		const double staSamP=samPSlider->GetPosition()*YsPi/2.0;
		const double staCannonH=cannonHSlider->GetPosition()*YsPi*2.0;
		const double staCannonP=cannonPSlider->GetPosition()*YsPi/2.0;


		shlGrp.SetClassHeading(state,YSDNM_CLASSID_GND_AAA_TURRET,staAaaH);
		shlGrp.SetClassPitch(state,YSDNM_CLASSID_GND_AAA_GUN,staAaaP);

		shlGrp.SetClassHeading(state,YSDNM_CLASSID_GND_SAM_TURRET,staSamH);
		shlGrp.SetClassPitch(state,YSDNM_CLASSID_GND_SAM,staSamP);

		shlGrp.SetClassHeading(state,YSDNM_CLASSID_GND_CANNON_TURRET,staCannonH);
		shlGrp.SetClassPitch(state,YSDNM_CLASSID_GND_CANNON_GUN,staCannonP);

		shlGrp.SetClassHeading(state,YSDNM_CLASSID_GND_HDGROTATION,staRadarAngle);
		shlGrp.SetClassHeading(state,YSDNM_CLASSID_GND_HDGROTATIONFAST,staRadarAngle*2.0);
	}

	shlGrp.SetClassPitch(state,YSDNM_CLASSID_TIRE,staWheelAngle);
	shlGrp.SetClassShow(state,YSDNM_CLASSID_LANDINGLIGHT,staLandingLight);  // Also is head light.

	if(ctlSteering>=0.0)
	{
		shlGrp.SetClassState(state,YSDNM_CLASSID_STEERING,0,1,ctlSteering);
	}
	else
	{
		shlGrp.SetClassState(state,YSDNM_CLASSID_STEERING,0,2,-ctlSteering);
	}

	shlGrp.SetClassState(state,YSDNM_CLASSID_LEFTDOOR,0,1,staLeftDoor);
	shlGrp.SetClassState(state,YSDNM_CLASSID_RIGHTDOOR,0,1,staRightDoor);
	shlGrp.SetClassState(state,YSDNM_CLASSID_REARDOOR,0,1,staRearDoor);
	shlGrp.SetClassShow(state,YSDNM_CLASSID_INTERIOR,staInterior);


	shlGrp.CacheTransformation(canvasPtr->dnmState);
	canvasPtr->SetNeedRedraw(YSTRUE);
}

/* virtual */ void PolyCreDnmPreviewDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==hideDlgBtn)
	{
		if(YSTRUE==hideDlgBtn->GetCheck())
		{
			HideAll();
			hideDlgBtn->Show();
		}
		else
		{
			ShowAll();
		}
	}
	else
	{
		UpdateDnmState();
	}
}

/* virtual */ void PolyCreDnmPreviewDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue)
{
	UpdateDnmState();
}

/* virtual */ void PolyCreDnmPreviewDialog::Interval(void)
{
	auto diff=FsSubSecondTimer()-lastTimer;
	lastTimer=FsSubSecondTimer();

	double dt=(double)diff/1000.0;

	YSBOOL needUpdate=YSFALSE;
	const double staThrottle=throttleSlider->GetPosition();
	if(YSTRUE==rotateTireBtn->GetCheck())
	{
		staWheelAngle-=staThrottle*dt*YsPi*2.0;
		needUpdate=YSTRUE;
	}
	if(YSTRUE==rotatePropellerBtn->GetCheck())
	{
		staRotorAngle+=10.0*staThrottle*dt*YsPi*2.0;
		needUpdate=YSTRUE;
	}
	if(YSTRUE==hdgRotationBtn->GetCheck())
	{
		staRadarAngle+=YsPi*dt;
		needUpdate=YSTRUE;
	}
	if(YSTRUE==strobeBtn->GetCheck() || YSTRUE==beaconBtn->GetCheck())
	{
		needUpdate=YSTRUE;
	}

	if(YSTRUE==needUpdate)
	{
		UpdateDnmState();
	}
}



void GeblGuiEditorBase::Dnm_Preview(void)
{
	Edit_ClearUIModeExceptDNMEditMode();

	auto *dlg=FsGuiDialog::CreateSelfDestructiveDialog<PolyCreDnmPreviewDialog>();
	dlg->Make(this);
	AddDialog(dlg);
	ArrangeDialog();
	SetNeedRedraw(YSTRUE);
}
