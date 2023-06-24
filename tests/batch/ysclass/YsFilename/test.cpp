/* ////////////////////////////////////////////////////////////

File Name: test.cpp
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

#include <ysclass.h>

//test scenario: replace filename extension with separator ('.') included
YSRESULT YsFilename_ReplaceExtensionSeparatorIncludedTest(void)
{
	char filename[] = "filename.txt";
	char replaceExt[] = ".xyz";
	char filenameExpected[] = "filename.xyz";
	YsReplaceExtension(filename, replaceExt);

	if (strcmp(filename, filenameExpected) != 0)
	{
		return YSERR;
	}
	return YSOK;
}

//test scenario: replace filename extension without separator ('.') included
YSRESULT YsFilename_ReplaceExtensionSeparatorNotIncludedTest(void)
{
	char filename[] = "filename.txt";
	char replaceExt[] = "xyz";
	char filenameExpected[] = "filename.xyz";
	YsReplaceExtension(filename, replaceExt);

	if (strcmp(filename, filenameExpected) != 0)
	{
		return YSERR;
	}
	return YSOK;
}

int main(void)
{
	int nFail = 0;
	if (YSOK != YsFilename_ReplaceExtensionSeparatorIncludedTest())
	{
		++nFail;
	}

	if (YSOK != YsFilename_ReplaceExtensionSeparatorNotIncludedTest())
	{
		++nFail;
	}

	printf("%d failed.\n", nFail);
	if (0 < nFail)
	{
		return 1;
	}
	return 0;
}
