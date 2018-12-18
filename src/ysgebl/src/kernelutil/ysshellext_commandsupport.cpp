/* ////////////////////////////////////////////////////////////

File Name: ysshellext_commandsupport.cpp
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

#include "ysshellext_commandsupport.h"
#include "ysshellext_proximityutil.h"



YsShellExt_CommandSupport::YsShellExt_CommandSupport()
{
	Initialize();
}
void YsShellExt_CommandSupport::Initialize(void)
{
	colorTolerance=3;
	nThread=8;
}
void YsShellExt_CommandSupport::SetColorTolerance(int colorTolerance)
{
	this->colorTolerance=colorTolerance;
}
int YsShellExt_CommandSupport::GetColorTolerance(void) const
{
	return colorTolerance;
}

void YsShellExt_CommandSupport::SetNumThread(int nt)
{
	nThread=nt;
}
int YsShellExt_CommandSupport::GetNumThread(void) const
{
	return nThread;
}

YsArray <YsShellExt::FaceGroupHandle> YsShellExt_CommandSupport::GetFaceGroupHandle(const YsShellExt &shl,YSSIZE_T &idx,const YsConstArrayMask <YsString> &argv) const
{
	YsArray <YsShellExt::FaceGroupHandle> fgHdArray;

	if(argv.size()<=idx)
	{
		return fgHdArray;
	}
	const auto &typeLabel=argv[idx];
	if(0==typeLabel.STRCMP("ALL"))
	{
		++idx;
		for(auto fgHd : shl.AllFaceGroup())
		{
			fgHdArray.push_back(fgHd);
		}
	}
	else if(0==typeLabel.STRCMP("NEARPOS"))
	{
		if(idx+4<=argv.size())
		{
			double x=atof(argv[idx+1]);
			double y=atof(argv[idx+2]);
			double z=atof(argv[idx+3]);

			YsShellExt::PassAll cond;
			double dist;
			YsVec3 nearPos;
			auto plHd=YsShellExt_FindNearestPolygon(dist,nearPos,shl,YsVec3(x,y,z),cond);
			auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
			if(nullptr!=fgHd)
			{
				fgHdArray.push_back(fgHd);
			}
			idx+=4;
		}
		else
		{
			idx=argv.size();
		}
	}
	else
	{
		const auto &identifierLabel=argv[idx+1];
		idx+=2;
		for(auto fgHd : shl.AllFaceGroup())
		{
			if(0==typeLabel.STRCMP("NAME"))
			{
				if(0==identifierLabel.Strcmp(shl.GetFaceGroupLabel(fgHd)))
				{
					fgHdArray.Append(fgHd);
				}
			}
			else if(0==typeLabel.STRCMP("IDENT"))
			{
				if(atoi(identifierLabel)==shl.GetFaceGroupIdent(fgHd))
				{
					fgHdArray.Append(fgHd);
				}
			}
			else if(0==typeLabel.STRCMP("COLOR"))
			{
				const int code=strtol(identifierLabel,NULL,16);
				const int r=(code>>16)&255;
				const int g=(code>> 8)&255;
				const int b=(code    )&255;

				auto fgPlHd=shl.GetFaceGroup(fgHd);
				auto col=shl.GetColor(fgPlHd[0]);

				const int rDiff=YsAbs(col.Ri()-r);
				const int gDiff=YsAbs(col.Gi()-g);
				const int bDiff=YsAbs(col.Bi()-b);
				if(rDiff<=colorTolerance && gDiff<=colorTolerance && bDiff<=colorTolerance)
				{
					fgHdArray.Append(fgHd);
				}
			}
		}
	}
	return fgHdArray;
}

