#ifndef RETROMAP_DATA_IS_INCLUDED
#define RETROMAP_DATA_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ystexturemanager.h>
#include <ysglcpp.h>
#include <ysglbuffer.h>

#ifdef LoadImage
	// Win32 s**t
	#undef LoadImage
#endif

class RetroMap_World
{
public:
	class Field;
	class ClipBoard;
	typedef YsEditArrayObjectHandle <Field,4> FieldHandle;

	class UndoLog
	{
	public:
		UndoLog *next,*prev;
		int undoCtr;
		UndoLog();
		virtual ~UndoLog();
		virtual void Undo(RetroMap_World &world)=0;
		virtual void Redo(RetroMap_World &world)=0;
		virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const=0;
	};
	template <class UndoLogClass>
	UndoLogClass *NewUndoLog(void);

	class UndoGuard
	{
	public:
		int prevIncUndo;
		RetroMap_World *worldPtr;

		UndoGuard(RetroMap_World &world);
		~UndoGuard();
	};
	friend class UndoGuard;

private:
	int undoCtr,undoInc;
	UndoLog *undoPtr,*redoPtr;
	mutable UndoLog *lastSavedUndoPtr;

public:
	class MapPiece
	{
	friend class RetroMap_World;
	friend class ClipBoard;
	friend class Field;

	private:
		MapPiece(const MapPiece &);

		void Swap(MapPiece &incoming);
		void MoveFrom(MapPiece &incoming);
		void CopyFrom(const MapPiece &incoming);

	public:
		class Shape
		{
		public:
			YsVec2i pos;
			YsRect2i visibleArea;
			YsVec2 scaling;

			/*! Set pos and scaling and call this function to calculate visibleArea based on world XY.
			*/
			YsRect2i VisibleAreaFromWorldXY(const YsVec2 &min,const YsVec2 &max) const;

			/*! Set pos and visibleArea and call this function to calculate scaling based on world XY.
			*/
			YsVec2 ScalingFromWorldXY(const YsVec2 &min,const YsVec2 &max) const;

			/*! Set visibleArea and scaling and call this function to calculate position. */
			YsVec2i PositionFromWorldXY(const YsVec2 &min) const;

			/*! Returns the bounding box in the world coordinate. */
			YsRect3 GetBoundingBox(void) const;

			/*! Returns the bounding box in the world coordinate in integer. */
			YsRect2i GetBoundingBox2i(void) const;

			/*! Applies the unit. (Adjust position to snap to the grid.) */
			void ApplyUnit(YsVec2i unit);
		};

	private:
		// (!)When adding a data member, make sure to update MoveFrom and CopyFrom
		// When moving or copying, make sure to update the ownership.
		Field *owner;
		YsTextureManager::TexHandle texHd;
		YsTextureManager::TexHandle diffTexHd;
		Shape shape;
		mutable Shape temporaryShape;

		// Cache for rendering >>
		mutable YSBOOL usingTentativePosition;

		mutable YsVec3 quadVtPos[4];
		mutable YsVec2 quadTexCoord[4];

		mutable YsGLColorBuffer colBuf;
		mutable YsGLVertexBuffer vtxBuf;
		mutable YsGLVertexBuffer2D texCoordBuf;
		// Cache for rendering <<

		const YsTextureManager &GetTexMan(void) const;
		YsTextureManager &GetTexMan(void);

		void Render(void) const;
		void RenderFrame(YsColor col) const;
		void RenderOverlap(MapPiece::Shape shape,YsTextureManager::TexHandle texHd,YsColor col) const;

	public:
		// (!)When adding a data member, make sure to update MoveFrom.
		/*! While loading, texLabelCache remembers the label name of a texture so that the texHd can be
		    connected after the file is loaded.
		*/
		YsString texLabelCache;

		/*! When the map changes after an event, you can control the visualization by the event level.
		    Default state of the map must be eventLevel==0.
		*/
		int eventLevel;

		/*! Solidness (1.0-transparency).
		*/
		double alpha;

		/*! Returns the width in pixels.
		*/
		int GetWidth(void) const;

		/*! Returns the height in pixels.
		*/
		int GetHeight(void) const;

		/*! Returns the position of the bottom-left corner. */
		YsVec2i GetPosition(void) const;

		/*! Returns the dimension of visible part in pixels.
		*/
		YsVec2i GetVisibleSize(void) const;

		/*! Returns the visible area in pixels in the bitmap's coordinate system. */
		YsRect2i GetVisibleArea(void) const;

		/*! Returns the dimension of the bitmap in pixels.
		*/
		YsVec2i GetBitmapSize(void) const;

		/*! Returns the bounding box of the visible part of the bitmap in the field coordinate. */
		YsRect3 GetBoundingBox(void) const;

		/*! Returns the bounding box of the visible part of the bitmap in the field coordinate in integer. */
		YsRect2i GetBoundingBox2i(void) const;

		/*! Returns the shape information. */
		Shape GetShape(void) const;

		/*! Returns the temporary shape that was used in the last rendering. */
		Shape GetTemporaryShape(void) const;

		/*! Returns the scaled bitmap of the visible area. */
		YsBitmap GetBitmap(void) const;

		MapPiece();
		MapPiece &operator=(const MapPiece &);
		~MapPiece();
		void Initialize(void);
		void CleanUp(void);

		/*! Set diff texture.
		*/
		void SetDiffTexture(const YsBitmap &diffBmp);

		/*! Make texture ready. */
		YSRESULT ReadyTexture(void) const;

