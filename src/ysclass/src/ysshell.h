/* ////////////////////////////////////////////////////////////

File Name: ysshell.h
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

#ifndef YSSHELL_IS_INCLUDED
#define YSSHELL_IS_INCLUDED
/* { */

#include "ysdef.h"
#include "yslist2.h"
#include "ysproperty.h"
#include "ysshelltype.h"
#include "yshash.h"
#include "ysstring.h"
#include "ysgeometry.h"
#include "yseditarray.h"
#include "yshastextmetadata.h"

#ifdef YS_COMPILING_YSSHELL_CPP
class YsShell;
static YSRESULT YsBlendShell1to2
    (YsShell &newShell,
     const YsShell &sh1,const YsShell &sh2,int f,YSBOOL elimOverlap);
#endif


// YsBaseClass
//   YsShell

// Declaration /////////////////////////////////////////////

class YsShellSpecialAttribute
	// If an application wants to have special attributes :
	//     Inherit YsShell class and override functions
	//             YsShell::AttachSpecialAttributeToNewVertex(),
	//             YsShell::AttachSpecialAttributeToNewPolygon(),
	//             YsShell::DetachSpecialAttributeBeforeDeletingVertex() and
	//             YsShell::AttachSpecialAttributeBeforeDeletingPolygon().
	//      In this case, special attribute must be created
	//      and attached inside AttachSpecialAttributeToNew?????()
	//      and must be detach & deleted inside DetachSpecialAttributeBeforeDeleting?????().
	//      To detach, program must call function YsShellItem::SpecialAttributeDetached().
	//      This will guarantee that all vertices and polygons have
	//      necessary attributes.
{
};

class YsShellItem
{
public:
	YsShellItem();
	~YsShellItem();
	void Initialize(void);

	void CopyAttribute(const YsShellItem &from);

	int freeAttribute0; // Use free for any purpose
	int freeAttribute1; // Use free for any purpose
	int freeAttribute2; // Use free for any purpose
	mutable int freeAttribute3; // Use free for any purpose
	mutable int freeAttribute4;

	// 2016/09/14 Hash key is managed by YsEditArray.  Deleted from YsShellItem.
	//            Still some legacy code is using Mapper function of YsShellVertex, which requires
	//            hashKey stored in the element.  Therefore, searchKey is now a member of YsShellVertex.

	// const class YsShell *belongTo;  // belongTo is set only in CreateVertex
	// 2006/02/07 Use vtHd/plHd->GetContainer() instead of belongTo

	YSRESULT SetSpecialAttribute(YsShellSpecialAttribute *attrib);
	YsShellSpecialAttribute *GetSpecialAttribute(void);
	const YsShellSpecialAttribute *GetSpecialAttribute(void) const;
	void SpecialAttributeDetached(void);

protected:
	// YsShellItem will initialize 'special' to NULL.
	// But never delete it.
	YsShellSpecialAttribute *special;
};

// Declaration /////////////////////////////////////////////
class YsShellVertex : public YsShellItem
{
public:
	YSHASHKEY searchKey; // Key for hash search

	YSHASHKEY GetSearchKey(void) const;

	YsShellVertex();
	void Initialize(void);

	YSRESULT IncrementReferenceCount(void);
	YSRESULT DecrementReferenceCount(void);
	YSRESULT SetPosition(const YsVec3 &pos);
	YSRESULT SetNormal(const YsVec3 &nom);
	const YsVec3 &GetPosition(void) const;
	const YsVec3 &GetNormal(void) const;
	int GetReferenceCount(void) const;

	YSRESULT SetReferenceCount(int n);  // This function must be eliminated soon

	YsEditArrayObjectHandle <YsShellVertex> Mapper(const class YsShell &owner) const;
	void SetMapper(class YsShell &owner,YsEditArrayObjectHandle <YsShellVertex> toVtHd);

	// 2016/12/16 Removed it.  Hope nobody was using it.  Sorry, if you were.
	//   YsBubble module of VolMesh was the only user of v2HdMap in my code base.
	//   And, it apparently was already unnecessitated long time ago.
	//   The value was set in some locations, but was not read.
	// union
	// {
	// 	const YsListItem <class YsShell2dVertex> *v2HdMap;  // 2005/01/21 For co-operation with YsShell2d
	// };

protected:
	YsVec3 pos;
	int refCount;
#ifndef YSLOWMEM
	YsVec3 nom;
#endif
};

// Declaration /////////////////////////////////////////////
class YsShellPolygon : public YsShellItem
{
public:
	YsShellPolygon();
	void Initialize(void);

	YSRESULT SetNormal(const YsVec3 &n);
	const YsVec3 &GetNormal(void) const;
	YSRESULT SetColor(const YsColor &c);
	const YsColor &GetColor(void) const;
	YSRESULT SetVertexList(const class YsShell &shl,YSSIZE_T nVtId,const int vtId[]);
	YSRESULT SetVertexList(YSSIZE_T nVtx,const YsEditArrayObjectHandle <YsShellVertex> vtx[]);
	int GetNumVertex(void) const;
	const YsEditArrayObjectHandle <YsShellVertex> *GetVertexArray(void) const;

	// 2015/07/26
	//   Transition from YsListContainer to YsEditArray.
	//   Object-handle itself no longer can cache the pointer to its owner.
	//   Need YsShell object to find its index.
	int GetVertexId(const YsShell &shl,YSSIZE_T i)const;

	YsEditArrayObjectHandle <YsShellVertex> GetVertexHandle(YSSIZE_T i) const;
	void Invert(void);

protected:
	YsVec3 nom;
	YsArray <YsEditArrayObjectHandle <YsShellVertex>,4> idx;
#ifndef YSLOWMEM
	YsColor col;
#endif
};

class YsShellReadSrfVariable
{
public:
	YsArray <YsString,16> args;
	int mode;
	YsEditArrayObjectHandle <YsShellPolygon> plHd;
	YSBOOL noShadingPolygon;
	YsArray <YsEditArrayObjectHandle <YsShellVertex>,16> plVtHdList;
	YsArray <YsEditArrayObjectHandle <YsShellVertex> > allVtHdList;
	YsArray <YsEditArrayObjectHandle <YsShellPolygon> > allPlHdList;
	YsColor color;
	YsVec3 nom;

	int errorCount;

	YsShellReadSrfVariable();
	void PrintErrorMessage(const char msg[]);
};


////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

class YsShellRepairFlipDirectionInfo
{
public:
	YsEditArrayObjectHandle <YsShellPolygon> plHd;
	YsEditArrayObjectHandle <YsShellVertex> vtHd1,vtHd2;
};

class YsShellVertexHandleKeyPair
{
public:
	YsEditArrayObjectHandle <YsShellVertex> vtHd;
	YSHASHKEY searchKey;
};

class YsShellPolygonHandlehKeyPair
{
public:
	YsEditArrayObjectHandle <YsShellPolygon> plHd;
	YSHASHKEY searchKey;
};

typedef enum 
{
	YSSHELL_NOELEM,
	YSSHELL_VERTEX,
	YSSHELL_EDGE,
	YSSHELL_POLYGON
} YSSHELLELEMTYPE;


/*! This class can answer the location of a vertex.  
    YsShell is indeed a vertex-position source.
    However, if I make YsShell as a vertex-position source, GetVertexPosition, 
    which is used very often, will become a virtual function.
    It might make a measurable performance difference.
    To avoid that, YsShell is not made a vertex-position source.
*/
class YsShellVertexPositionSource
{
public:
	virtual YsVec3 GetVertexPosition(YsEditArrayObjectHandle <YsShellVertex> vtHd) const=0;
};

/*! If the vertex position source needs to give the vertex coordinate stored in the YsShell without making any tweek,
    YsShellPassThroughSource can be used in place for YsShellVertexPositionSource. */
class YsShellPassThroughSource : public YsShellVertexPositionSource
{
private:
	const YsShell *shlPtr;
public:
	YsShellPassThroughSource(const YsShell &shl);
	virtual YsVec3 GetVertexPosition(YsEditArrayObjectHandle <YsShellVertex> vtHd) const;
};


// Declaration /////////////////////////////////////////////
class YsShellSearchTable;

/*! A simple polygonal shell class.
    It represents a shell with a list of vertices and polygons.
    It is a base class for YsShellExt, which has richer information such as constraint edges and face groups.
*/
class YsShell : public YsHasTextMetaData
{
public:
	/*! This class does nothing.  It exists just for making YsShell, YsShellExt, and YsShellExtEdit available to the same C++ template. */
	class StopIncUndo
	{
	public:
		StopIncUndo(YsShell *shl)
		{
		}
		StopIncUndo(YsShell &shl)
		{
		}
	};



	using YsHasTextMetaData::AllMetaData;
	using YsHasTextMetaData::MetaData;
	using YsHasTextMetaData::MetaDataHandle;
	using YsHasTextMetaData::MetaDataStore;
	using YsHasTextMetaData::AddMetaData;
	using YsHasTextMetaData::SetMetaDataValue;
	using YsHasTextMetaData::DeleteMetaData;
	using YsHasTextMetaData::FreezeMetaData;
	using YsHasTextMetaData::MeltMetaData;
	using YsHasTextMetaData::DeleteFrozenMetaData;
	using YsHasTextMetaData::GetSearchKey;
	using YsHasTextMetaData::GetNumMetaData;
	using YsHasTextMetaData::FindMetaData;
	using YsHasTextMetaData::GetMetaDataKey;
	using YsHasTextMetaData::GetMetaDataValue;
	using YsHasTextMetaData::GetMetaData;
	using YsHasTextMetaData::MetaDataEnumerator;




// CAUTION!
// Whenever add an item, do not forget to modify operator=
public:
	/*! A handle type for a vertex.
	    A vertex handle is needed for accessing a vertex-related information.
	    For legacy programs, it is also typedef-ed as YsShellVertexHandle.
	*/
	typedef YsEditArrayObjectHandle <YsShellVertex> VertexHandle;
	/*! A handle type for a polygon.
	    A polygon handle is needed for accessing a polygon-related information.
	    For legacy programs, it is also typedef-ed as YsShellpolygonHandle.
	*/
	typedef YsEditArrayObjectHandle <YsShellPolygon> PolygonHandle;

public:
	class Condition
	{
	public:
		virtual YSRESULT TestPolygon(const YsShell &shl,PolygonHandle plHd) const=0;
		virtual YSRESULT TestEdge(const YsShell &shl,VertexHandle edVtHd0,VertexHandle edVtHd1) const=0;
	};
	class PassAll : public Condition
	{
	public:
		virtual YSRESULT TestPolygon(const YsShell &shl,PolygonHandle plHd) const;
		virtual YSRESULT TestEdge(const YsShell &shl,VertexHandle edVtHd0,VertexHandle edVtHd1) const;
	};


private:
	/* Oh no!  Why did I do this.  This feature is used in some legacy code, which I don't want to run extensive tests
	   again, but it stinks.  Cannot get rid of some pointers.
	*/
	YsHashTable <VertexHandle> vtxMapping;

protected:
// Members to copy in operator= {
	YSBOOL matrixIsSet;
	YsMatrix4x4 mat;

