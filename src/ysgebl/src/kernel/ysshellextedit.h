/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit.h
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

#ifndef YSSHELLEXTEDIT_IS_INCLUDED
#define YSSHELLEXTEDIT_IS_INCLUDED
/* { */

#include "ysshellext.h"
#include <cstddef>

/*! This class provides a structure for a polygonal shell for interactive modelling.
    It keeps operation logs and supports undo- and redo-ing.

	This class privately-inherits YsShellExt.  However, the following labels are made public by 'using' keyword.

	- using YsShell::FindNextVertex;
	- using YsShell::FindNextPolygon;
	- using YsShell::VertexHandle;
	- using YsShell::PolygonHandle;
	- using YsShell::ComputeVolume;
	- using YsShell::ComputeTotalArea;
	- using YsShell::ComputeDihedralAngle;
	- using YsShell::GetNumVertex;
	- using YsShell::GetVertexPosition;
	- using YsShell::GetMultiVertexPosition;
	- using YsShell::GetNumPolygon;
	- using YsShell::GetVertexListOfPolygon;
	- using YsShell::GetNumVertexOfPolygon;
	- using YsShell::GetBoundingBox;
	- using YsShell::GetVertex;
	- using YsShell::GetNormalOfPolygon;
	- using YsShell::GetColorOfPolygon;
	- using YsShell::GetCenter;
	- using YsShell::GetCenterOfPolygon;
	- using YsShell::GetCenterOfEdge;
	- using YsShell::GetCenterOfVertexGroup;
	- using YsShell::GetCenterOfTriangle;
	- using YsShell::AllVertex;
	- using YsShell::AllPolygon;
	- using YsShell::GetPolygon;
	- using YsShell::GetPolygonVertex;
	- using YsShell::GetPolygonVertexPosition;
	- using YsShell::GetPolygonArea;
	- using YsShell::GetPolygonEdge;
	- using YsShell::GetPolygonEdgeVertex;
	- using YsShell::GetPolygonEdgeIndex;
	- using YsShell::GetPolygonNumVertex;
	- using YsShell::GetPosition;
	- using YsShell::GetColor;
	- using YsShell::GetNormal;
	- using YsShell::SaveStl;
	- using YsShell::SaveBinStl;
	- using YsShell::GetVertexIdFromHandle;
	- using YsShell::GetVertexHandleFromId;
	- using YsShell::GetVertexListFromPolygonList;
	- using YsShell::GetPolygonIdFromHandle;
	- using YsShell::GetPolygonHandleFromId;
	- using YsShell::GetEdgeLength;
	- using YsShell::GetEdgeVector;
	- using YsShell::IsPolygonUsingVertex;
	- using YsShell::IsPolygonUsingEdgePiece;
	- using YsShell::IsFrozen;
	- using YsShell::GetSearchKeyMaybeFrozen;
	- using YsShell::ShootInfiniteRay;
	- using YsShell::CheckInsideSolid;
	- using YsShellExt::Encache;
	- using YsShellExt::EnableSearch;
	- using YsShellExt::DisableSearch;
	- using YsShellExt::IsSearchEnabled;
	- using YsShellExt::TestSearchTable;
	- using YsShellExt::MoveToNextVertex;
	- using YsShellExt::MoveToNextPolygon;
	- using YsShellExt::GetVertexAttrib;
	- using YsShellExt::GetPolygonAttrib;
	- using YsShellExt::GetNumConstEdge;
	- using YsShellExt::MoveToNextConstEdge;
	- using YsShellExt::MoveToPrevConstEdge;
	- using YsShellExt::GetConstEdge;
	- using YsShellExt::GetConstEdgeAttrib;
	- using YsShellExt::GetConstEdgeEndPoint;
	- using YsShellExt::GetConstEdgeVertex;
	- using YsShellExt::IsConstEdgeLoop;
	- using YsShellExt::IsEdgeOnFaceGroupBoundary;
	- using YsShellExt::GetConstEdgeIdent;
	- using YsShellExt::GetConstEdgeLabel;
	- using YsShellExt::GetNumFaceGroup;
	- using YsShellExt::GetNumVolume;
	- using YsShellExt::GetVolume;
	- using YsShellExt::GetSearchKey;
	- using YsShellExt::MoveToNextFaceGroup;
	- using YsShellExt::GetFaceGroup;
	- using YsShellExt::GetFaceGroupRaw;
	- using YsShellExt::GetFaceGroupVertex;
	- using YsShellExt::GetFaceGroupAttrib;
	- using YsShellExt::GetFaceGroupIdent;
	- using YsShellExt::GetFaceGroupLabel;
	- using YsShellExt::GetNumEdge;
	- using YsShellExt::GetNumPolygonUsingEdge;
	- using YsShellExt::GetNumPolygonUsingVertex;
	- using YsShellExt::GetEdge;
	- using YsShellExt::MoveToNextEdge;
	- using YsShellExt::CacheVertexNormal;
	- using YsShellExt::FindVertex;
	- using YsShellExt::FindPolygon;
	- using YsShellExt::FindPolygonFromVertexSequence;
	- using YsShellExt::FindPolygonFromVertex;
	- using YsShellExt::FindPolygonFromEdgePiece;
	- using YsShellExt::FindConstEdge;
	- using YsShellExt::FindConstEdgeFromVertex;
	- using YsShellExt::FindConstEdgeFromEdgePiece;
	- using YsShellExt::FindConstEdgeFromFaceGroup;
	- using YsShellExt::IsOnCreaseConstEdge;
	- using YsShellExt::IsCreaseConstEdge;
	- using YsShellExt::FindConstEdgeFromPolygon;
	- using YsShellExt::GetNumConstEdgeUsingVertex;
	- using YsShellExt::GetNumFaceGroupUsingVertex;
	- using YsShellExt::IsEdgePieceConstrained;
	- using YsShellExt::IsConstEdgeUsingEdgePiece;
	- using YsShellExt::GetNumConstEdgeUsingEdgePiece;
	- using YsShellExt::FindFaceGroup;
	- using YsShellExt::FindFaceGroupFromPolygon;
	- using YsShellExt::FindFaceGroupFromEdgePiece;
	- using YsShellExt::FindFaceGroupFromConstEdge;
	- using YsShellExt::GetEdgeConnectedNeighborFaceGroup;
	- using YsShellExt::GetVertexConnectedNeighborFaceGroup;
	- using YsShellExt::GetConstEdgeConnectedNeighborFaceGroup;
	- using YsShellExt::GetDrawingBuffer;
	- using YsShellExt::GetConnectedVertex;
	- using YsShellExt::GetNumConstEdgeConnectedVertex;
	- using YsShellExt::GetConstEdgeConnectedVertex;
	- using YsShellExt::GetNeighborPolygon;
	- using YsShellExt::IsVertexUsed;
	- using YsShellExt::IsVertexUsedByConstEdge;
	- using YsShellExt::AllConstEdge;
	- using YsShellExt::AllFaceGroup;
	- using YsShellExt::AllVolume;
	- using YsShellExt::TemporaryModification;
	- using YsShellExt::OverrideVertexSearchKey;
	- using YsShellExt::OverridePolygonSearchKey;
	- using YsShellExt::OverrideConstEdgeSearchKey;
	- using YsShellExt::OverrideFaceGroupSearchKey;
	- using YsShellExt::OverrideVolumeSearchKey;

*/
class YsShellExtEdit : private YsShellExt
{
public:
	class StopIncUndo;

private:
	class EditLog
	{
	public:
		YSBOOL isRememberSelection; 
		// Remember Selection is a special undo type.
		// If undoPtr or redoPtr stops at Remember-Selection node, it will go one
		// more step to execute remember selection.