		/*! Make vertex array buffer. */
		YSRESULT ReadyVbo(void) const;

		/*! Make texture ready if the texture is not ready. */
		YSRESULT ReadyTextureIfNotReady(void) const;
		
		/*! Make vertex array buffer if it is not ready. */
		YSRESULT ReadyVboIfNotReady(void) const;

		/*! Make vertex array buffer with tweaked shape. */
		YSRESULT ReadyVbo(const Shape &shape,const double alpha) const;
	};
	typedef YsEditArrayObjectHandle <MapPiece,4> MapPieceHandle;
	typedef YsEditArray<MapPiece,4>::HandleStore MapPieceStore;


	class MarkUp
	{
	friend class RetroMap_World;
	friend class ClipBoard;
	friend class Field;

	public:
		enum MARKUPTYPE
		{
			// !!!! When adding a new drawing mark up type, make sure to update IsDrawing function as well.
			MARKUP_STRING,
			MARKUP_POINT_ARRAY,
			// !!!! When adding a new drawing mark up type, make sure to update IsDrawing function as well.
		};
		enum ARROWHEADTYPE
		{
			ARROWHEAD_NONE,
			ARROWHEAD_TRIANGLE,
		};

		class ArrowHeadInfo
		{
		public:
			static inline double DefaultSize(void)
			{
				return 24.0;
			}

			ARROWHEADTYPE arrowHeadType;
			double arrowHeadSize;

			ArrowHeadInfo()
			{
				Initialize();
			}
			void Initialize(void)
			{
				arrowHeadType=ARROWHEAD_NONE,
				arrowHeadSize=DefaultSize();
			}
			void Write(YsTextOutputStream &outStream,const char typeStr[]) const;
			void Read(const YsConstArrayMask <YsString> &argv);

			YsArray <YsVec2,4> GetPolygon(YsVec2i from,YsVec2i to) const;
			void MakeVertexArray(class YsGLVertexBuffer &vtxBuf,YsVec2i from,YsVec2i to) const;
		};

	private:
		// When moving or copying, make sure to update the ownership.
		Field *owner;

		YsWString markUp;
		YsTextureManager::TexHandle texHd; // It is a cache.  Make sure to set DONTSAVE flag.

	private:
		// Common Properties >>
		MARKUPTYPE markUpType;
		YsColor fgCol,bgCol;
		YsVec2i pos;
		// Common Properties <<

		// String >>
		int fontSize;
		YsWString txt;
		YsVec2i txtSizeCache;
		// String <<

		// Multiple points >>
		YsArray <YsVec2i> plg;
		YSBOOL line,fill,closed;
		ArrowHeadInfo frontArrow,backArrow;
		// Multiple points <<

		// Vertex array cache >>
		mutable int markerSize;  // It is not quite a vertex array, but if non-zero, point-array type must also draw markers at corners.
		mutable YsColor highlightCol;

		YSBOOL usingTentativePosition;
		YsGLVertexBuffer vtxBuf;
		YsGLVertexBuffer2D texCoordBuf;
		YsGLColorBuffer colBuf;
		YsGLColorBuffer filColBuf;

		YsGLVertexBuffer arrowVtxBuf;
		YsGLColorBuffer arrowColBuf;

		YsGLVertexBuffer frameVtxBuf;
		// Vertex array cache <<



		void Render(void) const;
		void RenderFrame(YsColor col) const;

		void Move(YsVec2i displacement);

	private:
		MarkUp(const MarkUp &);

		const YsTextureManager &GetTexMan(void) const;
		YsTextureManager &GetTexMan(void);

		void CopyFrom(const MarkUp &incoming);


	public:
		MarkUp();
		MarkUp &operator=(const MarkUp &);
		~MarkUp();
		void Initialize(void);
		void CleanUp(void);

		/*! Returns the mark up type. */
		MARKUPTYPE GetMarkUpType(void) const;

		/*! Returns the bounding box of the mark up. */
		YsRect3 GetBoundingBox(void) const;

		/*! Returns the bounding box of the mark up in integer. */
		YsRect2i GetBoundingBox2i(void) const;

		/*! Returns the font size of a text mark up. */
		int GetFontSize(void) const;

		/*! Returns the position of MARKUP_STRING type. */
		YsVec2i GetPosition(void) const;

		/*! Returns the mark-up text. */
		YsWString GetMarkUpText(void) const;

		/*! Make a text mark up. */
		void MakeStringMarkUp(const YsWString &str);
		void MakeStringMarkUp(const YsString &str);

		/*! Make a multi-point mark up. */
		void MakePointArrayMarkUp(const YsConstArrayMask <YsVec2i> &plg);

		/*! Returns an array of points of MARKUP_POINT_ARRAY type. */
		const YsArray <YsVec2i> GetPointArray(void) const;

		/*! Returns YSTRUE if it is a loop (closed) and is a MARKUP_POINT_ARRAY type.  YSFALSE otherwise. */
		YSBOOL IsLoop(void) const;

		/*! Returns YSTRUE if it is a drawing (not text) mark up.
		*/
		YSBOOL IsDrawing(void) const;

		/*! Set the position */
		void SetPosition(YsVec2i pos);

		/*! Returns the foreground color. */
		YsColor GetFgColor(void) const;

		/*! Set foreground color. */
		void SetFgColor(YsColor col);

		/*! Returns the foreground color. */
		YsColor GetBgColor(void) const;

		/*! Set background color */
		void SetBgColor(YsColor col);

