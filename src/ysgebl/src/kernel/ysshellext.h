/* ////////////////////////////////////////////////////////////

File Name: ysshellext.h
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



#ifndef YSSHELLEXT_IS_INCLUDED
#define YSSHELLEXT_IS_INCLUDED
/* { */

#include <stdio.h>
#include <ysclass.h>
#include <yseditarray.h>

// Question:
//   Should I allow a polygon to be used from multiple face groups?

/*! This class stores basic YsShell information plus additional attributes such as polygon grouping information, constraint edges, boundary-layer specification etc.

    This class privately inherits YsShell.  However, the following labels are made public from YsShell class.
	- using YsShell::FindNextVertex; 
	- using YsShell::FindNextPolygon;
	- using YsShell::VertexHandle;
	- using YsShell::PolygonHandle;
	- using YsShell::ComputeVolume;
	- using YsShell::ComputeTotalArea;
	- using YsShell::ComputeDihedralAngle;
	- using YsShell::GetNumVertex;
	- using YsShell::GetNumPolygon;
	- using YsShell::GetBoundingBox;
	- using YsShell::GetBoundingBoxDiagonalLength;
	- using YsShell::GetBoundingBoxCenter;
	- using YsShell::GetSearchKey;
	- using YsShell::GetSearchKeyMaybeFrozen;
	- using YsShell::GetVertex;
	- using YsShell::GetVertexPosition;
	- using YsShell::GetMultiVertexPosition;
	- using YsShell::GetVertexListOfPolygon;
	- using YsShell::GetNumVertexOfPolygon;
	- using YsShell::GetPolygonArea;
	- using YsShell::GetPolygonEdge;
	- using YsShell::GetPolygonEdgeVertex;
	- using YsShell::GetPolygonEdgeIndex;
	- using YsShell::GetPolygonNumVertex;
	- using YsShell::GetColorOfPolygon;
	- using YsShell::GetCenter;
	- using YsShell::GetCenterOfPolygon;
	- using YsShell::GetNormalOfPolygon;
	- using YsShell::GetVertexIdFromHandle;
	- using YsShell::GetPolygonIdFromHandle;
	- using YsShell::GetVertexHandleFromId;
	- using YsShell::GetPolygonHandleFromId;
	- using YsShell::GetCenterOfEdge;
	- using YsShell::GetCenterOfVertexGroup;
	- using YsShell::GetCenterOfTriangle;
	- using YsShell::GetEdgeLength;
	- using YsShell::AllVertex;
	- using YsShell::AllPolygon;
	- using YsShell::GetPolygon;
	- using YsShell::GetPolygonVertex;
	- using YsShell::GetPolygonVertexPosition;
	- using YsShell::GetPosition;
	- using YsShell::GetEdgeVector;
	- using YsShell::GetColor;
	- using YsShell::GetNormal;
	- using YsShell::IsPolygonUsingVertex;
	- using YsShell::IsPolygonUsingEdgePiece;
	- using YsShell::IsFrozen;
	- using YsShell::SaveStl;
	- using YsShell::SaveBinStl;
	- using YsShell::GetVertexListFromPolygonList;
	- using YsShell::ShootInfiniteRay;
	- using YsShell::CheckInsideSolid;
*/
class YsShellExt : protected YsShell
{
public:
	/*! This class does nothing.  It exists just for making YsShellExt and YsShellExtEdit available to the same C++ template. */
	class StopIncUndo
	{
	public:
		StopIncUndo(YsShellExt *shl)
		{
		}
		StopIncUndo(YsShellExt &shl)
		{
		}
	};

private:
	using YsShell::ModifyPolygon; // Use SetPolygonVertex instead.
public:
	// 2016/10/08 Doxygen can only describe using-specified typedefs.  Apparently.
	//    http://stackoverflow.com/questions/27964653/using-c11-keyword-in-doxygen
	//    http://stackoverflow.com/questions/12534024/doxygen-support-for-c11-template-aliases-the-using-syntax
	using YsShell::Edge;
	using YsShell::FindNextVertex; 
	using YsShell::NullVertex;
	using YsShell::FindNextPolygon;
	using YsShell::NullPolygon;
	using YsShell::VertexHandle;
	using YsShell::PolygonHandle;
	using YsShell::ComputeVolume;
	using YsShell::ComputeTotalArea;
	using YsShell::ComputeDihedralAngle;
	using YsShell::GetNumVertex;
	using YsShell::GetNumPolygon;
	using YsShell::GetBoundingBox;
	using YsShell::GetBoundingBoxDiagonalLength;
	using YsShell::GetBoundingBoxCenter;
	using YsShell::GetSearchKey;
	using YsShell::GetSearchKeyMaybeFrozen;
	using YsShell::GetVertex;
	using YsShell::GetVertexPosition;
	using YsShell::GetMultiVertexPosition;
	using YsShell::GetVertexListOfPolygon;
	using YsShell::GetNumVertexOfPolygon;
	using YsShell::GetPolygonArea;
	using YsShell::GetPolygonEdge;
	using YsShell::GetPolygonEdgeVertex;
	using YsShell::GetPolygonEdgeIndex;
	using YsShell::GetPolygonNumVertex;
	using YsShell::GetColorOfPolygon;
	using YsShell::GetCenter;
	using YsShell::GetCenterOfPolygon;
	using YsShell::GetNormalOfPolygon;
	using YsShell::GetVertexIdFromHandle;
	using YsShell::GetPolygonIdFromHandle;
	using YsShell::GetVertexHandleFromId;
	using YsShell::GetPolygonHandleFromId;
	using YsShell::GetCenterOfEdge;
	using YsShell::GetCenterOfVertexGroup;
	using YsShell::GetCenterOfTriangle;
	using YsShell::GetEdgeLength;
	using YsShell::AllVertex;
	using YsShell::AllPolygon;
	using YsShell::GetPolygon;
	using YsShell::GetPolygonVertex;
	using YsShell::GetPolygonVertexQuick;
	using YsShell::GetPolygonVertexPosition;
	using YsShell::GetPosition;
	using YsShell::GetEdgeVector;
	using YsShell::GetColor;
	using YsShell::GetNormal;
	using YsShell::IsPolygonUsingVertex;
	using YsShell::IsPolygonUsingEdgePiece;
	using YsShell::IsFrozen;
	using YsShell::SaveStl;
	using YsShell::SaveBinStl;
	using YsShell::GetVertexListFromPolygonList;
	using YsShell::ShootInfiniteRay;
	using YsShell::CheckInsideSolid;
	using YsShell::GetLength;
	using YsShell::FindFirstIntersection;

	using YsShell::MetaDataHandle;
	using YsShell::GetNumMetaData;
	using YsShell::AddMetaData;
	using YsShell::SetMetaDataValue;
	using YsShell::DeleteMetaData;
	using YsShell::FreezeMetaData;
	using YsShell::MeltMetaData;
	using YsShell::DeleteFrozenMetaData;
	using YsShell::FindMetaData;
	using YsShell::GetMetaDataKey;
	using YsShell::GetMetaDataValue;
	using YsShell::GetMetaData;
	using YsShell::MetaDataEnumerator;
	using YsShell::AllMetaData;

public:
	// ConstEdgeStore, FaceGrouopStore, and VolumeStore stores search keys.
	// Therefore, the same storage is valid for a clone shell.
	// However, search must be enabled to use.
	class ConstEdgeStore;
	class FaceGroupStore;
	class VolumeStore;
	class TemporaryModification;


	template <class toFind>
	class VertexFaceGroupAttribTable;

	template <class toFind>
	class FaceGroupAttribTable;

	class CommonProp
	{
	public:
		int ident;
		YsString label;
		int refCount;
		int refCountFromFrozenEntity;

		// Self-memo.  Make sure to update CopyFrom when adding a new variable.

		void Initialize(void);
		inline void CopyFrom(const CommonProp &incoming)
		{
			ident=incoming.ident;
			label=incoming.label;
			refCount=incoming.refCount;
			refCountFromFrozenEntity=incoming.refCountFromFrozenEntity;
		}
	};

	class BoundaryLayerSpec
	{
	public:
		int nLayer;
		double firstLayerThickness;
		int growthFuncType;
		double growthFuncParam;

		BoundaryLayerSpec();
		void Initialize(void);

		/*! Returns thickness of the boundary layer.
		    Each value is a thickness of a layer, not cumulative distance from the layer origin.
		*/
		YsArray <double> CalculateLayerThickness(void) const;
	};

	class ConstEdgeCoreAttrib // This class is shared with the meshing library
	{
	public:
		enum CEFLAG
		{
			ZEROFLAG=0,
			NOBUBBLE=1,
			NO_LONGER_USED=2,
			NO_LONGER_USED2=4,
			ISTEMPORARY=8,
			NOREMESH=16,
			CREASE=32,
		};

		CEFLAG flags;
		BoundaryLayerSpec bLayerSpec;
		double elemSize;

		// Self-memo.  Make sure to update CopyFrom when adding a new variable.

		inline void CopyFrom(const ConstEdgeCoreAttrib &incoming)
		{
			flags=incoming.flags;
			bLayerSpec=incoming.bLayerSpec;
			elemSize=incoming.elemSize;
		}

		YSBOOL IsCrease(void) const;
		void SetIsCrease(YSBOOL isCrease);
		void Initialize(void);
		void SetNoBubble(YSBOOL noBubble);
		YSBOOL NoBubble(void) const;

		void SetNoRemesh(YSBOOL noRemesh);
		YSBOOL NoRemesh(void) const;

		void SetIsTemporary(YSBOOL flag);
		YSBOOL IsTemporary(void) const;
	};

	class ConstEdgeAttrib : public CommonProp,public ConstEdgeCoreAttrib
	{
	public:
		void Initialize(void)
		{
			CommonProp::Initialize();
			ConstEdgeCoreAttrib::Initialize();
		}
		void CopyFrom(const ConstEdgeAttrib &incoming)
		{
			CommonProp::CopyFrom(incoming);
			ConstEdgeCoreAttrib::CopyFrom(incoming);
		}
	};

	class ConstEdge : public ConstEdgeAttrib
	{
	private:
		YSBOOL isLoop;
	public:
		YsArray <YsShellVertexHandle> vtHdArray;
		void Initialize(void);
		YSBOOL IsLoop(void) const;
		void SetIsLoop(YSBOOL isLoop);

		inline void CopyAttribFrom(const ConstEdgeAttrib &incoming)
		{
			ConstEdgeAttrib::CopyFrom(incoming);
		}
	};

	// ConstEdgeStore2, FaceGrouopStore2, and VolumeStore2 store raw indices of the YsEditArray.
	// Therefore, it does not require search to be enabled.  However, the same storage is not valid
	// for a clone shell.  A clone shell only shares search keys, not raw indices.
	typedef YsEditArray <ConstEdge>::HandleStore ConstEdgeStore2;

	class FaceGroupCoreAttrib // This class is shared with the meshing library.
	{
	public:
		enum FGFLAG
		{
			ZEROFLAG=0,
			NOBUBBLE=1,
			INTERIOR_CONST_SURFACE=2,
			_RESERVED1=4,
			ISTEMPORARY=8,
			NOREMESH=16,
		};

		FGFLAG flags;
		BoundaryLayerSpec bLayerSpec;
		double elemSize;

		// Self-memo.  Make sure to update CopyFrom when adding a new variable.