		YSSIZE_T undoCtr;
		EditLog *prev,*next;

		EditLog();
		virtual ~EditLog(){};
		virtual void Undo(YsShellExtEdit *shl)=0;
		virtual void Redo(YsShellExtEdit *shl)=0;
		virtual void NeverBeUndoneAgain(YsShellExtEdit *){};
		virtual void NeverBeRedoneAgain(YsShellExtEdit *){};
	};



	YsArray <unsigned int> selectedVtKey,selectedPlKey,selectedCeKey,selectedFgKey,selectedVlKey;

	class UndoPrintMessage;
	friend class UndoPrintMessage;
	class UndoRememberSelection;
	friend class UndoRememberSelection;
	class UndoAddVertex;
	friend class UndoAddVertex;
	class UndoChangeVertexAttrib;
	friend class UndoChangeVertexAttrib;
	class UndoDeleteVertex;
	friend class UndoDeleteVertex;
	class UndoAddPolygon;
	friend class UndoAddPolygon;
	class UndoSetPolygonColor;
	friend class UndoSetPolygonColor;
	class UndoDeletePolygon;
	friend class UndoDeletePolygon;
	class UndoChangePolygonAttrib;
	friend class UndoChangePolygonAttrib;
	class UndoVertexAttribChange;
	friend class UndoVertexAttribChange;
	class UndoSetPolygonNormal;
	friend class UndoSetPolygonNormal;
	class UndoAddConstEdge;
	friend class UndoAddConstEdge;
	class UndoAddFaceGroup;
	friend class UndoAddFaceGroup;
	class UndoSetVertexPosition;
	friend class UndoSetVertexPosition;
	class UndoSetMultiVertexPosition;
	friend class UndoSetMultiVertexPosition;
	class UndoSetPolygonVertex;
	friend class UndoSetPolygonVertex;
	class UndoModifyFaceGroup;
	friend class UndoModifyFaceGroup;
	class UndoDeleteFaceGroupPolygon;
	friend class UndoDeleteFaceGroupPolygon;
	class UndoDeleteFaceGroup;
	friend class UndoDeleteFaceGroup;
	class UndoSetFaceGroupAttrib;
	friend class UndoSetFaceGroupAttrib;
	class UndoSetFaceGroupLabel;
	friend class UndoSetFaceGroupLabel;
	class UndoModifyConstEdge;
	friend class UndoModifyConstEdge;
	class UndoSetConstEdgeAttrib;
	friend class UndoSetConstEdgeAttrib;
	class UndoSetConstEdgeLabel;
	friend class UndoSetConstEdgeLabel;
	class UndoDeleteMultiConstEdge;
	friend class UndoDeleteMultiConstEdge;
	class UndoSetConstEdgeIsLoop;
	friend class UndoSetConstEdgeIsLoop;
	class UndoSetConstEdgeIsCrease;
	friend class UndoSetConstEdgeIsCrease;
	class UndoAddMultiConstEdge;
	friend class UndoAddMultiConstEdge;
	class UndoAddMultiFaceGroup;
	friend class UndoAddMultiFaceGroup;
	class UndoAddMetaData;
	friend class UndoAddMetaData;
	class UndoDeleteMetaData;
	friend class UndoDeleteMetaData;
	class UndoSetMetaDataValue;
	friend class UndoMetaDataValue;

	YSBOOL takeEditLog;
	YSSIZE_T undoCtr;
	int incUndo;
	EditLog *undoPtr,*redoPtr;

public:
	template <class UndoType>
	class NewUndoLog
	{
	private:
		YSBOOL undoShouldBeDiscarded;
		UndoType *undoPtr;
		YsShellExtEdit *shlPtr;
	public:
		inline NewUndoLog(YsShellExtEdit *thisPtr)
		{
			if(YSTRUE==thisPtr->takeEditLog)
			{
				this->shlPtr=thisPtr;
				this->undoPtr=new UndoType;
				this->undoShouldBeDiscarded=YSFALSE;
			}
			else
			{
				this->undoPtr=nullptr;
			}
		}
		inline ~NewUndoLog()
		{
			if(nullptr!=undoPtr)
			{
				if(YSTRUE!=undoShouldBeDiscarded)
				{
					shlPtr->AddUndo(undoPtr);
				}
				else
				{
					delete undoPtr;
				}
				undoPtr=nullptr;
			}
		}
		inline bool IsNull(void) const
		{
			return nullptr==undoPtr;
		}
		inline bool IsNotNull(void) const
		{
			return nullptr!=undoPtr;
		}
		inline void Discard(void)
		{
			undoShouldBeDiscarded=YSTRUE;
		}
		inline UndoType *Ptr(void) const
		{
			return undoPtr;
		}
		inline bool operator==(std::nullptr_t)
		{
			return IsNull();
		}
		inline bool operator!=(std::nullptr_t)
		{
			return IsNotNull();
		}
	};


private:
	mutable YSSIZE_T lastSavedUndoCtr;
	mutable EditLog *lastSavedUndoPtr;
	mutable YsWString fn;