		/*! Remake a texture for a string.
		    It does nothing if the mark-up type is not a string.
		    Must be called before ReadyTexture if the text or color is updated.
		*/
		void RemakeStringTexture(void);

		/*! Returns marker size. */
		int GetMarkerSize(void) const;

		/*! Sets marker size.  Marker size is a temporary parameter and will not be saved to file.
		    It is used for highlighting a point-array object.
		*/
		void SetMarkerSize(int s) const;

		/*! Returns highlight color. */
		YsColor GetHighlightColor(void) const;

		/*! Sets highlight color.  Highlight color is a temporary parameter and will not be saved to file. 
		    It is used for highlighting a point-array object.
		*/
		void SetHighlightColor(YsColor col) const;

		/*! Make texture ready. */
		YSRESULT ReadyTexture(void);

		/*! Make vertex array buffer. */
		YSRESULT ReadyVbo(void);

		/*! Make vertex array buffer with tweaked shape. */
		YSRESULT ReadyVbo(const YsVec2i pos,YsColor highlightCol);

		/*! Make vertex array buffer with temporary displacement. */
		YSRESULT ReadyVboWithDisplacement(const YsVec2i disp,YsColor highlightCol);
	
		/*! Make vertex array buffer with temporary point array. */
		YSRESULT ReadyVboWithModifiedPointArray(const YsConstArrayMask <YsVec2i> &aryMask,YsColor highlightCol);

		ArrowHeadInfo &ArrowInfo(int frontOrBack)
		{
			if(0==frontOrBack)
			{
				return frontArrow;
			}
			else
			{
				return backArrow;
			}
		}
	};
	typedef YsEditArrayObjectHandle <MarkUp,4> MarkUpHandle;
	typedef YsEditArray<MarkUp,4>::HandleStore MarkUpStore;


	class MarkUpPointIndex
	{
	public:
		MarkUpHandle muHd;
		YSSIZE_T idx;
		inline MarkUpPointIndex()
		{
			muHd=nullptr;
			idx=-1;
		}
	};


	/*! MarkUp anchor ties a point of a point-array mark-up to a map piece or a text mark up.
	*/
	class MarkUpAnchor
	{
	friend class Field;
	friend class RetroMap_World;

	private:
		YSHASHKEY markUpKey;
		YSHASHKEY baseMapPieceKey;
		YSHASHKEY baseMarkUpKey;
		YSSIZE_T markUpPointIdx;
		YsVec2i relPosToBase;
	public:
		MarkUpAnchor();
		void CleanUp(void);

		void SetMarkUp(const class Field &field,MarkUpHandle muHd);
		MarkUpHandle GetMarkUp(const class Field &field) const;
		void SetBaseMapPiece(const class Field &field,MapPieceHandle mpHd);
		MapPieceHandle GetBaseMapPiece(const class Field &field) const;
		MarkUpHandle GetBaseMarkUp(const class Field &field) const;

		void SetMarkUpPointIndex(YSSIZE_T idx);
		YSSIZE_T GetMarkUpPointIndex(void) const;

		void SetRelPos(YsVec2i relPos);
		YsVec2i GetRelPos(void) const;
	};
	typedef YsEditArrayObjectHandle <MarkUpAnchor,4> AnchorHandle;

	class TentativeAnchor
	{
	public:
		YSSIZE_T markUpIdx;
		YSSIZE_T baseMapPieceIdx;
		YSSIZE_T baseMarkUpIdx;
		YSSIZE_T markUpPointIdx;
		YsVec2i relPosToBase;

		inline void Initialize(void)
		{
			markUpIdx=-1;
			baseMapPieceIdx=-1;
			baseMarkUpIdx=-1;
			markUpPointIdx=-1;
			relPosToBase.Set(0,0);
		}
	};

	class Field
	{
	friend class RetroMap_World;
	friend class MapPiece;

	private:
		// [Data members] >>
		// When moving or copying, make sure to update the ownership.

		// !!!! When adding a data member, make sure to update MoveFrom.
		RetroMap_World *owner;
		YsWString fieldName;
		YsEditArray <MapPiece,4> mapPiece;
		YsEditArray <MarkUp,4> markUp;

		YsEditArray <MarkUpAnchor,4> anchor;
		YsHashTable <YsArray <AnchorHandle> > baseMapPieceKeyToAnchor;
		YsHashTable <YsArray <AnchorHandle> > baseMarkUpKeyToAnchor;
		YsHashTable <YsArray <AnchorHandle> > markUpKeyToAnchor;

		mutable YsArray <YSHASHKEY> selectedMapPiece;
		mutable YsKeyStore selectedMapPieceKey;
		mutable YsKeyStore selectedMarkUpKey;
		// !!!! When adding a data member, make sure to update MoveFrom.

	public:
		// For saving view points when switching between fields.
		// It will not be saved to file.
		mutable YsArray <YsString> savedView;
		// [Data members] <<

	private:
		Field(const Field &);
		Field &operator=(const Field &);

		const YsTextureManager &GetTexMan(void) const;
		YsTextureManager &GetTexMan(void);
		void SetTexture(MapPieceHandle mpHd,YsTextureManager::TexHandle texHd);

		// Make entire texture visible.
		void MakeAllVisible(MapPieceHandle mpHd);

		void ChangePriority(MapPieceHandle fromMpHd,MapPieceHandle toMpHd);
		void PriorityUp(MapPieceHandle fromMpHd,MapPieceHandle toMpHd);
		void PriorityDown(MapPieceHandle fromMpHd,MapPieceHandle toMpHd);
		void SwapAnchorBase(MapPieceHandle mpHd1,MapPieceHandle mpHd2);

