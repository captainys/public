/* ////////////////////////////////////////////////////////////

File Name: ysshellext_duplicateutil.h
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

#ifndef YSSHELLEXT_DUPLICATEUTIL_IS_INCLUDED
#define YSSHELLEXT_DUPLICATEUTIL_IS_INCLUDED
/* { */

#include "ysshellext_mappingutil.h"

class YsShellExt_DuplicateUtil
{
protected:
	const class YsShellExt *srcShl;

private:
	YsShellExt_Mapping <YsShellVertexHandle,YSSIZE_T> vtHdMap;
	YsShellExt_Mapping <YsShellPolygonHandle,YSSIZE_T> plHdMap;
	YsShellExt_Mapping <YsShellExt::ConstEdgeHandle,YSSIZE_T> ceHdMap;
	YsShellExt_Mapping <YsShellExt::FaceGroupHandle,YSSIZE_T> fgHdMap;
	YsShellExt_Mapping <YsShellExt::VolumeHandle,YSSIZE_T> vlHdMap;

public:
	class Vertex
	{
	public:
		YsShellVertexHandle srcVtHd,newVtHd;
		YsVec3 pos;
		YsShellExt::VertexAttrib attrib;
		YSBOOL derived;
	};
	class Polygon
	{
	public:
		YsShellPolygonHandle srcPlHd,newPlHd;

		YsArray <YSSIZE_T> vertexArray;
		YsShellExt::PolygonAttrib attrib;
		YsColor col;
		YsVec3 nom;
		YSBOOL derived;
	};
	class ConstEdge
	{
	public:
		YsShellExt::ConstEdgeHandle srcCeHd,newCeHd;
		YSBOOL isLoop;
		YsArray <YSSIZE_T> vertexArray;
		YsShellExt::ConstEdgeAttrib attrib;
		YSBOOL derived;
	};
	class FaceGroup
	{
	public:
		YsShellExt::FaceGroupHandle srcFgHd,newFgHd;
		YsArray <YSSIZE_T> polygonArray;
		YsShellExt::FaceGroupAttrib attrib;
		YSBOOL derived;
	};
	class Volume
	{
	public:
		YsShellExt::VolumeHandle srcVlHd,newVlHd;
		YsArray <YSSIZE_T> faceGroupArray;
		YsShellExt::Volume attrib;
		YSBOOL derived;
	};
	class Pasted
	{
	public:
		YsArray <YsShellVertexHandle> vtHd;
		YsArray <YsShellPolygonHandle> plHd;
		YsArray <YsShellExt::ConstEdgeHandle> ceHd;
		YsArray <YsShellExt::FaceGroupHandle> fgHd;
		YsArray <YsShellExt::VolumeHandle> vlHd;
	};

	YsArray <Vertex> vertexArray;
	YsArray <Polygon> polygonArray;
	YsArray <ConstEdge> constEdgeArray;
	YsArray <FaceGroup> faceGroupArray;
	YsArray <Volume> volumeArray;

	YsShellExt_DuplicateUtil();

	YsShellExt_DuplicateUtil(const YsShellExt_DuplicateUtil &incoming);
	YsShellExt_DuplicateUtil &operator=(const YsShellExt_DuplicateUtil &incoming);
	YsShellExt_DuplicateUtil &CopyFrom(const YsShellExt_DuplicateUtil &incoming);

	YsShellExt_DuplicateUtil(YsShellExt_DuplicateUtil &&incoming);
	YsShellExt_DuplicateUtil &operator=(YsShellExt_DuplicateUtil &&incoming);
	YsShellExt_DuplicateUtil &MoveFrom(YsShellExt_DuplicateUtil &incoming);

	void CleanUp(void);

	// Step 1
	void SetShell(const YsShellExt &shl);

	// Step 2
	void AddVertex(YSSIZE_T n,const YsShellVertexHandle hdArray[]);
	template <class T>
	void AddVertex(const T &hdStore);
private:
	YSSIZE_T AddVertex(YsShellVertexHandle vtHd,YSBOOL derived);

public:
	// Step 3
	void AddPolygon(YSSIZE_T n,const YsShellPolygonHandle hdArray[]);
	template <class T>
	void AddPolygon(const T &hdStore);
private:
	YSSIZE_T AddPolygon(YsShellPolygonHandle plHd,YSBOOL derived);

public:
	// Step 4
	void AddConstEdge(YSSIZE_T n,const YsShellExt::ConstEdgeHandle hdArray[]);
	template <class T>
	void AddConstEdge(const T &hdStore);
private:
	YSSIZE_T AddConstEdge(YsShellExt::ConstEdgeHandle ceHd,YSBOOL derived);

public:
	// Step 5
	void AddFaceGroup(YSSIZE_T n,const YsShellExt::FaceGroupHandle hdArray[]);
	template <class T>
	void AddFaceGroup(const T &hdStore);
private:
	YSSIZE_T AddFaceGroup(YsShellExt::FaceGroupHandle fgHd,YSBOOL derived);

public:
	// Step 6
	void AddVolume(YSSIZE_T n,const YsShellExt::VolumeHandle hdArray[]);
	template <class T>
	void AddVolume(const T &hdStore);
private:
	YSSIZE_T AddVolume(YsShellExt::VolumeHandle vlHd,YSBOOL derived);


public:
	YsArray <YsShellVertexHandle> GetNonDerivedNewVertex(void) const;
	YsArray <YsShellPolygonHandle> GetNonDerivedNewPolygon(void) const;
	YsArray <YsShellExt::ConstEdgeHandle> GetNonDerivedNewConstEdge(void) const;
	YsArray <YsShellExt::FaceGroupHandle> GetNonDerivedNewFaceGroup(void) const;
	YsArray <YsShellExt::VolumeHandle> GetNonDerivedNewVolume(void) const;

protected:
	template <class SHLCLASS>
	void PasteConstEdge(SHLCLASS &dstShl);
	template <class SHLCLASS>
	void PasteFaceGroup(SHLCLASS &dstShl);

public:
	/*! Make a copy of entities stored in this class. */
	template <class SHLCLASS>
	YSRESULT Paste(SHLCLASS &dstShl);