	void CleanUpEditLog(void);
	void CleanUpUndoLog(void);
	void CleanUpRedoLog(void);
	void AddUndo(EditLog *editLog);
	void InsertRememberSelection(void);

public:
	class SavePoint
	{
	public:
		int undoCtr;
		const EditLog *undoPtr;
		inline SavePoint()
		{
			undoCtr=0;
			undoPtr=NULL;
		};
	};



public:
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
	using YsShell::GetVertexPosition;
	using YsShell::GetMultiVertexPosition;
	using YsShell::GetNumPolygon;
	using YsShell::GetVertexListOfPolygon;
	using YsShell::GetNumVertexOfPolygon;
	using YsShell::GetBoundingBox;
	using YsShell::GetVertex;
	using YsShell::GetNormalOfPolygon;
	using YsShell::GetColorOfPolygon;
	using YsShell::GetCenter;
	using YsShell::GetCenterOfPolygon;
	using YsShell::GetCenterOfEdge;
	using YsShell::GetCenterOfVertexGroup;
	using YsShell::GetCenterOfTriangle;
	using YsShell::AllVertex;
	using YsShell::AllPolygon;
	using YsShell::GetPolygon;
	using YsShell::GetPolygonVertex;
	using YsShell::GetPolygonVertexQuick;
	using YsShell::GetPolygonVertexPosition;
	using YsShell::GetPolygonArea;
	using YsShell::GetPolygonEdge;
	using YsShell::GetPolygonEdgeVertex;
	using YsShell::GetPolygonEdgeIndex;
	using YsShell::GetPolygonNumVertex;
	using YsShell::GetPosition;
	using YsShell::GetColor;
	using YsShell::GetNormal;
	using YsShell::SaveStl;
	using YsShell::SaveBinStl;
	using YsShell::GetVertexIdFromHandle;
	using YsShell::GetVertexHandleFromId;
	using YsShell::GetVertexListFromPolygonList;
	using YsShell::GetPolygonIdFromHandle;
	using YsShell::GetPolygonHandleFromId;
	using YsShell::GetEdgeLength;
	using YsShell::GetEdgeVector;
	using YsShell::IsPolygonUsingVertex;
	using YsShell::IsPolygonUsingEdgePiece;
	using YsShell::IsFrozen;
	using YsShell::GetSearchKeyMaybeFrozen;
	using YsShell::ShootInfiniteRay;
	using YsShell::CheckInsideSolid;
	using YsShell::FindFirstIntersection;
	using YsShellExt::NullConstEdge;
	using YsShellExt::NullFaceGroup;
	using YsShellExt::NullVolume;
	using YsShellExt::Encache;
	using YsShellExt::EnableSearch;
	using YsShellExt::DisableSearch;
	using YsShellExt::IsSearchEnabled;
	using YsShellExt::TestSearchTable;
	using YsShellExt::MoveToNextVertex;
	using YsShellExt::MoveToNextPolygon;
	using YsShellExt::GetVertexAttrib;
	using YsShellExt::GetPolygonAttrib;
	using YsShellExt::GetNumConstEdge;
	using YsShellExt::MoveToNextConstEdge;
	using YsShellExt::MoveToPrevConstEdge;
	using YsShellExt::GetConstEdge;
	using YsShellExt::GetConstEdgeAttrib;
	using YsShellExt::GetConstEdgeEndPoint;
	using YsShellExt::GetConstEdgeVertex;
	using YsShellExt::IsConstEdgeLoop;
	using YsShellExt::IsEdgeOnFaceGroupBoundary;
	using YsShellExt::GetConstEdgeIdent;
	using YsShellExt::GetConstEdgeLabel;
	using YsShellExt::GetNumFaceGroup;
	using YsShellExt::GetNumVolume;
	using YsShellExt::GetVolume;
	using YsShellExt::GetSearchKey;
	using YsShellExt::MoveToNextFaceGroup;
	using YsShellExt::GetFaceGroup;
	using YsShellExt::GetFaceGroupRaw;
	using YsShellExt::GetFaceGroupVertex;
	using YsShellExt::GetFaceGroupAttrib;
	using YsShellExt::GetFaceGroupIdent;
	using YsShellExt::GetFaceGroupLabel;
	using YsShellExt::GetNumEdge;
	using YsShellExt::GetNumPolygonUsingEdge;
	using YsShellExt::GetNumPolygonUsingVertex;
	using YsShellExt::GetEdge;
	using YsShellExt::MoveToNextEdge;
	using YsShellExt::CacheVertexNormal;
	using YsShellExt::FindVertex;
	using YsShellExt::FindPolygon;
	using YsShellExt::FindPolygonFromVertexSequence;
	using YsShellExt::FindPolygonFromVertex;
	using YsShellExt::FindPolygonFromEdgePiece;
	using YsShellExt::FindConstEdge;
	using YsShellExt::FindConstEdgeFromVertex;
	using YsShellExt::FindConstEdgeFromEdgePiece;
	using YsShellExt::FindConstEdgeFromFaceGroup;
	using YsShellExt::IsOnCreaseConstEdge;
	using YsShellExt::GetConstEdgeIsLoop;
	using YsShellExt::IsCreaseConstEdge;
	using YsShellExt::FindConstEdgeFromPolygon;
	using YsShellExt::GetNumConstEdgeUsingVertex;
	using YsShellExt::GetNumFaceGroupUsingVertex;
	using YsShellExt::IsEdgePieceConstrained;
	using YsShellExt::IsConstEdgeUsingEdgePiece;
	using YsShellExt::GetNumConstEdgeUsingEdgePiece;
	using YsShellExt::FindFaceGroup;
	using YsShellExt::FindFaceGroupFromVertex;
	using YsShellExt::FindFaceGroupFromPolygon;
	using YsShellExt::FindFaceGroupFromEdgePiece;
	using YsShellExt::FindFaceGroupFromConstEdge;
	using YsShellExt::GetEdgeConnectedNeighborFaceGroup;
	using YsShellExt::GetVertexConnectedNeighborFaceGroup;
	using YsShellExt::GetConstEdgeConnectedNeighborFaceGroup;
	using YsShellExt::GetDrawingBuffer;
	using YsShellExt::GetConnectedVertex;
	using YsShellExt::GetNumConstEdgeConnectedVertex;
	using YsShellExt::GetConstEdgeConnectedVertex;
	using YsShellExt::GetNeighborPolygon;
	using YsShellExt::IsVertexUsed;
	using YsShellExt::IsVertexUsedByConstEdge;
	using YsShellExt::IsVertexUsedByNonManifoldEdge;
	using YsShellExt::AllConstEdge;
	using YsShellExt::AllFaceGroup;
	using YsShellExt::AllVolume;
	using YsShellExt::TemporaryModification;
	using YsShellExt::OverrideVertexSearchKey;
	using YsShellExt::OverridePolygonSearchKey;
	using YsShellExt::OverrideConstEdgeSearchKey;
	using YsShellExt::OverrideFaceGroupSearchKey;
	using YsShellExt::OverrideVolumeSearchKey;
	using YsShellExt::GetFaceGroupInsideVertex;
	using YsShell::GetLength;
	using YsShell::MetaDataHandle;
	using YsShell::GetNumMetaData;
	using YsShell::FindMetaData;
	using YsShell::GetMetaDataKey;
	using YsShell::GetMetaDataValue;
	using YsShell::GetMetaData;
	using YsShell::MetaDataEnumerator;
	using YsShell::AllMetaData;

public:
	/*! Default constructor */
	YsShellExtEdit();