		void CopyFrom(const FaceGroupCoreAttrib &incoming)
		{
			flags=incoming.flags;
			bLayerSpec=incoming.bLayerSpec;
			elemSize=incoming.elemSize;
		}

		void Initialize(void);
		void SetElemSize(double elemSize)
		{
			this->elemSize=elemSize;
		}

		double GetElemSize(void) const
		{
			return elemSize;
		}

		void SetBoundaryLayerSpecification(int numLayer,const double firstLayerThickness,int funcType,const double funcParam)
		{
			bLayerSpec.Initialize();
			bLayerSpec.nLayer=numLayer;
			bLayerSpec.firstLayerThickness=firstLayerThickness;
			bLayerSpec.growthFuncType=funcType;
			bLayerSpec.growthFuncParam=funcParam;
		}
		void SetBoundaryLayerSpecification(const BoundaryLayerSpec &spec)
		{
			bLayerSpec=spec;
		}
		BoundaryLayerSpec GetBoundaryLayerSpecification(void)
		{
			return bLayerSpec;
		}

		void SetNoBubble(YSBOOL noBubble);
		YSBOOL NoBubble(void) const;

		void SetNoRemesh(YSBOOL noRemesh);
		YSBOOL NoRemesh(void) const;

		void SetInteriorConstSurfaceFlag(YSBOOL isInterior);
		YSBOOL InteriorConstSurfaceFlag(void) const;

		void SetIsTemporary(YSBOOL flag);
		YSBOOL IsTemporary(void) const;
	};

	class FaceGroupAttrib : public CommonProp,public FaceGroupCoreAttrib
	{
	public:
		using FaceGroupCoreAttrib::FGFLAG;
		void Initialize(void)
		{
			CommonProp::Initialize();
			FaceGroupCoreAttrib::Initialize();
		}
		void CopyFrom(const FaceGroupAttrib &incoming)
		{
			CommonProp::CopyFrom(incoming);
			FaceGroupCoreAttrib::CopyFrom(incoming);
		}
	};

	class FaceGroup : public FaceGroupAttrib
	{
	public:
		YsShell::PolygonStore2 fgPlHd;  
		// 2016/09/14 Changed from YsArray to YsShell::PolygonStore2.
		//            At first I was thinking to make it not for large number of polygons per face group.
		//            But, when dealing with Marching-Cubed polygons, or range-scanned polygons,
		//            This assumption was too optimistic, and DeleteFaceGroupPolygon was easily making O(N^2).
		// YsArray <YsShellPolygonHandle> plHdArray;
		void Initialize(void);

		inline void CopyAttribFrom(const FaceGroupAttrib &incoming)
		{
			FaceGroupAttrib::CopyFrom(incoming);
		}
	};

	typedef YsEditArray <FaceGroup>::HandleStore FaceGroupStore2;

	class Volume : public CommonProp
	{
	public:
		YsArray <YsEditArrayObjectHandle <FaceGroup> > fgHdArray;

		void Initialize(void);
	};

	typedef YsEditArray <Volume>::HandleStore VolumeStore2;

	class VertexAttrib
	{
	public:
		enum VTFLAG
		{
			ZEROFLAG=0,
			ROUND=1,
			CONSTRAINED=2
		};

		VTFLAG flags;
		double thickness;
		int oPlId;     // Mapping from vertex index to a polygon of another shell
		int oVtId[2];  // Mapping from vertex index to an edge or vertex of another shell

		int refCount;
		int refCountFromFrozenEntity;

		void Initialize(void);
		YSBOOL IsRound(void) const;
		void SetRound(YSBOOL round);
		YSBOOL IsConstrained(void) const;
		void SetConstrained(YSBOOL isConstrained);
		void SetVertexTrackingToVertex(int oVtId);
		void SetVertexTrackingToEdge(int oEdVtId0,int oEdVtId1);
		void SetVertexTrackingToPolygon(int oPlId);
		void CopyFrom(const VertexAttrib &incoming); // Copy except reference counts and vertex mapping.
	};

	class PolygonAttrib
	{
	public:
		enum PLFLAG
		{
			ZEROFLAG=0,
			NO_SHADING=1,
			TWO_SIDED=2,
			AS_LIGHT=4
		};

		PLFLAG flags;
		double elemSize,thickness;

		int refCount;
		int refCountFromFrozenEntity;

		void Initialize(void);
		YSBOOL GetNoShading(void) const;
		void SetNoShading(YSBOOL noShading);
		void SetTwoSided(YSBOOL twoSided);
		YSBOOL GetAsLight(void) const;
		void SetAsLight(YSBOOL asLight);
		void CopyFrom(const PolygonAttrib &incoming); // Copy except reference count.
	};

	class SearchTable;

	typedef YsEditArrayObjectHandle <ConstEdge> ConstEdgeHandle;
	typedef YsEditArrayObjectHandle <FaceGroup> FaceGroupHandle;
	typedef YsEditArrayObjectHandle <Volume> VolumeHandle;

	// Utility classes >>
	class EdgeAndConstEdge : public YsShell::Edge
	{
	public:
		ConstEdgeHandle ceHd;
		inline EdgeAndConstEdge()
		{
			ceHd=NULL;
		}
	};
	class EdgePosAndConstEdge : public YsShell::EdgeAndPos
	{
	public:
		ConstEdgeHandle ceHd;
		EdgePosAndConstEdge()
		{
			ceHd=NULL;
		}
	};
	class VertexAndConstEdge
	{
	public:
		YsShellVertexHandle vtHd;
		ConstEdgeHandle ceHd;
		inline VertexAndConstEdge()
		{
			vtHd=NULL;
			ceHd=NULL;
		}
	};
	class PolygonAndFaceGroup
	{
	public:
		YsShellPolygonHandle plHd;
		FaceGroupHandle fgHd;
	};
	class PolygonFaceGroupAndPos : public YsShell::PolygonAndPos
	{
	public:
		FaceGroupHandle fgHd;
		inline PolygonFaceGroupAndPos()
		{
			fgHd=NULL;
		}
	};
	class ConstEdgeAndPos
	{
	public:
		ConstEdgeHandle ceHd;
		YsVec3 pos;
	};
	// Utility classes <<

protected:
	YsShellSearchTable *shlSearch;
	SearchTable *extSearch;

public:
	/*! Support for STL-like iterator */
	class ConstEdgeEnumerator
	{
	public:
		const YsShellExt *shl;

		class iterator
		{
		public:
			const YsShellExt *shl;
			ConstEdgeHandle prev,ceHd,next;
			int dir;
		private:
			inline void Forward(void)
			{
				if(0<=dir)
				{
					prev=ceHd;
					ceHd=next;
					shl->MoveToNextConstEdge(next);
				}
				else
				{
					next=ceHd;
					ceHd=prev;
					shl->MoveToPrevConstEdge(prev);
				}
			}
			inline void Backward(void)
			{
				if(0<=dir)
				{
					next=ceHd;
					ceHd=prev;
					shl->MoveToPrevConstEdge(prev);
				}
				else
				{
					prev=ceHd;
					ceHd=next;
					shl->MoveToNextConstEdge(next);
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
				return (shl==from.shl && ceHd==from.ceHd);
			}
			inline bool operator!=(const iterator &from)
			{
				return (shl!=from.shl || ceHd!=from.ceHd);
			}
			inline ConstEdgeHandle &operator*()
			{
				return ceHd;
			}
		};

		/*! Support for STL-like iterator */
		inline iterator begin() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
			iter.next=NULL;
			shl->MoveToNextConstEdge(iter.ceHd);
			shl->MoveToNextConstEdge(iter.next);
			shl->MoveToNextConstEdge(iter.next);
			iter.dir=1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator end() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
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
			iter.ceHd=NULL;
			iter.prev=NULL;
			shl->MoveToPrevConstEdge(iter.ceHd);
			shl->MoveToPrevConstEdge(iter.prev);
			shl->MoveToPrevConstEdge(iter.prev);
			iter.dir=-1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator rend() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
			iter.next=NULL;
			iter.dir=-1;
			return iter;
		}