	/*! Make a copy of entities stored in this class. */
	template <class SHLCLASS>
	YSRESULT PasteWithTransformation(SHLCLASS &dstShl,YSBOOL invertPolygon,YSBOOL useMatrix,const YsMatrix4x4 &tfm);

	/*! Make a copy of entities stored in this class (Left for compatibility). */
	template <class SHLCLASS>
	YSRESULT PasteWithTransformation(SHLCLASS &dstShl,const YsMatrix4x4 &tfm);

	/*! Make a clone of entities stored in this class.
	    Parameter dstShl must be an empty YsShellExtEdit, or this function will fail and do nothing.
	    Also the search must be disabled by calling dstShl.DisableSearch() before this function.
	    The difference from Paste is that the search keys of the entities in dstShl will be
	    same as the search keys in the source shell.
	*/
	template <class SHLCLASS>
	YSRESULT MakeClone(SHLCLASS &dstShl);

	/*! This function returns newly-creatd entities in Paste or MakeClone.
	    This information is only valid after those functions are called.
	*/
	Pasted GetPasted(void) const;

	/*! This function returns the non-weighted average of the vertices stored in this object.
	*/
	YsVec3 GetCenter(void) const;
};


template <class T>
void YsShellExt_DuplicateUtil::AddVertex(const T &hdStore)
{
	for(auto vtHd : hdStore)
	{
		AddVertex(vtHd,YSFALSE);
	}
}

template <class T>
void YsShellExt_DuplicateUtil::AddPolygon(const T &hdStore)
{
	for(auto plHd : hdStore)
	{
		AddPolygon(plHd,YSFALSE);
	}
}

template <class T>
void YsShellExt_DuplicateUtil::AddConstEdge(const T &hdStore)
{
	for(auto ceHd : hdStore)
	{
		AddConstEdge(ceHd,YSFALSE);
	}
}

template <class T>
void YsShellExt_DuplicateUtil::AddFaceGroup(const T &hdStore)
{
	for(auto iter=hdStore.begin(); iter!=hdStore.end(); ++iter)
	{
		YsShellExt::FaceGroupHandle fgHd=*iter;
		AddFaceGroup(fgHd,YSFALSE);
	}
}

template <class T>
void YsShellExt_DuplicateUtil::AddVolume(const T &hdStore)
{
	for(auto vlHd : hdStore)
	{
		AddVolume(vlHd,YSFALSE);
	}
}

template <class SHLCLASS>
void YsShellExt_DuplicateUtil::PasteConstEdge(SHLCLASS &dstShl)
{
	for(auto &ce : constEdgeArray)
	{
		YsArray <YsShellVertexHandle,4> newVtHd;
		newVtHd.Set(ce.vertexArray.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<ce.vertexArray.GetN(); ++idx)
		{
			auto vtIdx=ce.vertexArray[idx];
			auto &vtx=vertexArray[vtIdx];
			newVtHd[idx]=vtx.newVtHd;     // 2015/07/28 I was inverting the order.  Fixed.  Was there a reason?
		}

		ce.newCeHd=dstShl.AddConstEdge(newVtHd,ce.isLoop);
		dstShl.SetConstEdgeAttrib(ce.newCeHd,ce.attrib);
	}
}

template <class SHLCLASS>
void YsShellExt_DuplicateUtil::PasteFaceGroup(SHLCLASS &dstShl)
{
	for(auto &fg : faceGroupArray)
	{
		YsArray <YsShellPolygonHandle> newPlHdArray;
		for(auto &newPlIdx : fg.polygonArray)
		{
			auto newPlHd=polygonArray[newPlIdx].newPlHd;
			if(NULL!=newPlHd)
			{
				newPlHdArray.Append(newPlHd);
			}
		}
		if(0<newPlHdArray.GetN())
		{
			fg.newFgHd=dstShl.AddFaceGroup(newPlHdArray);
			dstShl.SetFaceGroupAttrib(fg.newFgHd,fg.attrib);
		}
	}
}

