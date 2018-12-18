/* ////////////////////////////////////////////////////////////

File Name: ysshellextmisc.h
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

#ifndef YSSHELLEXTMISC_IS_INCLUDED
#define YSSHELLEXTMISC_IS_INCLUDED
/* { */

#include "ysshellext.h"

class YsShellExt_VertexInsertionInfo
{
public:
	YsShellVertexHandle beforeOrAfterThisVtHd;
	YsShellVertexHandle insertThisVtHd;
	int beforeOrAfter;        // Positive=>After  Negative=>Before
};

class YsShellExt_ConstEdgeVertexInsertionInfo : public YsShellExt_VertexInsertionInfo
{
public:
	YsShellExt::ConstEdgeHandle ceHd;
};

////////////////////////////////////////////////////////////

/*! YsShellExt::ConstEdgeStore stores a set of constraint edges.
    It internally stores search keys of const edges, therefore, the same ConstEdgeStore is
    valid for a clone shell.
    However, the search must be enabled to use ConstEdgeStore.
*/
class YsShellExt::ConstEdgeStore : public YsKeyStore
{
friend class const_iterator;

public:
	/*! In case it needs to iterate through polygon keys, you can write like:
		YsShellPolygonStore plStore;
			for(auto iter=plStore.AllKey().begin(); iter!=plStore.AllKey().end(); ++iter)
			{
				// Do something useful
			}
		or,
			for(auto key : plStore.AllKey()
			{
				// Do something useful
			}
		using range-based for.
	*/
	const YsKeyStore &AllKey(void) const;

	/*! STL-like iterator support. */
	class const_iterator
	{
	friend class ConstEdgeStore;

	private:
		const ConstEdgeStore *table;
		YsKeyStore::const_iterator iter;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline const YsShellExt::ConstEdgeHandle operator*();  // Due to dependency, it is written in ysshellsearch.h
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	ConstEdgeStore();
	ConstEdgeStore(const YsShellExt &shl);
	ConstEdgeStore(const YsShellExt &shl,YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[]);

	template <class T>
	ConstEdgeStore(const YsShellExt &shl,const T &hdStore);

	void CleanUp(void);
	void SetShell(const YsShellExt &shl);
	const YsShellExt &GetShell(void) const;

	YSRESULT Add(YsShellExt::ConstEdgeHandle ceHd);
	YSRESULT Add(YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[]);
	template <class T>
	YSRESULT Add(const T &hdStore);
	YSRESULT Delete(YsShellExt::ConstEdgeHandle ceHd);

	YSBOOL IsIncluded(YsShellExt::ConstEdgeHandle ceHd) const;

	/*! Older naming convention.  Use Add instead. */
	YSRESULT AddConstEdge(YsShellExt::ConstEdgeHandle ceHd);
	/*! Older naming convention.  Use Add instead. */
	YSRESULT AddConstEdge(YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[]);

	/*! Older naming convention.  Use Add instead. */
	template <class T>
	YSRESULT AddConstEdge(const T &hdStore);

	/*! Older naming convention.  Use Delete instead. */
	YSRESULT DeleteConstEdge(YsShellExt::ConstEdgeHandle ceHd);
protected:
	const YsShellExt *shl;
};

inline YsShellExt::ConstEdgeStore::const_iterator YsShellExt::ConstEdgeStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::begin();
	return iter;
}

inline YsShellExt::ConstEdgeStore::const_iterator YsShellExt::ConstEdgeStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::end();
	return iter;
}

inline YsShellExt::ConstEdgeStore::const_iterator &YsShellExt::ConstEdgeStore::const_iterator::operator++()
{
	++iter;
	return *this;
}

inline YsShellExt::ConstEdgeStore::const_iterator YsShellExt::ConstEdgeStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	++iter;
	return copy;
}


inline bool YsShellExt::ConstEdgeStore::const_iterator::operator==(const YsShellExt::ConstEdgeStore::const_iterator &from)
{
	return this->iter==from.iter;
}


inline bool YsShellExt::ConstEdgeStore::const_iterator::operator!=(const YsShellExt::ConstEdgeStore::const_iterator &from)
{
	return this->iter!=from.iter;
}

inline const YsShellExt::ConstEdgeHandle YsShellExt::ConstEdgeStore::const_iterator::operator*()
{
	return table->shl->FindConstEdge(*iter);
}

