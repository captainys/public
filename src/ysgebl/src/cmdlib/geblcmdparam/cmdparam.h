/* ////////////////////////////////////////////////////////////

File Name: cmdparam.h
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

#ifndef CMDPARAMINFO_IS_INCLUDED
#define CMDPARAMINFO_IS_INCLUDED
/* { */

#include <ysclass.h>

class GeblCmd_CommandParameterInfo
{
private:
	class OutputFileName
	{
	public:
		YsString fn;
	};

	YsString inFile;
	YsArray <OutputFileName> outFileArray;

	YsArray <YsString> workOrderArray;

public:
	void CleanUp(void);
	YSRESULT RecognizeCommandParameter(int ac,char *av[]);
	YSRESULT AddCommandParameter(int ac,char *av[]);

	const YsString &InputFileName(void) const;
	const YsArray <YsString> &WorkOrderArray(void) const;
	const YsArray <OutputFileName> &OutputFileNameArray(void) const;

	static void ShowHelp(void);
};


/* } */
#endif
