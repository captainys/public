/* ////////////////////////////////////////////////////////////

File Name: ysshellsearch.h
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

#ifndef YSSHELLSEARCH_IS_INCLUDED
#define YSSHELLSEARCH_IS_INCLUDED
/* { */

class YsShellVtxToVtxTable : protected YsHashTable <YsShellVertexHandle>
{
public:
	using YsHashTable <YsShellVertexHandle>::EnableAutoResizing;

	static int nAlloc;

	YsShellVtxToVtxTable(int hashSize);
	~YsShellVtxToVtxTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT DeleteVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YsShellVertexHandle FindVertex(const YsShell &shl,unsigned searchKey) const;
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
};

////////////////////////////////////////////////////////////

class YsShellPlgToPlgTable : protected YsHashTable <YsShellPolygonHandle>
{
public:
	using YsHashTable <YsShellPolygonHandle>::EnableAutoResizing;

	static int nAlloc;

	YsShellPlgToPlgTable(int hashSize);
	~YsShellPlgToPlgTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
	YsShellPolygonHandle FindPolygon(const YsShell &shl,unsigned searchKey) const;
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
};


////////////////////////////////////////////////////////////

class YsShellVtxToPlgTableEntry
{
public:
	static int nAlloc;
	inline YsShellVtxToPlgTableEntry();
	inline ~YsShellVtxToPlgTableEntry();

#ifndef YSLOWMEM
	YsArray <YsShellPolygonHandle> plHdList;
#else
	YsArray <YsShellPolygonHandle,1,short> plHdList;
#endif
};

YsShellVtxToPlgTableEntry::YsShellVtxToPlgTableEntry()
{
	nAlloc++;
}

YsShellVtxToPlgTableEntry::~YsShellVtxToPlgTableEntry()
{
	nAlloc--;
}


class YsShellVtxToPlgTable : protected YsHashTable <YsShellVtxToPlgTableEntry *>
{
public:
	using YsHashTable <YsShellVtxToPlgTableEntry *>::EnableAutoResizing;
	static int nAlloc;

	YsShellVtxToPlgTable(int hashSize);
	~YsShellVtxToPlgTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle> *FindPolygonList(const YsShell &shl,YsShellVertexHandle vtHd) const;
#else
	const YsArray <YsShellPolygonHandle,1,short> *FindPolygonList(const YsShell &shl,YsShellVertexHandle vtHd) const;
#endif
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
protected:
	YSRESULT CleanUp(void);
};

////////////////////////////////////////////////////////////

// Here the structure is getting complicated
//   If edVtKey1,edVtKey2 is fixed, searchKey is fixed edVtKey1+edVtKey2.
//   If searchKey is fixed, can find unique group of edges, that is YsShellEdgToPlgTableEdgeList
//   If the group of edges is found, can find polygon list.

class YsShellEdgToPlgTablePolygonList  // Group of polygons that shares a same edge
{
public:
	static int nAlloc;
	inline YsShellEdgToPlgTablePolygonList();
	inline ~YsShellEdgToPlgTablePolygonList();

	YsShellVertexHandle edVtHd1,edVtHd2;
#ifndef YSLOWMEM
	YsArray <YsShellPolygonHandle,2> plgList;
#else
	YsArray <YsShellPolygonHandle,2,short> plgList;
#endif

	YsShellEdgToPlgTablePolygonList *prev,*next;
};

YsShellEdgToPlgTablePolygonList::YsShellEdgToPlgTablePolygonList()
{
	nAlloc++;
}

YsShellEdgToPlgTablePolygonList::~YsShellEdgToPlgTablePolygonList()
{
	nAlloc--;
}



class YsShellEdgToPlgTableEdgeList  // Group of edges that shares same edVtKey1+edVtKey2
{
public:
	static int nAlloc;

	YsShellEdgToPlgTableEdgeList(YsListAllocator <YsShellEdgToPlgTablePolygonList> &edgeListAllocator);
	~YsShellEdgToPlgTableEdgeList();
	YsListContainer <YsShellEdgToPlgTablePolygonList> plgGroupList;

};

typedef YsShellEdgToPlgTablePolygonList * YsShellEdgeEnumHandle;


