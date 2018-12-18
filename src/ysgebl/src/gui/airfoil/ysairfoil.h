/* ////////////////////////////////////////////////////////////

File Name: ysairfoil.h
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

#ifndef AIRFOIL_IS_INCLUDED
#define AIRFOIL_IS_INCLUDED
/* { */


#include <ysclass.h>

class Airfoil
{
public:
	int frontIdx,backIdx;
	YsArray <YsVec2> point;

	void ResampleFromUpperAndLower(const YsArray <YsVec3> &upper,const YsArray <YsVec3> &lower,const YsArray <double> &tArray);
	void MatchTail(void);
	void Normalize(void);
};

class AirfoilNACA : public Airfoil
{
	YsArray <YsVec3> upper,lower;

public:
	void GenerateRawAirfoil(int naca,const double chordLength,int numDivision,YSBOOL sharpTail);
	void Resample(int numDivision);

private:
	double YCoord(int naca,double c,double x,double side,YSBOOL sharpTail);
};

class AirfoilNACA6 : public Airfoil
{
private:
	struct Chord
	{
		const char *label;
		double dat[72];
	};

	static const Chord AirfoilData[];
	static const Chord MeanLineAData[];
	static const Chord MeanLineLabelData[];

	YsArray <YsVec3> upper,lower;

public:
	static void GetAllLabel(YsArray <YsString> &labelArray);

	AirfoilNACA6();
	/*! Give a==-1.0 for symmetric airfoil. */
	void GenerateRawAirfoil(const char typeLabel[],const char meanLineLabel[],YSBOOL modified,double chordLength,int numDivision); 
	void Resample(int numDivision);

private:
	const double CalculateMeanLineData(const Chord &meanLine,const double percentChord) const;
};

class AirfoilSuperCritical : public Airfoil
{
public:
	static void GetCatalogue(YsArray <const char *> &catalogue);
	void Generate(int numDivision,const char type[]);
};

class AirfoilJoukowski : public Airfoil
{
public:
	void Generate(int numDivision,const double a,const double d,const double e);
	void GetCoordinate(double &x,double &y,const double a,const double d,const double e,const double theata);
};

class AirfoilClarkY : public Airfoil
{
public:
	void Generate(int numDivision);
};

class AirfoilLens : public Airfoil
{
public:
	void Generate(int numDivision,const double c,const double h1,const double b1,const double h2,const double b2);
};

/* } */
#endif