	// 2015/07/26 Transition from YsListContainer to YsEditArray
	//   Search keys are managed by YsShell at this time.  Don't confuse with the search key
	//   in the YsEditArray.
	YsEditArray <YsShellVertex> vtx;
	mutable VertexHandle currentVtHd;  // No good for multi-threading, but for backward-compatibility.

	YsEditArray <YsShellPolygon> plg;
	mutable PolygonHandle currentPlHd;
// }



public:
	typedef YsEditArray <YsShellVertex>::HandleStore VertexStore2;
	typedef YsEditArray <YsShellPolygon>::HandleStore PolygonStore2;


	// Utility classes
	class Edge
	{
	public:
		VertexHandle edVtHd[2];

		inline void Set(VertexHandle vtHd0,VertexHandle vtHd1)
		{
			edVtHd[0]=vtHd0;
			edVtHd[1]=vtHd1;
		}
		inline Edge()
		{
		}
		inline Edge(VertexHandle vtHd0,VertexHandle vtHd1)
		{
			Set(vtHd0,vtHd1);
		}
		explicit inline Edge(const VertexHandle vtHd[2])
		{
			Set(vtHd[0],vtHd[1]);
		}
		inline void Set(const VertexHandle vtHd[2])
		{
			Set(vtHd[0],vtHd[1]);
		}
		inline operator const VertexHandle *()
		{
			return edVtHd;
		}
		inline const VertexHandle operator[](YSSIZE_T idx) const
		{
			return edVtHd[idx];
		}
		inline VertexHandle &operator[](YSSIZE_T idx)
		{
			return edVtHd[idx];
		}
		bool operator==(const Edge &incoming) const
		{
			return (this->edVtHd[0]==incoming.edVtHd[0] && this->edVtHd[1]==incoming.edVtHd[1]) ||
			       (this->edVtHd[0]==incoming.edVtHd[1] && this->edVtHd[1]==incoming.edVtHd[0]);
		}
		bool operator!=(const Edge &incoming) const
		{
			return (this->edVtHd[0]!=incoming.edVtHd[0] || this->edVtHd[1]!=incoming.edVtHd[1]) &&
			       (this->edVtHd[0]!=incoming.edVtHd[1] || this->edVtHd[1]!=incoming.edVtHd[0]);
		}
	};

	template <const int N>
	class StaticPolygon
	{
	public:
		VertexHandle vtHd[N];
		inline operator const VertexHandle *()
		{
			return vtHd;
		}
		inline const VertexHandle operator[](YSSIZE_T idx) const
		{
			return vtHd[idx];
		}
		inline VertexHandle &operator[](YSSIZE_T idx)
		{
			return vtHd[idx];
		}

		StaticPolygon()
		{
		}
		explicit inline StaticPolygon(const VertexHandle vtHd[N])
		{
			Set(vtHd);
		}
		inline void Set(const VertexHandle incoming[3])
		{
			for(int i=0; i<N; ++i) // Hope optimizer is smart enough to expand the loop.
			{
				this->vtHd[i]=incoming[i];
			}
		}
	};

	class Triangle : public StaticPolygon<3>
	{
	public:
		inline void Set(VertexHandle vtHd0,VertexHandle vtHd1,VertexHandle vtHd2)
		{
			vtHd[0]=vtHd0;
			vtHd[1]=vtHd1;
			vtHd[2]=vtHd2;
		}
		inline Triangle()
		{
		}
		inline Triangle(VertexHandle vtHd0,VertexHandle vtHd1,VertexHandle vtHd2)
		{
			Set(vtHd0,vtHd1,vtHd2);
		}
		// I would stay away from defining operator== and operator!=.
		// Sometimes I want it to be order sensitive, and sometimes I don't.
	};

	class EdgeAndPos : public Edge
	{
	public:
		YsVec3 pos;
	};
	class PolygonAndPos
	{
	public:
		PolygonHandle plHd;
		YsVec3 pos;
	};
	class PolygonAndValue
	{
	public:
		PolygonHandle plHd;
		double value;
	};
	class PolygonAndIndex
	{
	public:
		PolygonHandle plHd;
		YSSIZE_T idx;
	};
	class VertexAndIndex
	{
	public:
		VertexHandle vtHd;
		YSSIZE_T idx;
	};
	class VertexAndPolygon
	{
	public:
		VertexHandle vtHd;
		PolygonHandle plHd;
	};
	class VertexAndPos
	{
	public:
		VertexHandle vtHd;
		YsVec3 pos;
	};
	class VertexAndEdge : public Edge
	{
	public:
		VertexHandle vtHd;
	};
	class VertexPolygonAndIndex
	{
	public:
		VertexHandle vtHd;
		PolygonHandle plHd;
		YSSIZE_T idx;
	};
	class VertexPolygonAndPos
	{
	public:
		VertexHandle vtHd;
		PolygonHandle plHd;
		YsVec3 pos;
	};
	class PolygonAndVertexHandleArray
	{
	public:
		PolygonHandle plHd;
		YsArray <VertexHandle> vtHd;
	};

	class Elem
	{
	public:
		YSSHELLELEMTYPE elemType;
		VertexHandle vtHd;
		VertexHandle edVtHd[2];
		PolygonHandle plHd;
	};

public:
	/*! Support for STL-like iterator */
	class VertexEnumerator
	{
	public:
		const YsShell *shl;

		class iterator
		{
		public:
			const YsShell *shl;
			VertexHandle prev,vtHd,next;
			int dir;
		private:
			inline void Forward(void)
			{
				if(0<=dir)
				{
					prev=vtHd;
					vtHd=next;
					next=shl->FindNextVertex(vtHd);
				}
				else
				{
					next=vtHd;
					vtHd=prev;
					prev=shl->FindPrevVertex(vtHd);
				}
			}
			inline void Backward(void)
			{
				if(0<=dir)
				{
					next=vtHd;
					vtHd=prev;
					prev=shl->FindPrevVertex(vtHd);
				}
				else
				{
					prev=vtHd;
					vtHd=next;
					next=shl->FindNextVertex(vtHd);
				}
			}
		public:
			inline iterator &operator++()
			{
				Forward();
				return *this;
			}
			inline iterator operator++(int)
			{
				iterator copy=*this;
				Forward();
				return copy;
			}
			inline iterator &operator--()
			{
				Backward();
				return *this;
			}
			inline iterator operator--(int)
			{
				iterator copy=*this;
				Backward();
				return copy;
			}
			inline bool operator==(const iterator &from)
			{
				return (shl==from.shl && vtHd==from.vtHd);
			}
			inline bool operator!=(const iterator &from)
			{
				return (shl!=from.shl || vtHd!=from.vtHd);
			}
			inline VertexHandle &operator*()
			{
				return vtHd;
			}
		};

		/*! Support for STL-like iterator */
		inline iterator begin() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.vtHd=shl->FindNextVertex(NULL);
			iter.next=shl->FindNextVertex(iter.vtHd);
			iter.dir=1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator end() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.vtHd=NULL;
			iter.next=NULL;
			iter.dir=1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator rbegin() const
		{
			iterator iter;
			iter.shl=shl;
			iter.next=NULL;
			iter.vtHd=shl->FindPrevVertex(NULL);
			iter.prev=shl->FindPrevVertex(iter.vtHd);
			iter.dir=-1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator rend() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.vtHd=NULL;
			iter.next=NULL;
			iter.dir=-1;
			return iter;
		}

		YsArray <VertexHandle> Array(void) const
		{
			YsArray <VertexHandle> hdArray(shl->GetNumVertex(),NULL);
			YSSIZE_T idx=0;
			for(auto hd=begin(); hd!=end(); ++hd)
			{
				hdArray[idx]=*hd;
				++idx;
			}
			return hdArray;
		}
	};

	/*! Support for STL-like iterator 
	    Vertices can be iterated by: 
	    YsShell shl; or const YsShell shl;
	    for(auto vtIter=shl.AllVertex().begin(); vtIter!=shl.AllVertex().end(); ++vtIter)
	    {
			(*vtIter) is a VertexHandle in the loop.
	    }

		Or, for range-based for:
		for(auto vtHd : shl.AllVertex())
		{
			vtHd is a VertexHandle in the loop.
		}

		Since the iterator caches the next vertex handle in itself, it is safe to delete a vertex inside the loop.
	    */
	inline const VertexEnumerator AllVertex(void) const
	{
		VertexEnumerator allVertex;
		allVertex.shl=this;
		return allVertex;
	}



	/*! Support for STL-like iterator */
	class PolygonEnumerator
	{
	public:
		const YsShell *shl;

		class iterator
		{
		public:
			const YsShell *shl;
			PolygonHandle prev,plHd,next;
			int dir;
		private:
			inline void Forward(void)
			{
				if(0<=dir)
				{
					prev=plHd;
					plHd=next;
					next=shl->FindNextPolygon(plHd);
				}
				else
				{
					next=plHd;
					plHd=prev;
					prev=shl->FindPrevPolygon(plHd);
				}
			}
			inline void Backward(void)
			{
				if(0<=dir)
				{
					next=plHd;
					plHd=prev;
					prev=shl->FindPrevPolygon(plHd);
				}
				else
				{
					prev=plHd;
					plHd=next;
					next=shl->FindNextPolygon(plHd);
				}
			}
		public:
			inline iterator &operator++()
			{
				Forward();
				return *this;
			}
			inline iterator operator++(int)
			{
				iterator copy=*this;
				Forward();
				return copy;
			}
			inline iterator &operator--()
			{
				Backward();
				return *this;
			}
			inline iterator operator--(int)
			{
				iterator copy=*this;
				Backward();
				return copy;
			}
			inline bool operator==(const iterator &from)
			{
				return (shl==from.shl && plHd==from.plHd);
			}
			inline bool operator!=(const iterator &from)
			{
				return (shl!=from.shl || plHd!=from.plHd);
			}
			inline PolygonHandle &operator*()
			{
				return plHd;
			}
		};

		/*! Support for STL-like iterator */
		inline iterator begin() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.plHd=shl->FindNextPolygon(NULL);
			iter.next=shl->FindNextPolygon(iter.plHd);
			iter.dir=1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator end() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.plHd=NULL;
			iter.next=NULL;
			iter.dir=1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator rbegin() const
		{
			iterator iter;
			iter.shl=shl;
			iter.next=NULL;
			iter.plHd=shl->FindPrevPolygon(NULL);
			iter.prev=shl->FindPrevPolygon(iter.plHd);
			iter.dir=-1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator rend() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.plHd=NULL;
			iter.next=NULL;
			iter.dir=-1;
			return iter;
		}

