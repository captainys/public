/* ////////////////////////////////////////////////////////////

File Name: sketch.h
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

#ifndef SKETCH_IS_INCLUDED
#define SKETCH_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysshellext.h>

class PolyCreSketchInterface
{
public:
	class StrokePoint
	{
	public:
		YsVec2 winCoord;
		YsVec3 prjCoord;
		YsShellPolygonHandle prjPlHd;

		void Initialize(void);
	};

private:
	YsArray <StrokePoint> stroke;

	const class Ys3DDrawingEnvironment *viewCtrl;
	const YsShellExt *shl;
	YsShellLattice ltc;
	YSBOOL drawing;

private:
	PolyCreSketchInterface();
	~PolyCreSketchInterface();

public:
	static PolyCreSketchInterface *Create(void);
	static void Delete(PolyCreSketchInterface *ptr);

	void StartSketchInterface(const YsShellExt &shl,const Ys3DDrawingEnvironment &viewControl);
	void BeginStroke(void);
	void AddStrokePoint(const YsVec2 &winCoord);
	void EndStroke(void);
	void ClearStroke(void);

	YSBOOL IsDrawing(void) const;

	void AdHocSmoothingInWindowCoordinate(void);

	/*! Returns YSTRUE if some part of the stroke was dropped. */
	YSBOOL DropHanging(void);

	YSRESULT PreliminaryProjection(void);
	YSRESULT UnprojectStrokePoint(YSSIZE_T idx);
	YSRESULT ExtendProjection(void);

	/*! This function makes the stroke clockwise on the window.
	*/
	YSRESULT MakeClockwise(void);

	/*! This function reverses the order of the stroke points.
	*/
	YSRESULT ReverseOrder(void);

	/*! This function will call PrelminiaryProjection, and then ExtendProjection. 
	    This function returns YSOK if all points have been successfully projected (directly on or extended),
	    or YSERR otherwise. */
	YSRESULT ProjectToShell(void);

	/*! This function will project the stroke to the view-projection plane. */
	YSRESULT ProjectToPlane(void);

	/*! This function will project the stroke to an arbitrary plane.  It returns YSOK if all points were projected successfully.  YSERR otherwise. */
	YSRESULT ProjectToPlane(const YsPlane &pln);

	YsShellPolygonHandle FindPolygonFromWindowCoordinate(YsVec3 &itsc,const YsVec2 &winCoord) const;

	YsConstArrayMask <StrokePoint> GetStroke(void) const;

	/*! Take screen coordinate from outside to emulate sketch stroke.
	*/
	void SetStroke(YsConstArrayMask <YsVec2> winCoord);
};


/* } */
#endif
