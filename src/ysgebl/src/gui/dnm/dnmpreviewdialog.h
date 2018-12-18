/* ////////////////////////////////////////////////////////////

File Name: dnmpreviewdialog.h
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

#ifndef DNMPREVIEWDIALOG_IS_INCLUDED
#define DNMPREVIEWDIALOG_IS_INCLUDED
/* { */


class PolyCreDnmPreviewDialog : public FsGuiDialog
{
private:
	class GeblGuiEditorBase *canvasPtr;

public:
	int airTabIdent,gndTabIdent;
	FsGuiTabControl *objTypeTab;

	FsGuiButton *hideDlgBtn;

	// For aircraft
	FsGuiSlider *landingGearSlider;
	FsGuiSlider *variableGeometryWingSlider;
	FsGuiSlider *throttleSlider;
	FsGuiButton *afterBurnerBtn;
	FsGuiButton *rotatePropellerBtn;
	FsGuiSlider *airBrakeSlider;
	FsGuiSlider *hookSlider;
	FsGuiSlider *flapSlider;
	FsGuiSlider *elevatorSlider;
	FsGuiSlider *aileronSlider;
	FsGuiSlider *rudderSlider;
	FsGuiSlider *steeringSlider;
	FsGuiSlider *bombBaySlider;
	FsGuiSlider *vtolNozzleSlider;
	FsGuiSlider *reverserSlider;
	FsGuiSlider *concordeNoseSlider;
	FsGuiSlider *turretHSlider;
	FsGuiSlider *turretPSlider;
	FsGuiButton *rotateTireBtn;
	FsGuiButton *navLightBtn;
	FsGuiButton *beaconBtn;
	FsGuiButton *strobeBtn;
	FsGuiButton *landingLightBtn;

	// For ground
	FsGuiSlider *aaaHSlider;
	FsGuiSlider *aaaPSlider;
	FsGuiSlider *samHSlider;
	FsGuiSlider *samPSlider;
	FsGuiSlider *cannonHSlider;
	FsGuiSlider *cannonPSlider;
	FsGuiButton *hdgRotationBtn;

	// Common
	FsGuiSlider *leftDoorSlider;
	FsGuiSlider *rightDoorSlider;
	FsGuiSlider *rearDoorSlider;
	FsGuiButton *interiorBtn;

	long long int lastTimer;
	double staRotorAngle;
	double staWheelAngle;
	double staRadarAngle;


	PolyCreDnmPreviewDialog();

	void Make(class GeblGuiEditorBase *canvasPtr);
	void UpdateDnmState(void);

	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);
	virtual void Interval(void);
};



/* } */
#endif