		YsArray <PolygonHandle> Array(void) const
		{
			YsArray <PolygonHandle> hdArray(shl->GetNumPolygon(),NULL);
			YSSIZE_T idx=0;
			for(auto hd=begin(); hd!=end(); ++hd)
			{
				hdArray[idx]=*hd;
				++idx;
			}
			return hdArray;
		}
	};

	/*! Support for STL-like iterator 
	    Polygons can be iterated by: 
	    YsShell shl; or const YsShell shl;
	    for(auto plIter=shl.AllPolygon().begin(); plIter!=shl.AllPolygon().end(); ++plIter)
	    {
			(*plIter) is a PolygonHandle in the loop.
	    }

		Or, with range-based for loop:
	    for(auto plHd : shl.AllPolygon())
	    {
			plHd is a PolygonHandle in the loop.
	    }

		Since the iterator caches the next Polygon handle in itself, it is safe to delete a Polygon inside the loop.
	    */
	inline const PolygonEnumerator AllPolygon(void) const
	{
		PolygonEnumerator allPolygon;
		allPolygon.shl=this;
		return allPolygon;
	}



public:
	YsShell();
	YsShell(const YsShell &from);
	~YsShell();

private:
	void CommonInitialization(void);

public:

	YSRESULT AttachSearchTable(YsShellSearchTable *search) const;
	  // NOTE:YsShell will not automatically release memory of search table
	  //      YsShell user is responsible for releasing memory block assigned to the search table
	YSRESULT DetachSearchTable(void) const;
	const YsShellSearchTable *GetSearchTable(void) const;


// Modifiers
public:
	const YsShell &operator=(const YsShell &from);

	/*! Copies a shell.
	*/
	const YsShell &CopyFrom(const YsShell &incoming);

	void CleanUp(void);
	int AddVertex(const YsVec3 &vec);
	// int AddPolygon(int nv,const int v[]);   2005/01/19 Deleted
	int AddPolygonVertex(YSSIZE_T nv,const YsVec3 v[]);
	VertexHandle AddVertexH(const YsVec3 &vec);
	PolygonHandle AddPolygonH(YSSIZE_T nv,const int v[]);
	PolygonHandle AddPolygonH(YSSIZE_T nv,const VertexHandle v[]);
	PolygonHandle AddPolygonVertexH(YSSIZE_T nv,const YsVec3 v[]);
	YSRESULT ModifyVertexPosition(int vtId,const YsVec3 &neoPos);
	YSRESULT ModifyVertexPosition(VertexHandle vtHd,const YsVec3 &neoPos);

	/*! Modifies the vertex list of the polygon.
	    (Made function name consistent with YsShellExt class.)
	*/
	YSRESULT SetPolygonVertex(PolygonHandle plHd,YSSIZE_T nv,const VertexHandle plVtHd[]);
	YSRESULT SetPolygonVertex(PolygonHandle plHd,const YsConstArrayMask <VertexHandle> &plVtHd);

	YSRESULT ModifyPolygon(int plId,YSSIZE_T nv,const int v[]);
	YSRESULT ModifyPolygon(PolygonHandle plHd,YSSIZE_T nv,const VertexHandle v[]);
	YSRESULT ModifyPolygon(PolygonHandle plHd,YsConstArrayMask <VertexHandle> plVtHd);
	YSRESULT DeletePolygon(int plId);
	YSRESULT DeletePolygon(PolygonHandle plHd);
	YSRESULT DeleteVertex(int vtId);
	YSRESULT DeleteVertex(VertexHandle vtHd);
	YSRESULT DeleteVertexAtTheSamePosition(int nLatX=32,int nLatY=32,int nLatZ=32);
	YSRESULT DeleteVertexAtTheSamePosition(int &nDeleted,int nLatX=32,int nLatY=32,int nLatZ=32);
	YSRESULT MakeVertexMappingToDeleteVertexAtTheSamePositionByKdTree(const double &tol);
	YSRESULT DeleteVertexAtTheSamePositionByKdTree(int &nDeleted,const double &tol=YsTolerance);
	YSRESULT RepairPolygonOrientationBasedOnNormal(void);  // search table required
	YSRESULT RepairPolygonOrientationAssumingClosedShell(void);  // search table required
	YSRESULT RepairPolygonOrientationBasedOnNormal(YsArray <PolygonHandle> &invertedPlHd);
	YSRESULT RepairPolygonOrientationAssumingClosedShell
	    (YsArray <PolygonHandle> &invertedPlHdList);  // search table required
	YSBOOL IsOverlappingPolygon(PolygonHandle plHd);  // search table required
	YSRESULT OmitPolygonAfterBlend(void);
	void SetMatrix(const YsMatrix4x4 &mat);
	YSRESULT Triangulate(YsArray <PolygonHandle> &newPlHd,PolygonHandle plHd,YSBOOL killOriginalPolygon);

	void SetTrustPolygonNormal(YSBOOL trust);
	YSBOOL TrustPolygonNormal(void) const;

	YSRESULT FreezeVertex(VertexHandle vtHd);
	YSRESULT FreezePolygon(PolygonHandle plHd);
	YSRESULT MeltVertex(VertexHandle vtHd);
	YSRESULT MeltPolygon(PolygonHandle plHd);
	YSRESULT DeleteFrozenVertex(VertexHandle vtHd);
	YSRESULT DeleteFrozenPolygon(PolygonHandle plHd);
	YSRESULT DisposeFrozenPolygon(void);
	YSRESULT DisposeFrozenVertex(void);

public:
	YsShellSpecialAttribute *GetSpecialAttributeVertex(VertexHandle vtHd);
	YsShellSpecialAttribute *GetSpecialAttributeVertexMaybeFrozen(VertexHandle vtHd);
	const YsShellSpecialAttribute *GetSpecialAttributeVertex(VertexHandle vtHd) const;
	YsShellSpecialAttribute *GetSpecialAttributePolygon(PolygonHandle plHd);
	YsShellSpecialAttribute *GetSpecialAttributePolygonMaybeFrozen(PolygonHandle plHd);
	const YsShellSpecialAttribute *GetSpecialAttributePolygon(PolygonHandle plHd) const;

	YSRESULT SetSpecialAttributeVertex(VertexHandle vtHd,YsShellSpecialAttribute *attr);
	YSRESULT SetSpecialAttributePolygon(PolygonHandle plHd,YsShellSpecialAttribute *attr);
protected:
	virtual YSRESULT AttachSpecialAttributeToNewVertex(YsShellVertex &vtx,VertexHandle vtHd);
	virtual YSRESULT AttachSpecialAttributeToNewPolygon(YsShellPolygon &plg,PolygonHandle plHd);
	virtual YSRESULT DetachSpecialAttributeBeforeDeletingVertex(YsShellVertex &vtx);
	virtual YSRESULT DetachSpecialAttributeBeforeDeletingPolygon(YsShellPolygon &plg);

protected:
	PolygonHandle AddPolygonVertex(YSSIZE_T nv,const YsVec3 v[],int fa3,const int fa4[]);

	mutable YsShellSearchTable *searchTable;

	YSBOOL FindRepairFlipDirectionOfOneReliablePolygon
	    (YsArray <PolygonHandle> &invertedPlHdList,PolygonHandle plHd);
	YSRESULT RecursivelyRepairFlipDirection
	    (int &nDone,YsArray <PolygonHandle> &invertedPlHdList,PolygonHandle plHd);
	YSRESULT RepairFlipDirectionOfOnePolygon
	    (YsArray <PolygonHandle> &invertedPlHdList,YsShellRepairFlipDirectionInfo &info);
	YSRESULT AddNeighborToList
	    (YsArray <YsShellRepairFlipDirectionInfo,4096> &todo,PolygonHandle plHd);

public:
	YSRESULT SaveFreeAttribute4OfPolygon(YsArray <int> &attrib4) const;
	YSRESULT RestoreFreeAttribute4OfPolygon(YsArray <int> &attrib4) const;
	YSRESULT ClearFreeAttribute4OfPolygon(int attrib4);
	YSRESULT SaveFreeAttribute4OfVertex(YsArray <int> &attrib4) const;
	YSRESULT RestoreFreeAttribute4OfVertex(YsArray <int> &attrib4) const;
	YSRESULT ClearFreeAttribute4OfVertex(int attrib4);

protected:
	YSRESULT SaveFreeAttribute4OfPolygonAndNeighbor(
	    YsArray <YsPair <PolygonHandle,int>,64> &savePlAtt4,
	    int nPl,const PolygonHandle plHdList[],int freeAtt4Value) const;
	YSRESULT RestoreFreeAttribute4OfPolygon(int nSavePlAtt4,const YsPair <PolygonHandle,int> savePlAtt4[]) const;
	YSRESULT SaveFreeAttribute4OfVertexAndNeighbor(
	    YsArray <YsPair <VertexHandle,int>,64> &saveVtAtt4,
	    int nVt,const VertexHandle vtHdList[],int freeAtt4Value) const;
	YSRESULT RestoreFreeAttribute4OfVertex(int nSaveVtAtt4,const YsPair <VertexHandle,int> saveVtAtt4[]) const;

// Non modifiers
public:
	void Encache(void) const;
	void Decache(void) const;

	void RewindVtxPtr(void) const;
	VertexHandle StepVtxPtr(void) const;
	void RewindPlgPtr(void) const;
	PolygonHandle StepPlgPtr(void) const;

	/*! Returns a handle of a vertex that is stored immediately after vtHd.
	    If vtHd is the last vertex, it returns nullptr.
	*/
	VertexHandle FindNextVertex(VertexHandle vtHd) const;
	/*! Returns a handle of a vertex that is stored immediately before vtHd.
	    If vtHd is the first vertex, it returns nullptr.
	*/
	VertexHandle FindPrevVertex(VertexHandle vtHd) const;

	/*! Returns a null vertex handle. */
	inline VertexHandle NullVertex(void) const
	{
		VertexHandle vtHd=nullptr;
		return vtHd;
	}

	/*! Returns a handle of a polygon that is stored immediately after plHd.
	    If plHd is the last polygon, it returns nullptr.
	*/
	PolygonHandle FindNextPolygon(PolygonHandle plHd) const;
	/*! Returns a handle of a polygon that is stored immediately before plHd.
	    If plHd is the first polygon, it returns nullptr.
	*/
	PolygonHandle FindPrevPolygon(PolygonHandle plHd) const;

	/*! Returns a null polygon handle.
	*/
	inline PolygonHandle NullPolygon(void) const
	{
		PolygonHandle plHd=nullptr;
		return plHd;
	}

	int GetVertexIdFromHandle(VertexHandle vtx) const;
	int GetPolygonIdFromHandle(PolygonHandle plg) const;
	VertexHandle GetVertexHandleFromId(YSSIZE_T vtId) const;
	PolygonHandle GetPolygonHandleFromId(YSSIZE_T plId) const;


	/*! Returns normal vector of the polygon plHd. */
	YsVec3 GetNormal(PolygonHandle plHd) const;