		void SelectMapPiece(MapPieceHandle mpHd);
		void SelectMapPiece(const YsConstArrayMask <MapPieceHandle> &mpHd);
		void AddSelectedMapPiece(MapPieceHandle mpHd);
		void AddSelectedMapPiece(const YsConstArrayMask <MapPieceHandle> &mpHd);
		void UnselectMapPiece(MapPieceHandle mpHd);
		void UnselectMapPiece(const YsConstArrayMask <MapPieceHandle> &mpHd);
		void UnselectAllMapPiece(void);

		void SelectMarkUp(MarkUpHandle muHd);
		void SelectMarkUp(const YsConstArrayMask <MarkUpHandle> &muHd);
		void AddSelectedMarkUp(MarkUpHandle muHd);
		void AddSelectedMarkUp(const YsConstArrayMask <MarkUpHandle> &muHd);
		void UnselectMarkUp(MarkUpHandle muHd);
		void UnselectMarkUp(const YsConstArrayMask <MarkUpHandle> &muHd);
		void UnselectAllMarkUp(void);

		void RemakeAnchorSearchTable(void);

	public:
		/*! Return YSTRUE if the map piece is selected.  YSFALSE otherwise. */
		YSBOOL IsSelected(MapPieceHandle mpHd) const;
		/*! Returns an array of selected map pieces. */
		YsArray <MapPieceHandle> SelectedMapPiece(void) const;

		/*! Return YSTRUE if the mark up is selected.  YSFALSE otherwise. */
		YSBOOL IsSelected(MarkUpHandle mpHd) const;
		/*! Returns an array of selected mark ups. */
		YsArray <MarkUpHandle> SelectedMarkUp(void) const;

	public:
		Field();
		~Field();
		void CleanUp(void);

		YSHASHKEY GetSearchKey(MarkUpHandle muHd) const;
		YSHASHKEY GetSearchKey(MapPieceHandle mpHd) const;
		MarkUpHandle FindMarkUp(YSHASHKEY key) const;
		MapPieceHandle FindMapPiece(YSHASHKEY key) const;

		/*! Moves field.
		*/
		void MoveFrom(Field &incoming);

	public:
		/*! Swaps fields. */
		void Swap(Field &incoming);

		/*! Returns an enumerator of all map pieces.
		*/
		YsEditArray<MapPiece,4>::HandleEnumerator AllMapPiece(void) const;

		/*! Returns an enumerator of all mark ups.
		*/
		YsEditArray <MarkUp,4>::HandleEnumerator AllMarkUp(void) const;

		/*! Returns an enumerator of all anchors. */
		YsEditArray <MarkUpAnchor,4>::HandleEnumerator AllAnchor(void) const;

		/*! Returns the last map piece. */
		YsEditArrayObjectHandle <MapPiece,4> LastMapPiece(void) const;

		/*! Returns the first map piece. */
		YsEditArrayObjectHandle <MapPiece,4> FirstMapPiece(void) const;

		/*! Returns the next map piece. */
		YsEditArrayObjectHandle <MapPiece,4> FindNext(YsEditArrayObjectHandle <MapPiece,4> mpHd) const;

		/*! Returns the previous map piece. */
		YsEditArrayObjectHandle <MapPiece,4> FindPrev(YsEditArrayObjectHandle <MapPiece,4> mpHd) const;

		/*! Returns the number of map pieces. 
		*/
		YSSIZE_T GetNumMapPiece(void) const;

		/*! Returns the number of mark ups. 
		*/
		YSSIZE_T GetNumMarkUp(void) const;

		/*! Creates a map piece.
		*/
		MapPieceHandle CreateMapPiece(void);

		/*! Creates a mark up.
		*/
		MarkUpHandle CreateMarkUp(void);

		/*! Returns a const pointer to the map piece. */
		const MapPiece *GetMapPiece(MapPieceHandle mpHd) const;

		/*! Returns a const pointer to the mark up. */
		const MarkUp *GetMarkUp(MarkUpHandle muHd) const;

		/*! Returns the bounding box of the field.  Minimum Z and maximum Z will always be -1 and 1. */
		YsRect3 GetBoundingBox(void) const;

		/*! Returns the bounding box of the field in integer. */
		YsRect2i GetBoundingBox2i(void) const;

		/*! Returns the top-most map piece that the visible area includes the given coordinate. */
		MapPieceHandle FindMapPieceFromCoordinate(const YsVec2 &pos) const;

		/*! Returns the top-most string mark up that the bounding rect includes the given coordinate. */
		MarkUpHandle FindStringMarkUpFromCoordinate(const YsVec2 &pos) const;

	private:
		class MakeBitmapThreadInfo
		{
		public:
			MapPieceHandle mpHd;
			YsBitmap *bmpPtr;
			YsBitmap *trimmedBmpPtr;
		};
		void MakeMapPieceBitmap(MakeBitmapThreadInfo *info) const;
	public:
		/*! Make a bitmap of the field of the given area. */
		YsBitmap MakeBitmap(YsVec2i origin,YsVec2i bmpSize,YsColor bgCol,YSBOOL mapPiece,YSBOOL markUp,const MapPieceStore &excludeMapPiece) const;

		/*! Make a bitmap of entire field. */
		YsBitmap MakeBitmap(YSBOOL mapPiece,YSBOOL markUp) const;

