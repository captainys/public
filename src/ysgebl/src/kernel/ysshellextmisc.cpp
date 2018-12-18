/* ////////////////////////////////////////////////////////////

File Name: ysshellextmisc.cpp
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

#include "ysshellextmisc.h"



YsShellExt::ConstEdgeStore::ConstEdgeStore()
{
	this->shl=NULL;
	EnableAutoResizing();
}

YsShellExt::ConstEdgeStore::ConstEdgeStore(const YsShellExt &shl)
{
	SetShell(shl);
	EnableAutoResizing();
}

YsShellExt::ConstEdgeStore::ConstEdgeStore(const YsShellExt &shl,YSSIZE_T nVtx,const YsShellExt::ConstEdgeHandle ceHdArray[])
{
	SetShell(shl);
	EnableAutoResizing();
	AddConstEdge(nVtx,ceHdArray);
}

void YsShellExt::ConstEdgeStore::CleanUp(void)
{
	YsKeyStore::CleanUp();
}

void YsShellExt::ConstEdgeStore::SetShell(const YsShellExt &shl)
{
	this->shl=&shl;
}

const YsShellExt &YsShellExt::ConstEdgeStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellExt::ConstEdgeStore::Add(YsShellExt::ConstEdgeHandle ceHd)
{
	return AddKey(shl->GetSearchKey(ceHd));
}

YSRESULT YsShellExt::ConstEdgeStore::Add(YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[])
{
	int i;
	for(i=0; i<nVt; i++)
	{
		AddKey(shl->GetSearchKey(ceHd[i]));
	}
	return YSOK;
}

YSRESULT YsShellExt::ConstEdgeStore::Delete(YsShellExt::ConstEdgeHandle ceHd)
{
	return DeleteKey(shl->GetSearchKey(ceHd));
}


YSRESULT YsShellExt::ConstEdgeStore::AddConstEdge(YsShellExt::ConstEdgeHandle ceHd)
{
	return Add(ceHd);
}

YSRESULT YsShellExt::ConstEdgeStore::AddConstEdge(YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[])
{
	return Add(nVt,ceHd);
}

YSRESULT YsShellExt::ConstEdgeStore::DeleteConstEdge(YsShellExt::ConstEdgeHandle ceHd)
{
	return Delete(ceHd);
}

YSBOOL YsShellExt::ConstEdgeStore::IsIncluded(YsShellExt::ConstEdgeHandle ceHd) const
{
	return YsKeyStore::IsIncluded(shl->GetSearchKey(ceHd));
}

////////////////////////////////////////////////////////////

YsShellExt::FaceGroupStore::FaceGroupStore()
{
	this->shl=NULL;
	EnableAutoResizing();
}

YsShellExt::FaceGroupStore::FaceGroupStore(const YsShellExt &shl)
{
	SetShell(shl);
	EnableAutoResizing();
}

YsShellExt::FaceGroupStore::FaceGroupStore(const YsShellExt &shl,YSSIZE_T nVtx,const YsShellExt::FaceGroupHandle fgHdArray[])
{
	SetShell(shl);
	EnableAutoResizing();
	Add(nVtx,fgHdArray);
}

void YsShellExt::FaceGroupStore::CleanUp(void)
{
	YsKeyStore::CleanUp();
}

void YsShellExt::FaceGroupStore::SetShell(const YsShellExt &shl)
{
	this->shl=&shl;
}

const YsShellExt &YsShellExt::FaceGroupStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellExt::FaceGroupStore::Add(YsShellExt::FaceGroupHandle fgHd)
{
	return AddKey(shl->GetSearchKey(fgHd));
}

YSRESULT YsShellExt::FaceGroupStore::Add(YSSIZE_T nVt,const YsShellExt::FaceGroupHandle fgHd[])
{
	int i;
	for(i=0; i<nVt; i++)
	{
		AddKey(shl->GetSearchKey(fgHd[i]));
	}
	return YSOK;
}

YSRESULT YsShellExt::FaceGroupStore::Delete(YsShellExt::FaceGroupHandle fgHd)
{
	return DeleteKey(shl->GetSearchKey(fgHd));
}

YSRESULT YsShellExt::FaceGroupStore::AddFaceGroup(YsShellExt::FaceGroupHandle fgHd)
{
	return Add(fgHd);
}

YSRESULT YsShellExt::FaceGroupStore::AddFaceGroup(YSSIZE_T nVt,const YsShellExt::FaceGroupHandle fgHd[])
{
	return Add(nVt,fgHd);
}

YSRESULT YsShellExt::FaceGroupStore::DeleteFaceGroup(YsShellExt::FaceGroupHandle fgHd)
{
	return Delete(fgHd);
}

YSBOOL YsShellExt::FaceGroupStore::IsIncluded(YsShellExt::FaceGroupHandle fgHd) const
{
	return YsKeyStore::IsIncluded(shl->GetSearchKey(fgHd));
}

////////////////////////////////////////////////////////////

YsShellExt::VolumeStore::VolumeStore()
{
	this->shl=NULL;
	EnableAutoResizing();
}

YsShellExt::VolumeStore::VolumeStore(const YsShellExt &shl)
{
	SetShell(shl);
	EnableAutoResizing();
}

YsShellExt::VolumeStore::VolumeStore(const YsShellExt &shl,YSSIZE_T nVtx,const YsShellExt::VolumeHandle vlHdArray[])
{
	SetShell(shl);
	EnableAutoResizing();
	AddVolume(nVtx,vlHdArray);
}

void YsShellExt::VolumeStore::CleanUp(void)
{
	YsKeyStore::CleanUp();
}

void YsShellExt::VolumeStore::SetShell(const YsShellExt &shl)
{
	this->shl=&shl;
}

const YsShellExt &YsShellExt::VolumeStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellExt::VolumeStore::AddVolume(YsShellExt::VolumeHandle vlHd)
{
	return AddKey(shl->GetSearchKey(vlHd));
}

YSRESULT YsShellExt::VolumeStore::AddVolume(YSSIZE_T nVt,const YsShellExt::VolumeHandle vlHd[])
{
	int i;
	for(i=0; i<nVt; i++)
	{
		AddKey(shl->GetSearchKey(vlHd[i]));
	}
	return YSOK;
}

YSRESULT YsShellExt::VolumeStore::DeleteVolume(YsShellExt::VolumeHandle vlHd)
{
	return DeleteKey(shl->GetSearchKey(vlHd));
}

YSBOOL YsShellExt::VolumeStore::IsIncluded(YsShellExt::VolumeHandle vlHd) const
{
	return YsKeyStore::IsIncluded(shl->GetSearchKey(vlHd));
}
