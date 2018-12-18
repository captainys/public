/* ////////////////////////////////////////////////////////////

File Name: ysshellext_commandsupport.h
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

#ifndef YSSHELLEXT_COMMANDSUPPORT_IS_INCLUDED
#define YSSHELLEXT_COMMANDSUPPORT_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_CommandSupport
{
protected:
	int colorTolerance;
	int nThread;

public:
	YsShellExt_CommandSupport();
	void Initialize(void);

	void SetColorTolerance(int colorTolerance);
	int GetColorTolerance(void) const;

	void SetNumThread(int nt);
	int GetNumThread(void) const;

	/*! Argv[idx] stores the type label followed by identifier label(s).

	    After this function, idx will be updated so that it points to the next argument.

	    Type label can be "NAME", "IDENT", or "COLOR".
	    If the type label is "COLOR", identifier label is "RRGGBB" in the hexa-decimal number.
	    It only checks the color of the first polygon in the face group.  If a face group is painted in multiple colors, the result may not match the expectation.
	*/
	YsArray <YsShellExt::FaceGroupHandle> GetFaceGroupHandle(const YsShellExt &shl,YSSIZE_T &idx,const YsConstArrayMask <YsString> &argv) const;
};


/* } */
#endif