class YsShellEdgToPlgTable : protected YsHashTable <YsShellEdgToPlgTableEdgeList *>
{
public:
	YsListAllocator <YsShellEdgToPlgTablePolygonList> edgeListAllocator;
	using YsHashTable <YsShellEdgToPlgTableEdgeList *>::EnableAutoResizing;
	static int nAlloc;

	YsShellEdgToPlgTable(int hashSize);
	~YsShellEdgToPlgTable();
	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd);
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *GetEdgToPlgTable
	    (const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const;
	const YsArray <YsShellPolygonHandle,2> *GetEdgToPlgTable
	    (const YsShell &shl,unsigned vtKey1,unsigned vtKey2) const;
	const YsArray <YsShellPolygonHandle,2> *GetEdgToPlgTable
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *GetEdgToPlgTable
	    (const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const;
	const YsArray <YsShellPolygonHandle,2,short> *GetEdgToPlgTable
	    (const YsShell &shl,unsigned vtKey1,unsigned vtKey2) const;
	const YsArray <YsShellPolygonHandle,2,short> *GetEdgToPlgTable
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#endif
	YSRESULT SelfDiagnostic(const YsShell &shl) const;

	int GetNumEdge(void) const;

	YSRESULT RewindEdgeEnumHandle(YsShellEdgeEnumHandle &handle) const;
	YSRESULT FindNextEdge(YsShellEdgeEnumHandle &handle) const;
	YSRESULT GetEdge(YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellEdgeEnumHandle handle) const;
protected:
	YSRESULT CleanUp(void);

	int nEdge;
	YsShellEdgToPlgTablePolygonList *head,*tail;
};

////////////////////////////////////////////////////////////

// Same idea as YsShellEdgToPlgTable
//   If vertex list is fixed, searchKey is fixed. (searchKey=sigma(vertex keys))
//   If searchKey is fixed, one group of polygon group is fixed, that have same searchKey)
//   Among the group, group of polygons can be found. (must have same number of vertices, and same vertices)

// class YsShellVtxLstToPlgTablePolygonGroup // Group of polygons that has a same set of vertices
// {
// public:
// 	YsArray <unsigned,4> plVtKey;
// 	YsArray <YsShellPolygonHandle> plgList;  // Not likely having more than one item in this list as long as no overlap
// };
// 
// class YsShellVtxLstToPlgTableGroupList  // Group of Polygon Groups that has same searchKey
// {
// public:
// 	YsShellVtxLstToPlgTableGroupList();
// 	~YsShellVtxLstToPlgTableGroupList();
// 	YsListContainer <YsShellVtxLstToPlgTablePolygonGroup> plgGroupList;
// };
// 
// class YsShellVtxLstToPlgTable : protected YsHashTable <YsShellVtxLstToPlgTableGroupList *>
// {
// public:
// 	using YsHashTable <YsShellVtxLstToPlgTableGroupList *>::EnableAutoResizing;
// 
// 	YsShellVtxLstToPlgTable(int hashSize);
// 	~YsShellVtxLstToPlgTable();
// 	YSRESULT Prepare(void);
// 	YSRESULT Resize(int newHashSize);
// 
// 	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd);
// 	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd);
// 	const YsArray <YsShellPolygonHandle> *GetVtxLstToPlgTable
// 	    (const YsShell &shl,int nVtHd,const YsShellVertexHandle vtHd[]) const;
// 	const YsArray <YsShellPolygonHandle> *GetVtxLstToPlgTable
// 	    (const YsShell &shl,int nVtKey,const unsigned vtKey[]) const;
// 	YSRESULT SelfDiagnostic(const YsShell &shl) const;
// 
// 	mutable YSBOOL overlapWarning;
// 
// protected:
// 	YSRESULT CleanUp(void);
// 	unsigned GetVertexKeyArray(YsArray <unsigned,4> &keyArray,const YsShell &shl,YsShellPolygonHandle plHd) const;
// 	YSBOOL TwoPolygonsAreEqual(const YsArray <unsigned,4> &key1,const YsArray <unsigned,4> &key2) const;
// };


////////////////////////////////////////////////////////////

class YsShellVtxToEdgTableElem
{
public:
	static int nAlloc;

	YsShellVtxToEdgTableElem();
	~YsShellVtxToEdgTableElem();

	YsShellVertexHandle connVtHd;
	unsigned refCount;
};

inline YsShellVtxToEdgTableElem::YsShellVtxToEdgTableElem()
{
	nAlloc++;
}

inline YsShellVtxToEdgTableElem::~YsShellVtxToEdgTableElem()
{
	nAlloc--;
}

inline int operator==(const YsShellVtxToEdgTableElem &a,const YsShellVtxToEdgTableElem &b)
{
	return a.connVtHd==b.connVtHd;
}

class YsShellVtxToEdgTable : protected YsFixedLengthToMultiHashTable <YsShellVtxToEdgTableElem,1,8>
{
public:
	using YsFixedLengthToMultiHashTable <YsShellVtxToEdgTableElem,1,8>::EnableAutoResizing;

	static int nAlloc;

	YsShellVtxToEdgTable(int hashSize);
	~YsShellVtxToEdgTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT SelfDiagnostic(const YsShell &shl) const;

	YSRESULT AddEdge(const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2);
	YSRESULT DeleteEdge(const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2);
	const YsArray <YsShellVtxToEdgTableElem,8> *FindEdge(const YsShell &shl,YsShellVertexHandle vtHd) const;
};

////////////////////////////////////////////////////////////


class YsShellSearchTable
{
private:
	const YsShell *attachedShell;

public:
	YsShellSearchTable();
	YsShellSearchTable(int hashSize);
	~YsShellSearchTable();

	/*! YsShellSearchTable has no shared memory cache any more.  
	    This function does nothing and is left for backward compatibility only. */
	static YSRESULT CollectGarbage(void);

	void SetShellPointer(const YsShell *shl);

	/*! Cleans up the contents.   It fails if attachedShell is non-NULL. */
	YSRESULT CleanUp(void);

	/*! Same as CleanUp. */
	YSRESULT Prepare(void);
	YSRESULT ResizeForShell(const YsShell &shl);
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
	void EnableAutoResizing(int minSize,int maxSize);
	void EnableAutoResizing(void);

	YSRESULT AddVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT DeleteVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd);

	YsShellVertexHandle FindVertex(const YsShell &shl,unsigned searchKey) const;

	YsShellPolygonHandle FindPolygon(const YsShell &shl,unsigned searchKey) const;

	/*! Returns the number of polygons using the vertex. */
	int GetNumPolygonUsingVertex(const YsShell &shl,YsShellVertexHandle vtHd) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle> *FindPolygonListByVertex
	    (const YsShell &shl,YsShellVertexHandle vtHd) const;
#else
	const YsArray <YsShellPolygonHandle,1,short> *FindPolygonListByVertex
	    (const YsShell &shl,YsShellVertexHandle vtHd) const;
#endif
	YSRESULT FindPolygonListByVertex
	    (int &nPl,const YsShellPolygonHandle *&plHdList,const YsShell &shl,YsShellVertexHandle vtHd) const;

	template <const int N>
	YSRESULT FindPolygonListByVertex(YsArray <YsShellPolygonHandle,N> &vtPlHdArray,const YsShell &shl,YsShellVertexHandle vtHd) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *FindPolygonListByEdge
	    (const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *FindPolygonListByEdge
	    (const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const;
#endif
	YSRESULT FindPolygonListByEdge
	    (int &nPl,const YsShellPolygonHandle *&plHdList,
	     const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *FindPolygonListByEdge
	    (const YsShell &shl,unsigned edVt1,unsigned edVt2) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *FindPolygonListByEdge
	    (const YsShell &shl,unsigned edVt1,unsigned edVt2) const;
#endif
	YSRESULT FindPolygonListByEdge
	    (int &nPl,const YsShellPolygonHandle *&plHdList,
	     const YsShell &shl,unsigned edVtKey1,unsigned edVtKey2) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *FindPolygonListByEdge
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *FindPolygonListByEdge
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#endif
	YSRESULT FindPolygonListByEdge
	    (int &nPl,const YsShellPolygonHandle *&plHdList,
	     const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
	int GetNumPolygonUsingEdge(const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
	int GetNumPolygonUsingEdge(const YsShell &shl,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	int GetNumPolygonUsingEdge(const YsShell &shl,unsigned int edVtKey1,unsigned int edVtKey2) const;

	YsShellPolygonHandle GetNeighborPolygon(const YsShell &shl,YsShellPolygonHandle plHd,YSSIZE_T n) const;
	YsShellPolygonHandle GetNeighborPolygon(const YsShell &shl,YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	YSBOOL CheckPolygonNeighbor(YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2,const YsShell &shl) const;
	YSRESULT GetSharedEdge(YsShellVertexHandle edVtHd[2],const YsShell &shl,YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2) const;

// 	const YsArray <YsShellPolygonHandle> *FindPolygonListByVertexList
// 	    (const YsShell &shl,int nVtx,const YsShellVertexHandle vtHd[]) const;
// 	YSRESULT FindPolygonListByVertexList
// 	    (int &nPl,const YsShellPolygonHandle *&plHdList,
// 	     const YsShell &shl,int nVtx,const YsShellVertexHandle vtHd[]) const;

	const YsArray <YsShellVtxToEdgTableElem,8> *FindEdge(const YsShell &shl,YsShellVertexHandle vtHd) const;

	template <int minSize>
	inline YSRESULT GetConnectedVertexList
    	(YsArray <YsShellVertexHandle,minSize> &connLst,const YsShell &shl,YsShellVertexHandle vtHd) const;

	int GetNumEdge(const YsShell &shl) const;

	YSRESULT RewindEdgeEnumHandle(const YsShell &shl,YsShellEdgeEnumHandle &handle) const;
	YSRESULT FindNextEdge(const YsShell &shl,YsShellEdgeEnumHandle &handle) const;
	YSRESULT GetEdge(const YsShell &shl,unsigned &edVtKey1,unsigned &edVtKey2,YsShellEdgeEnumHandle handle) const;
	YSRESULT GetEdge(const YsShell &shl,YsShellVertexHandle &edVtKey1,YsShellVertexHandle &edVtKey2,YsShellEdgeEnumHandle handle) const;
	YsShell::Edge GetEdge(const YsShell &shl,YsShellEdgeEnumHandle handle) const;

	YSBOOL CheckPolygonExist(const YsShell &shl,int nPlVt,const YsShellVertexHandle plVtHd[]) const;

	template <int minSize>
	inline YSRESULT FindPolygonListByVertexList
	    (YsArray <YsShellPolygonHandle,minSize> &plHdList,
	     const YsShell &shl,int nPlVt,const YsShellVertexHandle plVtHd[]) const;

// 	YSBOOL GetOverlapWarning(void) const;
// 	void ClearOverlapWarning(void) const;

protected:
	YsShellVtxToVtxTable vtxToVtx;
	YsShellVtxToEdgTable vtxToEdg;
	YsShellPlgToPlgTable plgToPlg;
	YsShellVtxToPlgTable vtxToPlg;
	YsShellEdgToPlgTable edgToPlg;
// 	YsShellVtxLstToPlgTable vtxLstToPlg;
};

template <int minSize>
YSRESULT YsShellSearchTable::GetConnectedVertexList
    (YsArray <YsShellVertexHandle,minSize> &connLst,const YsShell &shl,YsShellVertexHandle vtHd) const
{
	const YsArray <YsShellVtxToEdgTableElem,8> *edgList;

	edgList=FindEdge(shl,vtHd);
	if(edgList!=NULL)
	{
		int i;
		connLst.Set(edgList->GetN(),NULL);
		for(i=0; i<edgList->GetN(); i++)
		{
			connLst[i]=(*edgList)[i].connVtHd;
		}
	}
	else
	{
		connLst.Set(0,NULL);
	}

	return YSOK;
}

template <const int N>
YSRESULT YsShellSearchTable::FindPolygonListByVertex(YsArray <YsShellPolygonHandle,N> &vtPlHdArray,const YsShell &shl,YsShellVertexHandle vtHd) const
{
	int nPl;
	const YsShellPolygonHandle *vtPlHd;
	if(YSOK==FindPolygonListByVertex(nPl,vtPlHd,shl,vtHd))
	{
		vtPlHdArray.Set(nPl,vtPlHd);
		return YSOK;
	}
	vtPlHdArray.CleanUp();
	return YSERR;
}

template <int minSize>
YSRESULT YsShellSearchTable::FindPolygonListByVertexList
    (YsArray <YsShellPolygonHandle,minSize> &plHdList,
     const YsShell &cShl,int nPlVt,const YsShellVertexHandle plVtHd[]) const
{
	YSSIZE_T i,j;
	YsArray <YsShellPolygonHandle,32> plHdSave;
	YsArray <int,32> plAtt4Save;
	YsArray <int,32> vtAtt4Save;

	YsShell *shl;
	YSBOOL res;

	shl=(YsShell *)&cShl;
	plHdList.Set(0,NULL);

	for(i=0; i<nPlVt; i++)
	{
		vtAtt4Save.Append(cShl.GetVertex(plVtHd[i])->freeAttribute4);
	}

	for(i=0; i<nPlVt; i++)
	{
		int nPl;
		const YsShellPolygonHandle *plHdList;

		if(FindPolygonListByVertex(nPl,plHdList,cShl,plVtHd[i])!=YSOK || nPl==0)
		{
			return YSERR;
		}

		for(j=0; j<nPl; j++)
		{
			plHdSave.Append(plHdList[j]);
			plAtt4Save.Append(shl->GetPolygon(plHdList[j])->freeAttribute4);
		}
	}

	forYsArray(i,plHdSave)
	{
		shl->GetPolygon(plHdSave[i])->freeAttribute4=0;
	}

	forYsArray(i,plHdSave)
	{
		shl->GetPolygon(plHdSave[i])->freeAttribute4++;
	}

	res=YSFALSE;
	forYsArray(i,plHdSave)
	{
		if(cShl.GetPolygon(plHdSave[i])->freeAttribute4==nPlVt &&
		   cShl.GetNumVertexOfPolygon(plHdSave[i])==nPlVt &&
		   YsIsIncluded <YsShellPolygonHandle> (plHdList.GetN(),plHdList,plHdSave[i])!=YSTRUE)
		{
			const YsShellVertexHandle *tstPlVtHd;
			tstPlVtHd=cShl.GetVertexListOfPolygon(plHdSave[i]);

			for(j=0; j<nPlVt; j++)
			{
				shl->GetVertex(tstPlVtHd[j])->freeAttribute4=0;
			}

			for(j=0; j<nPlVt; j++)
			{
				shl->GetVertex(plVtHd[j])->freeAttribute4=1;
			}

			YSBOOL equal;
			equal=YSTRUE;
			for(j=0; j<nPlVt; j++)
			{
				if(shl->GetVertex(tstPlVtHd[j])->freeAttribute4==0)
				{
					equal=YSFALSE;
					break;
				}
			}

			if(equal==YSTRUE)
			{
				plHdList.Append(plHdSave[i]);
			}
		}
	}

	forYsArrayRev(i,plHdSave)
	{
		shl->GetPolygon(plHdSave[i])->freeAttribute4=plAtt4Save[i];
	}

	for(i=nPlVt-1; i>=0; i--)
	{
		shl->GetVertex(plVtHd[i])->freeAttribute4=vtAtt4Save[i];
	}

	if(plHdList.GetN()>0)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

inline YsShellPolygonHandle YsShellPolygonStore::const_iterator::operator*()
{
	const YsShell *shl=table->shl;
	const YsShellSearchTable *search=shl->GetSearchTable();
	if(NULL!=search)
	{
		return search->FindPolygon(*shl,*iter);
	}
	return NULL;
}

inline YsShellVertexHandle YsShellVertexStore::const_iterator::operator*()
{
	const YsShell *shl=table->shl;
	const YsShellSearchTable *search=shl->GetSearchTable();
	if(NULL!=search)
	{
		return search->FindVertex(*shl,*iter);
	}
	return NULL;
}

inline const YsStaticArray <YsShellVertexHandle,2> YsShellEdgeStore::const_iterator::operator*()
{
	const YsShell *shl=table->shl;
	const YsShellSearchTable *search=shl->GetSearchTable();
	YsStaticArray <YsShellVertexHandle,2> edVtHd;

	if(NULL!=search)
	{
		const YsStaticArray <YSHASHKEY,2> edVtKey=*iter;

		edVtHd[0]=search->FindVertex(*shl,edVtKey[0]);
		edVtHd[1]=search->FindVertex(*shl,edVtKey[1]);
		if(NULL==edVtHd[0] || NULL==edVtHd[1])
		{
			edVtHd[0]=NULL;
			edVtHd[1]=NULL;
		}
		return edVtHd;
	}
	edVtHd[0]=NULL;
	edVtHd[1]=NULL;
	return edVtHd;
}

/* } */
#endif
