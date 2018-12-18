/* ////////////////////////////////////////////////////////////

File Name: config.cpp
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

#include "polycreconfig.h"

#include <ysport.h>

PolyCreConfig::PolyCreConfig()
{
	Initialize();
}

PolyCreConfig::~PolyCreConfig()
{
}

/*static*/ PolyCreConfig *PolyCreConfig::Create(void)
{
	return new PolyCreConfig;
}

/*statid*/ void PolyCreConfig::Delete(PolyCreConfig *ptr)
{
	delete ptr;
}

void PolyCreConfig::Initialize(void)
{
	SetDefault();
}

void PolyCreConfig::SetDefault(void)
{
	coordSys=YSOPENGL;
	YsSpecialPath::GetUserDocDir(defDataDir);
	perspective=YSFALSE;
	warnSaveAsSTL=YSTRUE;
	saveBinarySTL=YSTRUE;

	saveWindowPositionAndSize=YSTRUE;

	autoFixOrientationInDnm=YSTRUE;

	autoAdjustTolerance=YSTRUE;

	bgColor[0].SetDoubleRGBA(0.80,0.90,1.00,1.0);
	bgColor[1]=YsWhite();
	bgColor[2].SetDoubleRGBA(0.75,0.75,0.75,1.0);

	specular[0]=1.0f;
	specular[1]=1.0f;
	specular[2]=1.0f;

	ambient[0]=0.0f;
	ambient[1]=0.0f;
	ambient[2]=0.0f;

	lightColor[0]=1.0f;
	lightColor[1]=1.0f;
	lightColor[2]=1.0f;
}

const float *PolyCreConfig::GetSpecularColor(void) const
{
	return specular;
}

void PolyCreConfig::SetSpecularColor(const float specular[3])
{
	this->specular[0]=specular[0];
	this->specular[1]=specular[1];
	this->specular[2]=specular[2];
}

const float *PolyCreConfig::GetAmbientColor(void) const
{
	return ambient;
}

void PolyCreConfig::SetAmbientColor(const float ambient[3])
{
	this->ambient[0]=ambient[0];
	this->ambient[1]=ambient[1];
	this->ambient[2]=ambient[2];
}

const float *PolyCreConfig::GetLightColor(void) const
{
	return lightColor;
}

void PolyCreConfig::SetLightColor(const float lightCol[3])
{
	this->lightColor[0]=lightCol[0];
	this->lightColor[1]=lightCol[1];
	this->lightColor[2]=lightCol[2];
}

YSBOOL PolyCreConfig::GetWarnSaveAsSTL(void) const
{
	return warnSaveAsSTL;
}
void PolyCreConfig::SetWarnSaveAsSTL(YSBOOL warn)
{
	this->warnSaveAsSTL=warn;
}

YSBOOL PolyCreConfig::SaveBinarySTL(void) const
{
	return saveBinarySTL;
}
void PolyCreConfig::SetSaveBinarySTL(YSBOOL binStl)
{
	this->saveBinarySTL=binStl;
}