	/*! Returns normal vector of the polygon in nom.  If plHd does not belong to this YsShell, this
	    function will return YSERR.  */
	YSRESULT GetNormal(YsVec3 &nom,PolygonHandle plHd) const;

	/*! Returns color of the polygon plHd. */
	YsColor GetColor(PolygonHandle plHd) const;

	/*! Returns color of the polygon in nom.  If plHd does not belong to this YsShell, this
	    function will return YSERR.  */
	YSRESULT GetColor(YsColor &col,PolygonHandle plHd) const;

	/*! Deprecated.  Use GetNormal instead.  Left for backward compatibility. */
	YSRESULT GetNormalOfPolygon(YsVec3 &nom,int plId) const;

	/*! Deprecated.  Use GetColor instead.  Left for backward compatibility. */
	YSRESULT GetColorOfPolygon(YsColor &col,int plId) const;

	/*! Deprecated.  Use GetNormal instead.  Left for backward compatibility. */
	YSRESULT GetNormalOfPolygon(YsVec3 &nom,PolygonHandle plHd) const;

	/*! Deprecated.  Use GetColor instead.  Left for backward compatibility. */
	YSRESULT GetColorOfPolygon(YsColor &col,PolygonHandle plHd) const;

	YSRESULT ComputeNormalOfPolygonFromGeometry(YsVec3 &nom,PolygonHandle plHd) const;
	YSRESULT MakePolygonKeyNormalPairFromGeometry(YsPair <unsigned int,YsVec3> &plKeyNomPair,PolygonHandle plHd) const;
	YSRESULT MakePolygonKeyNormalPairListFromGeometry(YsArray <YsPair <unsigned int,YsVec3> > &plKeyNomPair,int nPl,const PolygonHandle plHdList[]) const;
	double ComputeDihedralAngle(PolygonHandle plHd1,PolygonHandle plHd2) const;
	double ComputeEdgeLength(VertexHandle vtHd1,VertexHandle vtHd2) const;
	double ComputeEdgeLength(int nVt,const VertexHandle vtHd[]) const;
	double ComputeAngleCos(const VertexHandle vtHd[3]) const;  // Straight -> 0.0
	double ComputeMinimumAngleCos(PolygonHandle plHd) const;
	double ComputeMinimumAngleCos(int nPlVt,const VertexHandle plVtHd[]) const;
	double ComputeVolume(void) const;
	double ComputeTotalArea(void) const;


	/*! Returns the shell center by taking average of the vertex positions. */
	YsVec3 GetShellCenter(void) const;

	/*! Returns center (non-weight average) of vertex positions. */
	const YsVec3 GetCenter(YSSIZE_T nVt,const VertexHandle vtHd[]) const;

	/*! Returns center (non-weight average) of vertex positions. */
	template <const int N>
	inline const YsVec3 GetCenter(const YsArray <VertexHandle,N> &vtHdArray) const;

	/*! Returns center (non-weight average) of the vertices of the polygon. */
	const YsVec3 GetCenter(PolygonHandle plHd) const;

	/*! Returns center of the edge. */
	const YsVec3 GetCenter(VertexHandle edVtHd0,VertexHandle edVtHd1) const;

	/* Sorry, my bad.  I removed this function.  Please use GetCenter(Edge edge) instead.
	const YsVec3 GetCenter(const VertexHandle edVtHd[2]) const;
	*/

	/*! Returns center of the edge. */
	const YsVec3 GetCenter(Edge edge) const;


	const YsVec3 &GetCenterOfPolygon(YsVec3 &cen,PolygonHandle plHd) const;
	const YsVec3 &GetCenterOfEdge(YsVec3 &mid,VertexHandle edVtHd1,VertexHandle edVtHd2) const;
	const YsVec3 &GetCenterOfVertexGroup(YsVec3 &cen,YSSIZE_T  nVt,const VertexHandle vtHdList[]) const;
	const YsVec3 &GetCenterOfTriangle(YsVec3 &cen,VertexHandle trVtHd1,VertexHandle trVtHd2,VertexHandle trVtHd3) const;
	const YsVec3 &GetArbitraryInsidePointOfPolygon(YsVec3 &isp,PolygonHandle plHd) const;
	YSRESULT GetArbitraryInsidePointOfPolygonOnPlane(YsVec3 &isp,PolygonHandle plHd,const YsPlane &pln) const;

	const double GetEdgeSquareLength(VertexHandle edVtHd1,VertexHandle edVtHd2) const;
	const double GetEdgeSquareLength(VertexHandle edVtHd[2]) const;
	const double GetEdgeSquareLength(Edge edge) const;

	const double GetPointToVertexDistance(const YsVec3 &pos,VertexHandle vtHd) const;
	const double GetVertexToPointDistance(VertexHandle vtHd,const YsVec3 &pos) const;
	const double GetPointToVertexSquareDistance(const YsVec3 &pos,VertexHandle vtHd) const;
	const double GetVertexToPointSquareDistance(VertexHandle vtHd,const YsVec3 &pos) const;

	/*! Returns a length of line segments. */
	double GetLength(const YsConstArrayMask <VertexHandle> &vtHdSeq,YSBOOL isLoop=YSFALSE) const;

	const double GetEdgeLength(PolygonHandle plHd,YSSIZE_T edIdx) const;
	const double GetEdgeLength(VertexHandle edVtHd1,VertexHandle edVtHd2) const;
	const double GetEdgeLength(VertexHandle edVtHd[2]) const;
	const double GetEdgeLength(Edge edge) const;
	const double GetEdgeLength(YSSIZE_T nVt,const VertexHandle edVtHd[]) const;
	const double GetLoopLength(YSSIZE_T nVt,const VertexHandle edVtHd[]) const;

	YSRESULT SetNormalOfPolygon(int plId,const YsVec3 &nom);
	YSRESULT SetColorOfPolygon(int plId,const YsColor &col);
	YSRESULT SetNormalOfPolygon(PolygonHandle plHd,const YsVec3 &nom);
	YSRESULT SetColorOfPolygon(PolygonHandle plHd,const YsColor &col);

	YSRESULT ValidateVtId(YSSIZE_T nVtId,const int vtId[]) const;
	YSRESULT ValidatePlId(YSSIZE_T nPlId,const int plId[]) const;

	YSRESULT CheckPotentialPolygon(YSSIZE_T nVt,const VertexHandle vt[],const double &cosThr);

	int GetNumPolygon(void) const;
	int GetNumVertex(void) const;
	int GetMaxNumVertexOfPolygon(void) const;

	/*! Returns the vertex position in pos. */
	YSRESULT GetVertexPosition(YsVec3 &pos,int vtId) const;

	/*! Returns the vertex position in pos. */
	inline YSRESULT GetVertexPosition(YsVec3 &pos,VertexHandle vtHd) const;

	/*! Returns the vertex position. */
	inline YsVec3 GetVertexPosition(VertexHandle vtHd) const;

	/*! Alias for GetVertexPosition. */
	inline YSRESULT GetPosition(YsVec3 &pos,VertexHandle vtHd) const;

	/*! Alias for GetVertexPosition. */
	inline YsVec3 GetPosition(VertexHandle vtHd) const;

	/*! Returns a unit vector pointing from vtHd0 to vtHd1. */
	inline YsVec3 GetEdgeVector(VertexHandle edVtHd0,VertexHandle edVtHd1) const;

	/*! Returns a unit vector pointing from vtHd[0] to vtHd[1]. */
	inline YsVec3 GetEdgeVector(const VertexHandle edVtHd[2]) const;

	/*! Returns a unit vector pointing from vtHd[0] to vtHd[1]. 
	    2016/03/24 Changed from Edge to const Edge &.
	*/
	inline YsVec3 GetEdgeVector(const Edge &edge) const;

	template <const int N>
	inline YSRESULT GetVertexPosition(YsArray <YsVec3,N> &vtPos,YSSIZE_T np,const VertexHandle vtHd[]) const;

	template <const int N,const int M>
	inline YSRESULT GetVertexPosition(YsArray <YsVec3,N> &vtPos,const YsArray <VertexHandle,M> &vtHd) const;

	/*! Returns an array of multiple vertex positions.
	*/
	YsArray <YsVec3> GetMultiVertexPosition(const YsConstArrayMask <VertexHandle> &vtHdArray) const;

	int GetVertexReferenceCount(int vtId) const;
	int GetVertexReferenceCount(VertexHandle vtHd) const;

	// 10/22/2001
	YSRESULT GetAverageNormalAtVertex(YsVec3 &nom,VertexHandle vtHd) const;

	YSHASHKEY GetSearchKey(VertexHandle vtHd) const;
	YSHASHKEY GetSearchKey(PolygonHandle plHd) const;

	/*! This function returns a search key of the vertex pointed by vtHd even if the vertex is frozen. 
	    GetSearchKey returns a search key only when vtHd is not frozen. */
	YSHASHKEY GetSearchKeyMaybeFrozen(VertexHandle vtHd) const;

	/*! This function returns a search key of the polygon pointed by plHd even if the vertex is frozen. 
	    GetSearchKey returns a search key only when vtHd is not frozen. */
	YSHASHKEY GetSearchKeyMaybeFrozen(PolygonHandle plHd) const;

	void GetBoundingBox(YsVec3 &min,YsVec3 &max) const;

	void GetBoundingBox(YsVec3 bbx[2]) const;

	YsVec3 GetBoundingBoxCenter(void) const;

	double GetBoundingBoxDiagonalLength(void) const;


	/*! This function returns a bounding box of the given vertices. */
	void GetBoundingBox(YsVec3 &min,YsVec3 &max,YSSIZE_T nVtx,const VertexHandle vtHd[]) const;

	/*! This function returns a bounding box of the given vertices. */
	void GetBoundingBox(YsVec3 minmax[2],YSSIZE_T nVtx,const VertexHandle vtHd[]) const;

	/*! This function returns a bounding box of the given vertices. */
	void GetBoundingBox(YsVec3 &min,YsVec3 &max,YsConstArrayMask <VertexHandle> vtHd) const;

	/*! This function returns a bounding box of the given vertices. */
	void GetBoundingBox(YsVec3 minmax[2],YsConstArrayMask <VertexHandle> vtHd) const;



	/*! Returns the number of vertices of polygon plHd. */
	const int GetPolygonNumVertex(PolygonHandle plHd) const;

	/*! Deprecated.  Use GetPolygonNumVertex instead. */
	int GetNumVertexOfPolygon(int plId) const;

	/*! Deprecated.  Use GetPolygonNumVertex instead. */
	int GetNumVertexOfPolygon(PolygonHandle plHd) const;

	/*! Returns the list of vertices used by the polygon plHd. */
	YSRESULT GetPolygon(int &nPlVt,const VertexHandle *&plVtHd,PolygonHandle plHd) const;

	/*! Returns the list of vertices used by the polygon plHd. 
	    The polygon can be frozen. */
	YSRESULT GetPolygonMaybeFrozen(int &nPlVt,const VertexHandle *&plVtHd,PolygonHandle plHd) const;