		/*! Set up an anchor.  If an anchor is already set for the specified mark up and index, it fails and returns YSERR. */
		AnchorHandle CreateAnchor(MarkUpHandle muHd,YSSIZE_T idx,MapPieceHandle baseMpHd,YsVec2i relPos);

		/*! Set up an anchor.  If an anchor is already set for the specified mark up and index, it fails and returns YSERR. */
		AnchorHandle CreateAnchor(MarkUpHandle muHd,YSSIZE_T idx,MarkUpHandle baseMuHd,YsVec2i relPos);

		/*! Tentatively remove an anchor. */
		YSRESULT FreezeAnchor(AnchorHandle anHd);

		/*! Revive tentatively-removed anchor. */
		YSRESULT MeltAnchor(AnchorHandle anHd);

	private:
		void RegisterAnchor(AnchorHandle anHd);
		void UnregisterAnchor(AnchorHandle anHd);

	public:
		/*! Finds an anchor for the mark up and index. */
		AnchorHandle FindAnchorFromMarkUp(MarkUpHandle muHd, YSSIZE_T idx) const;

		/*! Finds anchors for the mark up that depends on the base position. */
		YsConstArrayMask <AnchorHandle> FindAnchorFromMarkUp(MarkUpHandle muHd) const;

		/*! Finds anchors for the base map piece. */
		YsConstArrayMask <AnchorHandle> FindAnchorFromBaseMapPiece(MapPieceHandle mpHd) const;

		/*! Finds anchors for the base mark up. */
		YsConstArrayMask <AnchorHandle> FindAnchorFromBaseMarkUp(MarkUpHandle muHd) const;

		/*! Returns an anchor pointer. */
		const MarkUpAnchor *GetAnchor(AnchorHandle anHd) const;

		/*! Returns the position at which the anchor should be located. */
		YsVec2i GetAnchorPosition(AnchorHandle anHd) const;

	private:
		MarkUpAnchor *GetEditableAnchor(AnchorHandle anHd);
	};

	friend class MapPiece;


public:
	class Clipboard
	{
	friend class RetroMap_World;

	private:
		YsArray <MapPiece> mapPiece;
		YsArray <MarkUp> markUp;
	public:
		Clipboard();
		~Clipboard();
		void CleanUp(void);
		void Add(const RetroMap_World &world,FieldHandle fdHd,MapPieceHandle mpHd);
		void Add(const RetroMap_World &world,FieldHandle fdHd,MarkUpHandle muHd);
		YsVec2i GetBoundingBoxCenter(void) const;
	};

	class MapPieceMarkUpSet
	{
	public:
		YsArray <MapPieceHandle> mapPiece;
		YsArray <MarkUpHandle> markUp;
	};

private:
	YsWString gameTitle;
	YsTextureManager texMan;
	YsTextureManager::TexHandle overlapTexHd;
	YsEditArray <Field,4> allField;
	YsRect2i trimRect;
	YsVec2 defScaling;
	int defFontSize;
	double defArrowSize;
	YsColor defMarkUpFgCol,defMarkUpBgCol;
	YsVec2i unit;
	mutable YSBOOL modified;
	int diffThr;

	YsWString fileName;
	YsWString lastImageFileName;

public:
	RetroMap_World();
	~RetroMap_World();
	void CleanUp(void);

	/*! Cleans up undo/redo log. */
	void CleanUpUndoLog(void);

private:
	void CleanUpRedoLogOnly(void);
	void Swap(FieldHandle fdHd,MapPieceHandle mpHd1,MapPieceHandle mpHd2);


public:
	YsEditArray<Field,4>::HandleEnumerator AllField(void) const;
	YsEditArray<MapPiece,4>::HandleEnumerator AllMapPiece(FieldHandle fdHd) const;

	/*! Clears the modified-flag. */
	void ClearModified(void);

	/*! Undo.  Returns the handle of the last-affected field. */
	FieldHandle Undo(void);

	/*! Redo.  Returns the handle of the last-affected field. */
	FieldHandle Redo(void);

	/*! Returns the modified-flag. */
	YSBOOL IsModified(void) const;

	/*! Returns the game title. */
	const YsWString &GetGameTitle(void) const;

	/*! Sets the game title. */
	void SetGameTitle(const YsWString &wstr);

	/*! Returns the unit. */
	YsVec2i GetUnit(void) const;

	/*! Sets the unit. */
	void SetUnit(YsVec2i unit);

	/*! Sets the default arrow-head size. */
	void SetDefaultArrowSize(double arwSize);

	/*! Returns the default arrow-head size. */
	double GetDefaultArrowSize(void) const;

	/*! Returns diff threshold.
	    Diff threshold is used for identifying different pixels of the overlapping map pieces.
	*/
	int GetDiffThreshold(void) const;

	/*! Set diff threshold.
	    Diff threshold is used for identifying different pixels of the overlapping map pieces.
	    This information is saved, but not an undo/redo target.
	*/
	void SetDiffThreshold(int diffThr);

	/*! Returns default mark-up font size. */
	int GetDefaultFontSize(void) const;

	/*! Sets default mark-up font size. */
	void SetDefaultFontSize(int fontSize);

	/*! Returns default mark-up foreground color. */
	YsColor GetDefaultMarkUpForegroundColor(void) const;

	/*! Sets default mark-up foreground color. */
	void SetDefaultMarkUpForegroundColor(YsColor col);

	/*! Returns default mark-up background color. */
	YsColor GetDefaultMarkUpBackgroundColor(void) const;

	/*! Sets default mark-up background color. */
	void SetDefaultMarkUpBackgroundColor(YsColor col);

