/* ////////////////////////////////////////////////////////////

File Name: ystextresource.cpp
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

#if (defined(_WIN32) || defined(_WIN64)) && !defined(_CRT_SECURE_NO_WARNINGS)
	// This disables annoying warnings VC++ gives for use of C standard library.  Shut the mouth up.
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ystextresource.h"

class YsTextResourceBinaryTree : public YsStringBinaryTree <YsString,YsWString>
{
};

YsTextResource::YsTextResource()
{
	tree=new YsTextResourceBinaryTree;
	Initialize();
}

YsTextResource::~YsTextResource()
{
	delete tree;
}

void YsTextResource::Initialize(void)
{
	tree->CleanUp();
}


YSRESULT YsTextResource::LoadFile(const char fn[])
{
	FILE *fp=fopen(fn,"r");
	YSRESULT res=LoadFile(fp);
	if(NULL!=fp)
	{
		fclose(fp);
	}
	return res;
}

YSRESULT YsTextResource::LoadFile(FILE *fp)
{
	if(NULL!=fp)
	{
		Initialize();

		YSRESULT res=YSOK;
		YsString buf;
		while(NULL!=buf.Fgets(fp))
		{
			while(0<buf.Strlen() && (buf.LastChar()=='\n' || buf.LastChar()=='\r'))
			{
				buf.BackSpace();
			}

			if(YSOK!=SendCommand(buf))
			{
				res=YSERR;
			}
		}
		return res;
	}
	return YSERR;
}

YSRESULT YsTextResource::SendCommand(const YsString &cmd)
{
	if(0==cmd.Strlen())
	{
		return YSOK;
	}

	switch(cmd[0])
	{
	case '*':
		return YSOK;
	case '$':
		return SendDollarCommand(cmd);
	case '+':
		return SendDictCommand(cmd);
	}
	return YSOK;
}

YSRESULT YsTextResource::SendDollarCommand(const YsString &cmd)
{
	YsArray <YsString,16> args;
	cmd.Arguments <16> (args);
	if(0<args.GetN())
	{
		args[0].Capitalize();
		if(0==strcmp(args[0],"$FGCOL"))
		{
			if(2==args.GetN())
			{
				fgCol.SetIntRGB(atoi(args[1]),atoi(args[1]),atoi(args[1]));
			}
			else if(4<=args.GetN())
			{
				fgCol.SetIntRGB(atoi(args[1]),atoi(args[2]),atoi(args[3]));
			}
		}
		else if(0==strcmp(args[0],"$BGCOL"))
		{
			if(2==args.GetN())
			{
				bgCol.SetIntRGB(atoi(args[1]),atoi(args[1]),atoi(args[1]));
			}
			else if(4<=args.GetN())
			{
				bgCol.SetIntRGB(atoi(args[1]),atoi(args[2]),atoi(args[3]));
			}
		}
		else if(0==strcmp(args[0],"$LABEL"))
		{
			if(2<=args.GetN())
			{
				label=args[1];
			}
		}
	}
	return YSOK;
}

YSRESULT YsTextResource::SendDictCommand(const YsString &cmd)
{
	YsString key,msg;
	int i;
	for(i=1; i<cmd.Strlen(); i++)
	{
		if('#'==cmd[i])
		{
			break;
		}
		key.Append(cmd[i]);
	}

	if('#'!=cmd[i])
	{
		return YSERR;
	}

	for(i=i+1; i<cmd.Strlen(); i++)
	{
		if('\\'==cmd[i] && 'n'==cmd[i+1])
		{
			msg.Append('\n');
			i++;
		}
		else if('\\'==cmd[i] && 't'==cmd[i+1])
		{
			msg.Append('\t');
			i++;
		}
		else
		{
			msg.Append(cmd[i]);
		}
	}

	key.DeleteTailSpace();

	YsWString wMsg;
	wMsg.SetUTF8String(msg);

	tree->AddNode(key,wMsg);

	return YSOK;
}

const wchar_t *YsTextResource::FindWString(const char key[])
{
	YsString ysKey(key);
	const YsWString *msg=tree->FindData(ysKey);
	if(NULL!=msg)
	{
		return msg->Txt();
	}
	return NULL;
}