	/*! Returns the list of vertices used by the polygon plHd. */
	template <int N>
	inline YSRESULT GetPolygon(YsArray <VertexHandle,N> &vtHd,PolygonHandle plHd) const;

	/*! Alias for GetPolygon.  Left for backward compatibility. */
	template <int N>
	inline YSRESULT GetPolygon(YsArray <YsVec3,N> &vtx,PolygonHandle plHd) const;

	/*! Returns an YsArray of the vertices of polygon 'plHd'. */
	inline YsArray <VertexHandle,4> GetPolygonVertex(PolygonHandle plHd) const;

	/*! Returns a YsConstArrayMask of the vertices of polygon 'plHd'.
	    Since it returns a link, making changes to this polygon may void the return value.
	*/
	inline YsConstArrayMask <VertexHandle> GetPolygonVertexQuick(PolygonHandle plHd) const;

	/*! Returns an YsArray of the polygon's vertex positions. */
	inline YsArray <YsVec3,4> GetPolygonVertexPosition(PolygonHandle plHd) const;

	/*! Alias for GetPolygon.  Left for backward compatibility. */
	YSRESULT GetVertexListOfPolygon(int &nPlVt,const VertexHandle *&plVtHd,PolygonHandle plHd) const;

	/*! Deprecated.  Use GetPolygon instead of this function.  Left for backward compatibility. */
	YSRESULT GetVertexListOfPolygon(int idx[],YSSIZE_T maxcount,int plId) const;

	/*! Deprecated.  Use GetPolygon instead of this function.  Left for backward compatibility. */
	YSRESULT GetVertexListOfPolygon(YsVec3 vtx[],YSSIZE_T maxcount,int plId) const;

	/*! Deprecated.  Use GetPolygon instead of this function.  Left for backward compatibility. */
	const VertexHandle *GetVertexListOfPolygon(PolygonHandle plHd) const;

	/*! Deprecated.  Use GetPolygon instead of this function.  Left for backward compatibility. */
	YSRESULT GetVertexListOfPolygon(VertexHandle vtHd[],YSSIZE_T maxcount,PolygonHandle plHd) const;

	/*! Deprecated.  Use GetPolygon instead of this function.  Left for backward compatibility. */
	YSRESULT GetVertexListOfPolygon(YsVec3 vtx[],YSSIZE_T maxcount,PolygonHandle plHd) const;

	/*! Alias for GetPolygon.  Left for backward compatibility. */
	template <int minSize>
	inline YSRESULT GetVertexListOfPolygon(YsArray <VertexHandle,minSize> &vtHd,PolygonHandle plHd) const;

	/*! Alias for GetPolygon.  Left for backward compatibility. */
	template <int minSize>
	YSRESULT GetVertexListOfPolygon(YsArray <YsVec3,minSize> &vtx,PolygonHandle plHd) const;

	template <int minSize>
	YSRESULT GetVertexListFromPolygonList(YsArray <VertexHandle,minSize> &vtHdList,YSSIZE_T nPl,const PolygonHandle plHdList[]) const;

	/*! Returns the area of polygon plHd. */
	const double GetPolygonArea(PolygonHandle plHd) const;

	/*! Deprecated.  Use GetPolygonArea instead. */
	double GetAreaOfPolygon(PolygonHandle plHd) const;

	/*! Returns YSTRUE if the polygon plHd is using the vertex vtHd. */
	YSBOOL IsPolygonUsingVertex(PolygonHandle plHd,VertexHandle vtHd) const;

	/*! Returns YSTRUE if the polygon plHd is using the edge piece edVtHd[0]-edVtHd[1]. */
	YSBOOL IsPolygonUsingEdgePiece(PolygonHandle plHd,const VertexHandle edVtHd[2]) const;

	double GetHeightOfPolygon(PolygonHandle plHd,VertexHandle baseEdVtHd[2]) const;
	double GetPolygonEdgeLength(PolygonHandle plHd,int edId) const;

	/*! Populate edVtHd[0] and edVtHd[1] with the Nth (zero-based) edge of polygon plHd.
	    N can be negative or greater than the number of polygon vertex, in which case the
	    function will take the polygon vertices as a cycle. */
	inline YSRESULT GetPolygonEdgeVertex(VertexHandle edVtHd[2],PolygonHandle plHd,YSSIZE_T edId) const;

	/*! Populate edVtHd0 and edVtHd1 with the Nth (zero-based) edge of polygon plHd.
	    N can be negative or greater than the number of polygon vertex, in which case the
	    function will take the polygon vertices as a cycle. */
	inline YSRESULT GetPolygonEdgeVertex(VertexHandle &edVtHd1,VertexHandle &edVtHd2,PolygonHandle plHd,YSSIZE_T edId) const;

	/*! Returns an edge of the given polygon. */
	Edge GetPolygonEdge(PolygonHandle plHd,YSSIZE_T edIdx) const;

	int GetPolygonEdgeIndex(PolygonHandle plHd,const Edge &edge) const;
	int GetPolygonEdgeIndex(PolygonHandle plHd,const VertexHandle edVtHd[2]) const;
	int GetPolygonEdgeIndex(PolygonHandle plHd,VertexHandle edVtHd0,VertexHandle edVtHd1) const;
	int GetPolygonEdgeIndex(YSSIZE_T nPlVt,const VertexHandle plVtHd[],VertexHandle edVtHd0,VertexHandle edVtHd1) const;

	inline VertexHandle PlVt(PolygonHandle plHd,int idx) const;
	inline const YsVec3 &PlVtPos(PolygonHandle plHd,int idx) const;
	inline VertexHandle PlVtLp(PolygonHandle plHd,int idx) const;
	inline const YsVec3 &PlVtPosLp(PolygonHandle plHd,int idx) const;


	YSRESULT GetPolygonGroupByNormalSimilarity
	   (YsArray <PolygonHandle,16> &plHdList,PolygonHandle seedPlHd,
	    YsKeyStore *excludePlKey=NULL,const double &cosThr=1.0-YsTolerance) const;
	YSRESULT GetCircumferentialAngleOfPolygonGroup
	   (double &minAngle,VertexHandle &minVtHd,double &maxAngle,VertexHandle &maxVtHd,
	    const YsVec3 &org,const YsVec3 &axs,int nPl,const PolygonHandle *plHdList) const;

	void GetMatrix(YsMatrix4x4 &mat) const;

	YsShellPolygon *GetPolygon(int id);
	YsShellVertex *GetVertex(int id);
	const YsShellPolygon *GetPolygon(int id) const;
	const YsShellVertex *GetVertex(int id) const;
	inline YsShellPolygon *GetPolygon(PolygonHandle plg);
	inline YsShellPolygon *GetPolygonMaybeFrozen(PolygonHandle plg);
	inline YsShellVertex *GetVertex(VertexHandle vtx);
	inline YsShellVertex *GetVertexMaybeFrozen(VertexHandle vtx);
	inline const YsShellPolygon *GetPolygon(PolygonHandle plHd) const;
	inline const YsShellVertex *GetVertex(VertexHandle vtHd) const;

	YSBOOL IsFrozen(PolygonHandle plHd) const;
	YSBOOL IsFrozen(VertexHandle vtHd) const;