	/*! Returns the number of fields in this world.
	*/
	YSSIZE_T GetNumField(void) const;

	/*! Returns the bounding box of the field.  Minimum Z and maximum Z will always be -1 and 1. */
	YsRect3 GetBoundingBox(FieldHandle fdHd) const;

	/*! Returns the first field. */
	FieldHandle GetFirstField(void) const;

	/*! Creates a field. */
	FieldHandle CreateField(void);
	class UndoCreateField;

	/*! Deletes a field. */
	void DeleteField(FieldHandle fdHd);
	class UndoDeleteField;

	/*! Returns the next field of fdHd. 
	    If fdHd==nullptr, it returns the first field.
	    If there is no next field, it returns nullptr.  */
	FieldHandle FindNextField(FieldHandle fdHd) const;

	/*! Returns the previous field of fdHd. 
	    If fdHd==nullptr, it returns the last field.
	    If there is no previous field, it returns nullptr.  */
	FieldHandle FindPrevField(FieldHandle fdHd) const;


	/*! Swaps fields.  */
	void Swap(FieldHandle fdHd1,FieldHandle fdHd2);
	class UndoSwapField;

	/*! Find field from the search key. */
	FieldHandle FindField(YSHASHKEY key) const;

	/*! Returns a const-pointer to the field. */
	const Field *GetField(FieldHandle fdHd) const;

	/*! Returns a const-pointer to the map piece. */
	const MapPiece *GetMapPiece(FieldHandle fdHd,MapPieceHandle mpHd) const;

	/*! Returns a const-pointer to the mark up. */
	const MarkUp *GetMarkUp(FieldHandle fdHd,MarkUpHandle muHd) const;

protected:
	/*! Returns a editable pointer to the map piece. */
	MapPiece *GetEditableMapPiece(FieldHandle fdHd,MapPieceHandle mpHd);
	MarkUp *GetEditableMarkUp(FieldHandle fdHd,MarkUpHandle muHd);

public:
	/*! Returns a search key of the field. */
	YSHASHKEY GetSearchKey(FieldHandle fdHd);

	/*! Loads a new image and creates a map piece in the field.
	*/
	MapPieceHandle LoadImage(FieldHandle fdHd,const YsVec2i pos,YsTextureManager::DATA_FORMAT fom,const YsConstArrayMask <unsigned char> &arrayMask);
	class UndoCreateMapPiece;

	/*! Pastes clipboard elements. */
	MapPieceMarkUpSet Paste(FieldHandle fdHd,const Clipboard &clipboard,YsVec2i offset);

private:
	/* Used from Clipboard. */
	MapPieceHandle CreateMapPiece(FieldHandle fdHd);

public:
	/*! Returns an expected shape of the next map piece. */
	MapPiece::Shape ExpectedShape(FieldHandle fdHd,const YsVec2i pos) const;

	/*! Deletes Map Pieces. */
	YSRESULT DeleteMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> mpHd);
	class UndoDeleteMapPiece;
	class UndoDeleteMultiMapPiece;

private:
	MarkUp *GetMarkUpPtr(FieldHandle fdHd,MarkUpHandle muHd);
	const MarkUp *GetMarkUpPtr(FieldHandle fdHd,MarkUpHandle muHd) const;