inline YsShellExt::ConstEdgeStore::const_iterator begin(const YsShellExt::ConstEdgeStore &store)
{
	return store.begin();
}

inline YsShellExt::ConstEdgeStore::const_iterator end(const YsShellExt::ConstEdgeStore &store)
{
	return store.end();
}

template <class T>
YsShellExt::ConstEdgeStore::ConstEdgeStore(const YsShellExt &shl,const T &hdStore)
{
	this->shl=&shl;
	EnableAutoResizing();
	for(auto ceHd : hdStore)
	{
		AddConstEdge(ceHd);
	}
}

template <class T>
YSRESULT YsShellExt::ConstEdgeStore::Add(const T &hdStore)
{
	for(auto ceHd : hdStore)
	{
		AddConstEdge(ceHd);
	}
	return YSOK;
}

template <class T>
YSRESULT YsShellExt::ConstEdgeStore::AddConstEdge(const T &hdStore)
{
	Add<T>(hdStore);
}

////////////////////////////////////////////////////////////

class YsShellExt::FaceGroupStore : public YsKeyStore
{
friend class const_iterator;

public:
	/*! In case it needs to iterate through polygon keys, you can write like:
		YsShellPolygonStore plStore;
			for(auto iter=plStore.AllKey().begin(); iter!=plStore.AllKey().end(); ++iter)
			{
				// Do something useful
			}
		or,
			for(auto key : plStore.AllKey()
			{
				// Do something useful
			}
		using range-based for.
	*/
	const YsKeyStore &AllKey(void) const;

	/*! STL-like iterator support. */
	class const_iterator
	{
	friend class FaceGroupStore;

	private:
		const FaceGroupStore *table;
		YsKeyStore::const_iterator iter;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline const YsShellExt::FaceGroupHandle operator*();  // Due to dependency, it is written in ysshellsearch.h
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	FaceGroupStore();
	FaceGroupStore(const YsShellExt &shl);
	FaceGroupStore(const YsShellExt &shl,YSSIZE_T nCe,const YsShellExt::FaceGroupHandle fgHdArray[]);

	template <class T>
	FaceGroupStore(const YsShellExt &shl,const T &hdStore);

	void CleanUp(void);
	void SetShell(const YsShellExt &shl);
	const YsShellExt &GetShell(void) const;


	YSRESULT Add(YsShellExt::FaceGroupHandle fgHd);
	YSRESULT Add(YSSIZE_T nVt,const YsShellExt::FaceGroupHandle fgHd[]);
	template <class T>
	YSRESULT Add(const T &hdStore);

	YSRESULT Delete(YsShellExt::FaceGroupHandle fgHd);

	YSBOOL IsIncluded(YsShellExt::FaceGroupHandle fgHd) const;

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddFaceGroup(YsShellExt::FaceGroupHandle fgHd);

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddFaceGroup(YSSIZE_T nVt,const YsShellExt::FaceGroupHandle fgHd[]);

	/*! Old naming convention.  Use Add instead. */
	template <class T>
	YSRESULT AddFaceGroup(const T &hdStore);

	/*! Old naming convention.  Use Delete instead. */
	YSRESULT DeleteFaceGroup(YsShellExt::FaceGroupHandle fgHd);
protected:
	const YsShellExt *shl;
};

inline YsShellExt::FaceGroupStore::const_iterator YsShellExt::FaceGroupStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::begin();
	return iter;
}

inline YsShellExt::FaceGroupStore::const_iterator YsShellExt::FaceGroupStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::end();
	return iter;
}

inline YsShellExt::FaceGroupStore::const_iterator &YsShellExt::FaceGroupStore::const_iterator::operator++()
{
	++iter;
	return *this;
}

inline YsShellExt::FaceGroupStore::const_iterator YsShellExt::FaceGroupStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	++iter;
	return copy;
}


inline bool YsShellExt::FaceGroupStore::const_iterator::operator==(const YsShellExt::FaceGroupStore::const_iterator &from)
{
	return this->iter==from.iter;
}


inline bool YsShellExt::FaceGroupStore::const_iterator::operator!=(const YsShellExt::FaceGroupStore::const_iterator &from)
{
	return this->iter!=from.iter;
}

inline const YsShellExt::FaceGroupHandle YsShellExt::FaceGroupStore::const_iterator::operator*()
{
	return table->shl->FindFaceGroup(*iter);
}

inline YsShellExt::FaceGroupStore::const_iterator begin(const YsShellExt::FaceGroupStore &store)
{
	return store.begin();
}