	/*! Copy constructor */
	YsShellExtEdit(const YsShellExtEdit &shl);

	/*! Copy assignment operator */
	YsShellExtEdit &operator=(const YsShellExtEdit &shl);

	/*! Move constructor */
	YsShellExtEdit(YsShellExtEdit &&shl);

	/*! Move assignment operator */
	YsShellExtEdit &operator=(YsShellExtEdit &&shl);

	/*! Copy constructor */
	YsShellExtEdit(const YsShellExt &shl);

	/*! Copy assignment operator */
	YsShellExtEdit &operator=(const YsShellExt &shl);

	/*! Move constructor */
	YsShellExtEdit(YsShellExt &&shl);

	/*! Move assignment operator */
	YsShellExtEdit &operator=(YsShellExt &&shl);

private:
	void CommonInitializer(void);

public:
	/*! Default destructor */
	~YsShellExtEdit();

	class Converter
	{
	public:
		YsShellExtEdit *shl;
		operator const YsShell &() const
		{
			return *(const YsShell *)shl;
		}
		operator const YsShellExt &() const
		{
			return *(const YsShellExt *)shl;
		}
		operator YsShellExtEdit &() const
		{
			return *shl;
		}
	};
	class ConstConverter
	{
	public:
		const YsShellExtEdit *shl;
		operator const YsShell &() const
		{
			return *(const YsShell *)shl;
		}
		operator const YsShellExt &() const
		{
			return *(const YsShellExt *)shl;
		}
		operator const YsShellExtEdit &() const
		{
			return *shl;
		}
	};

	/*! C++ apparently doesn't easily allow only constant access to the base class and ban non-constant access.
	    So, when conversion to const YsShell & or const YsShellExt & is needed, use Conv function, that returns 
	    ConstConverter class or Converter class, which will be automatically converted to const YsShell & or
	    const YsShellExt &. */
	inline Converter Conv(void);
	inline ConstConverter Conv(void) const;

	/*! This function returns YSTRUE if the model is empty, and has never been edited. */
	YSBOOL IsEmptyAndHasNeverBeenEdited(void) const;

	/*! Deletes all entities including the edit log.  Therefore no undo/redo is possible after this function. */
	void CleanUp(void);

	/*! Deletes all entities excluding the edit log.  Can undo back to the pre-DeleteAll state if 
	    takeEditLog is set YSTRUE. */
	void DeleteAll(void);

	/*! This function loads .SRF data. */
	YSRESULT LoadSrf(YsTextInputStream &inStream);

	/*! This function loads .SRF file. */
	YSRESULT LoadSrf(const char fn[]);

	/*! This function merges .SRF data into the shell. */
	YSRESULT MergeSrf(YsTextInputStream &inStream);

	/*! This function loads .STL data. */
	YSRESULT LoadStl(const char fn[]);

	/*! This function loads Wavefront .OBJ data. */
	YSRESULT LoadObj(const char fn[]);

	/*! This function loads Wavefront .OBJ data. */
	YSRESULT LoadObj(YsTextInputStream &inStream);

	/*! This function loads OpenMESH OFF data. */
	YSRESULT LoadOff(const char fn[]);

	/*! This function loads OpenMESH .OFF data. */
	YSRESULT LoadOff(YsTextInputStream &inStream);

	/*! Opens a .DXF file. */
	YSRESULT LoadDxf(const char fn[]);

	/*! Opens a .DXF file. */
	YSRESULT LoadDxf(YsTextInputStream &inStream);

	/*! This function returns YSTRUE if the shell has been modified since it has been loaded, saved, or created, or YSFALSE otherwise. */
	YSBOOL IsModified(void) const;

	/*! Call this function after the model has been saved. 
	    This function records undo pointer and undo counter so that it can tell if the model has been modified since being saved last time.  */
	void Saved(void) const;

	/*! This function returns a save point.  A save point can be used to check if the model has been modified.
	    Use CompareSavePoint function to check if the model has been modified since the SavePoint is acquired by this function. */
	YsShellExtEdit::SavePoint GetSavePoint(void) const;

	/*! This function returns YSTRUE if the model has been modified since the given SavePoint. */
	YSBOOL IsModified(const YsShellExtEdit::SavePoint &savePoint) const;

	/*! This function sets the file name of this shell. */
	void SetFileName(const wchar_t fn[]) const;

	/*! This function sets the file name of this shell. */
	void SetFileName(const char fn[]) const;

	/*! This function returns a pointer to the file name of this shell. */
	const wchar_t *GetFileName(void) const;