public:
	/*! Creates a mark-up. */
	MarkUpHandle CreateMarkUp(FieldHandle fdHd);
	class UndoCreateMarkUp;

	/*! Deletes a mark-up. */
	YSRESULT DeleteMarkUp(FieldHandle fdHd,MarkUpHandle muHd);
	class UndoDeleteMarkUp;

	/*! Deletes a mark-up. */
	YSRESULT DeleteMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &muHd);
	class UndoDeleteMultiMarkUp;

	/*! Make mark-up a string mark-up. */
	YSRESULT MakeStringMarkUp(FieldHandle fdHd,MarkUpHandle muHd,const YsWString &str);
	YSRESULT MakeStringMarkUp(FieldHandle fdHd,MarkUpHandle muHd,const YsString &str);
	class UndoMakeStringMarkUp;

	/*! Make mark-up a point-array. */
	YSRESULT MakePointArrayMarkUp(FieldHandle fdHd,MarkUpHandle muHd,const YsConstArrayMask <YsVec2i> &plg);
	class UndoMakePointArrayMarkUp;

	/*! Set mark-up foreground color. */
	YSRESULT SetMarkUpForegroundColor(FieldHandle fdHd,MarkUpHandle muHd,YsColor col);
	class UndoSetMarkUpFgColor;

	/*! Set mark-up background color. */
	YSRESULT SetMarkUpBackgroundColor(FieldHandle fdHd,MarkUpHandle muHd,YsColor col);
	class UndoSetMarkUpBgColor;

	/*! Set mark-up point array. */
	YSRESULT SetMarkUpPointArray(FieldHandle fdHd,MarkUpHandle muHd,const YsConstArrayMask <YsVec2i> &pointArray);
	class UndoSetMarkUpPointArray;

	/*! Set mark-up font size. */
	YSRESULT SetStringMarkUpFontSize(FieldHandle fdHd,MarkUpHandle muHd,int fontSize);
	class UndoSetStringMarkUpFontSize;

	/*! Set mark-up arrow-head info. */
	YSRESULT SetPointArrayMarkUpArrowHead(FieldHandle fdHd,MarkUpHandle muHd,int frontOrBack,MarkUp::ArrowHeadInfo ahInfo);
	class UndoSetPointArrayMarkUpArrowHead;

	/*! Make mark-up texture (for string mark up) */
	YSRESULT RemakeStringTexture(FieldHandle fdHd,MarkUpHandle muHd);

	/*! Ready texture of a mark up. */
	YSRESULT ReadyTexture(FieldHandle fdHd,MarkUpHandle muHd);

	/*! Ready vertex array of a mark up. */
	YSRESULT ReadyVbo(FieldHandle fdHd,MarkUpHandle muHd);

	/*! Ready vertex array of a mark up with a tentative position and highlight color. */
	YSRESULT ReadyVbo(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i pos,YsColor hlCol);

	/*! Ready vertex array of a mark up with a tentative displacement and highlight color. */
	YSRESULT ReadyVboWithDisplacement(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i disp,YsColor hlCol);

	/*! Ready vertex array of a mark up with a tentative point array and highlight color. */
	YSRESULT ReadyVboWithModifiedPointArray(FieldHandle fdHd,MarkUpHandle muHd,const YsConstArrayMask <YsVec2i> &pointArray,YsColor hlCol);

	/*! Ready texture.
	*/
	YSRESULT ReadyTexture(FieldHandle fdHd,MapPieceHandle mpHd);

	/*! Set Field Name */
	YSRESULT SetFieldName(FieldHandle fdHd,const YsWString nm);

	/*! Make entire image visible. */
	void MakeAllVisible(FieldHandle fdHd,MapPieceHandle mpHd);
	class UndoSetMapPieceTrimRect;

	/*! Move the map piece to front. 
	    This function does not change the order of the MapPieceHandles.
	    Instead, it moves the map piece pointed by mpHd to the front.
	    Therefore, the handle of the front map piece also stays the same,
	    but the contents of the map pieces move.

	    (I made it, but found some problems.  It works, but using it correctly is not as easy as I thought.
	    Use ChangeMapPiecePriority instead.)
	*/
	YSRESULT MoveToFront(FieldHandle fdHd,MapPieceHandle mpHd);
	class UndoMoveMapPieceToFront;

	/*! Change the map-piece priority. */
	YSRESULT ChangePriority(FieldHandle fdHd,MapPieceHandle fromMpHd,MapPieceHandle toMpHd);
	class UndoChangeMapPiecePriority;

	/*! Move the map piece to Back. */
	YSRESULT MoveToBack(FieldHandle fdHd,MapPieceHandle mpHd);
	class UndoMoveMapPieceToBack;

	/*! Add an anchor. */
	AnchorHandle CreateAnchor(FieldHandle fdHd,MarkUpHandle muHd,YSSIZE_T idx,MapPieceHandle baseUmpHd,YsVec2i relPos);
	AnchorHandle CreateAnchor(FieldHandle fdHd,MarkUpHandle muHd,YSSIZE_T idx,MarkUpHandle baseMuHd,YsVec2i relPos);
	class UndoCreateAnchor;

	/*! Remove an anchor. */
	YSRESULT DeleteAnchor(FieldHandle fdHd,AnchorHandle anHd);
	class UndoDeleteAnchor;

	/*! Remake all string textures of the mark-ups. */
	YSRESULT RemakeStringTexture(FieldHandle fdHd);

	/*! Ready vertex buffer.
	*/
	YSRESULT ReadyVbo(FieldHandle fdHd);

	/*! Ready vertex buffer.
	*/
	YSRESULT ReadyVbo(FieldHandle fdHd,MapPieceHandle mpHd);

	/*! Ready vertex array buffer for the anchored mark ups. */
	YSRESULT ReadyVboOfAnchoredMarkUp(FieldHandle fdHd,MapPieceHandle baseMpHd);
	YSRESULT ReadyVboOfAnchoredMarkUp(FieldHandle fdHd,MarkUpHandle baseMuHd);

	/*! Ready vertex buffer with customized shape.
	*/
	YSRESULT ReadyVbo(FieldHandle fdHd,MapPieceHandle mpHd,MapPiece::Shape shape,double alpha);

	/*! Reset vertex arrays of the map pieces and mark ups that are using tentative positions. 
	    Returns YSOK if at least one of the map pieces or mark ups is reset. YSERR otherwise.
	*/
	YSRESULT ResetVbo(FieldHandle fdHd);

	/*! Render field.  Call ReadyVbo for all map pieces before this function.
	*/
	YSRESULT Render(FieldHandle fdHd,YSBOOL drawMapPiece,YSBOOL drawMarkUp) const;

	/*! Create a texture handle for overlap info (such as diff).
	*/
	void MakeUtilityTexture(void);

	/*! Information returned by MakeDiff. 
	    totalError is the sum of absolute difference between pixel values, and numSample is the number of overlapping pixels, and numErrorPixel is the number of pixels identified as different based on the threshold.
	    
	*/
	class DiffInfo
	{
	public:
		YSSIZE_T totalError;
		YSSIZE_T numSample;
		YSSIZE_T numErrorPixel;
	};

	/*! Make diff textures. */
	DiffInfo MakeDiff(FieldHandle fdHd,const MapPieceStore &target,const MapPieceStore &exclusion,int thr,YsColor col);

	/*! Make diff bitmap for one map piece. */
	DiffInfo MakeDiffBitmap(YsBitmap &bmp,FieldHandle fdHd,MapPieceHandle mpHd,MapPiece::Shape shape,const MapPieceStore &exclusion,int thr,YsColor col) const;

private:
	YSBOOL CheckOverlappingMapPiece(FieldHandle fdHd,YsRect2i rect0,const MapPieceStore &exclusion) const;

