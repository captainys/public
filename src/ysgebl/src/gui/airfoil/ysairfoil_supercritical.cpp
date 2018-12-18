/* ////////////////////////////////////////////////////////////

File Name: ysairfoil_supercritical.cpp
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

#include "ysairfoil_supercritical_data.h"


static struct Ysairfoil_Supercritical_Table_Struct
{
	const char *label;
	int numSample;
	const double *dat;
} Ysairfoil_Supercritical_Table[]=
{
	{"SC2-0010",YsAirfoil_SC2_0010_n,YsAirfoil_SC2_0010},
	{"SC2-0012",YsAirfoil_SC2_0012_n,YsAirfoil_SC2_0012},
	{"SC2-0402",YsAirfoil_SC2_0402_n,YsAirfoil_SC2_0402},
	{"SC2-0403",YsAirfoil_SC2_0403_n,YsAirfoil_SC2_0403},
	{"SC2-0404",YsAirfoil_SC2_0404_n,YsAirfoil_SC2_0404},
	{"SC2-0406",YsAirfoil_SC2_0406_n,YsAirfoil_SC2_0406},
	{"SC2-0410",YsAirfoil_SC2_0410_n,YsAirfoil_SC2_0410},
	{"SC2-0412",YsAirfoil_SC2_0412_n,YsAirfoil_SC2_0412},
	{"SC2-0414",YsAirfoil_SC2_0414_n,YsAirfoil_SC2_0414},
	{"SC2-0503",YsAirfoil_SC2_0503_n,YsAirfoil_SC2_0503},
	{"SC2-0518",YsAirfoil_SC2_0518_n,YsAirfoil_SC2_0518},
	{"SC2-0606",YsAirfoil_SC2_0606_n,YsAirfoil_SC2_0606},
	{"SC2-0610",YsAirfoil_SC2_0610_n,YsAirfoil_SC2_0610},
	{"SC2-0612",YsAirfoil_SC2_0612_n,YsAirfoil_SC2_0612},
	{"SC2-0614",YsAirfoil_SC2_0614_n,YsAirfoil_SC2_0614},
	{"SC2-0706",YsAirfoil_SC2_0706_n,YsAirfoil_SC2_0706},
	{"SC2-0710",YsAirfoil_SC2_0710_n,YsAirfoil_SC2_0710},
	{"SC2-0712",YsAirfoil_SC2_0712_n,YsAirfoil_SC2_0712},
	{"SC2-0714",YsAirfoil_SC2_0714_n,YsAirfoil_SC2_0714},
	{"SC2-1006",YsAirfoil_SC2_1006_n,YsAirfoil_SC2_1006},
	{"SC2-1010",YsAirfoil_SC2_1010_n,YsAirfoil_SC2_1010},
};

/*static*/ void AirfoilSuperCritical::GetCatalogue(YsArray <const char *> &catalogue)
{
	catalogue.CleanUp();

	const int n=sizeof(Ysairfoil_Supercritical_Table)/sizeof(struct Ysairfoil_Supercritical_Table_Struct);
	for(int i=0; i<n; ++i)
	{
		catalogue.Append(Ysairfoil_Supercritical_Table[i].label);
	}
}

void AirfoilSuperCritical::Generate(int numDivision,const char typeLabel[])
{
	YsArray <YsVec3> upper,lower;

	const int n=sizeof(Ysairfoil_Supercritical_Table)/sizeof(struct Ysairfoil_Supercritical_Table_Struct);

	for(int i=0; i<n; ++i)
	{
		if(0==strcmp(Ysairfoil_Supercritical_Table[i].label,typeLabel))
		{
			const double numSample=Ysairfoil_Supercritical_Table[i].numSample;
			const double *sample=Ysairfoil_Supercritical_Table[i].dat;

			for(int j=0; j<=numSample-3; j+=3)
			{
				YsVec3 pos;
				pos.Set(sample[j],sample[j+1],0.0);
				upper.Append(pos);
				pos.Set(sample[j],sample[j+2],0.0);
				lower.Append(pos);
			}
			break;
		}
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
		else
		{
			if(2<step)
			{
				step-=2;
			}
			if(numDivision*2/3<i && 1<step)
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

