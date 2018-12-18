/* ////////////////////////////////////////////////////////////

File Name: ysshellext_duplicateutil.cpp
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

#include "ysshellext_duplicateutil.h"


YsShellExt_DuplicateUtil::YsShellExt_DuplicateUtil()
{
}

YsShellExt_DuplicateUtil::YsShellExt_DuplicateUtil(const YsShellExt_DuplicateUtil &incoming)
{
	srcShl=NULL;
	CopyFrom(incoming);
}

YsShellExt_DuplicateUtil &YsShellExt_DuplicateUtil::operator=(const YsShellExt_DuplicateUtil &incoming)
{
	return CopyFrom(incoming);
}

YsShellExt_DuplicateUtil &YsShellExt_DuplicateUtil::CopyFrom(const YsShellExt_DuplicateUtil &incoming)
{
	srcShl=incoming.srcShl;

	vtHdMap=incoming.vtHdMap;
	plHdMap=incoming.plHdMap;
	ceHdMap=incoming.ceHdMap;
	fgHdMap=incoming.fgHdMap;
	vlHdMap=incoming.vlHdMap;
	vertexArray=incoming.vertexArray;
	polygonArray=incoming.polygonArray;
	constEdgeArray=incoming.constEdgeArray;
	faceGroupArray=incoming.faceGroupArray;
	volumeArray=incoming.volumeArray;
	return *this;
}


YsShellExt_DuplicateUtil::YsShellExt_DuplicateUtil(YsShellExt_DuplicateUtil &&incoming)
{
	MoveFrom(incoming);
}

YsShellExt_DuplicateUtil &YsShellExt_DuplicateUtil::operator=(YsShellExt_DuplicateUtil &&incoming)
{
	return MoveFrom(incoming);
}

YsShellExt_DuplicateUtil &YsShellExt_DuplicateUtil::MoveFrom(YsShellExt_DuplicateUtil &incoming)
{
	srcShl=incoming.srcShl;

	vtHdMap=incoming.vtHdMap;
	plHdMap=incoming.plHdMap;
	ceHdMap=incoming.ceHdMap;
	fgHdMap=incoming.fgHdMap;
	vlHdMap=incoming.vlHdMap;
	vertexArray.MoveFrom(incoming.vertexArray);
	polygonArray.MoveFrom(incoming.polygonArray);
	constEdgeArray.MoveFrom(incoming.constEdgeArray);
	faceGroupArray.MoveFrom(incoming.faceGroupArray);
	volumeArray.MoveFrom(incoming.volumeArray);
	return *this;
}

void YsShellExt_DuplicateUtil::CleanUp(void)
{
	srcShl=NULL;

	vtHdMap.CleanUp();
	plHdMap.CleanUp();
	ceHdMap.CleanUp();
	fgHdMap.CleanUp();
	vlHdMap.CleanUp();
	vertexArray.CleanUp();
	polygonArray.CleanUp();
	constEdgeArray.CleanUp();
	faceGroupArray.CleanUp();
	volumeArray.CleanUp();
}

// Step 1
void YsShellExt_DuplicateUtil::SetShell(const YsShellExt &shl)
{
	srcShl=&shl;
}


// Step 2
void YsShellExt_DuplicateUtil::AddVertex(YSSIZE_T n,const YsShellVertexHandle hdArray[])
{
	for(YSSIZE_T idx=0; idx<n; ++idx)
	{
		AddVertex(hdArray[idx],YSFALSE);
	}
}

YSSIZE_T YsShellExt_DuplicateUtil::AddVertex(YsShellVertexHandle vtHd,YSBOOL derived)
{
	const YSSIZE_T newIdx=vertexArray.GetN();

	auto &last=vertexArray.New();
	last.srcVtHd=vtHd;
	srcShl->GetVertexPosition(last.pos,vtHd);
	last.attrib=(*srcShl->GetVertexAttrib(vtHd));
	last.derived=derived;

	vtHdMap.AddMapping(*srcShl,vtHd,newIdx);

	return newIdx;
}


// Step 3
void YsShellExt_DuplicateUtil::AddPolygon(YSSIZE_T n,const YsShellPolygonHandle hdArray[])
{
	for(YSSIZE_T idx=0; idx<n; ++idx)
	{
		AddPolygon(hdArray[idx],YSFALSE);
	}
}

YSSIZE_T YsShellExt_DuplicateUtil::AddPolygon(YsShellPolygonHandle plHd,YSBOOL derived)
{
	const YSSIZE_T newIdx=polygonArray.GetN();

	auto &last=polygonArray.New();
	last.srcPlHd=plHd;

	YsArray <YsShellVertexHandle,4> plVtHd;
	srcShl->GetPolygon(plVtHd,plHd);
	last.vertexArray.Set(plVtHd.GetN(),NULL);
	for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
	{
		YsArray <YSSIZE_T> found;
		vtHdMap.FindMapping(found,*srcShl,plVtHd[vtIdx]);
		if(0<found.GetN())
		{
			last.vertexArray[vtIdx]=found[0];
		}
		else
		{
			last.vertexArray[vtIdx]=AddVertex(plVtHd[vtIdx],YSTRUE);
		}
	}

	last.attrib=(*srcShl->GetPolygonAttrib(plHd));
	srcShl->GetColor(last.col,plHd);
	srcShl->GetNormal(last.nom,plHd);
	last.derived=derived;

	plHdMap.AddMapping(*srcShl,plHd,newIdx);

	return newIdx;
}


// Step 4
void YsShellExt_DuplicateUtil::AddConstEdge(YSSIZE_T n,const YsShellExt::ConstEdgeHandle hdArray[])
{
	for(YSSIZE_T idx=0; idx<n; ++idx)
	{
		AddConstEdge(hdArray[idx],YSFALSE);
	}
}

YSSIZE_T YsShellExt_DuplicateUtil::AddConstEdge(YsShellExt::ConstEdgeHandle ceHd,YSBOOL derived)
{
	const YSSIZE_T newIdx=constEdgeArray.GetN();

	auto &last=constEdgeArray.New();
	last.srcCeHd=ceHd;

	YsArray <YsShellVertexHandle,4> vtHdArray;
	srcShl->GetConstEdge(vtHdArray,last.isLoop,ceHd);
	last.vertexArray.Set(vtHdArray.GetN(),NULL);
	for(YSSIZE_T vtIdx=0; vtIdx<vtHdArray.GetN(); ++vtIdx)
	{
		YsArray <YSSIZE_T> found;
		vtHdMap.FindMapping(found,*srcShl,vtHdArray[vtIdx]);
		if(0<found.GetN())
		{
			last.vertexArray[vtIdx]=found[0];
		}
		else
		{
			last.vertexArray[vtIdx]=AddVertex(vtHdArray[vtIdx],YSTRUE);
		}
	}

	last.attrib=srcShl->GetConstEdgeAttrib(ceHd);
	last.derived=derived;

	ceHdMap.AddMapping(*srcShl,ceHd,newIdx);

	return newIdx;
}


// Step 5
void YsShellExt_DuplicateUtil::AddFaceGroup(YSSIZE_T n,const YsShellExt::FaceGroupHandle hdArray[])
{
	for(YSSIZE_T idx=0; idx<n; ++idx)
	{
		AddFaceGroup(hdArray[idx],YSFALSE);
	}
}

YSSIZE_T YsShellExt_DuplicateUtil::AddFaceGroup(YsShellExt::FaceGroupHandle fgHd,YSBOOL derived)
{
	const YSSIZE_T newIdx=faceGroupArray.GetN();

	auto &last=faceGroupArray.New();
	last.srcFgHd=fgHd;

	YsArray <YsShellPolygonHandle,4> plHdArray;
	srcShl->GetFaceGroup(plHdArray,fgHd);
	last.polygonArray.Set(plHdArray.GetN(),NULL);
	for(YSSIZE_T plIdx=0; plIdx<plHdArray.GetN(); ++plIdx)
	{
		YsArray <YSSIZE_T> found;
		plHdMap.FindMapping(found,*srcShl,plHdArray[plIdx]);
		if(0<found.GetN())
		{
			last.polygonArray[plIdx]=found[0];
		}
		else
		{
			last.polygonArray[plIdx]=AddPolygon(plHdArray[plIdx],YSTRUE);
		}
	}

	last.attrib=srcShl->GetFaceGroupAttrib(fgHd);
	last.derived=derived;

	fgHdMap.AddMapping(*srcShl,fgHd,newIdx);

	return newIdx;
}


// Step 6
void YsShellExt_DuplicateUtil::AddVolume(YSSIZE_T n,const YsShellExt::VolumeHandle hdArray[])
{
	for(YSSIZE_T idx=0; idx<n; ++idx)
	{
		AddVolume(hdArray[idx],YSFALSE);
	}
}

YSSIZE_T YsShellExt_DuplicateUtil::AddVolume(YsShellExt::VolumeHandle vlHd,YSBOOL derived)
{
	const YSSIZE_T newIdx=volumeArray.GetN();

	auto &last=volumeArray.New();
	last.srcVlHd=vlHd;

	YsArray <YsShellExt::FaceGroupHandle,4> fgHdArray;
	srcShl->GetVolume(fgHdArray,vlHd);
	last.faceGroupArray.Set(fgHdArray.GetN(),NULL);
	for(YSSIZE_T plIdx=0; plIdx<fgHdArray.GetN(); ++plIdx)
	{
		YsArray <YSSIZE_T> found;
		fgHdMap.FindMapping(found,*srcShl,fgHdArray[plIdx]);
		if(0<found.GetN())
		{
			last.faceGroupArray[plIdx]=found[0];
		}
		else
		{
			last.faceGroupArray[plIdx]=AddFaceGroup(fgHdArray[plIdx],YSTRUE);
		}
	}

	last.attrib=srcShl->GetVolumeAttrib(vlHd);
	last.derived=derived;

	vlHdMap.AddMapping(*srcShl,vlHd,newIdx);

	return newIdx;
}


YsArray <YsShellVertexHandle> YsShellExt_DuplicateUtil::GetNonDerivedNewVertex(void) const
{
	YsArray <YsShellVertexHandle> ary;
	for(auto elem : vertexArray)
	{
		if(YSTRUE!=elem.derived)
		{
			ary.Append(elem.newVtHd);
		}
	}
	return ary;
}

YsArray <YsShellPolygonHandle> YsShellExt_DuplicateUtil::GetNonDerivedNewPolygon(void) const
{
	YsArray <YsShellPolygonHandle> ary;
	for(auto elem : polygonArray)
	{
		if(YSTRUE!=elem.derived)
		{
			ary.Append(elem.newPlHd);
		}
	}
	return ary;
}

YsArray <YsShellExt::ConstEdgeHandle> YsShellExt_DuplicateUtil::GetNonDerivedNewConstEdge(void) const
{
	YsArray <YsShellExt::ConstEdgeHandle> ary;
	for(auto elem : constEdgeArray)
	{
		if(YSTRUE!=elem.derived)
		{
			ary.Append(elem.newCeHd);
		}
	}
	return ary;
}

YsArray <YsShellExt::FaceGroupHandle> YsShellExt_DuplicateUtil::GetNonDerivedNewFaceGroup(void) const
{
	YsArray <YsShellExt::FaceGroupHandle> ary;
	for(auto elem : faceGroupArray)
	{
		if(YSTRUE!=elem.derived)
		{
			ary.Append(elem.newFgHd);
		}
	}
	return ary;
}

YsArray <YsShellExt::VolumeHandle> YsShellExt_DuplicateUtil::GetNonDerivedNewVolume(void) const
{
	YsArray <YsShellExt::VolumeHandle> ary;
	for(auto elem : volumeArray)
	{
		if(YSTRUE!=elem.derived)
		{
			ary.Append(elem.newVlHd);
		}
	}
	return ary;
}

YsShellExt_DuplicateUtil::Pasted YsShellExt_DuplicateUtil::GetPasted(void) const
{
	Pasted pasted;

	for(auto &v : vertexArray)
	{
		pasted.vtHd.Add(v.newVtHd);
	}
	for(auto &p : polygonArray)
	{
		pasted.plHd.Add(p.newPlHd);
	}
	for(auto &c : constEdgeArray)
	{
		pasted.ceHd.Add(c.newCeHd);
	}
	for(auto &f : faceGroupArray)
	{
		pasted.fgHd.Add(f.newFgHd);
	}
	for(auto & v: volumeArray)
	{
		pasted.vlHd.Add(v.newVlHd);
	}

	return pasted;
}

YsVec3 YsShellExt_DuplicateUtil::GetCenter(void) const
{
	YsVec3 cen=YsVec3::Origin();
	if(0<vertexArray.size())
	{
		for(auto &vtx : vertexArray)
		{
			cen+=vtx.pos;
		}
		cen/=(double)vertexArray.size();
	}
	return cen;
}