		YsArray <YsShellExt::ConstEdgeHandle> Array(void) const
		{
			YsArray <YsShellExt::ConstEdgeHandle> hdArray(shl->GetNumConstEdge(),NULL);
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
	    for(auto vtIter=shl.ConstEdge().begin(); vtIter!=shl.ConstEdge().end(); ++vtIter)
	    {
			(*vtIter) is a ConstEdgeHandle in the loop.
	    }

		Since the iterator caches the next ConstEdge handle in itself, it is safe to delete a ConstEdge inside the loop.
	    */
	inline const ConstEdgeEnumerator AllConstEdge(void) const
	{
		ConstEdgeEnumerator allConstEdge;
		allConstEdge.shl=this;
		return allConstEdge;
	}

	/*! Support for STL-like iterator */
	class FaceGroupEnumerator
	{
	public:
		const YsShellExt *shl;

		class iterator
		{
		public:
			const YsShellExt *shl;
			FaceGroupHandle prev,ceHd,next;
			int dir;
		private:
			inline void Forward(void)
			{
				if(0<=dir)
				{
					prev=ceHd;
					ceHd=next;
					shl->MoveToNextFaceGroup(next);
				}
				else
				{
					next=ceHd;
					ceHd=prev;
					shl->MoveToPrevFaceGroup(prev);
				}
			}
			inline void Backward(void)
			{
				if(0<=dir)
				{
					next=ceHd;
					ceHd=prev;
					shl->MoveToPrevFaceGroup(prev);
				}
				else
				{
					prev=ceHd;
					ceHd=next;
					shl->MoveToNextFaceGroup(next);
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
				return (shl==from.shl && ceHd==from.ceHd);
			}
			inline bool operator!=(const iterator &from)
			{
				return (shl!=from.shl || ceHd!=from.ceHd);
			}
			inline FaceGroupHandle &operator*()
			{
				return ceHd;
			}
		};

		/*! Support for STL-like iterator */
		inline iterator begin() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
			iter.next=NULL;
			shl->MoveToNextFaceGroup(iter.ceHd);
			shl->MoveToNextFaceGroup(iter.next);
			shl->MoveToNextFaceGroup(iter.next);
			iter.dir=1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator end() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
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
			iter.ceHd=NULL;
			iter.prev=NULL;
			shl->MoveToPrevFaceGroup(iter.ceHd);
			shl->MoveToPrevFaceGroup(iter.prev);
			shl->MoveToPrevFaceGroup(iter.prev);
			iter.dir=-1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator rend() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
			iter.next=NULL;
			iter.dir=-1;
			return iter;
		}

		YsArray <YsShellExt::FaceGroupHandle> Array(void) const
		{
			YsArray <YsShellExt::FaceGroupHandle> hdArray(shl->GetNumFaceGroup(),NULL);
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
	    for(auto vtIter=shl.FaceGroup().begin(); vtIter!=shl.FaceGroup().end(); ++vtIter)
	    {
			(*vtIter) is a YsShellExt::FaceGroupHandle in the loop.
	    }

		Since the iterator caches the next FaceGroup handle in itself, it is safe to delete a FaceGroup inside the loop.
	    */
	inline const FaceGroupEnumerator AllFaceGroup(void) const
	{
		FaceGroupEnumerator allFaceGroup;
		allFaceGroup.shl=this;
		return allFaceGroup;
	}

	/*! Support for STL-like iterator */
	class VolumeEnumerator
	{
	public:
		const YsShellExt *shl;

		class iterator
		{
		public:
			const YsShellExt *shl;
			VolumeHandle prev,ceHd,next;
			int dir;
		private:
			inline void Forward(void)
			{
				if(0<=dir)
				{
					prev=ceHd;
					ceHd=next;
					shl->MoveToNextVolume(next);
				}
				else
				{
					next=ceHd;
					ceHd=prev;
					shl->MoveToPrevVolume(prev);
				}
			}
			inline void Backward(void)
			{
				if(0<=dir)
				{
					next=ceHd;
					ceHd=prev;
					shl->MoveToPrevVolume(prev);
				}
				else
				{
					prev=ceHd;
					ceHd=next;
					shl->MoveToNextVolume(next);
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
				return (shl==from.shl && ceHd==from.ceHd);
			}
			inline bool operator!=(const iterator &from)
			{
				return (shl!=from.shl || ceHd!=from.ceHd);
			}
			inline VolumeHandle &operator*()
			{
				return ceHd;
			}
		};

		/*! Support for STL-like iterator */
		inline iterator begin() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
			iter.next=NULL;
			shl->MoveToNextVolume(iter.ceHd);
			shl->MoveToNextVolume(iter.next);
			shl->MoveToNextVolume(iter.next);
			iter.dir=1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator end() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
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
			iter.ceHd=NULL;
			iter.prev=NULL;
			shl->MoveToPrevVolume(iter.ceHd);
			shl->MoveToPrevVolume(iter.prev);
			shl->MoveToPrevVolume(iter.prev);
			iter.dir=-1;
			return iter;
		}

		/*! Support for STL-like iterator */
		inline iterator rend() const
		{
			iterator iter;
			iter.shl=shl;
			iter.prev=NULL;
			iter.ceHd=NULL;
			iter.next=NULL;
			iter.dir=-1;
			return iter;
		}

		YsArray <YsShellExt::VolumeHandle> Array(void) const
		{
			YsArray <YsShellExt::VolumeHandle> hdArray(shl->GetNumVolume(),NULL);
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
	    for(auto vtIter=shl.Volume().begin(); vtIter!=shl.Volume().end(); ++vtIter)
	    {
			(*vtIter) is a YsShellExt::VolumeHandle in the loop.
	    }

		Since the iterator caches the next Volume handle in itself, it is safe to delete a Volume inside the loop.
	    */
	inline const VolumeEnumerator AllVolume(void) const
	{
		VolumeEnumerator allVolume;
		allVolume.shl=this;
		return allVolume;
	}

private:
	YsHashTable <YsEditArrayObjectHandle <VertexAttrib> > vtKeyToVtAttribArrayHandle;
	YsEditArray <VertexAttrib> vtAttribArray;

	YsHashTable <YsEditArrayObjectHandle <PolygonAttrib> > plKeyToPlAttribArrayHandle;
	YsEditArray <PolygonAttrib> plAttribArray;

	YsEditArray <ConstEdge> constEdgeArray;
	YsEditArray <FaceGroup> faceGroupArray;
	YsEditArray <Volume> volumeArray;

	mutable class YsShellExtDrawingBuffer *drawingBuffer;

public:
	class Condition
	{
	public:
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const=0;
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const=0;
	};
	class PassAll : public Condition
	{
	public:
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const;
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	};
	class PassFaceGroup : public Condition
	{
	public:
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const;
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
		YsShellExt::FaceGroupHandle fgHd;
	};
	class PassAllPolygonDontPassConstEdge : public Condition
	{
	public:
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const;
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	};
	class TrackingAlongEdgeStore : public Condition
	{
	private:
		const YsShellEdgeStore *edgeStorePointer;
	public:
		TrackingAlongEdgeStore(const YsShellEdgeStore &edgeStore);
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const;
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	};
	class TrackingAlongEdgeStoreOnSpecificConstEdge : public TrackingAlongEdgeStore
	{
	private:
		ConstEdgeHandle ceHd;
	public:
		TrackingAlongEdgeStoreOnSpecificConstEdge(const YsShellEdgeStore &edgeStore,ConstEdgeHandle ceHd);
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	};
	class TrackingAlongSingleUseEdge : public Condition
	{
	public:
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const;
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	};


public:
	/*! Default constructor */
	YsShellExt();

	/*! Default destructor */
	~YsShellExt();

	class Converter
	{
	public:
		YsShellExt *shl;
		operator const YsShell &() const
		{
			return *(const YsShell *)shl;
		}
		operator YsShellExt &() const
		{
			return *shl;
		}
	};
	class ConstConverter
	{
	public:
		const YsShellExt *shl;
		operator const YsShell &() const
		{
			return *(const YsShell *)shl;
		}
		operator const YsShellExt &() const
		{
			return *(const YsShellExt *)shl;
		}
	};

	/*! C++ apparently doesn't easily allow only constant access to the base class and ban non-constant access.
	    So, when conversion to const YsShell & or const YsShellExt & is needed, use Conv function, that returns 
	    ConstConverter class or Converter class, which will be automatically converted to const YsShell & or
	    const YsShellExt &. */
	inline Converter Conv(void);
	inline ConstConverter Conv(void) const;

	/*! Copy operator */
	const YsShellExt &operator=(const YsShellExt &from);

	/*! Copy operator */
	const YsShellExt &operator=(const YsShell &from);

	/*! Copy constructor */
	YsShellExt(const YsShellExt &from);

	/*! Copies YsShellExt */
	void CopyFrom(const YsShellExt &from);

	/*! Copies YsShell */
	void CopyFrom(const YsShell &from);

	/*! Moves YsShelLExt */
	void MoveFrom(YsShellExt &from);

	/*! Deletes all entities. */
	void CleanUp(void);

	/*! Cache normal per vertex. */
	void CacheVertexNormal(void);

	/*! Cache polygon/vertex/const-edge/face-group indexes so that an index can be found quickly from a handle. */
	void Encache(void) const;

	/*! This function loads .SRF data. */
	YSRESULT LoadSrf(YsTextInputStream &inStream);

	/*! This function loads a .STL file. */
	YSRESULT LoadStl(const char fn[]);

	/*! This function loads Wavefront .OBJ data. */
	YSRESULT LoadObj(YsTextInputStream &inStream);

	/*! This function returns a drawing-buffer. 
	    A drawing buffer stores primitive informations for rendering. */
	YsShellExtDrawingBuffer &GetDrawingBuffer(void) const;

	/*! This function adds a vertex and returns a handle to the new vertex. */
	YsShellVertexHandle AddVertex(const YsVec3 &pos);

	/*! This function moves vertex handle vtHd to the next vertex.  If incoming vtHd is NULL, vtHd will be the first vertex.  If the next vertex does not exist, vtHd will be NULL, and the function will return YSERR.  If successful, this function will return YSOK. */
	YSRESULT MoveToNextVertex(YsShellVertexHandle &vtHd) const;

	/*! This function moves polygon handle plHd to the next polygon.  If incoming plHd is NULL, vtHd will be the first polygon.  If the next polygon does not exist, plHd will be NULL, and the function will return YSERR.  If successful, this function will return YSOK. */
	YSRESULT MoveToNextPolygon(YsShellPolygonHandle &plHd) const;

	/*! This function moves vertex. */
	YSRESULT SetVertexPosition(VertexHandle VtHd,const YsVec3 &NewPos);

	/*! This function sets ROUND flag of the vertex. */
	YSRESULT SetVertexRoundFlag(YsShellVertexHandle vtHd,YSBOOL round);

	/*! This function sets CONSTRAINED flag of the vertex. */
	YSRESULT SetVertexConstrainedFlag(YsShellVertexHandle vtHd,YSBOOL round);

	/*! This function freezes (or tentatively delete) the vertex vtHd.  If the vertex is used by an entity, the function will fail and returns YSERR.  The vertex can be used by another frozen entity, however, this vertex must be melted before the using entity is melted. */
	YSRESULT FreezeVertex(YsShellVertexHandle vtHd);

	/*! This function undeletes a vertex that was frozen (or tentatively deleted).  If the vertex was not frozen, this function fails and returns YSERR. */
	YSRESULT MeltVertex(YsShellVertexHandle vtHd);

	/*! This function permanently delets a vertex.  If the vertex is used by a polygon or a constraint edge (even one is tentatively deleted), this function fails and returns YSERR. */
	YSRESULT DeleteVertex(YsShellVertexHandle vtHd);

	/*! This function permanently delets a frozen vertex.  If the vertex is used by a polygon or a constraint edge (even one is tentatively deleted), this function fails and returns YSERR. */
	YSRESULT DeleteFrozenVertex(YsShellVertexHandle vtHd);

	/*! This function sets vertex-tracking information to another shell vertex. */
	YSRESULT SetVertexTrackingToVertex(YsShellVertexHandle vtHd,int oVtId);

	/*! This function sets vertex-tracking information to another shell edge. */
	YSRESULT SetVertexTrackingToEdge(YsShellVertexHandle vtHd,int oEdVtId0,int oEdVtId1);

	/*! This function sets vertex-tracking information to another shell polygon. */
	YSRESULT SetVertexTrackingToPolygon(YsShellVertexHandle vtHd,int oPlId);

	/*! This function adds a polygon and returns a handle to the new polygon. */
	YsShellPolygonHandle AddPolygon(YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[]);

	/*! This function adds a polygon and returns a handle to the new polygon. */
	YsShellPolygonHandle AddPolygon(const YsConstArrayMask <YsShellVertexHandle> &vtHdArray);

	/*! This function modifies vertices of the polygon. */
	YSRESULT SetPolygonVertex(YsShellPolygonHandle plHd,YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[]);

	/*! This function modifies vertices of the polygon. */
	YSRESULT SetPolygonVertex(YsShellPolygonHandle plHd,const YsConstArrayMask <YsShellVertexHandle> &vtHdArray);

	/*! This function sets desired element size to the polygon. */
	YSRESULT SetPolygonDesiredElementSize(YsShellPolygonHandle plHd,const double elemSize);

	/*! This function sets no-shading flag of the polygon. */
	YSRESULT SetPolygonNoShadingFlag(YsShellPolygonHandle plHd,YSBOOL noShading);

	/*! This function sets as-light flag of the polygon. */
	YSRESULT SetPolygonAsLightFlag(YsShellPolygonHandle plHd,YSBOOL asLight);

	/*! This function sets two-sided flag of the polygon. */
	YSRESULT SetPolygonTwoSidedFlag(YsShellPolygonHandle plHd,YSBOOL noShading);

	/*! This function sets color of the polygon. */
	YSRESULT SetPolygonColor(YsShellPolygonHandle plHd,const YsColor &col);

	/*! This function sets normal vector of the polygon. */
	YSRESULT SetPolygonNormal(YsShellPolygonHandle plHd,const YsVec3 &nom);

	/*! This function sets alpha (solidness) value of the polygon. */
	YSRESULT SetPolygonAlpha(YsShellPolygonHandle plHd,const double alpha);

	/*! This funciton freezes (or tentatively deletes) a polygon plHd.  If the polygon is used by another entity, this function will fail anre returns YSERR. */
	YSRESULT FreezePolygon(YsShellPolygonHandle plHd);

	/*! This function undeletes a polygon that was frozen (or tentatively deleted).  If the polygon was not frozen, thi sfunction fails and returns YSERR. */
	YSRESULT MeltPolygon(YsShellPolygonHandle plHd);

	/*! This function permanently deletes a polygon.  If the polygon is used by an entity (een one is tentatively deleted), this function fails and returns YSERR. */
	YSRESULT DeletePolygon(YsShellPolygonHandle plHd);

	/*! This function deletes a polygon.  If the polygon is used by a face group, this function also deletes the face group. */
	YSRESULT ForceDeletePolygon(YsShellPolygonHandle plHd);

	/*! This function deletes multiple polygons.  If the polygon is used by a face group, this function also deletes the face group. */
	YSRESULT ForceDeleteMultiPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! This function deletes multiple polygons.  If the polygon is used by a face group, this function also deletes the face group. */
	YSRESULT ForceDeleteMultiPolygon(const YsConstArrayMask <YsShellPolygonHandle> &plHdArray);

	/*! This function permanently deletes a polygon.  If the polygon is used by an entity (een one is tentatively deleted), this function fails and returns YSERR. 
	    A frozen polygon may be using frozen vertices.  This function updates reference count of frozen vertices consistently. */
	YSRESULT DeleteFrozenPolygon(YsShellPolygonHandle plHd);

public:
	/*! This funciton returns a pointer to the attribute of the vertex vtHd. */
	const VertexAttrib *GetVertexAttrib(YsShellVertexHandle vtHd) const;
protected:
	/*! This funciton returns a pointer to the attribute of the vertex vtHd. */
	VertexAttrib *GetVertexAttrib(YsShellVertexHandle);

private:
	VertexAttrib *GetVertexAttribMaybeFrozen(YsShellVertexHandle vtHd);
	const VertexAttrib *GetVertexAttribMaybeFrozen(YsShellVertexHandle vtHd) const;

public:
	/*! This function sets vertex attribute except refCount and refCountFromFrozenEntity */
	YSRESULT SetVertexAttrib(YsShellVertexHandle vtHd,const VertexAttrib &incoming);

public:
	/*! This function returns YSTRUE if the vertex is used.  It does not count frozen entities. */
	YSBOOL IsVertexUsed(YsShellVertexHandle vtHd) const;

	/*! Returns YSTRUE if the vertex is used by the const edge ceHd. */
	YSBOOL IsVertexUsedByConstEdge(YsShellVertexHandle vtHd,ConstEdgeHandle ceHd) const;

	/*! Returns YSTRUE if the vertex is used by at least one non-manifold (including single-use) edge. */
	YSBOOL IsVertexUsedByNonManifoldEdge(YsShellVertexHandle vtHd) const;

	/*! Returns YSTRUE if the edge piece is used by the face group fgHd. */
	YSBOOL IsEdgePieceUsedByFaceGroup(YsShell::Edge edge,FaceGroupHandle fgHd) const;

public:
	/*! This funciton returns a pointer to the attribute of the polygon plHd. */
	const PolygonAttrib *GetPolygonAttrib(YsShellPolygonHandle plHd) const;
protected:
	/*! This funciton returns a pointer to the attribute of the polygon plHd. */
	PolygonAttrib *GetEditablePolygonAttrib(YsShellPolygonHandle plHd);

private:
	PolygonAttrib *GetPolygonAttribMaybeFrozen(YsShellPolygonHandle plHd);
	const PolygonAttrib *GetPolygonAttribMaybeFrozen(YsShellPolygonHandle plHd) const;

public:
	/*! This function sets polygon attributes excluding refrence count, color, and normal. */
	YSRESULT SetPolygonAttrib(YsShellPolygonHandle plHd,const PolygonAttrib &inAttrib);

	/*! This function returns the number of const-edges. */
	YSSIZE_T GetNumConstEdge(void) const;

	/*! This function returns the search key of ceHd. */
	YSHASHKEY GetSearchKey(ConstEdgeHandle ceHd) const;

	/*! This function moves the const-edge handle to the next.  If the incoming const-edge handle is NULL, the handle will point to the first const-edge. 
	    If there is no more next const-edge, ceHd will be set to NULL, and the function will return YSERR. */
	YSRESULT MoveToNextConstEdge(ConstEdgeHandle &ceHd) const;

	/*! This function returns a null const-edge handle. */
	inline ConstEdgeHandle NullConstEdge(void) const
	{
		ConstEdgeHandle ceHd=nullptr;
		return ceHd;
	}

	/*! This function moves the const-edge handle to the previous.  If the incoming const-edge handle is NULL, the handle will point to the last const-edge. 
	    If there is no more previous const-edge, ceHd will be set to NULL, and the function will return YSERR. */
	YSRESULT MoveToPrevConstEdge(ConstEdgeHandle &ceHd) const;

	/*! This function returns vertices of a constraint edge. */
	YSRESULT GetConstEdge(YSSIZE_T &nCeVt,const YsShellVertexHandle *&ceVtHd,YSBOOL &isLoop,ConstEdgeHandle ceHd) const;

	/*! This function returns vertices of a constraint edge. */
	YsConstArrayMask <YsShell::VertexHandle> GetConstEdgeVertex(ConstEdgeHandle ceHd) const;

	/*! This function returns YSTRUE if ceHd is a closed-loop const edge, YSFALSE if it is an open const edge. */
	YSBOOL IsConstEdgeLoop(ConstEdgeHandle ceHd) const;

	/*! This function returns vertices of a constraint edge. */
	template <const int N>
	YSRESULT GetConstEdge(YsArray <YsShellVertexHandle,N> &ceVtHd,YSBOOL &isLoop,ConstEdgeHandle ceHd) const;

	/*! This function returns constraint-edge attributes.  The vtHdArray member of the returned ConstEdge will be empty. */
	ConstEdgeAttrib GetConstEdgeAttrib(ConstEdgeHandle ceHd) const;

	/*! This function returns the two end points of the const edge. 
	    The return value will be YSOK if successful.  
	    If the const edge is a loop, it returns YSERR and p[0] and p[1] will be the first vertex position that is returned by GetConstEdge. */
	YSRESULT GetConstEdgeEndPoint(YsVec3 p[2],ConstEdgeHandle ceHd) const;

	/*! This function sets const-edge attrib of ceHd EXCEPT isLoop, ident, refCount, refCountFromFrozenEntity, and vtHdArray. */
	YSRESULT SetConstEdgeAttrib(ConstEdgeHandle ceHd,const ConstEdgeAttrib &attrib);

	/*! This function adds a constraint edge.  If nVt==0 and ceVtHd==NULL, this function creates an empty constraint edge that can later be populated. */
	ConstEdgeHandle AddConstEdge(YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[],YSBOOL isLoop);

	/*! This function adds a constraint edge. */
	ConstEdgeHandle AddConstEdge(const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,YSBOOL isLoop);

	/*! Returns YSTRUE if the const edge is a loop. */
	YSBOOL GetConstEdgeIsLoop(ConstEdgeHandle ceHd) const;

	/*! This function sets isLoop flag of the constraint edge. */
	YSRESULT SetConstEdgeIsLoop(ConstEdgeHandle ceHd,YSBOOL isLoop);

	/*! This function modifies a constraint edge with updated array of vertices and isLoop flag. 
	    If isLoop is YSTFUNKNOWN it will not change the current isLoop flag.  */
	YSRESULT ModifyConstEdge(ConstEdgeHandle ceHd,YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[],YSBOOL isLoop=YSTFUNKNOWN);

	/*! This function modifies a constraint edge with updated array of vertices and isLoop flag. 
	    If isLoop is YSTFUNKNOWN it will not change the current isLoop flag. */
	YSRESULT ModifyConstEdge(ConstEdgeHandle ceHd,const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,YSBOOL isLoop=YSTFUNKNOWN);

	/*! This function adds an array of vertices at the end of the const edge.  It does not change isLoop flag. */
	YSRESULT AddConstEdgeVertex(ConstEdgeHandle ceHd,YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[]);

	/*! This function adds an array of vertices at the end of the const edge.  It does not change isLoop flag. */
	YSRESULT AddConstEdgeVertex(ConstEdgeHandle ceHd,const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd);

	/*! This function sets a label on the const edge. */
	YSRESULT SetConstEdgeLabel(ConstEdgeHandle ceHd,const char label[]);

	/*! This function returns a label of the const edge. */
	const char *GetConstEdgeLabel(ConstEdgeHandle ceHd) const;

	/*! This function sets a const-edge identification number. */
	YSRESULT SetConstEdgeIdent(ConstEdgeHandle ceHd,YSSIZE_T ident);

	/*! This function returns an identification number of the const edge. */
	YSSIZE_T GetConstEdgeIdent(ConstEdgeHandle ceHd) const;

	/*! This function freezes (or tentatively deletes) a constraint edge. */
	YSRESULT FreezeConstEdge(ConstEdgeHandle ceHd);

	/*! This function melts (or undeletes) a constraint edge that is tentatively deleted. */
	YSRESULT MeltConstEdge(ConstEdgeHandle ceHd);

	/*! This function permanently delets a constraint edge. */
	YSRESULT DeleteConstEdge(ConstEdgeHandle ceHd);

	/*! This function delets multiples constraint edges. */
	YSRESULT DeleteMultiConstEdge(YSSIZE_T nCe,const ConstEdgeHandle ceHdArray[]);

	/*! This function deletes multiples constraint edges. */
	YSRESULT DeleteMultiConstEdge(const YsConstArrayMask <ConstEdgeHandle> &ceHdArray);

	/*! This function permanently delets a frozen constraint edge. 
	    Note that a frozen const-edge may be using a frozen vertex.
	    This function will update reference count of frozen vertices consistently.  */
	YSRESULT DeleteFrozenConstEdge(ConstEdgeHandle ceHd);

	/*! This function returns the number of face groups. */
	YSSIZE_T GetNumFaceGroup(void) const;

	/*! This function returns the search key of fgHd. */
	YSHASHKEY GetSearchKey(FaceGroupHandle fgHd) const;

	/*! This funtion moves the face group handle to point to the next face group. If the incoming fgHd is NULL, fgHd will be set to the first const edge.  The function returns YSOK if successful.  If no more next face group exists, the function will return YSERR, and fgHd will be NULL. */
	YSRESULT MoveToNextFaceGroup(FaceGroupHandle &fgHd) const;

	/*! This function returns the first face-group handle. */
	FaceGroupHandle FindFirstFaceGroup(void) const;

	/*! This function returns a null face-group handle. */
	inline FaceGroupHandle NullFaceGroup(void) const
	{
		FaceGroupHandle fgHd=nullptr;
		return fgHd;
	}

	/*! This funtion moves the face group handle to point to the previous face group. If the incoming fgHd is NULL, fgHd will be set to the first const edge.  The function returns YSOK if successful.  If no more next face group exists, the function will return YSERR, and fgHd will be NULL. */
	YSRESULT MoveToPrevFaceGroup(FaceGroupHandle &fgHd) const;

	// 2016/09/14 This function has been removed due to the internal data-structure change.
	/*! This function returns polygons of a face group. */
	// YSRESULT GetFaceGroup(YSSIZE_T &nFgPl,const YsShellPolygonHandle *&fgPlHd,FaceGroupHandle fgHd) const;

	/*! This function returns polygons of a face group. */
	YsArray <YsShellPolygonHandle> GetFaceGroup(FaceGroupHandle fgHd) const;

	/*! This function returns polygons of a face group. */
	const YsShell::PolygonStore2 &GetFaceGroupRaw(FaceGroupHandle fgHd) const;

	/*! This function returns polygons of a face group. */
	template <const int N>
	YSRESULT GetFaceGroup(YsArray <YsShellPolygonHandle,N> &fgPlHd,FaceGroupHandle fgHd) const;

	/*! This function returns vertices in the face group. */
	YsShellVertexStore GetFaceGroupVertex(FaceGroupHandle fgHd) const;

	/*! This function returns face-group attributes.  The plHdArray member of the returned attributes will be empty. */
	FaceGroupAttrib GetFaceGroupAttrib(FaceGroupHandle fgHd) const;

	/*! This function sets face-group attrib of fgHd EXCEPT ident, refCount, refCountFromFrozenEntity, and plHdArray. */
	YSRESULT SetFaceGroupAttrib(FaceGroupHandle fgHd,const FaceGroupAttrib &attrib);

	/*! This function adds a face group.  If nPl==0 and fgPlHd==NULL, this function creates an empty face group that can later be populated. */
	FaceGroupHandle AddFaceGroup(YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[]);

	/*! This function adds a face group.
	    std::vector <YsShell::PolygonHandle> or YsArray <YsShell::PolygonHandle> can be given as fgPlHd.
	*/
	FaceGroupHandle AddFaceGroup(const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd);

	/*! This function modifies a face group by replacing a list of polygons. */
	YSRESULT ModifyFaceGroup(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[]);

	/*! This function modifies a face group by replacing a list of polygons. 
	    std::vector <YsShell::PolygonHandle> or YsArray <YsShell::PolygonHandle> can be given as fgPlHd.
	*/
	YSRESULT ModifyFaceGroup(FaceGroupHandle fgHd,const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd);

	/*! This function adds polygons to a face group. */
	YSRESULT AddFaceGroupPolygon(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[]);

	/*! This function adds polygons to a face group. 
	    std::vector <YsShell::PolygonHandle> or YsArray <YsShell::PolygonHandle> can be given as fgPlHd.
	*/
	YSRESULT AddFaceGroupPolygon(FaceGroupHandle fgHd,const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd);

	/*! This function removes the polygon from the face group that it belongs to, and returns the handle to the 
	    face group from which the polygon was removed.  This function returns NULL if plHd does not belong to any face group. */
	FaceGroupHandle DeleteFaceGroupPolygon(YsShellPolygonHandle plHd);

	/*! This function removes the polygon from the face group that it belongs to, and returns the handle to the 
	    face group from which the polygon was removed.  This function returns NULL if plHd does not belong to any face group. */
	YSRESULT DeleteFaceGroupPolygonMulti(YSSIZE_T nPl,const YsShellPolygonHandle plHd[]);

	/*! This function removes the polygon from the face group that it belongs to, and returns the handle to the 
	    face group from which the polygon was removed.  This function returns NULL if plHd does not belong to any face group. 
	    std::vector <YsShell::PolygonHandle> or YsArray <YsShell::PolygonHandle> can be given as plHdArray.
	    */
	YSRESULT DeleteFaceGroupPolygonMulti(const YsConstArrayMask <YsShellPolygonHandle> &plHdArray);

private:
	void DeleteFaceGroupPolygon(FaceGroupHandle fgHd,YsShell::PolygonHandle plHd);

public:
	/*! This function sets INTERIOR_CONST_SURFACE flat to a face group.  
	    Although it is said "INTERIOR", the face group can be anywhere.  
	    The polygons that belong to this face group will not be considered as a volume boundary. */
	YSRESULT SetFaceGroupInteriorConstSurfaceFlag(FaceGroupHandle fgHd,YSBOOL isInteriorConst);

	/*! This function sets a label of the face group. */
	YSRESULT SetFaceGroupLabel(FaceGroupHandle fgHd,const char label[]);

	/*! This function returns a label of the face group. */
	const char *GetFaceGroupLabel(FaceGroupHandle fgHd) const;

	/*! This function sets a face-group identification number. */
	YSRESULT SetFaceGroupIdent(FaceGroupHandle fgHd,YSSIZE_T ident);

	/*! This function returns a face-group identification number. */
	YSSIZE_T GetFaceGroupIdent(FaceGroupHandle fgHd) const;

	/*! This function sets the temporary flag on the given face groups. */
	YSRESULT SetFaceGroupTemporaryFlag(YSSIZE_T nFg,const FaceGroupHandle fgHd[],YSBOOL isTemporary);

	/*! This function sets the temporary flag on the given face groups. 
	    std::vector <YsShell::FaceGroupHandle> or YsArray <YsShell::FaceGroupHandle> can be given as fgHdArray.
	*/
	YSRESULT SetFaceGroupTemporaryFlag(const YsConstArrayMask <FaceGroupHandle> &fgHdArray,YSBOOL isTemporary);

	/*! This function returns YSTRUE if the face group is marked as temporary. */
	YSBOOL IsFaceGroupTemporary(FaceGroupHandle fgHd) const;

	/*! This function returns YSTRUE if the face group is an INTERIOR_CONST_SURFACE, YSFALSE otherwise. */
	YSBOOL IsInteriorConstSurface(FaceGroupHandle fgHd) const;

	/*! This function freezes (or tentatively deletes) a face group.  This function fails and returns YSERR if the face group is used by a volume. */
	YSRESULT FreezeFaceGroup(FaceGroupHandle fgHd);

	/*! This function undeletes a face group that is tentatively deleted. */
	YSRESULT MeltFaceGroup(FaceGroupHandle fgHd);

	/*! This function permanently deletes a face group.  This function fails and returns YSERR if face group is used by a volume even if it is tentatively deleted. */
	YSRESULT DeleteFaceGroup(FaceGroupHandle fgHd);

	/*! This function permanently deletes a face group.  This function fails and returns YSERR if face group is used by a volume even if it is tentatively deleted. 
	    A frozen face group may be using frozen polygons.  This function updates reference counts of the frozen polygons consistently.  */
	YSRESULT DeleteFrozenFaceGroup(FaceGroupHandle fgHd);

	/*! This function returns YSTRUE if edge piece edVtHd0-edVtHd1 is on the face-group boundary, YSFALSE otherwise. */
	YSBOOL IsEdgeOnFaceGroupBoundary(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns YSTRUE if edge piece edVtHd[0]-edVtHd[1] is on the face-group boundary, YSFALSE otherwise. */
	YSBOOL IsEdgeOnFaceGroupBoundary(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function moves volume handle vlHd so that it points to the next volume.  If the incoming vlHd is NULL, vlHd wil be set to the first volume.  The function returns YSOK if successful.  If there is no more next volume, vlHd will be NULL, and the function will return YSERR. */
	YSRESULT MoveToNextVolume(VolumeHandle &vlHd) const;

	/*! This function returns a null volume handle. */
	inline VolumeHandle NullVolume(void) const
	{
		VolumeHandle vlHd=nullptr;
		return vlHd;
	}

	/*! This function moves volume handle vlHd so that it points to the previous volume.  If the incoming vlHd is NULL, vlHd wil be set to the first volume.  The function returns YSOK if successful.  If there is no more next volume, vlHd will be NULL, and the function will return YSERR. */
	YSRESULT MoveToPrevVolume(VolumeHandle &vlHd) const;

	/*! This function returns the number of volumes. */
	YSSIZE_T GetNumVolume(void) const;

	/*! This function returns search key of the volume. */
	YSHASHKEY GetSearchKey(VolumeHandle vlHd) const;

	/*! This function returns face groups that define a volume. */
	YSRESULT GetVolume(YSSIZE_T &nVlFg,const FaceGroupHandle *&vlFgHd,VolumeHandle vlHd) const;

	/*! This function returns face groups that define a volume. */
	template <const int N>
	YSRESULT GetVolume(YsArray <FaceGroupHandle,N> &vlFgHd,VolumeHandle vlHd) const;

	/*! This function returns volume attributes.  The fgHdArray member of the returned attributes will be empty. */
	Volume GetVolumeAttrib(VolumeHandle vlHd) const;

	/*! This function sets volume attrib of vlHd EXCEPT ident, refCount, refCountFromFrozenEntity, and fgHdArray. */
	YSRESULT SetVolumeAttrib(VolumeHandle fgHd,const Volume &attrib);

	/*! This function adds a volume defined by multiple face groups.  If nVlFg==0 && vlFgHd==NULL, this function creates an empty volume that can later be populated.. */
	VolumeHandle AddVolume(YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[]);

	/*! This function adds a volume defined by multiple face groups.*/
	template <const int N>
	VolumeHandle AddVolume(const YsArray <FaceGroupHandle,N> &vlFgHd);

	/*! This function modifies polygons included in the volume. */
	YSRESULT ModifyVolume(VolumeHandle vlHd,YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[]);

	/*! This function modifies polygons included in the volume. */
	template <const int N>
	YSRESULT ModifyVolume(VolumeHandle vlHd,const YsArray <FaceGroupHandle,N> &vlFgHd);

	/*! This function adds face groups to a volume. */
	YSRESULT AddVolumeFaceGroup(VolumeHandle vlHd,YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[]);

	/*! This function adds face groups to a volume. */
	template <const int N>
	YSRESULT AddVolumeFaceGroup(VolumeHandle vlHd,const YsArray <FaceGroupHandle,N> &vlFgHd);

	/*! This function freezes (or temporarily deletes) a volume.  If the volume does not exist or is already frozen, this function fails and returns YSERR. */
	YSRESULT FreezeVolume(VolumeHandle vlHd);

	/*! This function melts (or undeletes) a volume that is frozen.  If the volume is not frozen, this function fails and returns YSERR. */
	YSRESULT MeltVolume(VolumeHandle vlHd);

	/*! This function permanently deletes a volume. */
	YSRESULT DeleteVolume(VolumeHandle vlHd);

	/*! This function permanently deletes a volume. 
	    A frozen volume may be using frozen face groups.  
	    This function updates reference counts of the frozen face groups consistently.  */
	YSRESULT DeleteFrozenVolume(VolumeHandle vlHd);

	/*! This function sets a label of the volume. */
	YSRESULT SetVolumeLabel(VolumeHandle vlHd,const char label[]);

	/*! This function returns the number of edges.  Search table must be enabled to use this function, or this function returns zero. */
	YSSIZE_T GetNumEdge(void) const;

	/*! This function moves edge-enumeration handle to the next edge.  It returns YSERR when it iterated all the edges.  
	    This function requires search table enabled, or this function always returns YSERR.  */
	YSRESULT MoveToNextEdge(YsShellEdgeEnumHandle &handle) const;

	/*! This function returns two vertices of the edge of the given edge handle. 
	    This function requires search table enabled, or this function returns YSERR.*/
	YSRESULT GetEdge(YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellEdgeEnumHandle handle) const;

	/*! This function returns two vertices of the edge of the given edge handle. 
	    This function requires search table enabled, or this function returns YSERR.*/
	YSRESULT GetEdge(YsShellVertexHandle edVtHd[2],YsShellEdgeEnumHandle handle) const;

	/*! This function returns YsShell::Edge of the given edge handle. 
	    This function requires search table enabled, or this function returns YSERR.*/
	YsShell::Edge GetEdge(YsShellEdgeEnumHandle handle) const;

	/*! This function enables search table (topology table) that keeps track of entity usage. */
	YSRESULT EnableSearch(void);

	/*! This function returns YSTRUE if search table is enabled, or YSFALSE otherwise. */
	YSBOOL IsSearchEnabled(void) const;

	/*! This function disables search table (topology table) enabled by EnableSearch function. */
	YSRESULT DisableSearch(void);

	/*! This function tests integrity of the search table and prints diagnostics. */
	YSRESULT TestSearchTable(void) const;

	/*! This function returns a vertex that corresponds to the given search key (hash key). 
	    This function requires the search table to be enabled by EnableSearch function.  */
	YsShellVertexHandle FindVertex(YSHASHKEY vtKey) const;

	/*! This function returns a polygon that corresponds to the given search key (hash key). 
	    This function requires the search table to be enabled by EnableSearch function.  */
	YsShellPolygonHandle FindPolygon(YSHASHKEY plKey) const;

	/*! This function returns a polygon that has the same number of vertices and the same vertex sequence.
	    The order can be reversed, and the first vertex does not have to be the same.
	    If multiple polygons match, it returns the first one found.
	*/
	YsShell::PolygonHandle FindPolygonFromVertexSequence(YSSIZE_T nVt,const YsShellVertexHandle plVtHd[]) const;

	/*! This function returns a polygon that has the same number of vertices and the same vertex sequence.
	    The order can be reversed, and the first vertex does not have to be the same.
	    If multiple polygons match, it returns the first one found.
	*/
	YsShell::PolygonHandle FindPolygonFromVertexSequence(const YsConstArrayMask <YsShell::VertexHandle> &plVtHd) const;

	/*! This function returns the number of polygons that are using edge edHd. */
	int GetNumPolygonUsingEdge(YsShellEdgeEnumHandle edHd) const;

	/*! This function returns the number of polygons that are using edge edVtHd[0]-edVtHd[1]. */
	int GetNumPolygonUsingEdge(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns the number of polygons that are using edge edVtHd0-edVtHd1. */
	int GetNumPolygonUsingEdge(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns the number of polygons using the vertex. */
	int GetNumPolygonUsingVertex(YsShellVertexHandle vtHd) const;

	/*! This function returns the number of face groups using the vertex. */
	int GetNumFaceGroupUsingVertex(YsShellVertexHandle vtHd) const;

	/*! This function finds polygons that are using vertex vtHd. */
	YSRESULT FindPolygonFromVertex(YSSIZE_T &nPl,const YsShellPolygonHandle *&plHdArray,YsShellVertexHandle vtHd) const;

	/*! This function finds polygons that are using vertex vtHd. */
	template <const int N>
	YSRESULT FindPolygonFromVertex(YsArray <YsShellPolygonHandle,N> &plHdArray,YsShellVertexHandle vtHd) const;

	/*! This function finds polygons that are using vertex vtHd. */
	YsArray <YsShellPolygonHandle> FindPolygonFromVertex(YsShellVertexHandle vtHd) const;

	/*! This function finds polygons that are in the same group for smooth shading with the polygon plHd at vertex vtHd. */
	YsArray <YsShellPolygonHandle,8> FindSmoothShadingPolygonGroup(YsShellPolygonHandle plHd,YsShellVertexHandle vtHd) const;

	/*! This function finds polygons that are using edge edVtHd0-edVtHd1. */
	YSRESULT FindPolygonFromEdgePiece(YSSIZE_T &nPl,const YsShellPolygonHandle *&plHdArray,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function finds polygons that are using edge edVtHd[0]-edVtHd[1]. */
	YSRESULT FindPolygonFromEdgePiece(YSSIZE_T &nPl,const YsShellPolygonHandle *&plHdArray,const YsShellVertexHandle edVtHd[2]) const;

	/*! This function finds polygons that are using edge edVtHd0-edVtHd1. */
	template <const int N>
	YSRESULT FindPolygonFromEdgePiece(YsArray <YsShellPolygonHandle,N> &plHdArray,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function finds polygons that are using edge edVtHd[0]-edVtHd[1]. */
	template <const int N>
	YSRESULT FindPolygonFromEdgePiece(YsArray <YsShellPolygonHandle,N> &plHdArray,const YsShellVertexHandle edVtHd[2]) const;

	/*! This function finds polygons that are using edge pointed by edHd. */
	YsArray <YsShellPolygonHandle,2> FindPolygonFromEdgePiece(YsShellEdgeEnumHandle edHd) const;

	/*! This function finds polygons that are using edge. */
	YsArray <YsShellPolygonHandle,2> FindPolygonFromEdgePiece(YsShell::Edge edge) const;

	/*! This function finds polygons that are using edge edVtHd[0]-edVtHd[1]. */
	YsArray <YsShellPolygonHandle,2> FindPolygonFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function finds polygons that are using edge edVtHd[0]-edVtHd[1]. */
	YsArray <YsShellPolygonHandle,2> FindPolygonFromEdgePiece(const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1) const;

	/*! This function returns a polygon that is neighboring to plHd and is sharing nth edge of plHd.
	    If plVtHd[0] to plVtHd[N-1] are the vertices of the polygon plHd, nth edge is
	    plVtHd[n]-plVtHd[(n+1)%N].
	    If nth edge of plHd is not shared by two polygons (shared only by this polygon or by more than 2 polygons), it returns nullptr.
	    This function requires the search table to be enabled by EnableSearch function.  */
	YsShellPolygonHandle GetNeighborPolygon(YsShellPolygonHandle plHd,YSSIZE_T n) const;

	/*! This function returns a polygon that is neighboring to plHd along the edge edVtHd[0]-edVtHd[1]. 
	    If edVtHd[0]-edVtHd[1] of plHd is not shared by two polygons (shared only by this polygon or by more than 2 polygons), it returns nullptr.
	*/
	YsShellPolygonHandle GetNeighborPolygon(YsShellPolygonHandle plHd,const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns a polygon that is neighboring to plHd along the edge edVtHd0-edVtHd1. 
	    If edVtHd[0]-edVtHd[1] of plHd is not shared by two polygons (shared only by this polygon or by more than 2 polygons), it returns nullptr.
	*/
	YsShellPolygonHandle GetNeighborPolygon(YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns vertices that are connected by an edge from fromVtHd. */
	template <const int N>
	YSRESULT GetConnectedVertex(YsArray <YsShellVertexHandle,N> &connVtHd,YsShellVertexHandle fromVtHd) const;

	/*! This function returns vertices that are connected by an edge from fromVtHd. */
	YsArray <YsShellVertexHandle> GetConnectedVertex(YsShellVertexHandle fromVtHd) const;

	/*! This function returns vertices that are connected by an edge from fromVtHd that satisfies the condition. */
	YsArray <YsShellVertexHandle> GetConnectedVertex(YsShellVertexHandle fromVtHd,const Condition &cond) const;

	/*! This function returns number of vertices connected from vertex fromVtHd by a constraint edge. */
	int GetNumConstEdgeConnectedVertex(YsShellVertexHandle fromVtHd) const;

	/*! This function returns vertices that are connected by a const-edge from fromVtHd. */
	template <const int N>
	YSRESULT GetConstEdgeConnectedVertex(YsArray <YsShellVertexHandle,N> &connVtHd,YsShellVertexHandle fromVtHd) const;

	/*! This function returns vertices that are connected by a const-edge from fromVtHd. */
	YsArray <YsShellVertexHandle> GetConstEdgeConnectedVertex(YsShellVertexHandle fromVtHd) const;

	/*! This function returns a const-edge corresponds to the given search key (hash key). */
	ConstEdgeHandle FindConstEdge(YSHASHKEY ceKey) const;

	/*! This function returns a const-edge handle from an index.
	    An index is a zero-based sequential number and is different from an identifier.
	    This function always returns nullptr if not Encache()ed.
	*/
	ConstEdgeHandle GetConstEdgeHandleFromIndex(YSSIZE_T index) const;

	/*! This function finds constraint edges that are using vtHd. */
	YSRESULT FindConstEdgeFromVertex(YSSIZE_T &nCe,const ConstEdgeHandle *&ceHdPtr,YsShellVertexHandle vtHd) const;

	/*! This function finds constraint edges that are using vtHd. */
	YSRESULT FindConstEdgeFromVertex(YSSIZE_T &nCe,const ConstEdgeHandle *&ceHdPtr,YSHASHKEY vtKey) const;

	/*! This function finds constraint edges that are using vtHd. */
	template <const int N>
	YSRESULT FindConstEdgeFromVertex(YsArray <ConstEdgeHandle,N> &ceHdArray,YsShellVertexHandle vtHd) const;

	/*! This function finds constraint edges that are using vtHd. */
	YsArray <YsShellExt::ConstEdgeHandle> FindConstEdgeFromVertex(YsShellVertexHandle vtHd) const;

	/*! Finds constraint edges that shares at least one edge piece with the polygon plHd. */
	YsArray <YsShellExt::ConstEdgeHandle> FindConstEdgeFromPolygon(YsShellPolygonHandle plHd) const;

	/*! This function returns the number of constraint edges using vtHd. */
	int GetNumConstEdgeUsingVertex(YsShellVertexHandle vtHd) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd0-edVtHd1. */
	template <const int N>
	YSRESULT FindConstEdgeFromEdgePiece(YsArray <ConstEdgeHandle,N> &ceHdArray,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd[0]-edVtHd[1]. */
	template <const int N>
	YSRESULT FindConstEdgeFromEdgePiece(YsArray <ConstEdgeHandle,N> &ceHdArray,const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd0-edVtHd1. */
	template <const int N>
	YSRESULT FindConstEdgeFromEdgePiece(YsArray <ConstEdgeHandle,N> &ceHdArray,YSHASHKEY edVtKey0,YSHASHKEY edVtKey1) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd[0]-edVtHd[1]. */
	template <const int N>
	YSRESULT FindConstEdgeFromEdgePiece(YsArray <ConstEdgeHandle,N> &ceHdArray,const YSHASHKEY edVtKey[2]) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd0-edVtHd1. */
	YsArray <ConstEdgeHandle> FindConstEdgeFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd[0]-edVtHd[1]. */
	YsArray <ConstEdgeHandle> FindConstEdgeFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd0-edVtHd1. */
	YsArray <ConstEdgeHandle>  FindConstEdgeFromEdgePiece(YSHASHKEY edVtKey0,YSHASHKEY edVtKey1) const;

	/*! This function returns constraint edges that are using an edge piece edVtHd[0]-edVtHd[1]. */
	YsArray <ConstEdgeHandle>  FindConstEdgeFromEdgePiece(const YSHASHKEY edVtKey[2]) const;

	/*! This function returns constraint edges that is used by the face group fgHd. */
	YsArray <ConstEdgeHandle> FindConstEdgeFromFaceGroup(FaceGroupHandle fgHd) const;

	YSRESULT SetConstEdgeIsCrease(ConstEdgeHandle ceHd,YSBOOL isCreate);

	/*! This function returns YSTRUE if the const edge is marked as crease. */
	YSBOOL IsCreaseConstEdge(ConstEdgeHandle ceHd) const;

	/*! This function returns YSTRUE if the edge piece is used by a const edge marked as crease. */
	YSBOOL IsOnCreaseConstEdge(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns YSTRUE if the edge piece is used by a const edge marked as crease. */
	YSBOOL IsOnCreaseConstEdge(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns YSTRUE if the edge piece is used by a const edge marked as crease. */
	YSBOOL IsOnCreaseConstEdge(const YsShell::Edge &edge) const;

	/*! This function returns YSTRUE if the vertex is used by a const edge marked as crease. */
	YSBOOL IsOnCreaseConstEdge(YsShellVertexHandle vtHd) const;

	/*! This function returns YSTRUE if the given edge piece is used by at least one const edge. */
	YSBOOL IsEdgePieceConstrained(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns YSTRUE if the given edge piece is used by at least one const edge. */
	YSBOOL IsEdgePieceConstrained(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns YSTRUE if the given edge piece is used by at least one const edge. */
	YSBOOL IsEdgePieceConstrained(YsShellEdgeEnumHandle edHd) const;

	/*! This function returns YSTRUE if the given edge piece is used by at least one const edge. */
	YSBOOL IsEdgePieceConstrained(YsShell::Edge edge) const;

	/*! This function returns YSTRUE if the edge piece of the polygon is used by at least one const edge. */
	YSBOOL IsEdgePieceConstrained(YsShell::PolygonHandle plHd,YSSIZE_T edIdx) const;

	/*! This function returns YSTRUE if the const edge is using the edge piece. */
	YSBOOL IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns YSTRUE if the const edge is using the edge piece. */
	YSBOOL IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns YSTRUE if the const edge is using the edge piece. */
	YSBOOL IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,YsShellEdgeEnumHandle edHd) const;

	/*! This function returns YSTRUE if the const edge is using the edge piece. */
	YSBOOL IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,YsShell::Edge edge) const;

	/*! This function returns the number of const edges that are using an edge piece edVtHd0-edVtHd1. */
	int GetNumConstEdgeUsingEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns the number of const edges that are using an edge piece edVtHd0-edVtHd1. */
	int GetNumConstEdgeUsingEdgePiece(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns the number of const edges that are using an edge piece edVtHd0-edVtHd1. */
	int GetNumConstEdgeUsingEdgePiece(YsShellEdgeEnumHandle edHd) const;

	/*! This function returns the number of const edges that are using an edge piece edVtHd0-edVtHd1. */
	int GetNumConstEdgeUsingEdgePiece(YsShell::Edge edge) const;

	/*! This function returns a face-group handle corresponds to the given search key (hash key). */
	FaceGroupHandle FindFaceGroup(YSHASHKEY fgKey) const;

	/*! This function returns a face-group handle that includes the given polygon.  
	    If plHd==NULL, this function returns NULL.  */
	FaceGroupHandle FindFaceGroupFromPolygon(YsShellPolygonHandle plHd) const;

	/*! This function returns a face-group handles that are incident to the given vertex.  
	    If plHd==NULL, this function returns NULL.  */
	YsArray <FaceGroupHandle,4> FindFaceGroupFromVertex(YsShellVertexHandle vtHd) const;

	/*! This function returns a face-group handles that are incident to the given edge.  
	    If plHd==NULL, this function returns NULL.  */
	YsArray <FaceGroupHandle,2> FindFaceGroupFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! This function returns face-group handles that are incident to the given edge.  
	    If plHd==NULL, this function returns NULL.  */
	YsArray <FaceGroupHandle,2> FindFaceGroupFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const;

	/*! This function returns face-group handles that are incident to the given const-edge. */
	YsArray <FaceGroupHandle,2> FindFaceGroupFromConstEdge(ConstEdgeHandle ceHd) const;

	/*! This function retusns an array of neighboring face groups of fgHd. */
	YsArray <FaceGroupHandle> GetEdgeConnectedNeighborFaceGroup(FaceGroupHandle fromFgHd) const;

	/*! This function retusns an array of neighboring face groups of fgHd. */
	YsArray <FaceGroupHandle> GetVertexConnectedNeighborFaceGroup(FaceGroupHandle fromFgHd) const;

	/*! This function returns an array of neighboring face groups from fgHd across the const edge ceHd. */
	YsArray <FaceGroupHandle> GetConstEdgeConnectedNeighborFaceGroup(FaceGroupHandle fromFgHd,ConstEdgeHandle ceHd) const;

	/* This function returns vertices of the face group away from the face-group boundary. */
	YsArray <VertexHandle> GetFaceGroupInsideVertex(FaceGroupHandle fgHd) const;

	/*! This function returns a volume handle corresponds to the given search key (hash key). */
	VolumeHandle FindVolume(YSHASHKEY vlKey) const;


	/*! This function overrides the vertex search keys.  HandleKeyTable class must be a storage class of YsPair <YsShell::VertexHandle,YSHASHKEY> 
	    with forward iterators support.  Search table must be detached when calling this function, or this function will fail and do nothing.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverrideVertexSearchKey(const HandleKeyTable &table);

	/*! This function overrides the vertex search keys.  HandleKeyTable class must be a storage class of YsPair <YsShellPolygonHandle,YSHASHKEY> 
	    with forward iterators support.  Search table must be detached when calling this function, or this function will fail and do nothing.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverridePolygonSearchKey(const HandleKeyTable &table);

	/*! This function overrides the const-edge search keys.  HandleKeyTable class must be a storage class of YsPair <YsShellExt::ConstEdgeHandle,YSHASHKEY> 
	    with forward iterators support.  Search must be disabled by DisableSearch before this function, or this function will fail and do nothing.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverrideConstEdgeSearchKey(const HandleKeyTable &table);

	/*! This function overrides the face-grouop search keys.  HandleKeyTable class must be a storage class of YsPair <YsShellExt::FaceGroupHandle,YSHASHKEY> 
	    with forward iterators support.  Search must be disabled by DisableSearch before this function, or this function will fail and do nothing.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverrideFaceGroupSearchKey(const HandleKeyTable &table);

	/*! This function overrides the volume search keys.  HandleKeyTable class must be a storage class of YsPair <YsShellExt::VolumeHandle,YSHASHKEY> 
	    with forward iterators support.  Search must be disabled by DisableSearch before this function, or this function will fail and do nothing.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverrideVolumeSearchKey(const HandleKeyTable &table);
};

inline YsShellExt::Converter YsShellExt::Conv(void)
{
	Converter conv;
	conv.shl=this;
	return conv;
}

inline YsShellExt::ConstConverter YsShellExt::Conv(void) const
{
	ConstConverter conv;
	conv.shl=this;
	return conv;
}

/*! Support for STL-like iterator */
inline YsShellExt::ConstEdgeEnumerator::iterator begin(const YsShellExt::ConstEdgeEnumerator &allConstEdge)
{
	return allConstEdge.begin();
}

/*! Support for STL-like iterator */
inline YsShellExt::ConstEdgeEnumerator::iterator end(const YsShellExt::ConstEdgeEnumerator &allConstEdge)
{
	return allConstEdge.end();
}

/*! Support for STL-like iterator */
inline YsShellExt::ConstEdgeEnumerator::iterator rbegin(const YsShellExt::ConstEdgeEnumerator &allConstEdge)
{
	return allConstEdge.rbegin();
}

/*! Support for STL-like iterator */
inline YsShellExt::ConstEdgeEnumerator::iterator rend(const YsShellExt::ConstEdgeEnumerator &allConstEdge)
{
	return allConstEdge.rend();
}

/*! Support for STL-like iterator */
inline YsShellExt::FaceGroupEnumerator::iterator begin(const YsShellExt::FaceGroupEnumerator &allFaceGroup)
{
	return allFaceGroup.begin();
}

/*! Support for STL-like iterator */
inline YsShellExt::FaceGroupEnumerator::iterator end(const YsShellExt::FaceGroupEnumerator &allFaceGroup)
{
	return allFaceGroup.end();
}

/*! Support for STL-like iterator */
inline YsShellExt::FaceGroupEnumerator::iterator rbegin(const YsShellExt::FaceGroupEnumerator &allFaceGroup)
{
	return allFaceGroup.rbegin();
}

/*! Support for STL-like iterator */
inline YsShellExt::FaceGroupEnumerator::iterator rend(const YsShellExt::FaceGroupEnumerator &allFaceGroup)
{
	return allFaceGroup.rend();
}

/*! Support for STL-like iterator */
inline YsShellExt::VolumeEnumerator::iterator begin(const YsShellExt::VolumeEnumerator &allVolume)
{
	return allVolume.begin();
}

/*! Support for STL-like iterator */
inline YsShellExt::VolumeEnumerator::iterator end(const YsShellExt::VolumeEnumerator &allVolume)
{
	return allVolume.end();
}

/*! Support for STL-like iterator */
inline YsShellExt::VolumeEnumerator::iterator rbegin(const YsShellExt::VolumeEnumerator &allVolume)
{
	return allVolume.rbegin();
}

/*! Support for STL-like iterator */
inline YsShellExt::VolumeEnumerator::iterator rend(const YsShellExt::VolumeEnumerator &allVolume)
{
	return allVolume.rend();
}



#include "ysshellextdrawbuffer.h"

template <const int N>
YSRESULT YsShellExt::GetConstEdge(YsArray <YsShellVertexHandle,N> &ceVtHd,YSBOOL &isLoop,ConstEdgeHandle ceHd) const
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp)
	{
		isLoop=ceProp->IsLoop();
		ceVtHd=ceProp->vtHdArray;
		return YSOK;
	}
	ceVtHd.Clear();
	return YSERR;
}

template <const int N>
YSRESULT YsShellExt::GetFaceGroup(YsArray <YsShellPolygonHandle,N> &fgPlHd,FaceGroupHandle fgHd) const
{
	const FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp)
	{
		fgPlHd.Set(fgProp->fgPlHd.GetN(),nullptr);
		YSSIZE_T idx=0;
		for(auto plHd : fgProp->fgPlHd)
		{
			fgPlHd[idx++]=plHd;
		}
		return YSOK;
	}
	fgPlHd.Clear();
	return YSERR;
}

template <const int N>
YSRESULT YsShellExt::GetVolume(YsArray <YsShellExt::FaceGroupHandle,N> &vlFgHd,YsShellExt::VolumeHandle vlHd) const
{
	const Volume *vlProp=volumeArray[vlHd];
	if(NULL!=vlProp)
	{
		vlFgHd=vlProp->fgHdArray;
		return YSOK;
	}
	vlFgHd.Clear();
	return YSERR;
}

template <const int N>
YsShellExt::VolumeHandle YsShellExt::AddVolume(const YsArray <FaceGroupHandle,N> &vlFgHd)
{
	return AddVolume(vlFgHd.GetN(),vlFgHd);
}

template <const int N>
YSRESULT YsShellExt::ModifyVolume(VolumeHandle vlHd,const YsArray <FaceGroupHandle,N> &vlFgHd)
{
	return ModifyVolume(vlHd,vlFgHd.GetN(),vlFgHd);
}

template <const int N>
YSRESULT YsShellExt::AddVolumeFaceGroup(VolumeHandle vlHd,const YsArray <FaceGroupHandle,N> &vlFgHd)
{
	return AddVolumeFaceGroup(vlHd,vlFgHd.GetN(),vlFgHd);
}

template <const int N>
YSRESULT YsShellExt::FindPolygonFromVertex(YsArray <YsShellPolygonHandle,N> &plHdArray,YsShellVertexHandle vtHd) const
{
	YSSIZE_T nVtPl;
	const YsShellPolygonHandle *vtPlHd;
	if(YSOK==FindPolygonFromVertex(nVtPl,vtPlHd,vtHd))
	{
		plHdArray.Set(nVtPl,vtPlHd);
		return YSOK;
	}
	return YSERR;
}

/*! This function finds polygons that are using edge edVtHd0-edVtHd1. */
template <const int N>
YSRESULT YsShellExt::FindPolygonFromEdgePiece(YsArray <YsShellPolygonHandle,N> &plHdArray,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	YSSIZE_T nPl;
	const YsShellPolygonHandle *plHd;
	if(YSOK==FindPolygonFromEdgePiece(nPl,plHd,edVtHd0,edVtHd1))
	{
		plHdArray.Set(nPl,plHd);
		return YSOK;
	}
	return YSERR;
}

/*! This function finds polygons that are using edge edVtHd[0]-edVtHd[1]. */
template <const int N>
YSRESULT YsShellExt::FindPolygonFromEdgePiece(YsArray <YsShellPolygonHandle,N> &plHdArray,const YsShellVertexHandle edVtHd[2]) const
{
	return FindPolygonFromEdgePiece(plHdArray,edVtHd[0],edVtHd[1]);
}

template <const int N>
YSRESULT YsShellExt::GetConnectedVertex(YsArray <YsShellVertexHandle,N> &connVtHd,YsShellVertexHandle fromVtHd) const
{
	const YsShellSearchTable *search=YsShell::GetSearchTable();
	if(NULL!=search)
	{
		return search->GetConnectedVertexList(connVtHd,*(const YsShell *)this,fromVtHd);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return YSERR;
}

template <const int N>
YSRESULT YsShellExt::GetConstEdgeConnectedVertex(YsArray <YsShellVertexHandle,N> &connVtHd,YsShellVertexHandle fromVtHd) const
{
	connVtHd.CleanUp();

	YsArray <YsShellExt::ConstEdgeHandle,N> vtCeHd;
	if(YSOK==FindConstEdgeFromVertex(vtCeHd,fromVtHd))
	{
		for(auto ceHd : vtCeHd)
		{
			YsArray <YsShellVertexHandle,16> ceVtHd;
			YSBOOL isLoop;
			GetConstEdge(ceVtHd,isLoop,ceHd);
			if(0<ceVtHd.GetN())
			{
				if(YSTRUE==isLoop)
				{
					ceVtHd.Append(ceVtHd[0]);
				}
				for(auto idx : ceVtHd.AllIndex())
				{
					if(ceVtHd[idx]==fromVtHd)
					{
						if(0<idx)
						{
							connVtHd.Append(ceVtHd[idx-1]);
						}
						if(ceVtHd.GetN()-1>idx)
						{
							connVtHd.Append(ceVtHd[idx+1]);
						}
					}
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

template <const int N>
YSRESULT YsShellExt::FindConstEdgeFromVertex(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,YsShellVertexHandle vtHd) const
{
	YSSIZE_T nCe;
	const ConstEdgeHandle *ceHd;
	if(YSOK==FindConstEdgeFromVertex(nCe,ceHd,vtHd))
	{
		ceHdArray.Set(nCe,ceHd);
		return YSOK;
	}
	else
	{
		ceHdArray.CleanUp();
		return YSERR;
	}
}

template <const int N>
YSRESULT YsShellExt::FindConstEdgeFromEdgePiece(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,YSHASHKEY edVtKey0,YSHASHKEY edVtKey1) const
{
	if(edVtKey0==edVtKey1)
	{
		ceHdArray.CleanUp();
		return YSERR;
	}

	YSSIZE_T nCe0,nCe1;
	const ConstEdgeHandle *ceHdArray0,*ceHdArray1;

	if(YSOK==FindConstEdgeFromVertex(nCe0,ceHdArray0,edVtKey0) &&
	   YSOK==FindConstEdgeFromVertex(nCe1,ceHdArray1,edVtKey1) &&
	   0<nCe0 &&
	   0<nCe1)
	{
		ceHdArray.CleanUp();

		for(YSSIZE_T idx0=0; idx0<nCe0; ++idx0)
		{
			for(YSSIZE_T idx1=0; idx1<nCe1; ++idx1)
			{
				if(ceHdArray0[idx0]==ceHdArray1[idx1])
				{
					ceHdArray.Append(ceHdArray0[idx0]);
				}
			}
		}

		for(YSSIZE_T idx=ceHdArray.GetN()-1; 0<=idx; --idx)
		{
			YsArray <YsShellVertexHandle,16> ceVtHd;
			YSBOOL isLoop;
			GetConstEdge(ceVtHd,isLoop,ceHdArray[idx]);

			if(YSTRUE==isLoop && 0<ceVtHd.GetN())
			{
				YsShellVertexHandle vtHd0=ceVtHd[0];
				ceVtHd.Append(vtHd0);
			}

			YSBOOL reallyIncluded=YSFALSE;
			for(YSSIZE_T edIdx=0; edIdx<ceVtHd.GetN()-1; ++edIdx)
			{
				if(YSTRUE==YsSameEdge(
				    GetSearchKey(ceVtHd[edIdx]),GetSearchKey(ceVtHd[edIdx+1]),
				    edVtKey0,edVtKey1))
				{
					reallyIncluded=YSTRUE;
					break;
				}
			}

			if(YSTRUE!=reallyIncluded)
			{
				ceHdArray.DeleteBySwapping(idx);
			}
		}

		return YSOK;
	}
	return YSERR;
}


template <const int N>
YSRESULT YsShellExt::FindConstEdgeFromEdgePiece(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,const YSHASHKEY edVtKey[2]) const
{
	return FindConstEdgeFromEdgePiece(ceHdArray,edVtKey[0],edVtKey[1]);
}

template <const int N>
YSRESULT YsShellExt::FindConstEdgeFromEdgePiece(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	return FindConstEdgeFromEdgePiece(ceHdArray,GetSearchKey(edVtHd0),GetSearchKey(edVtHd1));
}


template <const int N>
YSRESULT YsShellExt::FindConstEdgeFromEdgePiece(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,const YsShellVertexHandle edVtHd[2]) const
{
	return FindConstEdgeFromEdgePiece(ceHdArray,GetSearchKey(edVtHd[0]),GetSearchKey(edVtHd[1]));
}


template <typename HandleKeyTable>
YSRESULT YsShellExt::OverrideVertexSearchKey(const HandleKeyTable &table)
{
	if(YSTRUE==IsSearchEnabled())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be detached before calling this function.\n");
		return YSERR;
	}

	YsHashTable <YSHASHKEY> vtKeyMap;
	vtKeyMap.PrepareTableForNElement(table.GetN());
	for(auto &handleKey : table)
	{
		YSHASHKEY fromKey=GetSearchKeyMaybeFrozen(handleKey.a);
		YSHASHKEY toKey=handleKey.b;
		vtKeyMap.Add(fromKey,toKey);
	}

	YsArray <YsPair <YSHASHKEY,YsEditArrayObjectHandle <VertexAttrib> > > keyAttrArray;
    for(auto handle=vtKeyToVtAttribArrayHandle.NullHandle(); YSOK==vtKeyToVtAttribArrayHandle.FindNextElement(handle); )
	{
		YSHASHKEY oldKey=vtKeyToVtAttribArrayHandle.Key(handle);
		YSHASHKEY newKey;
		if(YSOK!=vtKeyMap.FindElement(newKey,oldKey))
		{
			newKey=oldKey;
		}
		auto value=vtKeyToVtAttribArrayHandle.Value(handle);
		keyAttrArray.Increment();
		keyAttrArray.Last().a=newKey;
		keyAttrArray.Last().b=value;
	}

	if(YSOK==YsShell::OverrideVertexSearchKey<HandleKeyTable>(table))
	{
		vtKeyToVtAttribArrayHandle.PrepareTableForNElement(keyAttrArray.GetN());
		for(auto keyAttr : keyAttrArray)
		{
			vtKeyToVtAttribArrayHandle.Add(keyAttr.a,keyAttr.b);
		}
		return YSOK;
	}
	return YSERR;
}

template <typename HandleKeyTable>
YSRESULT YsShellExt::OverridePolygonSearchKey(const HandleKeyTable &table)
{
	if(YSTRUE==IsSearchEnabled())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be detached before calling this function.\n");
		return YSERR;
	}

	YsHashTable <YSHASHKEY> plKeyMap;
	plKeyMap.PrepareTableForNElement(table.GetN());
	for(auto &handleKey : table)
	{
		YSHASHKEY fromKey=GetSearchKeyMaybeFrozen(handleKey.a);
		YSHASHKEY toKey=handleKey.b;
		plKeyMap.Add(fromKey,toKey);
	}

	YsArray <YsPair <YSHASHKEY,YsEditArrayObjectHandle <PolygonAttrib> > > keyAttrArray;
    for(auto handle=plKeyToPlAttribArrayHandle.NullHandle(); YSOK==plKeyToPlAttribArrayHandle.FindNextElement(handle); )
	{
		YSHASHKEY oldKey=plKeyToPlAttribArrayHandle.Key(handle);
		YSHASHKEY newKey;
		if(YSOK!=plKeyMap.FindElement(newKey,oldKey))
		{
			newKey=oldKey;
		}
		auto value=plKeyToPlAttribArrayHandle.Value(handle);
		keyAttrArray.Increment();
		keyAttrArray.Last().a=newKey;
		keyAttrArray.Last().b=value;
	}

	if(YSOK==YsShell::OverridePolygonSearchKey<HandleKeyTable>(table))
	{
		plKeyToPlAttribArrayHandle.PrepareTableForNElement(keyAttrArray.GetN());
		for(auto keyAttr : keyAttrArray)
		{
			plKeyToPlAttribArrayHandle.Add(keyAttr.a,keyAttr.b);
		}
		return YSOK;
	}
	return YSERR;
}

template <typename HandleKeyTable>
YSRESULT YsShellExt::OverrideConstEdgeSearchKey(const HandleKeyTable &table)
{
	if(YSTRUE==IsSearchEnabled())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be detached before calling this function.\n");
		return YSERR;
	}
	constEdgeArray.DisableSearch();
	auto res=constEdgeArray.OverrideSearchKey<HandleKeyTable>(table);
	constEdgeArray.EnableSearch();
	return res;
}

template <typename HandleKeyTable>
YSRESULT YsShellExt::OverrideFaceGroupSearchKey(const HandleKeyTable &table)
{
	if(YSTRUE==IsSearchEnabled())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be detached before calling this function.\n");
		return YSERR;
	}
	faceGroupArray.DisableSearch();
	auto res=faceGroupArray.OverrideSearchKey<HandleKeyTable>(table);
	faceGroupArray.EnableSearch();
	return res;
}

template <typename HandleKeyTable>
YSRESULT YsShellExt::OverrideVolumeSearchKey(const HandleKeyTable &table)
{
	if(YSTRUE==IsSearchEnabled())
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Search table must be detached before calling this function.\n");
		return YSERR;
	}
	volumeArray.DisableSearch();
	auto res=volumeArray.OverrideSearchKey<HandleKeyTable>(table);
	volumeArray.EnableSearch();
	return res;
}


/* } */
#endif