	YSSIDE CountRayIntersection
	    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,int plIdSkip=-1,YSBOOL watch=YSFALSE) const;
	YSSIDE CountRayIntersection
	    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,PolygonHandle plHdSkip,YSBOOL watch=YSFALSE) const;
	YSSIDE CountRayIntersection
	    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,const class YsShellPolygonStore *limitToThesePlg,const class YsShellPolygonStore *excludePlg,YSBOOL watch=YSFALSE) const;

	YSSIDE CheckInsideSolid(const YsVec3 &pnt,const YsShellPolygonStore *limitToThesePlg=NULL,const YsShellPolygonStore *excludePlg=NULL,YSBOOL watch=YSFALSE) const;

	YSRESULT InvertPolygon(int plId);
	YSRESULT InvertPolygon(PolygonHandle plHd);


	YSRESULT ShootFiniteRay
	    (YsArray <YsVec3> &is,YsArray <PolygonHandle> &plHdLst,
	     const YsVec3 &p1,const YsVec3 &p2,const class YsShellLattice *ltc=NULL) const;
	YSRESULT ShootInfiniteRay
	    (YsArray <YsVec3> &is,YsArray <PolygonHandle> &plHdLst,
	     const YsVec3 &org,const YsVec3 &vec,const class YsShellLattice *ltc=NULL) const;




	int ShootRay(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const;
	PolygonHandle ShootRayH(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const;
	    // Return PlId or PlHd
	void ShootRay(YsArray <YsVec3> &intersect,const YsVec3 &p1,const YsVec3 &vec) const;


	/*! Shoot an infinite ray from o to v and returns the first intersection.
	    It exhaustively calculates intersections.  For faster processing, use YsShellLattice.
	*/
	PolygonAndPos FindFirstIntersection(const YsVec3 &o,const YsVec3 v) const;

	/*! Finds the nearst point to the point on the shell.
	    It exhaustively searches for the location.  For faster processing, use YsShellLattice.
	*/
	PolygonAndPos FindNearestPolygon(const YsVec3 &from) const;

	/*! Finds the nearst point to the point on the shell.
	    It exhaustively searches for the location.  For faster processing, use YsShellLattice.
	*/
	PolygonAndPos FindNearestPolygonHighPrecision(const YsVec3 &from) const;

private:
	PolygonAndPos FindNearestPolygonWithPrecisionFlag(const YsVec3 &from,YSBOOL highPrecision) const;


public:
	YSRESULT BuildOctree(class YsShellOctree &oct,int maxDepth,double bbxMargine=1.02) const;

	YSBOOL IsStarShape(const YsVec3 &withRespectTo) const;

	YSRESULT AutoComputeNormal(PolygonHandle plHd,YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal);
	YSRESULT AutoComputeNormalAll(YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal);

	YSRESULT AutoComputeVertexNormalAll(YSBOOL weighted);

protected:
	YSRESULT BlendPolygonVtId
	    (int &nvNew,VertexHandle vtIdNew[],int nBuf,
	     int nvOrg,const VertexHandle vtIdOrg[],int nv,const VertexHandle vtId[]);
	YSBOOL EliminateHangnail(int &nv,VertexHandle v[]);


	YSHASHKEY uniqueSearchKey;
	YSHASHKEY GetUniqueSearchKey(void);
	void RewindSearchKey(void);

	YSBOOL trustPolygonNormal;   // Added 2000/11/10

// File IO
public:
	YsShellReadSrfVariable *readSrfVar;

	YSRESULT BeginReadSrf(void);
	YSRESULT BeginMergeSrf(void);
	YSRESULT ReadSrfOneLine
	 (const char str[],
	  YsArray <PolygonHandle> *noShadingPolygon=NULL,YsArray <VertexHandle> *roundVtx=NULL);
	YSRESULT EndReadSrf(void);

	YSRESULT LoadSrf(const char fn[],
	    YsArray <PolygonHandle> *noShadingPolygon=NULL,YsArray <VertexHandle> *roundVtx=NULL);
	YSRESULT LoadSrf(FILE *fp,
	    YsArray <PolygonHandle> *noShadingPolygon=NULL,YsArray <VertexHandle> *roundVtx=NULL);
	YSRESULT MergeSrf(const char fn[],
	    YsArray <PolygonHandle> *noShadingPolygon=NULL,YsArray <VertexHandle> *roundVtx=NULL);
	YSRESULT MergeSrf(FILE *fp,
	    YsArray <PolygonHandle> *noShadingPolygon=NULL,YsArray <VertexHandle> *roundVtx=NULL);
	YSRESULT LoadTri(const char fn[]);
	YSRESULT LoadMsh(const char fn[]);
	YSRESULT MergeMsh(const char fn[]);
	YSRESULT LoadMs3(const char fn[]);
	YSRESULT MergeMs3(const char fn[]);
	YSRESULT LoadDat(const char fn[]);
	YSRESULT MergeDat(const char fn[]);
	YSRESULT SaveSrf(
	    const char fn[],
	    YSSIZE_T nNoShadingPolygon=0,const PolygonHandle noShadingPolygon[]=NULL,
	    YSSIZE_T nRoundVtx=0,const VertexHandle roundVtx[]=NULL) const;
	YSRESULT SaveSrf(
	    class YsTextOutputStream &textOut,
	    YSSIZE_T nNoShadingPolygon=0,const PolygonHandle noShadingPolygon[]=NULL,
	    YSSIZE_T nRoundVtx=0,const VertexHandle roundVtx[]=NULL) const;
	YSRESULT SaveMsh(int &nIgnored,const char fn[]) const;
	YSRESULT SaveMs3(int &nIgnored,const char fn[]) const;

	YSRESULT LoadStl(const char fn[]);
	YSRESULT MergeStl(const char fn[]);
	YSRESULT SaveStl(int &nIgnored,const char fn[],const char title[]=NULL) const;  // <- This function ignores non-triangular faces.
	YSRESULT SaveStl(int &nIgnored,FILE *fp,const char title[]=NULL) const;  // <- This function ignores non-triangular faces.
	YSRESULT SaveStl(const char fn[],const char title[]=NULL) const;  // <- This function ignores non-triangular faces.
	YSRESULT SaveStl(FILE *fp,const char title[]=NULL) const;  // <- This function ignores non-triangular faces.
	YSRESULT SaveBinStl(int &nIgnored,const char fn[],const char title[]=NULL) const;
	YSRESULT SaveBinStl(int &nIgnored,FILE *fp,const char title[]=NULL) const;
	YSRESULT SaveBinStl(const char fn[],const char title[]=NULL) const;
	YSRESULT SaveBinStl(FILE *fp,const char title[]=NULL) const;

	friend YSRESULT YsBlendShell1to2(YsShell &,const YsShell &,const YsShell &,int,YSBOOL);



public:
	/*! This function is left for backward compatibility.  The vertices and polygons are not globally memory-pooled any more, and
	    therefore this function does nothing.  Use CollectGarbage for cleaning up per-instance memory pool. */
	static void CleanUpVertexPolygonStore(void);

	/*! This function removeds unused memory pools as much as possible. */
	void CollectGarbage(void) const;
protected:
	PolygonHandle CreatePolygon(void);
	VertexHandle CreateVertex(void);

	// Bounding box becomes valid when
	//    GetBoundingBox function computes bounding box.
	// becomes invalid when
	//    a vertex position is modified
	//    a matrix is set
	//    a vertex is deleted
	//    the shell is cleaned up
	// automatically updated when
	//    a vertex is added outside the bounding box
	mutable YSBOOL bbxIsValid;
	mutable YsVec3 bbx1,bbx2;

public:
	void ResetVertexMapping(void);
	VertexHandle FindVertexMapping(const YsShellVertex *fromVtx) const;
	VertexHandle FindVertexMapping(VertexHandle vtHd) const;
	YSRESULT AddVertexMapping(VertexHandle fromVtHd,VertexHandle toVtHd); // fromVtHd needs to be the vertex of this shell.
	YSRESULT AddVertexMapping(const YsShellVertex *fromVtx,VertexHandle toVtHd); // fromVtHd needs to be the vertex of this shell.

	/*! This function overrides the vertex search keys.  HandleKeyTable class must be a storage class of YsPair <YsShell::VertexHandle,YSHASHKEY> 
	    with forward iterators support.  Search table must be detached when calling this function, or this function will fail and do nothing.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverrideVertexSearchKey(const HandleKeyTable &table);

	/*! This function overrides the vertex search keys.  HandleKeyTable class must be a storage class of YsPair <PolygonHandle,YSHASHKEY> 
	    with forward iterators support.  Search table must be detached when calling this function, or this function will fail and do nothing.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverridePolygonSearchKey(const HandleKeyTable &table);
};

typedef YsShell::VertexHandle YsShellVertexHandle;
typedef YsShell::PolygonHandle YsShellPolygonHandle;
typedef YsShell::Elem YsShellElemType;

template <const int N>
inline const YsVec3 YsShell::GetCenter(const YsArray <YsShellVertexHandle,N> &vtHdArray) const
{
	return GetCenter(vtHdArray.GetN(),vtHdArray);
}

template <typename HandleKeyTable>
YSRESULT YsShell::OverrideVertexSearchKey(const HandleKeyTable &table)
{
	if(nullptr!=GetSearchTable())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be detached before calling this function.\n");
		return YSERR;
	}

	auto vtxSearchEnabled=vtx.SearchEnabled();
	if(YSTRUE==vtxSearchEnabled)
	{
		vtx.DisableSearch();
	}
	auto res=vtx.OverrideSearchKey<HandleKeyTable>(table);

	YSHASHKEY maxKey=0;
	for(auto hdKeyPair : table)
	{
		auto *vtx=GetVertexMaybeFrozen(hdKeyPair.a);
		vtx->searchKey=hdKeyPair.b;
		YsMakeGreater(maxKey,hdKeyPair.b);
	}

	YsMakeGreater(uniqueSearchKey,maxKey+1);

	if(YSTRUE==vtxSearchEnabled)
	{
		vtx.EnableSearch();
	}

	return res;
}

template <typename HandleKeyTable>
YSRESULT YsShell::OverridePolygonSearchKey(const HandleKeyTable &table)
{
	if(nullptr!=GetSearchTable())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be detached before calling this function.\n");
		return YSERR;
	}

	auto plgSearchEnabled=plg.SearchEnabled();
	if(YSTRUE==plgSearchEnabled)
	{
		plg.DisableSearch();
	}
	auto res=plg.OverrideSearchKey<HandleKeyTable>(table);

	YSHASHKEY maxKey=0;
	for(auto hdKeyPair : table)
	{
		YsMakeGreater(maxKey,hdKeyPair.b);
	}

	YsMakeGreater(uniqueSearchKey,maxKey+1);

	if(YSTRUE==plgSearchEnabled)
	{
		plg.EnableSearch();
	}

	return res;
}


////////////////////////////////////////////////////////////

/*! Support for STL-like iterator */
inline YsShell::VertexEnumerator::iterator begin(const YsShell::VertexEnumerator &allVertex)
{
	return allVertex.begin();
}

/*! Support for STL-like iterator */
inline YsShell::VertexEnumerator::iterator end(const YsShell::VertexEnumerator &allVertex)
{
	return allVertex.end();
}

/*! Support for STL-like iterator */
inline YsShell::VertexEnumerator::iterator rbegin(const YsShell::VertexEnumerator &allVertex)
{
	return allVertex.rbegin();
}

/*! Support for STL-like iterator */
inline YsShell::VertexEnumerator::iterator rend(const YsShell::VertexEnumerator &allVertex)
{
	return allVertex.rend();
}

/*! Support for STL-like iterator */
inline YsShell::PolygonEnumerator::iterator begin(const YsShell::PolygonEnumerator &allPolygon)
{
	return allPolygon.begin();
}

/*! Support for STL-like iterator */
inline YsShell::PolygonEnumerator::iterator end(const YsShell::PolygonEnumerator &allPolygon)
{
	return allPolygon.end();
}

/*! Support for STL-like iterator */
inline YsShell::PolygonEnumerator::iterator rbegin(const YsShell::PolygonEnumerator &allPolygon)
{
	return allPolygon.rbegin();
}

/*! Support for STL-like iterator */
inline YsShell::PolygonEnumerator::iterator rend(const YsShell::PolygonEnumerator &allPolygon)
{
	return allPolygon.rend();
}

////////////////////////////////////////////////////////////

YsShellPolygon *YsShell::GetPolygon(PolygonHandle plHd)
{
	auto plgPtr=plg[plHd];
	if(nullptr!=plgPtr)
	{
		return plgPtr;
	}
	else
	{
		YsErrOut("YsShell::GetPolygon()\n");
		YsErrOut("  Trying to access to a frozen polygon or by a handle for another shell!!\n");
		return nullptr;
	}
}

YsShellPolygon *YsShell::GetPolygonMaybeFrozen(PolygonHandle plHd)
{
	auto plgPtr=plg.GetObjectMaybeFrozen(plHd);
	if(nullptr!=plgPtr)
	{
		return plgPtr;
	}
	else
	{
		YsErrOut("YsShell::GetPolygon()\n");
		YsErrOut("  Trying to access to a polygon by a handle for another shell!!\n");
		return nullptr;
	}
}

inline YsShellVertex *YsShell::GetVertex(VertexHandle vtHd)
{
	auto vtxPtr=vtx[vtHd];
	if(nullptr!=vtxPtr)
	{
		return vtxPtr;
	}
	else
	{
		YsErrOut("YsShell::GetVertex()\n");
		YsErrOut("  Trying to access to a frozen vertex or by a handle for another shell!!\n");
		return nullptr;
	}
}

inline YsShellVertex *YsShell::GetVertexMaybeFrozen(VertexHandle vtHd)
{
	auto vtxPtr=vtx.GetObjectMaybeFrozen(vtHd);
	if(nullptr!=vtxPtr)
	{
		return vtxPtr;
	}
	else
	{
		YsErrOut("YsShell::GetVertex()\n");
		YsErrOut("  Trying to access to a vertex by a handle for another shell!!\n");
		return nullptr;
	}
}

inline const YsShellPolygon *YsShell::GetPolygon(YsShellPolygonHandle plHd) const
{
	auto plgPtr=plg[plHd];
	if(nullptr!=plgPtr)
	{
		return plgPtr;
	}
	else
	{
		YsErrOut("YsShell::GetPolygon()\n");
		YsErrOut("  Trying to access to a frozen polygon or by a handle for another shell!!\n");
		return nullptr;
	}
}

inline const YsShellVertex *YsShell::GetVertex(YsShellVertexHandle vtHd) const
{
	auto vtxPtr=vtx[vtHd];
	if(nullptr!=vtxPtr)
	{
		return vtxPtr;
	}
	else
	{
		YsErrOut("YsShell::GetVertex()\n");
		YsErrOut("  Trying to access to a frozen vertex or a vertex by a handle for another shell!!\n");
		return nullptr;
	}
}

inline YSRESULT YsShell::GetVertexPosition(YsVec3 &pos,YsShellVertexHandle vtHd) const
{
	const YsShellVertex *vtx=GetVertex(vtHd);
	if(NULL!=vtx)
	{
		if(YSTRUE==matrixIsSet)
		{
			pos=mat*vtx->GetPosition();
		}
		else
		{
			pos=vtx->GetPosition();
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

inline YsVec3 YsShell::GetVertexPosition(YsShellVertexHandle vtHd) const
{
	YsVec3 pos;
	GetVertexPosition(pos,vtHd);
	return pos;
}

inline YSRESULT YsShell::GetPosition(YsVec3 &pos,YsShellVertexHandle vtHd) const
{
	return GetVertexPosition(pos,vtHd);
}

inline YsVec3 YsShell::GetEdgeVector(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	return YsUnitVector(GetPosition(edVtHd1)-GetPosition(edVtHd0));
}
inline YsVec3 YsShell::GetEdgeVector(const YsShellVertexHandle edVtHd[2]) const
{
	return YsUnitVector(GetPosition(edVtHd[1])-GetPosition(edVtHd[0]));
}
inline YsVec3 YsShell::GetEdgeVector(const Edge &edge) const
{
	return YsUnitVector(GetPosition(edge.edVtHd[1])-GetPosition(edge.edVtHd[0]));
}

inline YsVec3 YsShell::GetPosition(YsShellVertexHandle vtHd) const
{
	return GetVertexPosition(vtHd);
}

YSRESULT YsShell::GetPolygonEdgeVertex(YsShellVertexHandle edVtHd[2],YsShellPolygonHandle plHd,YSSIZE_T N) const
{
	return GetPolygonEdgeVertex(edVtHd[0],edVtHd[1],plHd,N);
}

YSRESULT YsShell::GetPolygonEdgeVertex(YsShellVertexHandle &edVtHd0,YsShellVertexHandle &edVtHd1,YsShellPolygonHandle plHd,YSSIZE_T N) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	if(YSOK==GetVertexListOfPolygon(nPlVt,plVtHd,plHd))
	{
		if(0>N)
		{
			YSSIZE_T mod=N%nPlVt;
			N=mod+nPlVt;
		}
		else if(nPlVt<=N)
		{
			N%=nPlVt;
		}
		edVtHd0=plVtHd[N];
		edVtHd1=plVtHd[(N+1)%nPlVt];
		return YSOK;
	}
	return YSERR;
}

inline YsShellVertexHandle YsShell::PlVt(YsShellPolygonHandle plHd,int idx) const
{
	return plg[plHd]->GetVertexHandle(idx);
}

inline const YsVec3 &YsShell::PlVtPos(YsShellPolygonHandle plHd,int idx) const
{
	return vtx[plg[plHd]->GetVertexHandle(idx)]->GetPosition();
}

inline YsShellVertexHandle YsShell::PlVtLp(YsShellPolygonHandle plHd,int idx) const
{
	return plg[plHd]->GetVertexHandle(idx%plg[plHd]->GetNumVertex());
}

inline const YsVec3 &YsShell::PlVtPosLp(YsShellPolygonHandle plHd,int idx) const
{
	return vtx[plg[plHd]->GetVertexHandle(idx%plg[plHd]->GetNumVertex())]->GetPosition();
}

template <int minSize>
inline YSRESULT YsShell::GetVertexListOfPolygon(YsArray <YsShellVertexHandle,minSize> &vtHd,YsShellPolygonHandle plHd) const
{
	int nVtxPlg;
	nVtxPlg=GetNumVertexOfPolygon(plHd);
	if(nVtxPlg>0)
	{
		vtHd.Set(nVtxPlg,GetVertexListOfPolygon(plHd));
		return YSOK;
	}
	return YSERR;
}

template <int N>
inline YSRESULT YsShell::GetPolygon(YsArray <YsShellVertexHandle,N> &vtHd,YsShellPolygonHandle plHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	if(YSOK==GetPolygon(nPlVt,plVtHd,plHd))
	{
		vtHd.Set(nPlVt,plVtHd);
		return YSOK;
	}
	return YSERR;
}

template <int N>
inline YSRESULT YsShell::GetPolygon(YsArray <YsVec3,N> &vtx,YsShellPolygonHandle plHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	if(YSOK==GetPolygon(nPlVt,plVtHd,plHd))
	{
		vtx.Set(nPlVt,NULL);
		for(YSSIZE_T idx=0; idx<nPlVt; ++idx)
		{
			GetVertexPosition(vtx[idx],plVtHd[idx]);
		}
		return YSOK;
	}
	return YSERR;
}

inline YsArray <YsShellVertexHandle,4> YsShell::GetPolygonVertex(YsShellPolygonHandle plHd) const
{
	YsArray <YsShellVertexHandle,4> plVtHd;
	GetPolygon(plVtHd,plHd);
	return plVtHd;
}

inline YsConstArrayMask <YsShell::VertexHandle> YsShell::GetPolygonVertexQuick(PolygonHandle plHd) const
{
	int nPlVt;
	const VertexHandle *plVtHd;
	if(YSOK==GetPolygon(nPlVt,plVtHd,plHd))
	{
		return YsConstArrayMask<VertexHandle>(nPlVt,plVtHd);
	}
	return YsConstArrayMask<VertexHandle>(0,nullptr);
}

inline YsArray <YsVec3,4> YsShell::GetPolygonVertexPosition(PolygonHandle plHd) const
{
	YsArray <YsVec3,4> plVtPos;
	GetPolygon(plVtPos,plHd);
	return plVtPos;
}

template <int minSize>
inline YSRESULT YsShell::GetVertexListOfPolygon(YsArray <YsVec3,minSize> &vtx,YsShellPolygonHandle plHd) const
{
	int nVtxPlg;
	nVtxPlg=GetNumVertexOfPolygon(plHd);
	if(nVtxPlg>0)
	{
		int i;
		const YsShellVertexHandle *vtHd;

		vtx.Set(nVtxPlg,NULL);
		vtHd=GetVertexListOfPolygon(plHd);
		for(i=0; i<nVtxPlg; i++)
		{
			YsVec3 vtxPos;
			GetVertexPosition(vtxPos,vtHd[i]);
			vtx.SetItem(i,vtxPos);
		}
		return YSOK;

	}
	return YSERR;
}

template <int minSize>
inline YSRESULT YsShell::GetVertexListFromPolygonList(YsArray <YsShellVertexHandle,minSize> &vtHdList,YSSIZE_T nPl,const YsShellPolygonHandle plHdList[]) const
{
	int i,nVtxMax,nVtx;
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	nVtxMax=0;
	for(i=0; i<nPl; i++)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);
		nVtxMax+=nPlVt;
	}

	vtHdList.Set(nVtxMax,NULL);
	nVtx=0;
	for(i=0; i<nPl; i++)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);

		int j;
		for(j=0; j<nPlVt; j++)
		{
			vtHdList[nVtx++]=plVtHd[j];;
		}
	}

	YsRemoveDuplicateInUnorderedArray(vtHdList);

	return YSOK;
}