	/*! This function copies the file name of this shell in fn and returns the pointer to the file name. */
	const wchar_t *GetFileName(YsWString &fn) const;

	/*! This function copies the file nameof this shell in fn in UTF8 format  and returns the pointer to the file name. */
	const char *GetFileName(YsString &fn) const;

	/*! This function cancels previous operation.  If no more cancellation is possible, it returns YSERR. */
	YSRESULT Undo(void);

	/*! This function cancels previous operation.  It only cancels only one step, ignoring grouped operations. */
	void UndoOneStep(void);
private:
	void SkipUndoOneStep(void);

public:
	/*! This function executes previously-cancelled operation.  If no more redo is possible, it returns YSERR. */
	YSRESULT Redo(void);

	/*! This function executes previously-cancelled operation.  It only redoes one step, ignoring grouped operations. */
	void RedoOneStep(void);
private:
	void SkipRedoOneStep(void);

public:
	/*! This function stops incrmenting undo counter, so that subsequent operations are in a group.  
	    The return value should be given to the PopIncUndo function to resume undo-counter incrementation. */
	int PushStopIncUndo(void);

	/*! This function sets undo-counter incrementation.  The value must be the return value of previous PushStopIncUndo function. */
	void PopIncUndo(int incUndo);

public:
	/*! This function returns the current undo pointer.  This pointer can be given to UndoUntil function to revert the model
	    back to the time when the current undo pointer is saved. */
	const EditLog *GetUndoPointer(void) const;

	/*! This function reverts the state of the model to the state when undo pointer was saved. 
	    This function returns YSOK if successful. 
	    If undoPtr is no longer in the undo chain, this function will fail and return YSERR. */
	YSRESULT UndoUntil(const EditLog *undoPtr);

	/*! This function disables logging modifications.  If logging is disabled, YsShellExtEdit behaves almost same as YsShellExt. 
	    All logging information is deleted. */
	void DisableEditLog(void);

	/*! This function enables logging modifications. */
	void EnableEditLog(void);

	/*! Add a note in the undo log. */
	void AddUndoMessage(const YsString &str);

public:
	/*! This function adds a vertex and returns a handle to the new vertex. */
	YsShellVertexHandle AddVertex(const YsVec3 &pos);

	/*! This function modifies vertex attributes. */
	YSRESULT SetVertexAttrib(YsShellVertexHandle vtHd,const VertexAttrib &incoming);

	/*! This function adds multiple vertices and returns handles to the new vertex. */
	YSRESULT AddMultiVertex(YsArray <YsShellVertexHandle> &vtHdArray,const YSSIZE_T nVt,const YsVec3 pos[]);

	/*! This function adds multiple vertices and returns handles to the new vertex. */
	template <const int N>
	YSRESULT AddMultiVertex(YsArray <YsShellVertexHandle,N> &vtHdArray,const YSSIZE_T nVt,const YsVec3 pos[]);

	/*! This function adds multiple vertices and returns handles to the new vertex. */
	template <const int N>
	YSRESULT AddMultiVertex(YsArray <YsShellVertexHandle,N> &vtHdArray,const YsConstArrayMask <YsVec3> &posArray);

	/*! This function modifies position of vertex vtHd. */
	YSRESULT SetVertexPosition(YsShellVertexHandle vtHd,const YsVec3 &newPos);

	/*! This function modifies positions of vertices. */
	YSRESULT SetMultiVertexPosition(YSSIZE_T nVtx,const YsShellVertexHandle vtHdArray[],const YsVec3 vtxPosArray[]);

	/*! This function modifies positions of vertices. */
	YSRESULT SetMultiVertexPosition(const YsConstArrayMask <YsShellVertexHandle> &vtHdArray,const YsVec3 vtxPosArray[]);

	/*! This function sets ROUND flag of the vertex. */
	YSRESULT SetVertexRoundFlag(YsShellVertexHandle vtHd,YSBOOL round);

	/*! This function sets CONSTRAINED flag of the vertex. */
	YSRESULT SetVertexConstrainedFlag(YsShellVertexHandle vtHd,YSBOOL constFlag);

	/*! This function deletes a vertex.  If the vertex is used by a polygon or a constraint edge (even one is tentatively deleted), this function fails and returns YSERR. */
	YSRESULT DeleteVertex(YsShellVertexHandle vtHd);

	/*! This function deletes multiple vertices.  If a vertex is used by a polygon or a constraint edge (even one is tentatively deleted), this function fails and returns YSERR. */
	YSRESULT DeleteMultiVertex(const YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);

	/*! This function deletes multiple vertices.  If a vertex is used by a polygon or a constraint edge (even one is tentatively deleted), this function fails and returns YSERR. */
	YSRESULT DeleteMultiVertex(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray);

	/*! This function sets vertex-tracking information to another shell vertex. */
	YSRESULT SetVertexTrackingToVertex(YsShellVertexHandle vtHd,int oVtId);

	/*! This function sets vertex-tracking information to another shell edge. */
	YSRESULT SetVertexTrackingToEdge(YsShellVertexHandle vtHd,int oEdVtId0,int oEdVtId1);

	/*! This function selects vertices. */
	YSRESULT SelectVertex(const YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);

	/*! This function selects vertices. */
	YSRESULT SelectVertex(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray);

	/*! This function adds vertex selection. */
	YSRESULT AddSelectedVertex(const YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);

	/*! This function adds vertex selection. */
	YSRESULT AddSelectedVertex(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray);

	/*! This function returns selected vertices. */
	template <const int N>
	YSRESULT GetSelectedVertex(YsArray <YsShellVertexHandle,N> &vtHdArray) const;

	/*! This function returns selected vertices. */
	YsArray <YsShellVertexHandle> GetSelectedVertex(void) const;

	/*! This function selects polygons. */
	YSRESULT SelectPolygon(const YSSIZE_T nPl,const YsShellPolygonHandle plHd[]);

