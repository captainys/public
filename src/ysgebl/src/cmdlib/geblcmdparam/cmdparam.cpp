/* ////////////////////////////////////////////////////////////

File Name: cmdparam.cpp
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

#include "cmdparam.h"
#include "../geblworkorder/geblworkorder.h" // Needed for showing help.



void GeblCmd_CommandParameterInfo::CleanUp(void)
{
	inFile="";
	outFileArray.CleanUp();
	workOrderArray.CleanUp();
}

YSRESULT GeblCmd_CommandParameterInfo::RecognizeCommandParameter(int ac,char *av[])
{
	CleanUp();
	return AddCommandParameter(ac,av);
}
YSRESULT GeblCmd_CommandParameterInfo::AddCommandParameter(int ac,char *av[])
{

	if(2<=ac)
	{
		int ptr=1;
		while(ptr<ac)
		{
			YsString opt(av[ptr]);
			opt.Uncapitalize();
			if(0==strcmp("-h",opt) || 0==strcmp("-help",opt))
			{
				workOrderArray.Increment();
				workOrderArray.Last()="SHOWHELP";
				++ptr;
			}
			else if(0==strcmp("-in",opt) || 0==strcmp("-infile",opt))
			{
				if(ptr+1<ac)
				{
					inFile=av[ptr+1];
					ptr+=2;
				}
				else
				{
					printf("Option error!  File name is not given to %s option.\n",(const char *)av[ptr]);
					return YSERR;
				}
			}
			else if(0==strcmp("-out",opt) || 0==strcmp("-outfile",opt))
			{
				if(ptr+1<ac)
				{
					outFileArray.Increment();
					outFileArray.Last().fn=av[ptr+1];
					ptr+=2;
				}
				else
				{
					printf("Option error!  File name is not given to %s option.\n",(const char *)av[ptr]);
					return YSERR;
				}
			}
			else if(0==strcmp("-cmd",opt))
			{
				if(ptr+1<ac)
				{
					workOrderArray.Increment();
					workOrderArray.Last()=av[ptr+1];
					ptr+=2;
				}
				else
				{
					printf("Option error! Work order is not given to %s option.\n",(const char *)av[ptr]);
				}
			}
			else if(0==strcmp("-cmdfile",opt))
			{
				if(ptr+1<ac)
				{
					FILE *fp=fopen(av[ptr+1],"r");
					if(nullptr!=fp)
					{
						YsString str;
						while(nullptr!=str.Fgets(fp))
						{
							workOrderArray.Increment();
							workOrderArray.Last().MoveFrom(str);
						}
						fclose(fp);
						ptr+=2;
					}
					else
					{
						printf("Error!  Cannot open command file. [%s]\n",(const char *)av[ptr]);
						return YSERR;
					}
				}
				else
				{
					printf("Error!  Insufficient parameter. [%s]\n",(const char *)av[ptr]);
					return YSERR;
				}
			}
			else
			{
				printf("Error!  Unsupported option [%s]\n",(const char *)av[ptr]);
				return YSERR;
			}
		}
		return YSOK;
	}
	ShowHelp();
	return YSERR;
}

const YsString &GeblCmd_CommandParameterInfo::InputFileName(void) const
{
	return inFile;
}

const YsArray <YsString> &GeblCmd_CommandParameterInfo::WorkOrderArray(void) const
{
	return workOrderArray;
}

const YsArray <GeblCmd_CommandParameterInfo::OutputFileName> &GeblCmd_CommandParameterInfo::OutputFileNameArray(void) const
{
	return outFileArray;
}

/* static */ void GeblCmd_CommandParameterInfo::ShowHelp(void)
{
	printf("geblcmd help.\n");
	printf("<<Options>>\n");
	printf("  -h, -help, -H         Show help.\n");
	printf("  -in, -infile          Input file name.\n");
	printf("  -out, -outfile        Output file name. Can be multiple.  Only the ones that the file type is identified by the extension.\n");
	printf("  -cmd \"WORKORDER\"      Add work order string.  Work order will be executed in the order as given.\n");
	printf("  -cmdfile \"work.txt\"   Read work order string from \"work.txt\"\n");
}