template <const int N>
inline YSRESULT YsShell::GetVertexPosition(YsArray <YsVec3,N> &vtPos,YSSIZE_T np,const YsShellVertexHandle vtHd[]) const
{
	int i;
	vtPos.Set(np,NULL);
	for(i=0; i<np; i++)
	{
		GetVertexPosition(vtPos[i],vtHd[i]);
	}
	return YSOK;
}

template <const int N,const int M>
inline YSRESULT YsShell::GetVertexPosition(YsArray <YsVec3,N> &vtPos,const YsArray <YsShellVertexHandle,M> &vtHd) const
{
	return GetVertexPosition(vtPos,vtHd.GetN(),(const YsShellVertexHandle *)vtHd);
}

////////////////////////////////////////////////////////////


// Declaration /////////////////////////////////////////////

class YsShellEdge
{
public:
	int edId;
	YsShellVertexHandle edVt1,edVt2;
	YsArray <YsShellPolygonHandle> usedBy;
};

class YsShellEdgeList
{
	enum
	{
		HashTableSize=16384,
	};

public:
	YsShellEdgeList();
	YsShellEdgeList(const YsShellEdgeList &from);
	~YsShellEdgeList();

	int GetNumEdge(void) const;
	YSRESULT CleanUp(void);
	YSRESULT Build(const YsShell &shl);
	YSRESULT GetEdge(YsShellVertexHandle &edVt1,YsShellVertexHandle &edVt2,int edId) const;
	YSRESULT LookUpEdge(int &edId,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YSBOOL takeReverse=YSTRUE) const;
	YSRESULT LookUpEdge(YsShellEdge *&edge,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YSBOOL takeReverse=YSTRUE) const;

	const YsShellEdge *GetEdge(int edId) const;

protected:
	YSRESULT AddEdge(YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2,YsShellPolygonHandle plId,const YsShell &shl);

	int nEdge;
	YsArray <YsShellEdge *> edgeHash[HashTableSize]; // Hash by (edVtKey1+edVtKey2)%HashTableSize
	YsList <YsShellEdge> *edgeList;
	const YsShell *shell;
};


////////////////////////////////////////////////////////////

class YsShellVertexStore : public YsKeyStore
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
	friend class YsShellVertexStore;

	private:
		const YsShellVertexStore *table;
		YsKeyStore::const_iterator iter;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline YsShellVertexHandle operator*();  // Due to dependency, it is written in ysshellsearch.h
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	YsShellVertexStore();
	YsShellVertexStore(const YsShell &shl);
	YsShellVertexStore(const YsShell &shl,YSSIZE_T nVtx,const YsShellVertexHandle vtHdArray[]);
	YsShellVertexStore(const YsShell &shl,YsConstArrayMask <YsShellVertexHandle> vtHdArray);

	void CleanUp(void);
	void SetShell(const YsShell &shl);
	const YsShell &GetShell(void) const;

	YSRESULT Add(YsShellVertexHandle vtHd);
	YSRESULT Add(YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);
	YSRESULT Add(YsConstArrayMask <YsShellVertexHandle> vtHdArray);

	YSRESULT Delete(YsShellVertexHandle vtHd);

	YSBOOL IsIncluded(YsShellVertexHandle vtHd) const;
	using YsKeyStore::IsIncluded;  // Is it necessary?  I don't think so.  Clang requires it.  Another obvious wrong interpretation of C++ by Clang.

	/*! Returns an array that contains all vertex handles stored in this object. */
	YsArray <YsShellVertexHandle> GetArray(void) const;

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddVertex(YsShellVertexHandle vtHd);
	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddVertex(YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddVertex(YsConstArrayMask <YsShellVertexHandle> vtHdArray);

	/*! Old naming convention.  Use Delete instead. */
	YSRESULT DeleteVertex(YsShellVertexHandle vtHd);

protected:
	const YsShell *shl;
};