	/*! This function selects polygons. */
	YSRESULT SelectPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdstore);

	/*! This function selects polygons. */
	template <class T>
	YSRESULT SelectPolygon(const T &plHdstore);

	/*! This function adds polygon selection. */
	YSRESULT AddSelectedPolygon(const YSSIZE_T nPl,const YsShellPolygonHandle plHd[]);

	/*! This function adds polygon selection. */
	YSRESULT AddSelectedPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray);

	/*! This function returns selected polygons. */
	template <const int N>
	YSRESULT GetSelectedPolygon(YsArray <YsShellPolygonHandle,N> &plHdArray) const;

	/*! This function returns selected polygons. */
	YsArray <YsShellPolygonHandle> GetSelectedPolygon(void) const;

	/*! This function selects constraint edges. */
	YSRESULT SelectConstEdge(const YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[]);

	/*! This function selects constraint edges. */
	YSRESULT SelectConstEdge(const YsConstArrayMask <YsShellExt::ConstEdgeHandle> &ceHdArray);

	/*! This function selects constraint edges. */
	template <class T>
	YSRESULT SelectConstEdge(const T &ce);

	/*! This function adds constraint-edge selection. */
	YSRESULT AddSelectedConstEdge(const YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[]);

	/*! This function adds constraint-edge selection. */
	YSRESULT AddSelectedConstEdge(const YsConstArrayMask <YsShellExt::ConstEdgeHandle> &ceHdArray);

	/*! This function returns selected constraint-edges. */
	template <const int N>
	YSRESULT GetSelectedConstEdge(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray) const;

	/*! This function returns selected constraint-edges. */
	YsArray <YsShellExt::ConstEdgeHandle> GetSelectedConstEdge(void) const;

	/*! This function selects face groups. */
	YSRESULT SelectFaceGroup(const YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHd[]);

	/*! This function selects face groups. */
	YSRESULT SelectFaceGroup(const YsConstArrayMask <YsShellExt::FaceGroupHandle> &fgHdArray);

	/*! This function selects face groups. */
	template <class T>
	YSRESULT SelectFaceGroup(const T &fgHdStore);

	/*! This function adds face-group selection. */
	YSRESULT AddSelectedFaceGroup(const YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHd[]);

	/*! This function adds face-group selection. */
	YSRESULT AddSelectedFaceGroup(const YsConstArrayMask <YsShellExt::FaceGroupHandle> &fgHdArray);

	/*! This function returns selected face groups. */
	template <const int N>
	YSRESULT GetSelectedFaceGroup(YsArray <YsShellExt::FaceGroupHandle,N> &fgHdArray) const;

	/*! This function returns selected face groups. */
	YsArray <YsShellExt::FaceGroupHandle> GetSelectedFaceGroup(void) const;

	/*! This function sets vertex-tracking information to another shell polygon. */
	YSRESULT SetVertexTrackingToPolygon(YsShellVertexHandle vtHd,int oPlId);

	/*! This function adds a polygon and returns a handle to the new polygon. */
	YsShellPolygonHandle AddPolygon(YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[]);

	/*! This function adds a polygon and returns a handle to the new polygon. */
	YsShell::PolygonHandle AddPolygon(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray);

	/*! This function modifies vertex of the polygon plHd. */
	YSRESULT SetPolygonVertex(YsShellPolygonHandle plHd,YSSIZE_T nPlVt,const YsShellVertexHandle plVtHd[]);

	/*! This function modifies vertex of the polygon plHd. */
	YSRESULT SetPolygonVertex(YsShell::PolygonHandle plHd,const YsConstArrayMask <YsShell::VertexHandle> &plVtHd);

	/*! This function sets color of the polygon. */
	YSRESULT SetPolygonColor(YsShellPolygonHandle plHd,const YsColor &col);

	/*! This function sets alpha (solidness) value of the polygon. */
	YSRESULT SetPolygonAlpha(YsShellPolygonHandle plHd,const double alpha);

	/*! This function deletes a polygon.  If the polygon is used by an entity, this function fails and returns YSERR. */
	YSRESULT DeletePolygon(YsShellPolygonHandle plHd);

	/*! This function delets multiple polygons.  Polygons used by an entity will not be deleted.  This function returns YSOK if all polygons are deleted, YSERR otherwise. */
	YSRESULT DeleteMultiPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! This function delets multiple polygons.  Polygons used by an entity will not be deleted.  This function returns YSOK if all polygons are deleted, YSERR otherwise. */
	YSRESULT DeleteMultiPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray);

	/*! This function deletes a polygon.  If the polygon is used by a face group, this function also deletes the face group. */
	YSRESULT ForceDeletePolygon(YsShellPolygonHandle plHd);

	/*! This function deletes multiple polygons.  If the polygon is used by a face group, this function also deletes the face group. */
	YSRESULT ForceDeleteMultiPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! This function deletes multiple polygons.  If the polygon is used by a face group, this function also deletes the face group. */
	YSRESULT ForceDeleteMultiPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray);

	/*! This function sets desired element size to the polygon. */
	YSRESULT SetPolygonDesiredElementSize(YsShellPolygonHandle plHd,const double elemSize);

	/*! This function sets no-shading flag of the polygon. */
	YSRESULT SetPolygonNoShadingFlag(YsShellPolygonHandle plHd,YSBOOL noShading);

	/*! This function sets as-light flag of the polygon. */
	YSRESULT SetPolygonAsLightFlag(YsShellPolygonHandle plHd,YSBOOL noShading);

	/*! This function sets no-shading flag of the polygon. */
	YSRESULT SetPolygonTwoSidedFlag(YsShellPolygonHandle plHd,YSBOOL twoSided);

	/*! This function sets polygon attributes excluding refrence count, color, and normal. */
	YSRESULT SetPolygonAttrib(YsShellPolygonHandle plHd,const PolygonAttrib &inAttrib);

	/*! This function sets normal vector of the polygon. */
	YSRESULT SetPolygonNormal(YsShellPolygonHandle plHd,const YsVec3 &nom);

	/*! This function sets normal vector of the multiple polygons. */
	YSRESULT SetPolygonNormalMulti(const YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[],const YsVec3 nomArray[]);

	/*! This function sets normal vector of the multiple polygons. */
	YSRESULT SetPolygonNormalMulti(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray,const YsVec3 nomArray[]);

	/*! This function adds a constraint edge.  If nVt==0 and ceVtHd==NULL, this function creates an empty constraint edge that can later be populated. */
	ConstEdgeHandle AddConstEdge(YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[],YSBOOL isLoop);

	/*! This function adds a constraint edge. */
	ConstEdgeHandle AddConstEdge(const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,YSBOOL isLoop);

	/*! This function sets isLoop flag of the constraint edge. */
	YSRESULT SetConstEdgeIsLoop(ConstEdgeHandle ceHd,YSBOOL isLoop);

	YSRESULT SetConstEdgeIsCrease(ConstEdgeHandle ceHd,YSBOOL isCrease);

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

	/*! This function sets an identification number on the const edge. */
	YSRESULT SetConstEdgeIdent(ConstEdgeHandle ceHd,YSSIZE_T ident);

	/*! This function delets a constraint edge. */
	YSRESULT DeleteConstEdge(ConstEdgeHandle ceHd);

	/*! This function delets multiples constraint edges. */
	YSRESULT DeleteMultiConstEdge(YSSIZE_T nCe,const ConstEdgeHandle ceHdArray[]);

	/*! This function deletes multiples constraint edges. */
	YSRESULT DeleteMultiConstEdge(const YsConstArrayMask <ConstEdgeHandle> &ceHdArray);

	/*! This function copies incoming const-edge attributes to the const edge ceHd. */
	YSRESULT SetConstEdgeAttrib(YsShellExt::ConstEdgeHandle ceHd,const YsShellExt::ConstEdgeAttrib &attrib);

	/*! This function adds a face group.  If nPl==0 and fgPlHd==NULL, this function creates an empty face group that can later be populated. */
	FaceGroupHandle AddFaceGroup(YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[]);

	/*! This function adds a face group. */
	FaceGroupHandle AddFaceGroup(const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd);

	/*! This function modifies a face group by replacing a list of polygons. */
	YSRESULT ModifyFaceGroup(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[]);

	/*! This function modifies a face group by replacing a list of polygons. */
	YSRESULT ModifyFaceGroup(FaceGroupHandle fgHd,const YsConstArrayMask <YsShell::PolygonHandle> &fgPlHd);

	/*! This function adds polygons to a face group. */
	YSRESULT AddFaceGroupPolygon(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[]);

	/*! This function adds polygons to a face group. */
	YSRESULT AddFaceGroupPolygon(FaceGroupHandle fgHd,const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd);

	/*! This function removes the polygon from the face group that it belongs to, and returns the handle to the 
	    face group from which the polygon was removed.  This function returns NULL if plHd does not belong to any face group. */
	FaceGroupHandle DeleteFaceGroupPolygon(YsShellPolygonHandle plHd);

	/*! This function removes the polygon from the face group that it belongs to, and returns the handle to the 
	    face group from which the polygon was removed.  This function returns NULL if plHd does not belong to any face group. */
	YSRESULT DeleteFaceGroupPolygonMulti(YSSIZE_T nPl,const YsShellPolygonHandle plHd[]);

	/*! This function removes the polygon from the face group that it belongs to, and returns the handle to the 
	    face group from which the polygon was removed.  This function returns NULL if plHd does not belong to any face group. */
	YSRESULT DeleteFaceGroupPolygonMulti(const YsConstArrayMask <YsShellPolygonHandle> &plHdArray);

	/*! This function copies the incoming face group attributes to the face group fgHd. */
	YSRESULT SetFaceGroupAttrib(YsShellExt::FaceGroupHandle fgHd,const YsShellExt::FaceGroupAttrib &attrib);

	/*! This function sets INTERIOR_CONST_SURFACE flat to a face group.  
	    Although it is said "INTERIOR", the face group can be anywhere.  
	    The polygons that belong to this face group will not be considered as a volume boundary. */
	YSRESULT SetFaceGroupInteriorConstSurfaceFlag(FaceGroupHandle fgHd,YSBOOL isInteriorConst);

	/*! This function sets a label of the face group. */
	YSRESULT SetFaceGroupLabel(FaceGroupHandle fgHd,const char label[]);

	/*! This function returns YSTRUE if the face group is an INTERIOR_CONST_SURFACE, YSFALSE otherwise. */
	YSBOOL IsInteriorConstSurface(FaceGroupHandle fgHd) const;

	/*! This function permanently deletes a face group.  This function fails and returns YSERR if face group is used by a volume even if it is tentatively deleted. */
	YSRESULT DeleteFaceGroup(FaceGroupHandle fgHd);

	/*! This function moves volume handle vlHd so that it points to the next volume.  If the incoming vlHd is NULL, vlHd wil be set to the first volume.  The function returns YSOK if successful.  If there is no more next volume, vlHd will be NULL, and the function will return YSERR. */
	YSRESULT MoveToNextVolume(VolumeHandle &vlHd) const;

	/*! This function adds a volume defined by multiple face groups.  If nVlFg==0 && vlFgHd==NULL, this function creates an empty volume that can later be populated.. */
	VolumeHandle AddVolume(YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[]);

	/*! This function adds a volume defined by multiple face groups.*/
	VolumeHandle AddVolume(const YsConstArrayMask <FaceGroupHandle> &vlFgHd);

	/*! This function modifies polygons included in the volume. */
	YSRESULT ModifyVolume(VolumeHandle vlHd,YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[]);

	/*! This function modifies polygons included in the volume. */
	YSRESULT ModifyVolume(VolumeHandle vlHd,const YsConstArrayMask <FaceGroupHandle> &vlFgHd);

	/*! This function adds face groups to a volume. */
	YSRESULT AddVolumeFaceGroup(VolumeHandle vlHd,YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[]);

	/*! This function adds face groups to a volume. */
	YSRESULT AddVolumeFaceGroup(VolumeHandle vlHd,const YsConstArrayMask <FaceGroupHandle> &vlFgHd);

	/*! This function permanently deletes a volume. */
	YSRESULT DeleteVolume(VolumeHandle vlHd);

	/*! This function sets a label of the volume. */
	YSRESULT SetVolumeLabel(VolumeHandle vlHd,const char label[]);


	/*! This function adds a meta data. */
	MetaDataHandle AddMetaData(const char key[],const char value[]);

	/*! This function updates a value associated with the meta data. */
	YSRESULT SetMetaDataValue(MetaDataHandle mdHd,const char value[]);

	/*! This function delets a meta data. */
	YSRESULT DeleteMetaData(MetaDataHandle mdHd);
};

