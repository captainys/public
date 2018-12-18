/* ////////////////////////////////////////////////////////////

File Name: ysshellext_mappingutil.h
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

#ifndef YSSHELLEXT_MAPPINGUTIL_IS_INCLUDED
#define YSSHELLEXT_MAPPINGUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

// From can be anything that can be given to shl.GetSearchKey.
template <class From,class toFind>
class YsShellExt_Mapping
{
public:
	class Mapping
	{
	public:
		From from;
		YsArray <toFind> mapped;
	};

private:
	YsSegmentedArray <Mapping,4> mapArray;
	YsHashTable <YSSIZE_T> mapIndexSearch;

public:
	void CleanUp(void);
	YSRESULT AddMapping(const YsShellExt &shl,From from,const toFind &mapped);
	YSRESULT DeleteMapping(const YsShellExt &shl,From from);
	YSBOOL CheckMappingExist(const YsShellExt &shl,From from) const;

	template <const int N>
	YSRESULT FindMapping(YsArray <toFind,N> &found,const YsShellExt &shl,From from) const;
};

template <class From,class toFind>
void YsShellExt_Mapping <From,toFind>::CleanUp(void)
{
	mapArray.CleanUp();
	mapIndexSearch.CleanUp();
}

template <class From,class toFind>
YSRESULT YsShellExt_Mapping <From,toFind>::AddMapping(const YsShellExt &shl,From from,const toFind &mapped)
{
	YSSIZE_T mapIdx;
	if(YSOK!=mapIndexSearch.FindElement(mapIdx,shl.GetSearchKey(from)))
	{
		mapIdx=mapArray.GetN();
		mapArray.Increment();
		mapArray[mapIdx].from=from;
		mapArray[mapIdx].mapped.CleanUp();
		mapIndexSearch.AddElement(shl.GetSearchKey(from),mapIdx);
	}
	mapArray[mapIdx].mapped.Append(mapped);
	return YSOK;
}

template <class From,class toFind>
YSRESULT YsShellExt_Mapping <From,toFind>::DeleteMapping(const YsShellExt &shl,From from)
{
	YSSIZE_T mapIdx;
	if(YSOK==mapIndexSearch.FindElement(mapIdx,shl.GetSearchKey(from)) && YSTRUE==mapArray.IsInRange(mapIdx))
	{
		mapArray[mapIdx].from=from;
		mapArray[mapIdx].mapped.CleanUp();
	}
	return YSOK;
}

template <class From,class toFind>
YSBOOL YsShellExt_Mapping <From,toFind>::CheckMappingExist(const YsShellExt &shl,From from) const
{
	YSSIZE_T mapIdx;
	if(YSOK==mapIndexSearch.FindElement(mapIdx,shl.GetSearchKey(from)) && 0<mapArray[mapIdx].mapped.GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class From,class toFind>
template <const int N>
YSRESULT YsShellExt_Mapping <From,toFind>::FindMapping(YsArray <toFind,N> &found,const YsShellExt &shl,From from) const
{
	YSSIZE_T mapIdx;
	if(YSOK==mapIndexSearch.FindElement(mapIdx,shl.GetSearchKey(from)))
	{
		found=mapArray[mapIdx].mapped;
		return YSOK;
	}
	else
	{
		found.CleanUp();
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

class YsShellExt_VertexToVertexMapping : public YsShellExt_Mapping <YsShellVertexHandle,YsShellVertexHandle>
{
public:
	template <const int N>
	YSRESULT MapVertexArray(YsArray <YsShellVertexHandle,N> &vtHdArray,const YsShellExt &shl) const;
};

template <const int N>
YSRESULT YsShellExt_VertexToVertexMapping::MapVertexArray(YsArray <YsShellVertexHandle,N> &vtHdArray,const YsShellExt &shl) const
{
	YsArray <YsShellVertexHandle,2> mappedVtHdArray;
	for(auto &vtHd : vtHdArray)
	{
		if(YSOK!=FindMapping(mappedVtHdArray,shl,vtHd) ||
		   1!=mappedVtHdArray.GetN())
		{
			return YSERR;
		}
		vtHd=mappedVtHdArray[0];
	}
	return YSOK;
}


////////////////////////////////////////////////////////////

YSRESULT YsShellExt_MakeVertexMappingWithTolerance(YsShellExt_VertexToVertexMapping &vtxMap,const YsShellExt &shl,const double tol,const YsShellVertexStore *limitedVertex=NULL,const YsShellVertexStore *excludedVertex=NULL);

/* } */
#endif
