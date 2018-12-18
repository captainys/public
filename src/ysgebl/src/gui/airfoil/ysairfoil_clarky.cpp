/* ////////////////////////////////////////////////////////////

File Name: ysairfoil_clarky.cpp
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

#include "ysairfoil_clarky_data.h"



void AirfoilClarkY::Generate(int numDivision)
{
	YsArray <YsVec3> upper,lower;

	const double numSample=YsAirfoil_ClarkY_numSample;
	const double *sample=YsAirfoil_ClarkY_sample;

	for(int j=0; j<=numSample-3; j+=3)
	{
		YsVec3 pos;
		pos.Set(sample[j],sample[j+1],0.0);
		upper.Append(pos);
		pos.Set(sample[j],sample[j+2],0.0);
		lower.Append(pos);
	}



	int ix=0,step=1;

	YsArray <double> tArray;
	for(int i=0; i<numDivision; ++i)
	{
		tArray.Append((double)ix);
		ix+=step;

		if(i<numDivision/2)
		{
			if(1<=i)
			{
				++step;
			}
			if(2<=i)
			{
				++step;
			}
		}
		else if(numDivision*2/3<i)
		{
			if(1<step)
			{
				--step;
			}
		}
	}

	for(int i=0; i<tArray.GetN(); ++i)
	{
		tArray[i]/=tArray.GetEnd();
	}

	ResampleFromUpperAndLower(upper,lower,tArray);
}