inline YsShellVertexStore::const_iterator YsShellVertexStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::begin();
	return iter;
}

inline YsShellVertexStore::const_iterator YsShellVertexStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::end();
	return iter;
}

inline YsShellVertexStore::const_iterator &YsShellVertexStore::const_iterator::operator++()
{
	++iter;
	return *this;
}

inline YsShellVertexStore::const_iterator YsShellVertexStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	++iter;
	return copy;
}


inline bool YsShellVertexStore::const_iterator::operator==(const YsShellVertexStore::const_iterator &from)
{
	return this->iter==from.iter;
}


inline bool YsShellVertexStore::const_iterator::operator!=(const YsShellVertexStore::const_iterator &from)
{
	return this->iter!=from.iter;
}


inline YsShellVertexStore::const_iterator begin(const YsShellVertexStore &store)
{
	return store.begin();
}

inline YsShellVertexStore::const_iterator end(const YsShellVertexStore &store)
{
	return store.end();
}


////////////////////////////////////////////////////////////


class YsShellPolygonStore : public YsKeyStore
{
friend class const_iterator;

public:
	YsShellPolygonStore();
	YsShellPolygonStore(const YsShell &shl);
	YsShellPolygonStore(const YsShell &shl,int nPlHd,const YsShellPolygonHandle plHdArray[]);
	YsShellPolygonStore(const YsShell &shl,YsConstArrayMask <YsShellPolygonHandle> plHdArray);

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

	/*! Support for STL-like iterator. */
	class const_iterator
	{
	friend class YsShellPolygonStore;

	private:
		const YsShellPolygonStore *table;
		YsKeyStore::const_iterator iter;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline YsShellPolygonHandle operator*();  // Due to dependency, it is written in ysshellsearch.h
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	/*! Returns an array that contains all polygon handles stored in this object. */
	YsArray <YsShellPolygonHandle> GetArray(void) const;

	void CleanUp(void);
	void SetShell(const YsShell &shl);
	const YsShell &GetShell(void) const;

	YSRESULT Add(YsShell::PolygonHandle plHd);
	YSRESULT Add(YSSIZE_T nPl,const YsShell::PolygonHandle plHd[]);
	YSRESULT Add(const YsConstArrayMask <YsShell::PolygonHandle> plHdArray);
	YSRESULT Delete(YsShellPolygonHandle plHd);

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddPolygon(YsShellPolygonHandle plHd);

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHd[]);

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddPolygon(YsConstArrayMask <YsShellPolygonHandle> plHdArray);

	/*! Old naming convention.  Use Delete instead. */
	YSRESULT DeletePolygon(YsShellPolygonHandle plHd);

	YSBOOL IsIncluded(YsShellPolygonHandle plHd) const;
protected:
	const YsShell *shl;
};

inline YsShellPolygonStore::const_iterator YsShellPolygonStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::begin();
	return iter;
}

inline YsShellPolygonStore::const_iterator YsShellPolygonStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsKeyStore::end();
	return iter;
}

inline YsShellPolygonStore::const_iterator &YsShellPolygonStore::const_iterator::operator++()
{
	++iter;
	return *this;
}

inline YsShellPolygonStore::const_iterator YsShellPolygonStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	++iter;
	return copy;
}


inline bool YsShellPolygonStore::const_iterator::operator==(const YsShellPolygonStore::const_iterator &from)
{
	return this->iter==from.iter;
}


inline bool YsShellPolygonStore::const_iterator::operator!=(const YsShellPolygonStore::const_iterator &from)
{
	return this->iter!=from.iter;
}


inline YsShellPolygonStore::const_iterator begin(const YsShellPolygonStore &store)
{
	return store.begin();
}

inline YsShellPolygonStore::const_iterator end(const YsShellPolygonStore &store)
{
	return store.end();
}

////////////////////////////////////////////////////////////


class YsShellEdgeStore : public YsTwoKeyStore
{
friend class const_iterator;

protected:
	const YsShell *shl;

public:
	using YsTwoKeyStore::IsIncluded;

	YsShellEdgeStore();
	YsShellEdgeStore(const YsShell &shl);

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
	const YsTwoKeyStore &AllKey(void) const;

	/*! Support for STL-like iterator. */
	class const_iterator
	{
	friend class YsShellEdgeStore;

	private:
		const YsShellEdgeStore *table;
		YsTwoKeyStore::const_iterator iter;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline const YsStaticArray <YsShellVertexHandle,2> operator*();  // Due to dependency, it is written in ysshellsearch.h
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	void CleanUp(void);
	void SetShell(const YsShell &shl);
	const YsShell &GetShell(void) const;

	YSRESULT Add(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);
	YSRESULT Add(const YsShellVertexHandle edVtHd[2]);
	YSRESULT Delete(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);
	YSRESULT Delete(const YsShellVertexHandle edVtHd[2]);
	YSBOOL IsIncluded(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	YSBOOL IsIncluded(const YsShellVertexHandle edVtHd[2]) const;

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);
	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddEdge(const YsShellVertexHandle edVtHd[2]);
	/*! Old naming convention.  Use Delete instead. */
	YSRESULT DeleteEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);
	/*! Old naming convention.  Use Delete instead. */
	YSRESULT DeleteEdge(const YsShellVertexHandle edVtHd[2]);
};



inline YsShellEdgeStore::const_iterator YsShellEdgeStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsTwoKeyStore::begin();
	return iter;
}

inline YsShellEdgeStore::const_iterator YsShellEdgeStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.iter=YsTwoKeyStore::end();
	return iter;
}

inline YsShellEdgeStore::const_iterator &YsShellEdgeStore::const_iterator::operator++()
{
	++iter;
	return *this;
}

inline YsShellEdgeStore::const_iterator YsShellEdgeStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	++iter;
	return copy;
}


inline bool YsShellEdgeStore::const_iterator::operator==(const YsShellEdgeStore::const_iterator &from)
{
	return this->iter==from.iter;
}


inline bool YsShellEdgeStore::const_iterator::operator!=(const YsShellEdgeStore::const_iterator &from)
{
	return this->iter!=from.iter;
}


inline YsShellEdgeStore::const_iterator begin(const YsShellEdgeStore &store)
{
	return store.begin();
}

inline YsShellEdgeStore::const_iterator end(const YsShellEdgeStore &store)
{
	return store.end();
}



////////////////////////////////////////////////////////////


/*! This function is deprecated.  Don't use.  Use YsCheckShellCollisionEx */
YSBOOL YsCheckShellCollision(YsShell &sh1,YsShell &sh2);
	// Note:This function will use freeAttribute3 of YsShellPolygon

YSBOOL YsCheckShellCollisionEx(
    YsVec3 &firstFound,YsShellPolygonHandle &foundPlHd1,YsShellPolygonHandle &foundPlHd2,
    const YsShell &shl1,const YsShell &shl2);

YSRESULT YsBlendShell
    (YsShell &newShell,
     const YsShell &sh1,
     const YsShell &sh2,
     YSBOOLEANOPERATION boolop=YSBOOLBLEND);
    // Note:This function will use freeAttribute2 and 3.

YSRESULT YsSeparateShell
    (YsShell &sh1,YsShell &sh2,const YsShell &org,const YsShell &ref);
    // Note:This function will use freeAttribute2,3 and 4.
    // sh1=org-ref
    // sh2=org&ref

YSRESULT YsReplaceShellEdge
    (YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2,YSSIZE_T nNewEdVt,const YsShellVertexHandle newEdVt[]);
YSRESULT YsAddShell(YsShell &shl1,const YsShell &shl2);

/*! Applies boolean operation to shl0 and shl1 and returns the result in newShell. 
    This function internally calls YsBlendShell3.  
    However, YSBOOLSEPARATE is not supported because only one output shell can be given to this function.
    This function is deprecated and left for backward compatibility. */
YSRESULT YsBlendShell2(
    YsShell &newShell,const YsShell &shl0,const YsShell &shl1,YSBOOLEANOPERATION boolop,
    int ltcX,int ltcY,int ltcZ,int ltcHashSize);

/*! Works same as YsBlendShell2 except that it supports YSBOOLSEPARATE.
    If YSBOOLSEPARATE is given as YSBOOLEANTYPE, subtraction shl0 minus shl1 is returned in newShell, and intersection of shl0 and shl1 is returned in newShell2.  
    newShell2 will not be used if boolop is not YSBOOLSEPARATE.  A dummy YsShell should be given as newShell2 in that
    case. */
YSRESULT YsBlendShell3(
    YsShell &newShell,YsShell &newShell2,const YsShell &shl0,const YsShell &shl1,YSBOOLEANOPERATION boolop,
    int nLtcCell);

YSRESULT YsBlendShell3(
    YsShell &newShell,YsShell &newShell2,const YsShell &shl0,const YsShellLattice &ltc0,const YsShell &shl1,const YsShellLattice &ltc1,YSBOOLEANOPERATION boolop,int nLtcCell);

/*! This function can be called after shl0 and shl1 are dumped into newShell, and newShellLtc is SetDomain'ed.
    Can be useful when three lattices are built in parallel.

  Example:
	int nLtcCell=1000000;
	YsShell newShell,newShell2;
	YsShellLattice ltc,lat0,lat1;
	ltc.EnableAutoResizingHashTable();

	lat0.SetDomain(shl0,nLtcCell);
	lat1.SetDomain(shl1,nLtcCell);

	newShell.CleanUp();
	YsDumpOldShellIntoNewShell(newShell,shl0,0);
	YsDumpOldShellIntoNewShell(newShell,shl1,1);
	ltc.SetDomain(newShell,nLtcCell);

	auto res=YsBlendShell3(newShell,newShell2,newShellLtc,shl0,lat0,shl1,lat1,boolop,nLtcCell);
*/
YSRESULT YsBlendShell3(
    YsShell &newShell,YsShell &newShell2,YsShellLattice &newShellLtc,const YsShell &shl0,const YsShellLattice &lat0,const YsShell &shl1,const YsShellLattice &lat1,YSBOOLEANOPERATION boolop);

/*! Needed for using the third variation of YsBlendShell3. */
YSRESULT YsDumpOldShellIntoNewShell(YsShell &newShell,const YsShell &oldShell,int shlId);



YSRESULT YsShellRemoveFreeIntersection(YsShellVertexStore &itscVtx,YsShell &newShell,const YsShell &shl0,int nLtcCell);

YSRESULT YsBlendShellPolygon
    (YsArray <YsShellPolygonHandle> &newPlHd,YsShell &shl,const YsShellPolygonHandle plHd[2],YsShellLattice *ltc);

/* } */
#endif
