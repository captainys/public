/* ////////////////////////////////////////////////////////////

File Name: ysairfoil_naca6.cpp
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
#include <math.h>

#include <ysclass.h>
#include "ysairfoil.h"



/* static */ void AirfoilNACA6::GetAllLabel(YsArray <YsString> &labelArray)
{
	labelArray.CleanUp();
	for(YSSIZE_T idx=0; AirfoilData[idx].label!=NULL; ++idx)
	{
		labelArray.Increment();
		labelArray.Last().Set(AirfoilData[idx].label);
	}
}

AirfoilNACA6::AirfoilNACA6()
{
}

void AirfoilNACA6::GenerateRawAirfoil(const char typeLabel[],const char meanLineLabel[],YSBOOL modified,double chordLength,int numDivision)
{
	YsArray <double> upperSample,lowerSample;

	for(YSSIZE_T idx=0; AirfoilData[idx].label!=NULL; ++idx)
	{
		if(0==strcmp(AirfoilData[idx].label,typeLabel))
		{
			for(int t=0; ; ++t)
			{
				const double percentChord=AirfoilData[idx].dat[t*2];
				const double x=AirfoilData[idx].dat[t*2];
				const double y=AirfoilData[idx].dat[t*2+1];
				if(YSTRUE==modified && 85.1<percentChord && percentChord<99.9)
				{
					continue;
				}

				upperSample.Append(x);
				upperSample.Append(y);
				lowerSample.Append(x);
				lowerSample.Append(-y);

				if(99.9<AirfoilData[idx].dat[t*2])
				{
					break;
				}
			}
			break;
		}
	}

	if(0==strncmp(meanLineLabel,"a=",2))
	{
		const double a=YsBound(atof(meanLineLabel+2),0.001,0.999);
		YSSIZE_T aIdx=(int)(a/0.1);
		const double aMod=(a-(double)aIdx*0.1)/0.1;
		for(YSSIZE_T sampleIdx=0; sampleIdx<=upperSample.GetN()-2; sampleIdx+=2)
		{
			const double y0=CalculateMeanLineData(MeanLineAData[aIdx],upperSample[sampleIdx]);
			const double y1=CalculateMeanLineData(MeanLineAData[aIdx+1],upperSample[sampleIdx]);
			const double y=y0*(1.0-aMod)+y1*aMod;
			upperSample[sampleIdx+1]+=y;
			lowerSample[sampleIdx+1]+=y;
		}
	}
	else if(0==strncmp(meanLineLabel,"MeanLine",8))
	{
		// See Report 824 pp. 5.  Second paragraph of the left column.
		// It explains how to derive NACA xy mean line fron NACA 62,63,64,65,66, and 67.
		// NACA aX mean line can be obtained by scaling NACA bX mean line by b:a ratio.
		// Implies that the tables can multiplied into NACA ?2, ?3, ?4, ?5, ?6, and ?7.

		const double ratio=(double)(meanLineLabel[8]-'0');

		YsString tableName(meanLineLabel);
		tableName.Set(8,'6');  // Only table available are 62,63,...,67

		YSSIZE_T found=-1;
		for(YSSIZE_T idx=0; NULL!=MeanLineLabelData[idx].label; ++idx)
		{
			if(0==strcmp(MeanLineLabelData[idx].label,tableName))
			{
				found=idx;
				break;
			}
		}
		if(0<=found)
		{
			for(YSSIZE_T sampleIdx=0; sampleIdx<=upperSample.GetN()-2; sampleIdx+=2)
			{
				const double y=CalculateMeanLineData(MeanLineLabelData[found],upperSample[sampleIdx])*ratio/6.0;
				upperSample[sampleIdx+1]+=y;
				lowerSample[sampleIdx+1]+=y;
			}
		}
	}

	upper.CleanUp();
	lower.CleanUp();
	for(int t=0; t<numDivision; ++t)
	{
		const double percent=(double)t*100.0/(double)(numDivision-1);

		const double x=chordLength*percent/100.0;
		const double yUpper=chordLength*YsCalculatePiecewiseLinearFunction(upperSample.GetN()/2,upperSample.GetArray(),percent)/100.0;
		const double yLower=chordLength*YsCalculatePiecewiseLinearFunction(lowerSample.GetN()/2,lowerSample.GetArray(),percent)/100.0;

		upper.Append(YsVec3(x,yUpper,0.0));
		lower.Append(YsVec3(x,yLower,0.0));
	}
}

void AirfoilNACA6::Resample(int numDivision)
{
	int ix=0,step=1;

	YsArray <double> tArray;
	for(int i=0; i<numDivision; ++i)
	{
		tArray.Append((double)ix);
		ix+=step;

		if(1<=i)
		{
			++step;
		}
		if(2<=i)
		{
			++step;
		}
		if(numDivision/2<i)
		{
			++step;
		}
		if(numDivision*2/3<i)
		{
			++step;
		}
		if(numDivision*5/6<i)
		{
			++step;
		}
	}

	for(int i=0; i<tArray.GetN(); ++i)
	{
		tArray[i]/=tArray.GetEnd();
	}

	ResampleFromUpperAndLower(upper,lower,tArray);
}

const double AirfoilNACA6::CalculateMeanLineData(const Chord &meanLine,const double percentChord) const
{
	YSSIZE_T n=0;
	while(meanLine.dat[n*2]<99.99)
	{
		++n;
	}
	++n;

	return YsCalculatePiecewiseLinearFunction(n,meanLine.dat,percentChord);
}