template <class UndoType>
inline bool operator==(std::nullptr_t,const YsShellExtEdit::NewUndoLog<UndoType> &undo)
{
	return undo.IsNull();
}
template <class UndoType>
inline bool operator!=(std::nullptr_t,const YsShellExtEdit::NewUndoLog<UndoType> &undo)
{
	return undo.IsNotNull();
}

inline YsShellExtEdit::Converter YsShellExtEdit::Conv(void)
{
	Converter conv;
	conv.shl=this;
	return conv;
}

inline YsShellExtEdit::ConstConverter YsShellExtEdit::Conv(void) const
{
	ConstConverter conv;
	conv.shl=this;
	return conv;
}

template <const int N>
YSRESULT YsShellExtEdit::AddMultiVertex(YsArray <YsShellVertexHandle,N> &vtHdArray,const YSSIZE_T nVt,const YsVec3 pos[])
{
	YsArray <YsShellVertexHandle> vtHdArrayTemp;
	YSRESULT res=AddMultiVertex(vtHdArrayTemp,nVt,pos);
	vtHdArray=vtHdArrayTemp;
	return res;
}

template <const int N>
YSRESULT YsShellExtEdit::AddMultiVertex(YsArray <YsShellVertexHandle,N> &vtHdArray,const YsConstArrayMask <YsVec3> &posArray)
{
	return AddMultiVertex <N> (vtHdArray,posArray.GetN(),posArray);
}

