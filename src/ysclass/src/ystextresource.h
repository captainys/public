/* ////////////////////////////////////////////////////////////

File Name: ystextresource.h
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

#ifndef YSTEXTRESOURCE_IS_INCLUDED
#define YSTEXTRESOURCE_IS_INCLUDED
/* { */

#include "ysstring.h"
#include "ysproperty.h"
#include "ysbinarytree.h"

class YsTextResource
{
private:
	YsString label;
	YsColor fgCol,bgCol;

	class YsTextResourceBinaryTree *tree;
	// I want to write YsStringBinaryTree directly here, but if I do so,
	// Visual C++ 2008 gives a warning that totally doesn't make sense:
	//     ysbinarytree.h(371) : warning C4505: 'YsStringBinaryTree<KEYTYPE,ASSOCTYPE>::Compare' : 
	//     unreferenced local function has been removed
	// Compare is a template function and if it is not used, it is deleted.  Period.
	// I say Visual Studio 2008 is wrongfully accusing this function.
public:
	YsTextResource();
	virtual ~YsTextResource();
	void Initialize(void);

	YSRESULT LoadFile(const char fn[]);
	YSRESULT LoadFile(FILE *fp);
protected:
	YSRESULT SendCommand(const YsString &cmd);
	YSRESULT SendDollarCommand(const YsString &cmd);
	YSRESULT SendDictCommand(const YsString &cmd);

public:
	const wchar_t *FindWString(const char key[]);
};

/* } */
#endif