inline YsShellExt::FaceGroupStore::const_iterator end(const YsShellExt::FaceGroupStore &store)
{
	return store.end();
}

template <class T>
YsShellExt::FaceGroupStore::FaceGroupStore(const YsShellExt &shl,const T &hdStore)
{
	this->shl=&shl;
	EnableAutoResizing();
	for(auto fgHd : hdStore)
	{
		AddFaceGroup(fgHd);
	}
}

template <class T>
YSRESULT YsShellExt::FaceGroupStore::Add(const T &hdStore)
{
	for(auto fgHd : hdStore)
	{
		Add(fgHd);
	}
	return YSOK;
}

template <class T>
YSRESULT YsShellExt::FaceGroupStore::AddFaceGroup(const T &hdStore)
{
	for(auto fgHd : hdStore)
	{
		AddFaceGroup(fgHd);
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

class YsShellExt::VolumeStore : public YsKeyStore
{
friend class const_iterator;

public:
	/*! In case it needs to iterate through polygon keys, you can write like:
		YsShellPolygonStore plStore;
			for(auto iter=plStore.AllKey().begin(); iter!=plStore.AllKey().end(); ++iter)
			{
				// Do something useful
			}
		or,
			for(auto key : plStore.AllKey()
			{
				// Do something useful
			}
		using range-based for.
	*/
	const YsKeyStore &AllKey(void) const;

	/*! STL-like iterator support. */
	class const_iterator
	{
	friend class VolumeStore;

	private:
		const VolumeStore *table;
		YsKeyStore::const_iterator iter;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline const YsShellExt::VolumeHandle operator*();  // Due to dependency, it is written in ysshellsearch.h
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	VolumeStore();
	VolumeStore(const YsShellExt &shl);
	VolumeStore(const YsShellExt &shl,YSSIZE_T nCe,const YsShellExt::VolumeHandle vlHdArray[]);

	template <class T>
	VolumeStore(const YsShellExt &shl,const T &hdStore);

	void CleanUp(void);
	void SetShell(const YsShellExt &shl);
	const YsShellExt &GetShell(void) const;
	YSRESULT AddVolume(YsShellExt::VolumeHandle vlHd);
	YSRESULT AddVolume(YSSIZE_T nVt,const YsShellExt::VolumeHandle vlHd[]);

	template <class T>
	YSRESULT AddVolume(const T &hdStore);

	YSRESULT DeleteVolume(YsShellExt::VolumeHandle vlHd);
	YSBOOL IsIncluded(YsShellExt::VolumeHandle vlHd) const;
protected:
	const YsShellExt *shl;
};

inline YsShellExt::VolumeStore::const_iterator YsShellExt::VolumeStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::begin();
	return iter;
}

inline YsShellExt::VolumeStore::const_iterator YsShellExt::VolumeStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::end();
	return iter;
}

inline YsShellExt::VolumeStore::const_iterator &YsShellExt::VolumeStore::const_iterator::operator++()
{
	++iter;
	return *this;
}

inline YsShellExt::VolumeStore::const_iterator YsShellExt::VolumeStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	++iter;
	return copy;
}


inline bool YsShellExt::VolumeStore::const_iterator::operator==(const YsShellExt::VolumeStore::const_iterator &from)
{
	return this->iter==from.iter;
}


inline bool YsShellExt::VolumeStore::const_iterator::operator!=(const YsShellExt::VolumeStore::const_iterator &from)
{
	return this->iter!=from.iter;
}

inline const YsShellExt::VolumeHandle YsShellExt::VolumeStore::const_iterator::operator*()
{
	return table->shl->FindVolume(*iter);
}

inline YsShellExt::VolumeStore::const_iterator begin(const YsShellExt::VolumeStore &store)
{
	return store.begin();
}

inline YsShellExt::VolumeStore::const_iterator end(const YsShellExt::VolumeStore &store)
{
	return store.end();
}

template <class T>
YsShellExt::VolumeStore::VolumeStore(const YsShellExt &shl,const T &hdStore)
{
	this->shl=&shl;
	EnableAutoResizing();
	for(auto vlHd : hdStore)
	{
		AddVolume(vlHd);
	}
}