template <const int N>
YSRESULT YsShellExtEdit::GetSelectedVertex(YsArray <YsShellVertexHandle,N> &vtHdArray) const
{
	if(NULL!=shlSearch)
	{
		vtHdArray.Set(selectedVtKey.GetN(),NULL);

		YSSIZE_T n=0;
		for(YSSIZE_T idx=0; idx<selectedVtKey.GetN(); ++idx)
		{
			vtHdArray[n]=shlSearch->FindVertex(*this,selectedVtKey[idx]);
			if(NULL!=vtHdArray[n])
			{
				++n;
			}
		}

		vtHdArray.Resize(n);

		return YSOK;
	}
	else
	{
		YsPrintf("%s %d\n",__FUNCTION__,__LINE__);
		YsPrintf("Search Table must be enabled for this function.\n");
		vtHdArray.CleanUp();
		return YSERR;
	}
}

template <class T>
YSRESULT YsShellExtEdit::SelectPolygon(const T &plHdStore)
{
	YsArray <YsShellPolygonHandle> plHdArray(plHdStore);
	return SelectPolygon(plHdArray.GetN(),plHdArray);
}

template <const int N>
YSRESULT YsShellExtEdit::GetSelectedPolygon(YsArray <YsShellPolygonHandle,N> &plHdArray) const
{
	if(NULL!=shlSearch)
	{
		plHdArray.Set(selectedPlKey.GetN(),NULL);

		YSSIZE_T n=0;
		for(YSSIZE_T idx=0; idx<selectedPlKey.GetN(); ++idx)
		{
			plHdArray[n]=shlSearch->FindPolygon(*this,selectedPlKey[idx]);
			if(NULL!=plHdArray[n])
			{
				++n;
			}
		}

		plHdArray.Resize(n);

		return YSOK;
	}
	else
	{
		YsPrintf("%s %d\n",__FUNCTION__,__LINE__);
		YsPrintf("Search Table must be enabled for this function.\n");
		plHdArray.CleanUp();
		return YSERR;
	}
}

template <class T>
YSRESULT YsShellExtEdit::SelectConstEdge(const T &ce)
{
	YsArray <YsShellExt::ConstEdgeHandle> ceHdArray;
	for(auto ceHd : ce)
	{
		ceHdArray.Add(ceHd);
	}
	return SelectConstEdge(ceHdArray.GetN(),ceHdArray);
}

template <const int N>
YSRESULT YsShellExtEdit::GetSelectedConstEdge(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray) const
{
	ceHdArray.CleanUp();
	for(YSSIZE_T idx=0; idx<selectedCeKey.GetN(); ++idx)
	{
		YsShellExt::ConstEdgeHandle ceHd=FindConstEdge(selectedCeKey[idx]);
		if(NULL!=ceHd)
		{
			ceHdArray.Append(ceHd);
		}
	}
	return YSOK;
}

template <class T>
YSRESULT YsShellExtEdit::SelectFaceGroup(const T &fgHdStore)
{
	YsArray <FaceGroupHandle> fgHdArray;
	for(auto hd : fgHdStore)
	{
		fgHdArray.Add(hd);
	}
	return SelectFaceGroup(fgHdArray.GetN(),fgHdArray);
}

template <const int N>
YSRESULT YsShellExtEdit::GetSelectedFaceGroup(YsArray <YsShellExt::FaceGroupHandle,N> &fgHdArray) const
{
	fgHdArray.CleanUp();
	for(YSSIZE_T idx=0; idx<selectedFgKey.GetN(); ++idx)
	{
		YsShellExt::FaceGroupHandle fgHd=FindFaceGroup(selectedFgKey[idx]);
		if(NULL!=fgHd)
		{
			fgHdArray.Append(fgHd);
		}
	}
	return YSERR;
}


////////////////////////////////////////////////////////////

/*! This class helps grouping operations of YsShellExtEdit.  The constructor of this class will call 
    YsShellExtEdit::PushStopIncUndo and stops the undo-counter incrementation, and the destructor will 
    call YsShellExtEdit::PopIncUndo to resume the undo-counter incrementation.  By adding:

        YsShellExtEdit::StopIncUndo incUndo(shl);

    where shl is a non-const pointer or reference to an YsShellExtEdit object, the operations performed in 
    the same scope will be grouped as a single sequence of operations. */
class YsShellExtEdit::StopIncUndo
{
public:
	int incUndo;
	YsShellExtEdit *shl;

	StopIncUndo(YsShellExtEdit *shl);
	StopIncUndo(YsShellExtEdit &shl);
	~StopIncUndo();
};

/* } */
#endif
