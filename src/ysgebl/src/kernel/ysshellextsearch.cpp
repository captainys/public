/* ////////////////////////////////////////////////////////////

File Name: ysshellextsearch.cpp
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

#include "ysshellextsearch.h"


YsShellExt::SearchTable::SearchTable()
{
	shl=NULL;
	CleanUp();
}

YsShellExt::SearchTable::~SearchTable()
{
}


void YsShellExt::SearchTable::CleanUp(void)
{
	shl=NULL;
	vtKeyToCeHd.CleanUp();
	plKeyToFgHd.CleanUp();
}

void YsShellExt::SearchTable::Attach(const YsShellExt *shl)
{
	CleanUp();
	this->shl=shl;

	YsShellExt::ConstEdgeHandle ceHd=NULL;
	while(YSOK==shl->MoveToNextConstEdge(ceHd))
	{
		ConstEdgeAdded(ceHd);
	}

	plKeyToFgHd.Resize(1+shl->GetNumPolygon()/4);
	YsShellExt::FaceGroupHandle fgHd=NULL;
	while(YSOK==shl->MoveToNextFaceGroup(fgHd))
	{
		FaceGroupAdded(fgHd);
	}

	YsShellExt::VolumeHandle vlHd=NULL;
	while(YSOK==shl->MoveToNextVolume(vlHd))
	{
		VolumeAdded(vlHd);
	}
}

void YsShellExt::SearchTable::Detach(void)
{
	CleanUp();
	this->shl=NULL;
}

void YsShellExt::SearchTable::ConstEdgeAdded(YsShellExt::ConstEdgeHandle ceHd)
{
	// I am checking this pointer against NULL just in case, and cannot stop clang from complaining about it.
	// I KNOW IT!  SHUT UP!
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		YSSIZE_T nCeVt;
		const YsShellVertexHandle *ceVtHd;
		YSBOOL isLoop;
		if(YSOK==shl->GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd))
		{
			ConstEdgeVertexAdded(ceHd,nCeVt,ceVtHd);
		}
	}
}

void YsShellExt::SearchTable::ConstEdgeVertexAdded(YsShellExt::ConstEdgeHandle ceHd,const YSSIZE_T nCeVt,const YsShellVertexHandle ceVtHd[])
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		for(YSSIZE_T idx=0; idx<nCeVt; ++idx)
		{
			unsigned int key[1]=
			{
				shl->GetSearchKey(ceVtHd[idx])
			};
			vtKeyToCeHd.AddElement(1,key,ceHd);
		}
	}
}

void YsShellExt::SearchTable::ConstEdgeAboutToBeDeleted(YsShellExt::ConstEdgeHandle ceHd)
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		YSSIZE_T nCeVt;
		const YsShellVertexHandle *ceVtHd;
		YSBOOL isLoop;
		if(YSOK==shl->GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd))
		{
			for(YSSIZE_T idx=0; idx<nCeVt; ++idx)
			{
				unsigned int key[1]=
				{
					shl->GetSearchKey(ceVtHd[idx])
				};
				vtKeyToCeHd.DeleteElement(1,key,ceHd);
			}
		}
	}
}


void YsShellExt::SearchTable::FaceGroupAdded(YsShellExt::FaceGroupHandle fgHd)
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		for(auto plHd : shl->GetFaceGroupRaw(fgHd))
		{
			plKeyToFgHd.UpdateElement(shl->GetSearchKey(plHd),fgHd);
		}
	}
}

void YsShellExt::SearchTable::FaceGroupAboutToBeDeleted(YsShellExt::FaceGroupHandle fgHd)
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		for(auto plHd : shl->GetFaceGroupRaw(fgHd))
		{
			plKeyToFgHd.DeleteElement(shl->GetSearchKey(plHd),fgHd);
		}
	}
}

void YsShellExt::SearchTable::PolygonAddedToFaceGroup(YsShellExt::FaceGroupHandle fgHd,YsShellPolygonHandle plHd)
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		plKeyToFgHd.UpdateElement(shl->GetSearchKey(plHd),fgHd);
	}
}

void YsShellExt::SearchTable::PolygonRemovedFromFaceGroup(YsShellExt::FaceGroupHandle fgHd,YsShellPolygonHandle plHd)
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		plKeyToFgHd.DeleteElement(shl->GetSearchKey(plHd),fgHd);
	}
}

void YsShellExt::SearchTable::MultiplePolygonAddedToFaceGroup(YsShellExt::FaceGroupHandle fgHd,YSSIZE_T nPl,const YsShellPolygonHandle plHd[])
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		for(YSSIZE_T idx=0; idx<nPl; ++idx)
		{
			plKeyToFgHd.UpdateElement(shl->GetSearchKey(plHd[idx]),fgHd);
		}
	}
}

void YsShellExt::SearchTable::MultiplePolygonRemovedFromFaceGroup(YsShellExt::FaceGroupHandle fgHd,YSSIZE_T nPl,YsShellPolygonHandle plHd[])
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		for(YSSIZE_T idx=0; idx<nPl; ++idx)
		{
			plKeyToFgHd.DeleteElement(shl->GetSearchKey(plHd[idx]),fgHd);
		}
	}
}


void YsShellExt::SearchTable::VolumeAdded(YsShellExt::VolumeHandle vlHd)
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		YSSIZE_T nVlFg;
		const YsShellExt::FaceGroupHandle *vlFgHd;
		if(YSOK==shl->GetVolume(nVlFg,vlFgHd,vlHd))
		{
			for(YSSIZE_T idx=0; idx<nVlFg; ++idx)
			{
				unsigned int key[1]=
				{
					shl->GetSearchKey(vlFgHd[idx])
				};
				fgKeyToVlHd.AddElement(1,key,vlHd);
			}
		}
	}
}

void YsShellExt::SearchTable::VolumeAboutToBeDeleted(YsShellExt::VolumeHandle vlHd)
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		YSSIZE_T nVlFg;
		const YsShellExt::FaceGroupHandle *vlFgHd;
		if(YSOK==shl->GetVolume(nVlFg,vlFgHd,vlHd))
		{
			for(YSSIZE_T idx=0; idx<nVlFg; ++idx)
			{
				unsigned int key[1]=
				{
					shl->GetSearchKey(vlFgHd[idx])
				};
				fgKeyToVlHd.DeleteElement(1,key,vlHd);
			}
		}
	}
}

void YsShellExt::SearchTable::MultipleFaceGroupAddedToVolume(YsShellExt::VolumeHandle vlHd,YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHd[])
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		for(YSSIZE_T fgIdx=0; fgIdx<nFg; ++fgIdx)
		{
			unsigned int key[1]=
			{
				shl->GetSearchKey(fgHd[fgIdx])
			};
			fgKeyToVlHd.AddElement(1,key,vlHd);
		}
	}
}

YSRESULT YsShellExt::SearchTable::FindConstEdgeFromVertex(YSSIZE_T &nCe,const YsShellExt::ConstEdgeHandle *&ceHdPtr,YsShellVertexHandle vtHd) const
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		unsigned int n;
		YSHASHKEY key[1]={shl->GetSearchKey(vtHd)};
		if(YSOK==vtKeyToCeHd.FindElement(n,ceHdPtr,1,key))
		{
			nCe=n;
			return YSOK;
		}
	}
	nCe=0;
	ceHdPtr=NULL;
	return YSERR;
}

YSRESULT YsShellExt::SearchTable::FindConstEdgeFromVertex(YSSIZE_T &nCe,const YsShellExt::ConstEdgeHandle *&ceHdPtr,YSHASHKEY vtKey) const
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		unsigned int n;
		YSHASHKEY key[1]={vtKey};
		if(YSOK==vtKeyToCeHd.FindElement(n,ceHdPtr,1,key))
		{
			nCe=n;
			return YSOK;
		}
	}
	nCe=0;
	ceHdPtr=NULL;
	return YSERR;
}

YsShellExt::FaceGroupHandle YsShellExt::SearchTable::FindFaceGroupFromPolygon(YsShellPolygonHandle plHd) const
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		YsShellExt::FaceGroupHandle fgHd;
		if(YSOK==plKeyToFgHd.FindElement(fgHd,shl->GetSearchKey(plHd)))
		{
			return fgHd;
		}
	}
	return NULL;
}

YSRESULT YsShellExt::SearchTable::FindVolumeFromFaceGroup(YSSIZE_T &nVl,const YsShellExt::VolumeHandle *&fgVlHd,YsShellExt::FaceGroupHandle fgHd) const
{
	auto thisPtr=this;
	if(NULL!=thisPtr && NULL!=shl)
	{
		unsigned int key[1]=
		{
			shl->GetSearchKey(fgHd)
		};
		return fgKeyToVlHd.FindElement(nVl,fgVlHd,1,key);
	}
	nVl=0;
	return YSERR;
}