template <class SHLCLASS>
YSRESULT YsShellExt_DuplicateUtil::Paste(SHLCLASS &dstShl)
{
	return PasteWithTransformation(dstShl,YSFALSE,YSFALSE,YsMatrix4x4());
}

template <class SHLCLASS>
YSRESULT YsShellExt_DuplicateUtil::PasteWithTransformation(SHLCLASS &dstShl,const YsMatrix4x4 &tfm)
{
	return PasteWithTransformation(dstShl,YSFALSE,YSTRUE,tfm);
}

template <class SHLCLASS>
YSRESULT YsShellExt_DuplicateUtil::PasteWithTransformation(SHLCLASS &dstShl,YSBOOL invertPolygon,YSBOOL useMatrix,const YsMatrix4x4 &tfm)
{
	typename SHLCLASS::StopIncUndo incUndo(&dstShl);

	for(auto &vtx : vertexArray)
	{
		auto newPos=vtx.pos;
		if(YSTRUE==useMatrix)
		{
			newPos=tfm*newPos;
		}
		vtx.newVtHd=dstShl.AddVertex(newPos);
		dstShl.SetVertexAttrib(vtx.newVtHd,vtx.attrib);
	}

	for(auto &plg : polygonArray)
	{
		YsArray <YsShellVertexHandle,4> newPlVtHd;
		newPlVtHd.Set(plg.vertexArray.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<plg.vertexArray.GetN(); ++idx)
		{
			auto vtIdx=plg.vertexArray[idx];
			auto &vtx=vertexArray[vtIdx];
			newPlVtHd[idx]=vtx.newVtHd;
		}

		if(YSTRUE==invertPolygon)
		{
			newPlVtHd.Invert();
		}

		auto newPlHd=dstShl.AddPolygon(newPlVtHd);
		dstShl.SetPolygonAttrib(newPlHd,plg.attrib);
		dstShl.SetPolygonColor(newPlHd,plg.col);

		YsVec3 nom=plg.nom;
		if(YSTRUE==useMatrix)
		{
			tfm.Mul(nom,nom,0);
		}

		dstShl.SetPolygonNormal(newPlHd,nom);
		plg.newPlHd=newPlHd;
	}

	PasteConstEdge(dstShl);
	PasteFaceGroup(dstShl);

	return YSOK;
}

template <class SHLCLASS>
YSRESULT YsShellExt_DuplicateUtil::MakeClone(SHLCLASS &dstShl)
{
	if(YSTRUE==dstShl.IsSearchEnabled())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be disabled before this function.\n");
		return YSERR;
	}

	if(0<dstShl.GetNumVertex() ||
	   0<dstShl.GetNumPolygon() ||
	   0<dstShl.GetNumConstEdge() ||
	   0<dstShl.GetNumFaceGroup() ||
	   0<dstShl.GetNumVolume())
	{
		return YSERR;
	}

	if(YSOK==Paste(dstShl))
	{
		{
			YsArray <YsPair <YsShell::VertexHandle,YSHASHKEY> > vtHdKeyTable;
			for(auto &v : vertexArray)
			{
				vtHdKeyTable.Increment();
				vtHdKeyTable.Last().a=v.newVtHd;
				vtHdKeyTable.Last().b=srcShl->GetSearchKey(v.srcVtHd);
			}
			dstShl.OverrideVertexSearchKey(vtHdKeyTable);
		}
		{
			YsArray <YsPair <YsShellPolygonHandle,YSHASHKEY> > plHdKeyTable;
			for(auto &p : polygonArray)
			{
				plHdKeyTable.Increment();
				plHdKeyTable.Last().a=p.newPlHd;
				plHdKeyTable.Last().b=srcShl->GetSearchKey(p.srcPlHd);
			}
			dstShl.OverridePolygonSearchKey(plHdKeyTable);
		}
		{
			YsArray <YsPair <YsShellExt::ConstEdgeHandle,YSHASHKEY> > ceHdKeyTable;
			for(auto &c : constEdgeArray)
			{
				ceHdKeyTable.Increment();
				ceHdKeyTable.Last().a=c.newCeHd;
				ceHdKeyTable.Last().b=srcShl->GetSearchKey(c.srcCeHd);
			}
			dstShl.OverrideConstEdgeSearchKey(ceHdKeyTable);
		}
		{
			YsArray <YsPair <YsShellExt::FaceGroupHandle,YSHASHKEY> > fgHdKeyTable;
			for(auto &f : faceGroupArray)
			{
				fgHdKeyTable.Increment();
				fgHdKeyTable.Last().a=f.newFgHd;
				fgHdKeyTable.Last().b=srcShl->GetSearchKey(f.srcFgHd);
			}
			dstShl.OverrideFaceGroupSearchKey(fgHdKeyTable);
		}
		{
			//for(auto &v : volumeArray)
			//{
			//}
		}

		return YSOK;
	}
	return YSERR;
}



/* } */
#endif