YSRESULT PolyCreConfig::Load(YsTextInputStream &inStream)
{
	YsString str;
	YsArray <YsString,16> args;
	while(NULL!=inStream.Gets(str))
	{
		if(YSOK==str.Arguments(args) && 0<args.GetN())
		{
			if(0==strcmp(args[0],"COORDSYS") && 2<=args.GetN())
			{
				if(0==strcmp(args[1],"LEFT"))
				{
					coordSys=YSLEFT_ZPLUS_YPLUS;
				}
				else if(0==strcmp(args[1],"RIGHT"))
				{
					coordSys=YSRIGHT_ZMINUS_YPLUS;
				}
			}
			else if(0==strcmp(args[0],"PERSPECTIVE") && 2<=args.GetN())
			{
				perspective=YsStrToBool(args[1]);
			}
			else if(0==strcmp(args[0],"DATADIR") && 2<=args.GetN())
			{
				defDataDir.SetUTF8String(args[1]);
			}
			else if(0==strcmp(args[0],"BGCOLOR") && 5<=args.GetN())
			{
				const int colorIdx=atoi(args[1]);
				if(0<=colorIdx && colorIdx<3)
				{
					bgColor[colorIdx].SetDoubleRGB(atof(args[2]),atof(args[3]),atof(args[4]));
				}
			}
			else if(0==strcmp(args[0],"WARNSAVEASSTL") && 2<=args.GetN())
			{
				warnSaveAsSTL=YsStrToBool(args[1]);
			}
			else if(0==strcmp(args[0],"SAVEWINDOWPOS") && 2<=args.GetN())
			{
				saveWindowPositionAndSize=YsStrToBool(args[1]);
			}
			else if(0==args[0].Strcmp("AUTOFIXDNMPLG") && 2<=args.GetN())
			{
				autoFixOrientationInDnm=YsStrToBool(args[1]);
			}
			else if(0==args[0].Strcmp("AUTOTOLERANCE") && 2<=args.GetN())
			{
				autoAdjustTolerance=YsStrToBool(args[1]);
			}
			else if(0==args[0].STRCMP("SAVEBINSTL") && 2<=args.GetN())
			{
				saveBinarySTL=YsStrToBool(args[1]);
			}
			// else if(0==strcmp(args[0],"SPECULAR") && 5<=args.GetN())
			// {
			// 	specular[0]=(float)atof(args[1]);
			// 	specular[1]=(float)atof(args[2]);
			// 	specular[2]=(float)atof(args[3]);
			// 	specular[3]=(float)atof(args[4]);
			// }
		}
	}
	return YSOK;
}

YSRESULT PolyCreConfig::Load(const wchar_t fn[])
{
	FILE *fp=YsFileIO::Fopen(fn,"r");
	if(NULL!=fp)
	{
		YsTextFileInputStream inStream(fp);
		Load(inStream);
		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

YSRESULT PolyCreConfig::Save(YsTextOutputStream &outStream) const
{
	switch(coordSys)
	{
	case YSLEFT_ZPLUS_YPLUS:
		outStream.Printf("COORDSYS LEFT\n");
		break;
	case YSRIGHT_ZMINUS_YPLUS:
		outStream.Printf("COORDSYS RIGHT\n");
		break;
	}

	outStream.Printf("PERSPECTIVE %s\n",YsBoolToStr(perspective));

	{
		YsString utf8;
		utf8.EncodeUTF8 <wchar_t> (defDataDir);
		outStream.Printf("DATADIR \"%s\"\n",utf8.Txt());
	}

	outStream.Printf("WARNSAVEASSTL %s\n",YsBoolToStr(warnSaveAsSTL));

	outStream.Printf("BGCOLOR 0 %.2lf %.2lf %.2lf\n",bgColor[0].Rd(),bgColor[0].Gd(),bgColor[0].Bd());
	outStream.Printf("BGCOLOR 1 %.2lf %.2lf %.2lf\n",bgColor[1].Rd(),bgColor[1].Gd(),bgColor[1].Bd());
	outStream.Printf("BGCOLOR 2 %.2lf %.2lf %.2lf\n",bgColor[2].Rd(),bgColor[2].Gd(),bgColor[2].Bd());

	outStream.Printf("SAVEWINDOWPOS %s\n",YsBoolToStr(saveWindowPositionAndSize));

	outStream.Printf("AUTOFIXDNMPLG %s\n",YsBoolToStr(autoFixOrientationInDnm));

	outStream.Printf("AUTOTOLERANCE %s\n",YsBoolToStr(autoAdjustTolerance));

	outStream.Printf("SAVEBINSTL %s\n",YsBoolToStr(saveBinarySTL));

	// outStream.Printf("SPECULAR %.3lf %.3lf %.3lf %.3lf\n",specular[0],specular[1],specular[2],specular[3]);

	return YSOK;
}

YSRESULT PolyCreConfig::Save(const wchar_t fn[])
{
	FILE *fp=YsFileIO::Fopen(fn,"w");
	if(NULL!=fp)
	{
		YsTextFileOutputStream outStream(fp);
		Save(outStream);
		fclose(fp);
		return YSOK;
	}
	return YSERR;
}