template <class T>
YSRESULT YsShellExt::VolumeStore::AddVolume(const T &hdStore)
{
	for(auto vlHd : hdStore)
	{
		AddVolume(vlHd);
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

template <class toFind>
class YsShellExt::VertexFaceGroupAttribTable : public YsFixedLengthHashTable <toFind,2>
{
private:
	const YsShellExt *shlPtr;

public:
	VertexFaceGroupAttribTable();
	VertexFaceGroupAttribTable(const YsShellExt &shl);
	VertexFaceGroupAttribTable(const VertexFaceGroupAttribTable <toFind> &from);
	VertexFaceGroupAttribTable(VertexFaceGroupAttribTable <toFind> &&from);
	VertexFaceGroupAttribTable &operator=(const VertexFaceGroupAttribTable <toFind> &from);
	VertexFaceGroupAttribTable &operator=(VertexFaceGroupAttribTable <toFind> &&from);
	~VertexFaceGroupAttribTable();
protected:
	void Initialize(void);
	YSRESULT MakeKey(YSHASHKEY key[2],YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd) const;
public:
	void SetShell(const YsShellExt &shl);

	YSRESULT Add(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,const toFind &attrib);
	YSRESULT Add(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,toFind &&attrib);

	YSRESULT Update(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,const toFind &attrib);
	YSRESULT Update(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,toFind &&attrib);

	const toFind *Find(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd) const;
	toFind *Find(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd);

	const toFind *Find(YSHASHKEY vtKey,YSHASHKEY fgKey) const;
	toFind *Find(YSHASHKEY vtKey,YSHASHKEY fgKey);
};

template <class toFind>
YsShellExt::VertexFaceGroupAttribTable<toFind>::VertexFaceGroupAttribTable(const VertexFaceGroupAttribTable <toFind> &from)
{
	this->CopyFrom(from);
	this->shlPtr=from.shlPtr;
}

template <class toFind>
YsShellExt::VertexFaceGroupAttribTable<toFind>::VertexFaceGroupAttribTable(VertexFaceGroupAttribTable <toFind> &&from)
{
	this->MoveFrom(from);
	this->shlPtr=from.shlPtr;
}

template <class toFind>
YsShellExt::VertexFaceGroupAttribTable <toFind> &YsShellExt::VertexFaceGroupAttribTable<toFind>::operator=(const VertexFaceGroupAttribTable <toFind> &from)
{
	this->CopyFrom(from);
	this->shlPtr=from.shlPtr;
	return *this;
}

template <class toFind>
YsShellExt::VertexFaceGroupAttribTable <toFind> &YsShellExt::VertexFaceGroupAttribTable<toFind>::operator=(VertexFaceGroupAttribTable <toFind> &&from)
{
	this->MoveFrom(from);
	this->shlPtr=from.shlPtr;
	return *this;
}

template <class toFind>
YsShellExt::VertexFaceGroupAttribTable<toFind>::VertexFaceGroupAttribTable()
{
	Initialize();
}

template <class toFind>
YsShellExt::VertexFaceGroupAttribTable<toFind>::~VertexFaceGroupAttribTable()
{
	Initialize();
}

template <class toFind>
YsShellExt::VertexFaceGroupAttribTable<toFind>::VertexFaceGroupAttribTable(const YsShellExt &shl)
{
	Initialize();
	SetShell(shl);
}

template <class toFind>
void YsShellExt::VertexFaceGroupAttribTable<toFind>::Initialize(void)
{
	this->SetOrderSensitivity(YSTRUE);
	shlPtr=NULL;
}

template <class toFind>
YSRESULT YsShellExt::VertexFaceGroupAttribTable<toFind>::MakeKey(YSHASHKEY key[2],YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd) const
{
	if(NULL==shlPtr)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  YsShellExt pointer not set!\n");
		key[0]=0;
		key[1]=0;
		return YSERR;
	}

	key[0]=shlPtr->GetSearchKey(vtHd);
	key[1]=shlPtr->GetSearchKey(fgHd);
	return YSOK;
}

template <class toFind>
void YsShellExt::VertexFaceGroupAttribTable<toFind>::SetShell(const YsShellExt &shl)
{
	shlPtr=&shl;
}

template <class toFind>
YSRESULT YsShellExt::VertexFaceGroupAttribTable<toFind>::Add(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,const toFind &attrib)
{
	YSHASHKEY key[2];
	if(YSOK==MakeKey(key,vtHd,fgHd))
	{
		return AddElement(2,key,attrib);
	}
	return YSERR;
}
template <class toFind>
YSRESULT YsShellExt::VertexFaceGroupAttribTable<toFind>::Add(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,toFind &&attrib)
{
	YSHASHKEY key[2];
	if(YSOK==MakeKey(key,vtHd,fgHd))
	{
		return AddElement(2,key,attrib);
	}
	return YSERR;
}
template <class toFind>
YSRESULT YsShellExt::VertexFaceGroupAttribTable<toFind>::Update(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,const toFind &attrib)
{
	YSHASHKEY key[2];
	if(YSOK==MakeKey(key,vtHd,fgHd))
	{
		return this->UpdateElement(2,key,attrib);
	}
	return YSERR;
}
template <class toFind>
YSRESULT YsShellExt::VertexFaceGroupAttribTable<toFind>::Update(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd,toFind &&attrib)
{
	YSHASHKEY key[2];
	if(YSOK==MakeKey(key,vtHd,fgHd))
	{
		return this->UpdateElement(2,key,attrib);
	}
	return YSERR;
}
template <class toFind>
const toFind *YsShellExt::VertexFaceGroupAttribTable<toFind>::Find(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd) const
{
	YSHASHKEY key[2];
	if(YSOK==MakeKey(key,vtHd,fgHd))
	{
		return this->FindElement(2,key);
	}
	return nullptr;
}
template <class toFind>
toFind *YsShellExt::VertexFaceGroupAttribTable<toFind>::Find(YsShellVertexHandle vtHd,YsShellExt::FaceGroupHandle fgHd)
{
	YSHASHKEY key[2];
	if(YSOK==MakeKey(key,vtHd,fgHd))
	{
		return this->FindElement(2,key);
	}
	return nullptr;
}
template <class toFind>
const toFind *YsShellExt::VertexFaceGroupAttribTable<toFind>::Find(YSHASHKEY vtKey,YSHASHKEY fgKey) const
{
	const YSHASHKEY key[2]={vtKey,fgKey};
	return this->FindElement(2,key);
}
template <class toFind>
toFind *YsShellExt::VertexFaceGroupAttribTable<toFind>::Find(YSHASHKEY vtKey,YSHASHKEY fgKey)
{
	const YSHASHKEY key[2]={vtKey,fgKey};
	return this->FindElement(2,key);
}


////////////////////////////////////////////////////////////

template <class toFind>
class YsShellExt::FaceGroupAttribTable : public YsHandleToAttribTable <toFind,YsShellExt,YsShellExt::FaceGroupHandle>
{
public:
	FaceGroupAttribTable();
	FaceGroupAttribTable(const YsShellExt &shl);
	FaceGroupAttribTable(const FaceGroupAttribTable &incoming);
	FaceGroupAttribTable(FaceGroupAttribTable &&incoming);
	FaceGroupAttribTable <toFind> &operator=(const FaceGroupAttribTable &incoming);
	FaceGroupAttribTable <toFind> &operator=(FaceGroupAttribTable &&incoming);
	void SetShell(const YsShellExt &shl);
};

template <class toFind>
YsShellExt::FaceGroupAttribTable<toFind>::FaceGroupAttribTable()
{
}
template <class toFind>
YsShellExt::FaceGroupAttribTable<toFind>::FaceGroupAttribTable(const YsShellExt &shl)
{
	YsHandleToAttribTable <toFind,YsShellExt,YsShellExt::FaceGroupHandle>::SetStorage(shl);
}
template <class toFind>
YsShellExt::FaceGroupAttribTable<toFind>::FaceGroupAttribTable(const FaceGroupAttribTable &incoming)
{
	this->CopyFrom(incoming);
}
template <class toFind>
YsShellExt::FaceGroupAttribTable<toFind>::FaceGroupAttribTable(FaceGroupAttribTable &&incoming)
{
	this->MoveFrom(incoming);
}
template <class toFind>
YsShellExt::FaceGroupAttribTable <toFind> &YsShellExt::FaceGroupAttribTable<toFind>::operator=(const FaceGroupAttribTable &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}
template <class toFind>
YsShellExt::FaceGroupAttribTable <toFind> &YsShellExt::FaceGroupAttribTable<toFind>::operator=(FaceGroupAttribTable &&incoming)
{
	this->MoveFrom(incoming);
	return *this;
}
template <class toFind>
void YsShellExt::FaceGroupAttribTable<toFind>::SetShell(const YsShellExt &shl)
{
	YsHandleToAttribTable <toFind,YsShellExt,YsShellExt::FaceGroupHandle>::SetStorage(shl);
}

/* } */
#endif