public:
	/*! Render diff of the specified map pieces.
	    Since it makes bitimap on the fly, it is a non-const member.
	*/
	YSRESULT RenderDiff(FieldHandle fdHd,const MapPieceStore &target,int thr,YsColor col);

public:
	/*! Select a map piece(s). */
	void SelectMapPiece(FieldHandle fdHd,MapPieceHandle mpHd);
	void SelectMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> &mpHd);

	/*! Add map piece(s) to the selection */
	void AddSelectedMapPiece(FieldHandle fdHd,MapPieceHandle mpHd);
	void AddSelectedMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> &mpHd);

	/*! Unselect given map piece(s). */
	void UnselectMapPiece(FieldHandle fdHd,MapPieceHandle mpHd);
	void UnselectMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> &mpHd);

	/*! Unselect all map pieces. */
	void UnselectAllMapPiece(FieldHandle fdHd);

	/*! Return YSTRUE if the map piece is selected.  YSFALSE otherwise. */
	YSBOOL IsSelected(FieldHandle fdHd,MapPieceHandle mpHd) const;

	/*! Returns an array of selected map pieces. */
	YsArray <MapPieceHandle> SelectedMapPiece(FieldHandle fdHd) const;

	/*! Select a mark up(s). */
	void SelectMarkUp(FieldHandle fdHd,MarkUpHandle mpHd);
	void SelectMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &mpHd);

	/*! Add mark up(s) to the selection */
	void AddSelectedMarkUp(FieldHandle fdHd,MarkUpHandle mpHd);
	void AddSelectedMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &mpHd);

	/*! Unselect given mark up(s). */
	void UnselectMarkUp(FieldHandle fdHd,MarkUpHandle mpHd);
	void UnselectMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &mpHd);

	/*! Unselect all mark up. */
	void UnselectAllMarkUp(FieldHandle fdHd);

	/*! Return YSTRUE if the mark up is selected.  YSFALSE otherwise. */
	YSBOOL IsSelected(FieldHandle fdHd,MarkUpHandle muHd) const;

	/*! Returns an array of selected map pieces. */
	YsArray <MarkUpHandle> SelectedMarkUp(FieldHandle fdHd) const;

	/*! Returns the default trim rect.
	    If the width or height of the rect is zero, it means that the trim rect is not set.
	*/
	YsRect2i GetDefaultTrimRect(void) const;

	/*! Set the default trim rect. */
	void SetDefaultTrimRect(YsRect2i rect);

	/*! Set the default scaling. */
	void SetDefaultScaling(const YsVec2 &scaling);

	/*! Set trim rect on a map piece. */
	YSRESULT SetTrimRect(FieldHandle fdHd,MapPieceHandle mpHd,YsRect2i rect);

	/*! Set position of the map piece. */
	YSRESULT SetPosition(FieldHandle fdHd,MapPieceHandle mpHd,YsVec2i pos);
	class UndoSetMapPiecePosition;

	/*! Set position of the string mark up. 
	    It does not change position of a point-array mark up.
	*/
	YSRESULT SetPosition(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i pos);
	class UndoSetMarkUpPosition;

	/*! Move a mark up. */
	YSRESULT MoveMarkUp(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i displacement);
	class UndoMoveMarkUp;

	/*! Set position of the map piece. */
	YSRESULT SetScaling(FieldHandle fdHd,MapPieceHandle mpHd,YsVec2 sca);
	class UndoSetMapPieceScaling;

	/*! Re-apply anchor. */
	YSRESULT ReapplyAnchor(FieldHandle fdHd,MapPieceHandle baseMpHd);
	YSRESULT ReapplyAnchor(FieldHandle fdHd,MarkUpHandle baseMuHd);
	YSRESULT ReapplyAnchor(FieldHandle fdHd,const YsConstArrayMask <AnchorHandle> &anHd);
	class UndoMoveMarkUpPoint;

	/*! Set file name. */
	void SetFileName(const YsWString &fn);

	/*! Returns the file name. */
	const YsWString &GetFileName(void) const;

	/*! Sets no-save flag to the textures that are not currently used,
	    and clears no-save flag of the currently-used textures.
	*/
	void ConfigureTextureNoSaveFlag(void);

	/*! Save to file. */
	// written in retromap_data_io.cpp
	YSRESULT Save(FILE *fp) const;

	/*! Open a file. */
	// written in retromap_data_io.cpp
	YSRESULT Open(FILE *fp);

	/*! Remember last-used image file name. */
	void SetLastUsedImageFile(const YsWString fn);

	/*! Get last-used image file name. */
	YsWString GetLastUsedImageFile(void) const;

	/*! Returns the field name. */
	YsWString GetFieldName(FieldHandle fdHd) const;

	/*! Check if the internal data structure is correct. */
	YSRESULT DataIntegrityCheck(FieldHandle fdHd) const;
};

template <class UndoLogClass>
UndoLogClass *RetroMap_World::NewUndoLog(void)
{
	// if(take_undo_log)
	{
		CleanUpRedoLogOnly();

		this->undoCtr+=this->undoInc;

		auto newUndoPtr=new UndoLogClass;
		newUndoPtr->undoCtr=this->undoCtr;
		if(nullptr!=undoPtr)
		{
			undoPtr->next=newUndoPtr;
			newUndoPtr->prev=undoPtr;
		}
		undoPtr=newUndoPtr;
		redoPtr=nullptr;

		return newUndoPtr;
	}
	// return nullptr;
}


/* } */
#endif
