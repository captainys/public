/* ////////////////////////////////////////////////////////////

File Name: ysshellkdtree.h
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

#ifndef YSSHELLKDTREE_IS_INCLUDED
#define YSSHELLKDTREE_IS_INCLUDED
/* { */

class YsShell3dTree : protected Ys3dTree <YsShellVertexHandle>
{
public:
	using Ys3dTree <YsShellVertexHandle>::GetRootNode;
	using Ys3dTree <YsShellVertexHandle>::PreAllocateNode;

	YSRESULT Build(const class YsShell &shl,const class YsShellVertexStore *limitedVtx=NULL,const class YsShellVertexStore *excludedVtx=NULL);
	YSRESULT AddVertex(const class YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT MakeVertexList(YsArray <YsShellVertexHandle,128> &lst,const YsVec3 &min,const YsVec3 &max) const;

	inline YSRESULT ThisIsADummyFunctionAndMustNeverBeCalledButNecessaryForOmittingOneOfStupicCompilerWarnings(void);

protected:
	virtual YSRESULT Enumerate(YsKdTreeEnumParam *enumParam,const YsVec3 &pos,const YsShellVertexHandle &attrib) const;
};

inline YSRESULT YsShell3dTree::ThisIsADummyFunctionAndMustNeverBeCalledButNecessaryForOmittingOneOfStupicCompilerWarnings(void)
{
	YsVec3 pos=YsOrigin();
	YsShellVertexHandle vtHd=NULL;
	YsKdTreeEnumParam *enumParam=NULL;
	return Ys3dTree <YsShellVertexHandle>::Enumerate(enumParam,pos,vtHd);
}

/* } */
#endif
