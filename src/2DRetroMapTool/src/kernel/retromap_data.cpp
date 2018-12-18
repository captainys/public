#include <thread>

#include <yssystemfont.h>
#include <ysbitmaputil.h>

#include "retromap_data.h"


RetroMap_World::UndoLog::UndoLog()
{
	next=nullptr;
	prev=nullptr;
}
/* virtual */ RetroMap_World::UndoLog::~UndoLog()
{
}

////////////////////////////////////////////////////////////

RetroMap_World::UndoGuard::UndoGuard(RetroMap_World &world)
{
	prevIncUndo=world.undoInc;
	worldPtr=&world;
	world.undoCtr+=world.undoInc;
	world.undoInc=0;
}
RetroMap_World::UndoGuard::~UndoGuard()
{
	worldPtr->undoInc=prevIncUndo;
	worldPtr->undoCtr+=prevIncUndo;
}

////////////////////////////////////////////////////////////

YsArray <YsVec2,4> RetroMap_World::MarkUp::ArrowHeadInfo::GetPolygon(YsVec2i fromi,YsVec2i toi) const
{
	YsArray <YsVec2,4> plg;
	switch(arrowHeadType)
	{
	case MarkUp::ARROWHEAD_TRIANGLE:
		{
			YsVec2 from(fromi.x(),fromi.y()),to(toi.x(),toi.y());
			YsVec2 vec=to-from;
			if(YSOK!=vec.Normalize())
			{
				return plg;
			}
			YsVec2 left(-vec.y(),vec.x());
			YsVec2 tri[3]=
			{
				to,
				to-vec*arrowHeadSize+left*arrowHeadSize/4.0,
				to-vec*arrowHeadSize-left*arrowHeadSize/4.0,
			};
			plg.push_back(tri[0]);
			plg.push_back(tri[1]);
			plg.push_back(tri[2]);
		}
		break;
	}
	return plg;
}

void RetroMap_World::MarkUp::ArrowHeadInfo::MakeVertexArray(class YsGLVertexBuffer &vtxBuf,YsVec2i fromi,YsVec2i toi) const
{
	for(auto p : GetPolygon(fromi,toi))
	{
		vtxBuf.Add(p.x(),p.y(),0.0);
	}
}

////////////////////////////////////////////////////////////

YsRect2i RetroMap_World::MapPiece::Shape::VisibleAreaFromWorldXY(const YsVec2 &min,const YsVec2 &max) const
{
	YsVec2 visiMin,visiMax;
	YsVec2 posd(pos.x(),pos.y());

	visiMin=min-posd;
	visiMax=max-posd;

	visiMin.DivX(scaling.x());
	visiMin.DivY(scaling.y());
	visiMax.DivX(scaling.x());
	visiMax.DivY(scaling.y());

	return YsRect2i(YsVec2i((int)visiMin.x(),(int)visiMin.y()),YsVec2i((int)visiMax.x(),(int)visiMax.y()));
}
YsVec2 RetroMap_World::MapPiece::Shape::ScalingFromWorldXY(const YsVec2 &min,const YsVec2 &max) const
{
	double dx=(double)(visibleArea.Max().x()-visibleArea.Min().x());
	double dy=(double)(visibleArea.Max().y()-visibleArea.Min().y());

	return YsVec2((max.x()-min.x())/dx,(max.y()-min.y())/dy);
}

YsVec2i RetroMap_World::MapPiece::Shape::PositionFromWorldXY(const YsVec2 &min) const
{
	auto visiMin=visibleArea.Min();  // visiMin is scaled by scaling.
	YsVec2 visiMind(visiMin.x(),visiMin.y());
	visiMind.MulX(scaling.x());
	visiMind.MulY(scaling.y());

	YsVec2 posd=min-visiMind;
	return YsVec2i((int)posd.x(),(int)posd.y());
}

YsRect3 RetroMap_World::MapPiece::Shape::GetBoundingBox(void) const
{
	auto visArea=visibleArea;

	YsVec2 visMin((double)visArea.Min().x()*scaling.x(),(double)visArea.Min().y()*scaling.y());
	YsVec2 visMax((double)visArea.Max().x()*scaling.x(),(double)visArea.Max().y()*scaling.y());

	YsVec3 min(pos.x()+visMin.x(),pos.y()+visMin.y(),-1.0);
	YsVec3 max(pos.x()+visMax.x(),pos.y()+visMax.y(), 1.0);

	YsRect3 rect;
	rect.Set(min,max);
	return rect;
}

YsRect2i RetroMap_World::MapPiece::Shape::GetBoundingBox2i(void) const
{
	auto visArea=visibleArea;

	YsVec2 visMin((double)visArea.Min().x()*scaling.x(),(double)visArea.Min().y()*scaling.y());
	YsVec2 visMax((double)visArea.Max().x()*scaling.x(),(double)visArea.Max().y()*scaling.y());

	YsVec2i min(pos.x()+visMin.xi(),pos.y()+visMin.yi());
	YsVec2i max(pos.x()+visMax.xi(),pos.y()+visMax.yi());
	return YsRect2i(min,max);
}

void RetroMap_World::MapPiece::Shape::ApplyUnit(YsVec2i unit)
{
	auto visArea=visibleArea;
	YsVec2 visMin((double)visArea.Min().x()*scaling.x(),(double)visArea.Min().y()*scaling.y());

	YsVec3 min(pos.x()+visMin.x(),pos.y()+visMin.y(),-1.0);

	double x=floor(min.x()/(double)unit.x())*(double)unit.x();
	double y=floor(min.y()/(double)unit.y())*(double)unit.y();

	pos=PositionFromWorldXY(YsVec2(x,y));
}

////////////////////////////////////////////////////////////

RetroMap_World::MapPiece::MapPiece()
{
	Initialize();
}

RetroMap_World::MapPiece &RetroMap_World::MapPiece::operator=(const MapPiece &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}

void RetroMap_World::MapPiece::Initialize(void)
{
	owner=nullptr;
	texHd=nullptr;
	diffTexHd=nullptr;
	CleanUp();
}

void RetroMap_World::MapPiece::Swap(MapPiece &incoming)
{
	MapPiece tmp;
	tmp.MoveFrom(incoming);
	incoming.MoveFrom(*this);
	this->MoveFrom(tmp);
}

void RetroMap_World::MapPiece::MoveFrom(MapPiece &incoming)
{
	if(this!=&incoming)
	{
		this->owner=incoming.owner;
		this->texHd=incoming.texHd;
		incoming.texHd=nullptr;
		this->diffTexHd=incoming.diffTexHd;
		incoming.diffTexHd=nullptr;
		this->shape=incoming.shape;
		this->usingTentativePosition=incoming.usingTentativePosition;
		this->eventLevel=incoming.eventLevel;
		this->alpha=incoming.alpha;
		for(int i=0; i<4; ++i)
		{
			this->quadVtPos[i]=incoming.quadVtPos[i];
			this->quadTexCoord[i]=incoming.quadTexCoord[i];
		}

		this->colBuf.MoveFrom(incoming.colBuf);
		this->vtxBuf.MoveFrom(incoming.vtxBuf);
		this->texCoordBuf.MoveFrom(incoming.texCoordBuf);
		this->texLabelCache.MoveFrom(incoming.texLabelCache);
	}
}

void RetroMap_World::MapPiece::CopyFrom(const MapPiece &incoming)
{
	if(this!=&incoming)
	{
		this->owner=incoming.owner;
		this->texHd=incoming.texHd;
		this->diffTexHd=nullptr;
		this->shape=incoming.shape;
		this->usingTentativePosition=incoming.usingTentativePosition;
		this->eventLevel=incoming.eventLevel;
		this->alpha=incoming.alpha;
		for(int i=0; i<4; ++i)
		{
			this->quadVtPos[i]=incoming.quadVtPos[i];
			this->quadTexCoord[i]=incoming.quadTexCoord[i];
		}

		this->colBuf.CleanUp();
		this->vtxBuf.CleanUp();
		this->texCoordBuf.CleanUp();
		this->texLabelCache.CleanUp();
	}
}

const YsTextureManager &RetroMap_World::MapPiece::GetTexMan(void) const
{
	return owner->GetTexMan();
}

YsTextureManager &RetroMap_World::MapPiece::GetTexMan(void)
{
	return owner->GetTexMan();
}

RetroMap_World::MapPiece::~MapPiece()
{
	CleanUp();
}
void RetroMap_World::MapPiece::CleanUp(void)
{
	// Do not delete texture handle here.
	// Should allow multiple map piece referring to the same texture.
	owner=nullptr;
	usingTentativePosition=YSFALSE;
	texHd=nullptr;
	texLabelCache="";
	eventLevel=0;
	alpha=1.0;
	shape.pos.Set(0,0);
	shape.scaling.Set(1,1);
}

YsVec2i RetroMap_World::MapPiece::GetPosition(void) const
{
	return shape.pos;
}

int RetroMap_World::MapPiece::GetWidth(void) const
{
	auto &texMan=GetTexMan();
	auto unitPtr=texMan.GetTexture(texHd);
	if(nullptr!=unitPtr)
	{
		return unitPtr->GetWidth();
	}
	return 0;
}
int RetroMap_World::MapPiece::GetHeight(void) const
{
	auto &texMan=GetTexMan();
	auto unitPtr=texMan.GetTexture(texHd);
	if(nullptr!=unitPtr)
	{
		return unitPtr->GetHeight();
	}
	return 0;
}
YsVec2i RetroMap_World::MapPiece::GetVisibleSize(void) const
{
	return shape.visibleArea.GetSize();
}
YsRect2i RetroMap_World::MapPiece::GetVisibleArea(void) const
{
	return shape.visibleArea;
}
YsVec2i RetroMap_World::MapPiece::GetBitmapSize(void) const
{
	YsVec2i d(0,0);
	auto &texMan=GetTexMan();
	auto unitPtr=texMan.GetTexture(texHd);
	if(nullptr!=unitPtr)
	{
		d.Set(unitPtr->GetWidth(),unitPtr->GetHeight());
	}
	return d;
}

YSRESULT RetroMap_World::MapPiece::ReadyTexture(void) const
{
	auto &texMan=owner->GetTexMan();
	auto unitPtr=texMan.GetTexture(texHd);
	if(nullptr!=unitPtr)
	{
		texMan.SetTextureFilterType(texHd,YsTextureManager::Unit::FILTERTYPE_LINEAR);
		return unitPtr->MakeActualTexture();
	}
	return YSERR;
}

YSRESULT RetroMap_World::MapPiece::ReadyVbo(void) const
{
	auto res=ReadyVbo(this->shape,this->alpha);
	usingTentativePosition=YSFALSE;
	return res;
}

YSRESULT RetroMap_World::MapPiece::ReadyTextureIfNotReady(void) const
{
	auto &texMan=GetTexMan();
	auto unitPtr=texMan.GetTexture(texHd);
	if(nullptr!=unitPtr && (YSTRUE!=unitPtr->IsActualTextureReady() || YSTRUE!=unitPtr->IsBitmapLoaded()))
	{
		return ReadyTexture();
	}
	return YSERR;
}

YSRESULT RetroMap_World::MapPiece::ReadyVboIfNotReady(void) const
{
	if(0==vtxBuf.size())
	{
		return ReadyVbo();
	}
	return YSERR;
}

void RetroMap_World::MapPiece::SetDiffTexture(const YsBitmap &bmp)
{
	auto &texMan=GetTexMan();
	auto w=bmp.GetWidth();
	auto h=bmp.GetHeight();
	auto rgba=bmp.GetRGBABitmapPointer();
	if(nullptr==diffTexHd)
	{
		auto label=texMan.MakeUniqueLabel();
		diffTexHd=texMan.AddTexture(label,texMan.FOM_RAW_RGBA,w,h,w*h*4,rgba);
		texMan.SetDontSave(diffTexHd,YSTRUE);
	}
	else
	{
		texMan.SetTextureFileData(diffTexHd,texMan.FOM_RAW_RGBA,w,h,w*h*4,rgba);
	}
}

YSRESULT RetroMap_World::MapPiece::ReadyVbo(const Shape &shape,const double alpha) const
{
	usingTentativePosition=YSTRUE;

	auto bmpSize=GetBitmapSize();
	auto visArea=shape.visibleArea;
	auto pos=shape.pos;

	const double uMin=(double)visArea.Min().x()/(double)bmpSize.x();
	const double vMin=(double)visArea.Min().y()/(double)bmpSize.y();
	const double uMax=(double)visArea.Max().x()/(double)bmpSize.x();
	const double vMax=(double)visArea.Max().y()/(double)bmpSize.y();

	auto realRect=shape.GetBoundingBox();

	quadVtPos[0]=YsVec3(realRect.Min().x(),realRect.Min().y(),0.0);
	quadTexCoord[0]=YsVec2(uMin,vMin);

	quadVtPos[1]=YsVec3(realRect.Max().x(),realRect.Min().y(),0.0);
	quadTexCoord[1]=YsVec2(uMax,vMin);

	quadVtPos[2]=YsVec3(realRect.Max().x(),realRect.Max().y(),0.0);
	quadTexCoord[2]=YsVec2(uMax,vMax);

	quadVtPos[3]=YsVec3(realRect.Min().x(),realRect.Max().y(),0.0);
	quadTexCoord[3]=YsVec2(uMin,vMax);

	YsColor col;
	col.SetDoubleRGBA(1,1,1,alpha);

	vtxBuf.CleanUp();
	texCoordBuf.CleanUp();
	colBuf.CleanUp();
	vtxBuf.Add(quadVtPos[0]);
	vtxBuf.Add(quadVtPos[1]);
	vtxBuf.Add(quadVtPos[2]);
	vtxBuf.Add(quadVtPos[3]);
	texCoordBuf.Add(quadTexCoord[0]);
	texCoordBuf.Add(quadTexCoord[1]);
	texCoordBuf.Add(quadTexCoord[2]);
	texCoordBuf.Add(quadTexCoord[3]);
	colBuf.Add(col);
	colBuf.Add(col);
	colBuf.Add(col);
	colBuf.Add(col);

	temporaryShape=shape;

	return YSOK;
}

YsRect3 RetroMap_World::MapPiece::GetBoundingBox(void) const
{
	return shape.GetBoundingBox();
}

YsRect2i RetroMap_World::MapPiece::GetBoundingBox2i(void) const
{
	return shape.GetBoundingBox2i();
}

RetroMap_World::MapPiece::Shape RetroMap_World::MapPiece::GetShape(void) const
{
	return shape;
}

RetroMap_World::MapPiece::Shape RetroMap_World::MapPiece::GetTemporaryShape(void) const
{
	return temporaryShape;
}

YsBitmap RetroMap_World::MapPiece::GetBitmap(void) const
{
	auto &texMan=GetTexMan();
	YsBitmap scaledBmp;

	ReadyTextureIfNotReady();

	auto texUnit=texMan.GetTexture(texHd);
	if(nullptr!=texUnit)
	{
		auto texBmp=texUnit->GetBitmap();

		YsBitmap trimmedBmp;
		auto bmpUsingSize=shape.visibleArea.GetSize();
		trimmedBmp.PrepareBitmap(bmpUsingSize.x(),bmpUsingSize.y());
		trimmedBmp.Copy(texBmp,shape.visibleArea[0].x(),shape.visibleArea[0].y(),0,0,bmpUsingSize.x(),bmpUsingSize.y());

		auto d=shape.GetBoundingBox2i().GetSize();
		if(shape.visibleArea.GetSize()==d)
		{
			return trimmedBmp;
		}
		else
		{
			scaledBmp.ScaleCopy(d.x(),d.y(),trimmedBmp);
			return scaledBmp;
		}
	}

	return scaledBmp;
}

////////////////////////////////////////////////////////////

static YsSystemFontCache sysFont;

const YsTextureManager &RetroMap_World::MarkUp::GetTexMan(void) const
{
	return owner->GetTexMan();
}
YsTextureManager &RetroMap_World::MarkUp::GetTexMan(void)
{
	return owner->GetTexMan();
}

void RetroMap_World::MarkUp::Move(YsVec2i displacement)
{
	if(MARKUP_STRING==markUpType)
	{
		pos+=displacement;
	}
	else if(MARKUP_POINT_ARRAY==markUpType)
	{
		for(auto &p : plg)
		{
			p+=displacement;
		}
	}
}

RetroMap_World::MarkUp::MarkUp()
{
	Initialize();
}
RetroMap_World::MarkUp &RetroMap_World::MarkUp::operator=(const MarkUp &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}
void RetroMap_World::MarkUp::CopyFrom(const MarkUp &incoming)
{
	if(this!=&incoming)
	{
		this->CleanUp();

		// Don't copy texHd

		this->owner=incoming.owner;

		this->markUp=incoming.markUp;

		// Common Properties >>
		this->markUpType=incoming.markUpType;
		this->fgCol=incoming.fgCol;
		this->bgCol=incoming.bgCol;
		this->pos=incoming.pos;
		// Common Properties <<

		// String >>
		this->fontSize=incoming.fontSize;
		this->txt=incoming.txt;
		this->txtSizeCache=incoming.txtSizeCache;
		// String <<

		// Multiple points >>
		this->plg=incoming.plg;
		this->line=incoming.line;
		this->fill=incoming.fill;
		this->closed=incoming.closed;
		this->frontArrow=incoming.frontArrow;
		this->backArrow=incoming.backArrow;
		// Multiple points <<
	}
}
RetroMap_World::MarkUp::~MarkUp()
{
	CleanUp();
}

void RetroMap_World::MarkUp::Initialize(void)
{
	owner=nullptr;
	texHd=nullptr;
	CleanUp();
}

void RetroMap_World::MarkUp::CleanUp(void)
{
	if(nullptr!=owner)
	{
		owner->GetTexMan().Delete(texHd);
	}

	texHd=nullptr;

	usingTentativePosition=YSFALSE;
	pos=pos.Origin();

	plg.CleanUp();
	line=YSTRUE;
	fill=YSFALSE;
	closed=YSFALSE;
	frontArrow.Initialize();
	backArrow.Initialize();

	fgCol=YsRed();
	bgCol.SetIntRGBA(0,0,0,0);
	highlightCol.SetIntRGBA(0,0,0,0);
	markerSize=0;

	markUpType=MARKUP_STRING;
	fontSize=24;
}

RetroMap_World::MarkUp::MARKUPTYPE RetroMap_World::MarkUp::GetMarkUpType(void) const
{
	return markUpType;
}

const YsArray <YsVec2i> RetroMap_World::MarkUp::GetPointArray(void) const
{
	return plg;
}

YSBOOL RetroMap_World::MarkUp::IsLoop(void) const
{
	if(MARKUP_POINT_ARRAY==GetMarkUpType())
	{
		return closed;
	}
	return YSFALSE;
}

YSBOOL RetroMap_World::MarkUp::IsDrawing(void) const
{
	if(MARKUP_POINT_ARRAY==GetMarkUpType())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

int RetroMap_World::MarkUp::GetFontSize(void) const
{
	return fontSize;
}

YsVec2i RetroMap_World::MarkUp::GetPosition(void) const
{
	return pos;
}

YsWString RetroMap_World::MarkUp::GetMarkUpText(void) const
{
	return txt;
}

YsRect3 RetroMap_World::MarkUp::GetBoundingBox(void) const
{
	if(MARKUP_STRING==markUpType)
	{
		YsVec3 min(pos.x(),pos.y(),-1.0);
		YsVec3 max(pos.x()+txtSizeCache.x(),pos.y()+txtSizeCache.y(), 1.0);
		return YsRect3(min,max);
	}
	else if(MARKUP_POINT_ARRAY==markUpType)
	{
		YsBoundingBoxMaker <YsVec2i> bbx(plg);
		YsRect2i rect(bbx[0],bbx[1]);
		YsVec3 min(rect[0].x(),rect[0].y(),-1.0);
		YsVec3 max(rect[1].x(),rect[1].y(), 1.0);
		return YsRect3(min,max);
	}
	return YsRect3(YsVec3::Origin(),YsVec3::Origin());
}

YsRect2i RetroMap_World::MarkUp::GetBoundingBox2i(void) const
{
	if(MARKUP_STRING==markUpType)
	{
		YsVec2i min(pos.x(),pos.y());
		YsVec2i max(pos.x()+txtSizeCache.x(),pos.y()+txtSizeCache.y());
		return YsRect2i(min,max);
	}
	else if(MARKUP_POINT_ARRAY==markUpType)
	{
		YsBoundingBoxMaker <YsVec2i> bbx(plg);
		return YsRect2i(bbx[0],bbx[1]);
	}
	return YsRect2i(YsVec2i::Origin(),YsVec2i::Origin());
}

void RetroMap_World::MarkUp::SetPosition(YsVec2i pos)
{
	this->pos=pos;
}

YsColor RetroMap_World::MarkUp::GetFgColor(void) const
{
	return this->fgCol;
}
void RetroMap_World::MarkUp::SetFgColor(YsColor col)
{
	this->fgCol=col;
}

YsColor RetroMap_World::MarkUp::GetBgColor(void) const
{
	return this->bgCol;
}
void RetroMap_World::MarkUp::SetBgColor(YsColor col)
{
	this->bgCol=col;
}

void RetroMap_World::MarkUp::MakeStringMarkUp(const YsWString &str)
{
	this->markUpType=MARKUP_STRING;
	this->txt=str;
	RemakeStringTexture();
}
void RetroMap_World::MarkUp::MakeStringMarkUp(const YsString &str)
{
	YsWString wstr;
	wstr.SetUTF8String(str);
	MakeStringMarkUp(wstr);
}

void RetroMap_World::MarkUp::MakePointArrayMarkUp(const YsConstArrayMask <YsVec2i> &plg)
{
	this->markUpType=MARKUP_POINT_ARRAY;
	this->plg=plg;
}

void RetroMap_World::MarkUp::RemakeStringTexture(void)
{
	if(MARKUP_STRING==markUpType)
	{
		sysFont.RequestDefaultFontWithHeight(fontSize);

		const unsigned char fgCol[3]=
		{
			(unsigned char)this->fgCol.Ri(),
			(unsigned char)this->fgCol.Gi(),
			(unsigned char)this->fgCol.Bi()
		};
		const unsigned char bgCol[3]=
		{
			(unsigned char)this->bgCol.Ri(),
			(unsigned char)this->bgCol.Gi(),
			(unsigned char)this->bgCol.Bi()
		};
		auto convTxt=txt;
		convTxt.ConvertEscape();
		auto txtBmp=sysFont.MakeRGBABitmap(convTxt,fgCol,bgCol,YSFALSE);

		auto wid=txtBmp.Width();
		auto hei=txtBmp.Height();
		auto rgba=txtBmp.TransferBitmap();

		txtSizeCache.Set(wid,hei);
		for(decltype(wid) i=0; i<wid*hei; ++i)
		{
			auto *pixPtr=rgba+i*4;
			if(pixPtr[0]==fgCol[0] && pixPtr[1]==fgCol[1] && pixPtr[2]==fgCol[2])
			{
				pixPtr[3]=this->fgCol.Ai();
			}
			else 
			{
				pixPtr[3]=this->bgCol.Ai();
			}
		}

		auto &texMan=GetTexMan();
		if(nullptr==texHd)
		{
			texHd=texMan.AddTexture(texMan.MakeUniqueLabel(),YsTextureManager::FOM_RAW_RGBA,wid,hei,YsConstArrayMask <unsigned char>(wid*hei*4,rgba));
		}
		else
		{
			texMan.SetTextureFileData(texHd,YsTextureManager::FOM_RAW_RGBA,wid,hei,YsConstArrayMask <unsigned char>(wid*hei*4,rgba));
		}
		texMan.SetDontSave(texHd,YSTRUE);

		delete [] rgba;

		printf("Created %dx%d bitmap for text mark up.\n",wid,hei);
	}
}

int RetroMap_World::MarkUp::GetMarkerSize(void) const
{
	return markerSize;
}
void RetroMap_World::MarkUp::SetMarkerSize(int s) const
{
	markerSize=s;
}
YsColor RetroMap_World::MarkUp::GetHighlightColor(void) const
{
	return highlightCol;
}
void RetroMap_World::MarkUp::SetHighlightColor(YsColor col) const
{
	highlightCol=col;
}

YSRESULT RetroMap_World::MarkUp::ReadyTexture(void)
{
	if(MARKUP_STRING==markUpType)
	{
		auto &texMan=GetTexMan();
		auto unitPtr=texMan.GetTexture(texHd);
		if(nullptr!=unitPtr)
		{
			return unitPtr->MakeActualTexture();
		}
		return YSERR;
	}
	return YSOK;
}
YSRESULT RetroMap_World::MarkUp::ReadyVbo(void)
{
	YsColor trans;
	trans.SetIntRGBA(0,0,0,0);
	auto res=ReadyVbo(pos,trans);
	SetMarkerSize(0);
	usingTentativePosition=YSFALSE;
	return res;
}
YSRESULT RetroMap_World::MarkUp::ReadyVbo(YsVec2i pos,YsColor hlCol)
{
	usingTentativePosition=YSTRUE;

	vtxBuf.CleanUp();
	highlightCol=hlCol;
	texCoordBuf.CleanUp();
	colBuf.CleanUp();
	filColBuf.CleanUp();
	frameVtxBuf.CleanUp();

	arrowVtxBuf.CleanUp();
	arrowColBuf.CleanUp();

	if(MARKUP_STRING==markUpType)
	{
		YsVec2i bmpSize=txtSizeCache;
		auto &texMan=GetTexMan();
		auto unitPtr=texMan.GetTexture(texHd);
		if(nullptr!=unitPtr)
		{
			bmpSize.Set(unitPtr->GetWidth(),unitPtr->GetHeight());
		}

		double uMin=0.0;
		double vMin=0.0;
		double uMax=(double)txtSizeCache.x()/(double)bmpSize.x();
		double vMax=(double)txtSizeCache.y()/(double)bmpSize.y();

		YsVec3 quadVtPos[4];
		YsVec2 quadTexCoord[4];

		quadVtPos[0]=YsVec3(pos.x()                 ,pos.y(),0.0);
		quadTexCoord[0]=YsVec2(uMin,vMin);

		quadVtPos[1]=YsVec3(pos.x()+txtSizeCache.x(),pos.y(),0.0);
		quadTexCoord[1]=YsVec2(uMax,vMin);

		quadVtPos[2]=YsVec3(pos.x()+txtSizeCache.x(),pos.y()+txtSizeCache.y(),0.0);
		quadTexCoord[2]=YsVec2(uMax,vMax);

		quadVtPos[3]=YsVec3(pos.x()                 ,pos.y()+txtSizeCache.y(),0.0);
		quadTexCoord[3]=YsVec2(uMin,vMax);

		YsColor col;
		col.SetDoubleRGBA(1,1,1,1);

		vtxBuf.Add(quadVtPos[0]);
		vtxBuf.Add(quadVtPos[1]);
		vtxBuf.Add(quadVtPos[2]);
		vtxBuf.Add(quadVtPos[3]);
		texCoordBuf.Add(quadTexCoord[0]);
		texCoordBuf.Add(quadTexCoord[1]);
		texCoordBuf.Add(quadTexCoord[2]);
		texCoordBuf.Add(quadTexCoord[3]);
		colBuf.Add(col);
		colBuf.Add(col);
		colBuf.Add(col);
		colBuf.Add(col);

		frameVtxBuf.Add(quadVtPos[0]);
		frameVtxBuf.Add(quadVtPos[1]);
		frameVtxBuf.Add(quadVtPos[2]);
		frameVtxBuf.Add(quadVtPos[3]);

		return YSOK;
	}
	else if(MARKUP_POINT_ARRAY==markUpType)
	{
		for(auto p : plg)
		{
			vtxBuf.Add(YsVec3(p.x(),p.y(),0));
			colBuf.Add(fgCol);
			filColBuf.Add(bgCol);
		}
		auto rect=GetBoundingBox2i();
		frameVtxBuf.Add(YsVec3(rect[0].x(),rect[0].y(),0));
		frameVtxBuf.Add(YsVec3(rect[1].x(),rect[0].y(),0));
		frameVtxBuf.Add(YsVec3(rect[1].x(),rect[1].y(),0));
		frameVtxBuf.Add(YsVec3(rect[0].x(),rect[1].y(),0));

		if(2<=plg.size())
		{
			for(int i=0; i<2; ++i)
			{
				const ArrowHeadInfo *arrowInfoPtr=(0==i ? &frontArrow : &backArrow);
				YsVec2i p1,p2;
				if(0==i)
				{
					p1=plg[1];
					p2=plg[0];
				}
				else
				{
					p1=plg[plg.size()-2];
					p2=plg.back();
				}
				arrowInfoPtr->MakeVertexArray(arrowVtxBuf,p1,p2);
			}
			for(int j=0; j<arrowVtxBuf.GetN(); ++j)
			{
				arrowColBuf.Add(fgCol);
			}
		}
	}
	return YSERR;
}

YSRESULT RetroMap_World::MarkUp::ReadyVboWithDisplacement(const YsVec2i disp,YsColor hlCol)
{
	usingTentativePosition=YSTRUE;

	vtxBuf.CleanUp();
	highlightCol=hlCol;
	texCoordBuf.CleanUp();
	colBuf.CleanUp();
	filColBuf.CleanUp();
	frameVtxBuf.CleanUp();
	if(MARKUP_STRING==markUpType)
	{
		YsVec2i bmpSize=txtSizeCache;
		auto &texMan=GetTexMan();
		auto unitPtr=texMan.GetTexture(texHd);
		if(nullptr!=unitPtr)
		{
			bmpSize.Set(unitPtr->GetWidth(),unitPtr->GetHeight());
		}

		double uMin=0.0;
		double vMin=0.0;
		double uMax=(double)txtSizeCache.x()/(double)bmpSize.x();
		double vMax=(double)txtSizeCache.y()/(double)bmpSize.y();

		YsVec3 quadVtPos[4];
		YsVec2 quadTexCoord[4];

		auto pos=this->pos+disp;
		quadVtPos[0]=YsVec3(pos.x()                 ,pos.y(),0.0);
		quadTexCoord[0]=YsVec2(uMin,vMin);

		quadVtPos[1]=YsVec3(pos.x()+txtSizeCache.x(),pos.y(),0.0);
		quadTexCoord[1]=YsVec2(uMax,vMin);

		quadVtPos[2]=YsVec3(pos.x()+txtSizeCache.x(),pos.y()+txtSizeCache.y(),0.0);
		quadTexCoord[2]=YsVec2(uMax,vMax);

		quadVtPos[3]=YsVec3(pos.x()                 ,pos.y()+txtSizeCache.y(),0.0);
		quadTexCoord[3]=YsVec2(uMin,vMax);

		YsColor col;
		col.SetDoubleRGBA(1,1,1,1);

		vtxBuf.Add(quadVtPos[0]);
		vtxBuf.Add(quadVtPos[1]);
		vtxBuf.Add(quadVtPos[2]);
		vtxBuf.Add(quadVtPos[3]);
		texCoordBuf.Add(quadTexCoord[0]);
		texCoordBuf.Add(quadTexCoord[1]);
		texCoordBuf.Add(quadTexCoord[2]);
		texCoordBuf.Add(quadTexCoord[3]);
		colBuf.Add(col);
		colBuf.Add(col);
		colBuf.Add(col);
		colBuf.Add(col);

		frameVtxBuf.Add(quadVtPos[0]);
		frameVtxBuf.Add(quadVtPos[1]);
		frameVtxBuf.Add(quadVtPos[2]);
		frameVtxBuf.Add(quadVtPos[3]);
		return YSOK;
	}
	else if(MARKUP_POINT_ARRAY==markUpType)
	{
		YsBoundingBoxMaker <YsVec2i> rect;
		for(auto p : plg)
		{
			p+=disp;
			rect.Add(p);
			vtxBuf.Add(YsVec3(p.x(),p.y(),0));
			colBuf.Add(fgCol);
			filColBuf.Add(bgCol);
		}
		frameVtxBuf.Add(YsVec3(rect[0].x(),rect[0].y(),0));
		frameVtxBuf.Add(YsVec3(rect[1].x(),rect[0].y(),0));
		frameVtxBuf.Add(YsVec3(rect[1].x(),rect[1].y(),0));
		frameVtxBuf.Add(YsVec3(rect[0].x(),rect[1].y(),0));
		return YSOK;
	}
	return YSERR;
}

YSRESULT RetroMap_World::MarkUp::ReadyVboWithModifiedPointArray(const YsConstArrayMask <YsVec2i> &aryMask,YsColor hlCol)
{
	usingTentativePosition=YSTRUE;

	vtxBuf.CleanUp();
	highlightCol=hlCol;
	texCoordBuf.CleanUp();
	colBuf.CleanUp();
	filColBuf.CleanUp();
	frameVtxBuf.CleanUp();
	if(MARKUP_POINT_ARRAY==markUpType)
	{
		YsBoundingBoxMaker <YsVec2i> rect;
		for(auto p : aryMask)
		{
			rect.Add(p);
			vtxBuf.Add(YsVec3(p.x(),p.y(),0));
			colBuf.Add(fgCol);
			filColBuf.Add(bgCol);
		}
		frameVtxBuf.Add(YsVec3(rect[0].x(),rect[0].y(),0));
		frameVtxBuf.Add(YsVec3(rect[1].x(),rect[0].y(),0));
		frameVtxBuf.Add(YsVec3(rect[1].x(),rect[1].y(),0));
		frameVtxBuf.Add(YsVec3(rect[0].x(),rect[1].y(),0));
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////


RetroMap_World::MarkUpAnchor::MarkUpAnchor()
{
	CleanUp();
}
void RetroMap_World::MarkUpAnchor::CleanUp(void)
{
	markUpKey=YSNULLHASHKEY;
	baseMapPieceKey=YSNULLHASHKEY;
	baseMarkUpKey=YSNULLHASHKEY;
	markUpPointIdx=0;
	relPosToBase.Set(0,0);
}
void RetroMap_World::MarkUpAnchor::SetMarkUp(const class Field &field,MarkUpHandle muHd)
{
	markUpKey=field.GetSearchKey(muHd);
}
RetroMap_World::MarkUpHandle RetroMap_World::MarkUpAnchor::GetMarkUp(const class Field &field) const
{
	return field.FindMarkUp(markUpKey);
}
void RetroMap_World::MarkUpAnchor::SetBaseMapPiece(const class Field &field,MapPieceHandle mpHd)
{
	baseMapPieceKey=field.GetSearchKey(mpHd);
	baseMarkUpKey=YSNULLHASHKEY;
}
RetroMap_World::MapPieceHandle RetroMap_World::MarkUpAnchor::GetBaseMapPiece(const class Field &field) const
{
	return field.FindMapPiece(baseMapPieceKey);
}
RetroMap_World::MarkUpHandle RetroMap_World::MarkUpAnchor::GetBaseMarkUp(const class Field &field) const
{
	return field.FindMarkUp(baseMarkUpKey);
}
void RetroMap_World::MarkUpAnchor::SetMarkUpPointIndex(YSSIZE_T idx)
{
	markUpPointIdx=idx;
}
YSSIZE_T RetroMap_World::MarkUpAnchor::GetMarkUpPointIndex(void) const
{
	return markUpPointIdx;
}
void RetroMap_World::MarkUpAnchor::SetRelPos(YsVec2i relPos)
{
	relPosToBase=relPos;
}
YsVec2i RetroMap_World::MarkUpAnchor::GetRelPos(void) const
{
	return relPosToBase;
}


////////////////////////////////////////////////////////////

RetroMap_World::Field::Field()
{
	CleanUp();
}
RetroMap_World::Field::~Field()
{
	CleanUp();
}
void RetroMap_World::Field::CleanUp(void)
{
	fieldName=L"";

	{
		MapPieceHandle mpHd=nullptr;
		while(YSOK==mapPiece.MoveToNextIncludingFrozen(mpHd) && nullptr!=mpHd)
		{
			mapPiece.GetObjectMaybeFrozen(mpHd)->CleanUp();
		}
	}
	mapPiece.CleanUp();
	mapPiece.EnableSearch();

	{
		MarkUpHandle muHd=nullptr;
		while(YSOK==markUp.MoveToNextIncludingFrozen(muHd) && nullptr!=muHd)
		{
			markUp.GetObjectMaybeFrozen(muHd)->CleanUp();
		}
	}
	markUp.CleanUp();
	markUp.EnableSearch();

	anchor.CleanUp();
	baseMapPieceKeyToAnchor.CleanUp();
	baseMarkUpKeyToAnchor.CleanUp();
	markUpKeyToAnchor.CleanUp();

	selectedMapPiece.CleanUp();
	selectedMapPieceKey.CleanUp();
	selectedMarkUpKey.CleanUp();
}

YSHASHKEY RetroMap_World::Field::GetSearchKey(MarkUpHandle muHd) const
{
	return this->markUp.GetSearchKey(muHd);
}
YSHASHKEY RetroMap_World::Field::GetSearchKey(MapPieceHandle mpHd) const
{
	return this->mapPiece.GetSearchKey(mpHd);
}
RetroMap_World::MarkUpHandle RetroMap_World::Field::FindMarkUp(YSHASHKEY key) const
{
	return this->markUp.FindObject(key);
}
RetroMap_World::MapPieceHandle RetroMap_World::Field::FindMapPiece(YSHASHKEY key) const
{
	return this->mapPiece.FindObject(key);
}

const YsTextureManager &RetroMap_World::Field::GetTexMan(void) const
{
	return owner->texMan;
}
YsTextureManager &RetroMap_World::Field::GetTexMan(void)
{
	return owner->texMan;
}

void RetroMap_World::Field::SetTexture(MapPieceHandle mpHd,YsTextureManager::TexHandle texHd)
{
	auto mpPtr=mapPiece[mpHd];
	if(nullptr!=mpPtr)
	{
		mpPtr->texHd=texHd;
	}
}

void RetroMap_World::Field::MakeMapPieceBitmap(MakeBitmapThreadInfo *info) const
{
	auto &texMan=GetTexMan();

	auto mpHd=info->mpHd;
	auto mpPtr=mapPiece[mpHd];

	// Did I need the following two lines?
	// mpPtr->ReadyTextureIfNotReady();
	// mpPtr->ReadyVboIfNotReady(); // May not be necessary.

	auto texUnit=texMan.GetTexture(mpPtr->texHd);
	auto texBmp=texUnit->GetBitmap();

	auto shape=mpPtr->shape;
	auto bbx=shape.GetBoundingBox2i();

	YsBitmap trimmedBmp;
	auto bmpUsingSize=shape.visibleArea.GetSize();
	trimmedBmp.PrepareBitmap(bmpUsingSize.x(),bmpUsingSize.y());
	trimmedBmp.Copy(texBmp,shape.visibleArea[0].x(),shape.visibleArea[0].y(),0,0,bmpUsingSize.x(),bmpUsingSize.y());

	YsBitmap scaledBmp;
	auto d=bbx.GetSize();
	if(bmpUsingSize==d)
	{
		scaledBmp.MoveFrom(trimmedBmp);
	}
	else
	{
		scaledBmp.ScaleCopy(d.x(),d.y(),trimmedBmp);
	}

	info->bmpPtr->MoveFrom(scaledBmp);
}

YsBitmap RetroMap_World::Field::MakeBitmap(YsVec2i origin,YsVec2i bmpSize,YsColor bgCol,YSBOOL drawMapPiece,YSBOOL drawMarkUp,const MapPieceStore &excludeMapPiece) const
{
	YsBitmap bmp;
	YsBitmapDrawing drawing(bmp);
	bmp.PrepareBitmap(bmpSize.x(),bmpSize.y());
	bmp.Clear(bgCol.Ri(),bgCol.Gi(),bgCol.Bi(),bgCol.Ai());

	YsRect2i clipRect(origin,origin+bmpSize);

	auto &texMan=GetTexMan();

	if(YSTRUE==drawMapPiece)
	{
		YsArray <MapPieceHandle> toRender;

		for(auto mpHd : mapPiece.AllHandle())
		{
			if(YSTRUE!=excludeMapPiece.IsIncluded(mpHd))
			{
				auto mpPtr=mapPiece[mpHd];
				auto shape=mpPtr->shape;
				auto bbx=shape.GetBoundingBox2i();

				if(YSTRUE!=clipRect.DoesOverlap(bbx))
				{
					continue;
				}

				toRender.push_back(mpHd);
			}
		}

		const int nThread=8;
		for(YSSIZE_T baseIdx=0; baseIdx<toRender.size(); baseIdx+=nThread)
		{
			YsBitmap mpBmp[nThread];
			std::thread thr[nThread];
			MakeBitmapThreadInfo thrInfo[nThread];

			for(YSSIZE_T mpIdx=0; mpIdx<nThread && baseIdx+mpIdx<toRender.size(); ++mpIdx)
			{
				thrInfo[mpIdx].mpHd=toRender[baseIdx+mpIdx];
				thrInfo[mpIdx].bmpPtr=&mpBmp[mpIdx];
				std::thread t(&RetroMap_World::Field::MakeMapPieceBitmap,this,&thrInfo[mpIdx]);
				thr[mpIdx].swap(t);
			}
			for(YSSIZE_T mpIdx=0; mpIdx<nThread && baseIdx+mpIdx<toRender.size(); ++mpIdx)
			{
				thr[mpIdx].join();
			}
			for(YSSIZE_T mpIdx=0; mpIdx<nThread && baseIdx+mpIdx<toRender.size(); ++mpIdx)
			{
				auto mpHd=toRender[baseIdx+mpIdx];

				auto mpPtr=mapPiece[mpHd];
				auto shape=mpPtr->shape;
				auto bbx=shape.GetBoundingBox2i();

				auto &scaledBmp=mpBmp[mpIdx];

				auto posInBigBitmap=bbx[0]-origin;
				bmp.Copy(scaledBmp,posInBigBitmap.x(),posInBigBitmap.y());
			}
		}
	}
	if(YSTRUE==drawMarkUp)
	{
		for(auto muHd : markUp.AllHandle())
		{
			auto muPtr=markUp[muHd];
			if(muPtr->GetMarkUpType()==muPtr->MARKUP_STRING)
			{
				auto texUnit=texMan.GetTexture(muPtr->texHd);
				auto texBmp=texUnit->GetBitmap();
				auto posInBigBitmap=muPtr->pos-origin;
				bmp.Copy(texBmp,posInBigBitmap.x(),posInBigBitmap.y());
			}
			else if(muPtr->GetMarkUpType()==muPtr->MARKUP_POINT_ARRAY)
			{
				YsBitmapDrawing::LineOption opt;
				opt.SetColor(muPtr->fgCol);
				if(YSTRUE==muPtr->line)
				{
					for(auto idx : muPtr->plg.AllIndex())
					{
						auto p1=muPtr->plg[idx]-origin;
						auto p2=muPtr->plg.GetCyclic(idx+1)-origin;
						drawing.DrawLine(p1.x(),p1.y(),p2.x(),p2.y(),opt);
						if(YSTRUE!=muPtr->closed && idx==muPtr->plg.GetN()-1)
						{
							break;
						}
					}
				}
			}
		}
	}
	return bmp;
}

YsBitmap RetroMap_World::Field::MakeBitmap(YSBOOL mapPiece,YSBOOL markUp) const
{
	MapPieceStore excludeMapPiece;
	auto bbx=GetBoundingBox2i();
	return MakeBitmap(bbx[0],bbx.GetSize(),YsWhite(),mapPiece,markUp,excludeMapPiece);
}

YSRESULT RetroMap_World::Field::FreezeAnchor(AnchorHandle anHd)
{
	auto *anchorPtr=GetAnchor(anHd);
	if(nullptr!=anchorPtr)
	{
		UnregisterAnchor(anHd);
		anchor.Freeze(anHd);
		return YSOK;
	}
	return YSERR;
}
YSRESULT RetroMap_World::Field::MeltAnchor(AnchorHandle anHd)
{
	if(nullptr!=anHd && YSOK==anchor.Melt(anHd))
	{
		RegisterAnchor(anHd);
		return YSOK;
	}
	return YSERR;
}
RetroMap_World::AnchorHandle RetroMap_World::Field::CreateAnchor(MarkUpHandle muHd,YSSIZE_T idx,MapPieceHandle baseMpHd,YsVec2i relPos)
{
	if(nullptr!=FindAnchorFromMarkUp(muHd,idx) || nullptr==muHd || nullptr==baseMpHd)
	{
		return nullptr;
	}
	AnchorHandle anHd=anchor.Create();
	auto anchorPtr=anchor[anHd];
	anchor[anHd]->CleanUp();
	anchorPtr->markUpKey=GetSearchKey(muHd);
	anchorPtr->baseMapPieceKey=GetSearchKey(baseMpHd);
	anchorPtr->markUpPointIdx=idx;
	anchorPtr->relPosToBase=relPos;
	RegisterAnchor(anHd);

	return anHd;
}
RetroMap_World::AnchorHandle RetroMap_World::Field::CreateAnchor(MarkUpHandle muHd,YSSIZE_T idx,MarkUpHandle baseMuHd,YsVec2i relPos)
{
	if(nullptr!=FindAnchorFromMarkUp(muHd,idx) || nullptr==muHd || nullptr==baseMuHd)
	{
		return nullptr;
	}
	AnchorHandle anHd=anchor.Create();
	auto anchorPtr=anchor[anHd];
	anchor[anHd]->CleanUp();
	anchorPtr->markUpKey=GetSearchKey(muHd);
	anchorPtr->baseMarkUpKey=GetSearchKey(baseMuHd);
	anchorPtr->markUpPointIdx=idx;
	anchorPtr->relPosToBase=relPos;
	RegisterAnchor(anHd);

	return anHd;
}
void RetroMap_World::Field::RegisterAnchor(AnchorHandle anHd)
{
	auto anchorPtr=anchor[anHd];
	if(nullptr==markUpKeyToAnchor[anchorPtr->markUpKey])
	{
		YsArray <AnchorHandle> empty;
		markUpKeyToAnchor.Update(anchorPtr->markUpKey,empty);
	}
	markUpKeyToAnchor[anchorPtr->markUpKey]->push_back(anHd);

	if(YSNULLHASHKEY!=anchorPtr->baseMapPieceKey)
	{
		if(nullptr==baseMapPieceKeyToAnchor[anchorPtr->baseMapPieceKey])
		{
			YsArray <AnchorHandle> empty;
			baseMapPieceKeyToAnchor.Update(anchorPtr->baseMapPieceKey,empty);
		}
		baseMapPieceKeyToAnchor[anchorPtr->baseMapPieceKey]->push_back(anHd);
	}
	if(YSNULLHASHKEY!=anchorPtr->baseMarkUpKey)
	{
		if(nullptr==baseMarkUpKeyToAnchor[anchorPtr->baseMarkUpKey])
		{
			YsArray <AnchorHandle> empty;
			baseMarkUpKeyToAnchor.Update(anchorPtr->baseMarkUpKey,empty);
		}
		baseMarkUpKeyToAnchor[anchorPtr->baseMarkUpKey]->push_back(anHd);
	}

}
void RetroMap_World::Field::UnregisterAnchor(AnchorHandle anHd)
{
	auto anchorPtr=anchor[anHd];
	auto markUpAnchorPtr=markUpKeyToAnchor[anchorPtr->markUpKey];
	if(nullptr!=markUpAnchorPtr) // Should be non-nullptr...
	{
		for(auto idx : markUpAnchorPtr->ReverseIndex())
		{
			if((*markUpAnchorPtr)[idx]==anHd)
			{
				markUpAnchorPtr->Delete(idx);
			}
		}
	}
	if(YSNULLHASHKEY!=anchorPtr->baseMapPieceKey)
	{
		auto baseMapPieceAnchorPtr=baseMapPieceKeyToAnchor[anchorPtr->baseMapPieceKey];
		if(nullptr!=baseMapPieceAnchorPtr)
		{
			for(auto idx : baseMapPieceAnchorPtr->ReverseIndex())
			{
				if((*baseMapPieceAnchorPtr)[idx]==anHd)
				{
					baseMapPieceAnchorPtr->Delete(idx);
				}
			}
		}
	}
	if(YSNULLHASHKEY!=anchorPtr->baseMarkUpKey)
	{
		auto baseMarkUpAnchorPtr=baseMarkUpKeyToAnchor[anchorPtr->baseMarkUpKey];
		if(nullptr!=baseMarkUpAnchorPtr)
		{
			for(auto idx : baseMarkUpAnchorPtr->ReverseIndex())
			{
				if((*baseMarkUpAnchorPtr)[idx]==anHd)
				{
					baseMarkUpAnchorPtr->Delete(idx);
				}
			}
		}
	}
}
RetroMap_World::AnchorHandle RetroMap_World::Field::FindAnchorFromMarkUp(MarkUpHandle muHd, YSSIZE_T idx) const
{
	auto markUpToAnchorPtr=markUpKeyToAnchor[GetSearchKey(muHd)];
	if(nullptr!=markUpToAnchorPtr)
	{
		for(auto anHd : (*markUpToAnchorPtr))
		{
			auto anchorPtr=GetAnchor(anHd);
			if(anchorPtr->markUpKey==GetSearchKey(muHd) &&
			   anchorPtr->markUpPointIdx==idx)
			{
				return anHd;
			}
		}
	}
	return nullptr;
}
YsConstArrayMask <RetroMap_World::AnchorHandle> RetroMap_World::Field::FindAnchorFromMarkUp(MarkUpHandle muHd) const
{
	auto found=markUpKeyToAnchor[GetSearchKey(muHd)];
	if(nullptr!=found)
	{
		return *found;
	}
	return YsConstArrayMask <RetroMap_World::AnchorHandle> (0,nullptr);
}
YsConstArrayMask <RetroMap_World::AnchorHandle> RetroMap_World::Field::FindAnchorFromBaseMapPiece(MapPieceHandle mpHd) const
{
	auto found=baseMapPieceKeyToAnchor[GetSearchKey(mpHd)];
	if(nullptr!=found)
	{
		return *found;
	}
	return YsConstArrayMask <RetroMap_World::AnchorHandle> (0,nullptr);
}
YsConstArrayMask <RetroMap_World::AnchorHandle> RetroMap_World::Field::FindAnchorFromBaseMarkUp(MarkUpHandle muHd) const
{
	auto found=baseMarkUpKeyToAnchor[GetSearchKey(muHd)];
	if(nullptr!=found)
	{
		return *found;
	}
	return YsConstArrayMask <RetroMap_World::AnchorHandle> (0,nullptr);
}
const RetroMap_World::MarkUpAnchor *RetroMap_World::Field::GetAnchor(AnchorHandle anHd) const
{
	return anchor[anHd];
}
RetroMap_World::MarkUpAnchor *RetroMap_World::Field::GetEditableAnchor(AnchorHandle anHd)
{
	return anchor[anHd];
}

YsVec2i RetroMap_World::Field::GetAnchorPosition(AnchorHandle anHd) const
{
	auto anchorPtr=GetAnchor(anHd);
	auto baseMpHd=anchorPtr->GetBaseMapPiece(*this);
	if(nullptr!=baseMpHd)
	{
		auto mapPiecePtr=GetMapPiece(baseMpHd);
		return mapPiecePtr->GetPosition()+anchorPtr->relPosToBase;
	}
	auto baseMuHd=anchorPtr->GetBaseMarkUp(*this);
	if(nullptr!=baseMuHd)
	{
		auto markUpPtr=GetMarkUp(baseMuHd);
		return markUpPtr->GetPosition()+anchorPtr->relPosToBase;
	}
	return YsVec2i(0,0);
}

void RetroMap_World::Field::MakeAllVisible(MapPieceHandle mpHd)
{
	auto mpPtr=mapPiece[mpHd];
	if(nullptr!=mpPtr)
	{
		auto &texMan=GetTexMan();
		auto unitPtr=texMan.GetTexture(mpPtr->texHd);
		if(nullptr!=unitPtr)
		{
			mpPtr->shape.visibleArea.Set(YsVec2i::Origin(),unitPtr->GetSize());
		}
	}
}

class RetroMap_World::UndoMoveMapPieceToFront : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MapPieceHandle whatsMoved;
	void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.GetField(fdHd);
		if(nullptr!=fieldPtr && whatsMoved!=fieldPtr->mapPiece.Last())
		{
			auto mpHd=fieldPtr->mapPiece.Last();
			while(nullptr!=mpHd && mpHd!=whatsMoved)
			{
				auto prevMpHd=fieldPtr->mapPiece.FindPrev(mpHd);
				world.Swap(fdHd,mpHd,prevMpHd);
				mpHd=prevMpHd;
			}
		}
	}
	void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.GetField(fdHd);
		if(nullptr!=fieldPtr && whatsMoved!=fieldPtr->mapPiece.Last())
		{
			auto mpHd=whatsMoved;
			while(nullptr!=mpHd && nullptr!=fieldPtr->mapPiece.FindNext(mpHd))
			{
				auto nextMpHd=fieldPtr->mapPiece.FindNext(mpHd);
				world.Swap(fdHd,mpHd,nextMpHd);
				mpHd=nextMpHd;
			}
		}
	}
	FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::MoveToFront(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		if(mpHd!=fieldPtr->mapPiece.Last())
		{
			auto undoPtr=NewUndoLog<UndoMoveMapPieceToFront>();
			if(nullptr!=undoPtr)
			{
				undoPtr->fdHd=fdHd;
				undoPtr->whatsMoved=mpHd;
			}
			while(nullptr!=mpHd && nullptr!=fieldPtr->mapPiece.FindNext(mpHd))
			{
				auto nextMpHd=fieldPtr->mapPiece.FindNext(mpHd);
				Swap(fdHd,mpHd,nextMpHd);
				mpHd=nextMpHd;
			}
		}
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoChangeMapPiecePriority : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MapPieceHandle fromMpHd,toMpHd;
	void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->ChangePriority(toMpHd,fromMpHd);
		}
	}
	void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->ChangePriority(fromMpHd,toMpHd);
		}
	}
	FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::ChangePriority(FieldHandle fdHd,MapPieceHandle fromMpHd,MapPieceHandle toMpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		if(fromMpHd!=toMpHd)
		{
			auto undoPtr=NewUndoLog<UndoChangeMapPiecePriority>();
			if(nullptr!=undoPtr)
			{
				undoPtr->fdHd=fdHd;
				undoPtr->fromMpHd=fromMpHd;
				undoPtr->toMpHd=toMpHd;
			}
			fieldPtr->ChangePriority(fromMpHd,toMpHd);
		}
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoCreateAnchor : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	AnchorHandle anHd;

	void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->FreezeAnchor(anHd);
		}
	}
	void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->MeltAnchor(anHd);
		}
	}
	FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
RetroMap_World::AnchorHandle RetroMap_World::CreateAnchor(FieldHandle fdHd,MarkUpHandle muHd,YSSIZE_T idx,MapPieceHandle baseMpHd,YsVec2i relPos)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto anHd=fieldPtr->CreateAnchor(muHd,idx,baseMpHd,relPos);
		if(nullptr!=anHd)
		{
			auto undoPtr=NewUndoLog<UndoCreateAnchor>();
			if(nullptr!=undoPtr)
			{
				undoPtr->fdHd=fdHd;
				undoPtr->anHd=anHd;
			}
			return anHd;
		}
	}
	return nullptr;
}
RetroMap_World::AnchorHandle RetroMap_World::CreateAnchor(FieldHandle fdHd,MarkUpHandle muHd,YSSIZE_T idx,MarkUpHandle baseMuHd,YsVec2i relPos)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto anHd=fieldPtr->CreateAnchor(muHd,idx,baseMuHd,relPos);
		if(nullptr!=anHd)
		{
			auto undoPtr=NewUndoLog<UndoCreateAnchor>();
			if(nullptr!=undoPtr)
			{
				undoPtr->fdHd=fdHd;
				undoPtr->anHd=anHd;
			}
			return anHd;
		}
	}
	return nullptr;
}

class RetroMap_World::UndoDeleteAnchor : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	AnchorHandle anHd;

	void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->MeltAnchor(anHd);
		}
	}
	void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->FreezeAnchor(anHd);
		}
	}
	FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::DeleteAnchor(FieldHandle fdHd,AnchorHandle anHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		if(YSOK==fieldPtr->FreezeAnchor(anHd))
		{
			auto undoPtr=NewUndoLog<UndoDeleteAnchor>();
			if(nullptr!=undoPtr)
			{
				undoPtr->fdHd=fdHd;
				undoPtr->anHd=anHd;
			}
			return YSOK;
		}
	}
	return YSERR;
}

void RetroMap_World::Field::MoveFrom(Field &incoming)
{
	if(this!=&incoming)
	{
		this->owner=incoming.owner;
		this->fieldName.MoveFrom(incoming.fieldName);
		this->mapPiece.MoveFrom(incoming.mapPiece);
		this->markUp.MoveFrom(incoming.markUp);
		this->anchor.MoveFrom(incoming.anchor);
		this->RemakeAnchorSearchTable();

		this->selectedMapPiece.MoveFrom(incoming.selectedMapPiece);
		this->selectedMapPieceKey.MoveFrom(incoming.selectedMapPieceKey);
		this->selectedMarkUpKey.MoveFrom(incoming.selectedMarkUpKey);

		this->savedView.MoveFrom(incoming.savedView);

		for(auto &mp : mapPiece)
		{
			mp.owner=this;
		}
		for(auto &mu : markUp)
		{
			mu.owner=this;
		}
	}
}

void RetroMap_World::Field::Swap(Field &incoming)
{
	Field tmp;
	tmp.MoveFrom(incoming);
	incoming.MoveFrom(*this);
	this->MoveFrom(tmp);
}

YsEditArray<RetroMap_World::MapPiece,4>::HandleEnumerator RetroMap_World::Field::AllMapPiece(void) const
{
	return mapPiece.AllHandle();
}

YsEditArray <RetroMap_World::MarkUp,4>::HandleEnumerator RetroMap_World::Field::AllMarkUp(void) const
{
	return markUp.AllHandle();
}
YsEditArray <RetroMap_World::MarkUpAnchor,4>::HandleEnumerator RetroMap_World::Field::AllAnchor(void) const
{
	return anchor.AllHandle();
}

YSSIZE_T RetroMap_World::Field::GetNumMapPiece(void) const
{
	return mapPiece.GetN();
}

YsEditArrayObjectHandle <RetroMap_World::MapPiece,4> RetroMap_World::Field::LastMapPiece(void) const
{
	return mapPiece.Last();
}
YsEditArrayObjectHandle <RetroMap_World::MapPiece,4> RetroMap_World::Field::FirstMapPiece(void) const
{
	return mapPiece.First();
}
YsEditArrayObjectHandle <RetroMap_World::MapPiece,4> RetroMap_World::Field::FindNext(YsEditArrayObjectHandle <MapPiece,4> mpHd) const
{
	return mapPiece.FindNext(mpHd);
}
YsEditArrayObjectHandle <RetroMap_World::MapPiece,4> RetroMap_World::Field::FindPrev(YsEditArrayObjectHandle <MapPiece,4> mpHd) const
{
	return mapPiece.FindPrev(mpHd);
}

YSSIZE_T RetroMap_World::Field::GetNumMarkUp(void) const
{
	return markUp.GetN();
}

RetroMap_World::MapPieceHandle RetroMap_World::Field::CreateMapPiece(void)
{
	auto mpHd=mapPiece.Create();
	auto mpPtr=mapPiece[mpHd];
	if(nullptr!=mpPtr)
	{
		mpPtr->Initialize();
		mpPtr->owner=this;
		return mpHd;
	}
	return nullptr;
}

RetroMap_World::MarkUpHandle RetroMap_World::Field::CreateMarkUp(void)
{
	auto muHd=markUp.Create();
	auto muPtr=markUp[muHd];
	if(nullptr!=muPtr)
	{
		muPtr->Initialize();
		muPtr->owner=this;
		return muHd;
	}
	return nullptr;
}

const RetroMap_World::MapPiece *RetroMap_World::Field::GetMapPiece(MapPieceHandle mpHd) const
{
	return mapPiece[mpHd];
}

const RetroMap_World::MarkUp *RetroMap_World::Field::GetMarkUp(MarkUpHandle muHd) const
{
	return markUp[muHd];
}

YsRect3 RetroMap_World::Field::GetBoundingBox(void) const
{
	YsBoundingBoxMaker3 mkBbx;

	for(auto mpHd : AllMapPiece())
	{
		auto bbx=mapPiece[mpHd]->GetBoundingBox();
		mkBbx.Add(bbx.Min());
		mkBbx.Add(bbx.Max());
	}
	for(auto muHd : AllMarkUp())
	{
		auto bbx=markUp[muHd]->GetBoundingBox();
		mkBbx.Add(bbx.Min());
		mkBbx.Add(bbx.Max());
	}

	YsVec3 minmax[2];
	mkBbx.Get(minmax);

	YsRect3 rect;
	rect.Set(minmax[0],minmax[1]);
	return rect;
}

YsRect2i RetroMap_World::Field::GetBoundingBox2i(void) const
{
	YsBoundingBoxMaker <YsVec2i> mkBbx;

	for(auto mpHd : AllMapPiece())
	{
		mkBbx.Add(mapPiece[mpHd]->GetBoundingBox2i());
	}
	for(auto muHd : AllMarkUp())
	{
		mkBbx.Add(markUp[muHd]->GetBoundingBox2i());
	}

	YsRect2i rect;
	rect.Set(mkBbx[0],mkBbx[1]);
	return rect;
}

RetroMap_World::MapPieceHandle RetroMap_World::Field::FindMapPieceFromCoordinate(const YsVec2 &pos) const
{
	MapPieceHandle lastFound=nullptr;
	YsVec2i tst(pos.xi(),pos.yi());
	for(auto mpHd : AllMapPiece())
	{
		auto mapPiecePtr=GetMapPiece(mpHd);
		auto rect=mapPiecePtr->GetBoundingBox2i();
		if(YSTRUE==rect.IsInside(tst))
		{
			lastFound=mpHd;
		}
	}
	return lastFound;
}

RetroMap_World::MarkUpHandle RetroMap_World::Field::FindStringMarkUpFromCoordinate(const YsVec2 &pos) const
{
	MarkUpHandle lastFound=nullptr;
	YsVec2i tst(pos.xi(),pos.yi());
	for(auto muHd : AllMarkUp())
	{
		auto markUpPtr=GetMarkUp(muHd);
		if(markUpPtr->GetMarkUpType()==RetroMap_World::MarkUp::MARKUP_STRING)
		{
			auto rect=markUpPtr->GetBoundingBox2i();
			if(YSTRUE==rect.IsInside(tst))
			{
				lastFound=muHd;
			}
		}
	}
	return lastFound;
}

void RetroMap_World::Field::ChangePriority(MapPieceHandle fromMpHd,MapPieceHandle toMpHd)
{
	if(fromMpHd<toMpHd)
	{
		PriorityUp(fromMpHd,toMpHd);
	}
	else if(fromMpHd>toMpHd)
	{
		PriorityDown(fromMpHd,toMpHd);
	}
}
void RetroMap_World::Field::PriorityUp(MapPieceHandle fromMpHd,MapPieceHandle toMpHd)
{
	auto mpHd=fromMpHd;
	while(nullptr!=mpHd && nullptr!=mapPiece.FindNext(mpHd) && mpHd!=toMpHd)
	{
		auto nextMpHd=mapPiece.FindNext(mpHd);
		mapPiece[mpHd]->Swap(*mapPiece[nextMpHd]);
		SwapAnchorBase(mpHd,nextMpHd);
		mpHd=nextMpHd;
	}
}
void RetroMap_World::Field::PriorityDown(MapPieceHandle fromMpHd,MapPieceHandle toMpHd)
{
	auto mpHd=fromMpHd;
	while(nullptr!=mpHd && nullptr!=mapPiece.FindPrev(mpHd) && mpHd!=toMpHd)
	{
		auto prevMpHd=mapPiece.FindPrev(mpHd);
		mapPiece[mpHd]->Swap(*mapPiece[prevMpHd]);
		SwapAnchorBase(mpHd,prevMpHd);
		mpHd=prevMpHd;
	}
}

void RetroMap_World::Field::SwapAnchorBase(MapPieceHandle mpHd1,MapPieceHandle mpHd2)
{
	const MapPieceHandle baseMpHd[2]=
	{
		mpHd1,mpHd2
	};
	const YsArray <AnchorHandle> anchoredTo[2]=
	{
		FindAnchorFromBaseMapPiece(mpHd1),
		FindAnchorFromBaseMapPiece(mpHd2)
	};

	for(int i=0; i<2; ++i)
	{
		for(auto anHd : anchoredTo[i])
		{
			auto &a=*this->anchor[anHd];
			a.SetBaseMapPiece(*this,baseMpHd[1-i]);
		}
		baseMapPieceKeyToAnchor.Update(GetSearchKey(baseMpHd[i]),anchoredTo[1-i]);
	}

}

void RetroMap_World::Field::SelectMapPiece(MapPieceHandle mpHd)
{
	auto key=mapPiece.GetSearchKey(mpHd);
	selectedMapPiece.CleanUp();
	selectedMapPieceKey.CleanUp();
	selectedMapPiece.Add(key);
	selectedMapPieceKey.Add(key);
}
void RetroMap_World::Field::SelectMapPiece(const YsConstArrayMask <MapPieceHandle> &mpHd)
{
	selectedMapPiece.CleanUp();
	selectedMapPieceKey.CleanUp();
	for(auto hd : mpHd)
	{
		auto key=mapPiece.GetSearchKey(hd);
		selectedMapPiece.Add(key);
		selectedMapPieceKey.Add(key);
	}
}
void RetroMap_World::Field::AddSelectedMapPiece(MapPieceHandle mpHd)
{
	auto key=mapPiece.GetSearchKey(mpHd);
	if(YSTRUE!=selectedMapPieceKey.IsIncluded(key))
	{
		selectedMapPiece.Add(key);
		selectedMapPieceKey.Add(key);
	}
}
void RetroMap_World::Field::AddSelectedMapPiece(const YsConstArrayMask <MapPieceHandle> &mpHd)
{
	for(auto hd : mpHd)
	{
		AddSelectedMapPiece(hd);
	}
}
void RetroMap_World::Field::UnselectMapPiece(MapPieceHandle mpHd)
{
	auto key=mapPiece.GetSearchKey(mpHd);
	if(YSTRUE==selectedMapPieceKey.IsIncluded(key))
	{
		selectedMapPieceKey.Delete(key);
		for(auto i=selectedMapPiece.GetN()-1; 0<=i; --i)
		{
			if(key==selectedMapPiece[i])
			{
				selectedMapPiece.Delete(i);
			}
		}
	}
}
void RetroMap_World::Field::UnselectMapPiece(const YsConstArrayMask <MapPieceHandle> &mpHd)
{
	for(auto hd : mpHd)
	{
		UnselectMapPiece(hd);
	}
}
void RetroMap_World::Field::UnselectAllMapPiece(void)
{
	selectedMapPiece.CleanUp();
	selectedMapPieceKey.CleanUp();
}
YSBOOL RetroMap_World::Field::IsSelected(MapPieceHandle mpHd) const
{
	auto key=mapPiece.GetSearchKey(mpHd);
	return selectedMapPieceKey.IsIncluded(key);
}
YsArray <RetroMap_World::MapPieceHandle> RetroMap_World::Field::SelectedMapPiece(void) const
{
	YsArray <MapPieceHandle> ary;
	for(auto key : selectedMapPiece)
	{
		auto mpHd=mapPiece.FindObject(key);
		if(nullptr!=mpHd)
		{
			ary.Add(mpHd);
		}
	}
	return ary;
}

void RetroMap_World::Field::SelectMarkUp(MarkUpHandle muHd)
{
	auto key=markUp.GetSearchKey(muHd);
	selectedMarkUpKey.CleanUp();
	selectedMarkUpKey.Add(key);
}
void RetroMap_World::Field::SelectMarkUp(const YsConstArrayMask <MarkUpHandle> &muHd)
{
	selectedMarkUpKey.CleanUp();
	for(auto hd : muHd)
	{
		auto key=markUp.GetSearchKey(hd);
		selectedMarkUpKey.Add(key);
	}
}
void RetroMap_World::Field::AddSelectedMarkUp(MarkUpHandle muHd)
{
	auto key=markUp.GetSearchKey(muHd);
	if(YSTRUE!=selectedMarkUpKey.IsIncluded(key))
	{
		selectedMarkUpKey.Add(key);
	}
}
void RetroMap_World::Field::AddSelectedMarkUp(const YsConstArrayMask <MarkUpHandle> &muHd)
{
	for(auto hd : muHd)
	{
		AddSelectedMarkUp(hd);
	}
}
void RetroMap_World::Field::UnselectMarkUp(MarkUpHandle muHd)
{
	auto key=markUp.GetSearchKey(muHd);
	if(YSTRUE==selectedMarkUpKey.IsIncluded(key))
	{
		selectedMarkUpKey.Delete(key);
	}
}
void RetroMap_World::Field::UnselectMarkUp(const YsConstArrayMask <MarkUpHandle> &muHd)
{
	for(auto hd : muHd)
	{
		UnselectMarkUp(hd);
	}
}
void RetroMap_World::Field::UnselectAllMarkUp(void)
{
	selectedMarkUpKey.CleanUp();
}

void RetroMap_World::Field::RemakeAnchorSearchTable(void)
{
	baseMapPieceKeyToAnchor.CleanUp();
	baseMarkUpKeyToAnchor.CleanUp();
	markUpKeyToAnchor.CleanUp();

	for(auto anHd : AllAnchor())
	{
		RegisterAnchor(anHd);
	}
}

YSBOOL RetroMap_World::Field::IsSelected(MarkUpHandle muHd) const
{
	auto key=markUp.GetSearchKey(muHd);
	return selectedMarkUpKey.IsIncluded(key);
}
YsArray <RetroMap_World::MarkUpHandle> RetroMap_World::Field::SelectedMarkUp(void) const
{
	YsArray <MarkUpHandle> ary;
	for(auto key : selectedMarkUpKey)
	{
		auto muHd=markUp.FindObject(key);
		if(nullptr!=muHd)
		{
			ary.Add(muHd);
		}
	}
	return ary;
}

////////////////////////////////////////////////////////////

RetroMap_World::Clipboard::Clipboard()
{
}
RetroMap_World::Clipboard::~Clipboard()
{
}
void RetroMap_World::Clipboard::CleanUp(void)
{
	mapPiece.CleanUp();
	markUp.CleanUp();
}
void RetroMap_World::Clipboard::Add(const RetroMap_World &world,FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto *mapPiece=world.GetMapPiece(fdHd,mpHd);
	if(nullptr!=mapPiece)
	{
		this->mapPiece.push_back(*mapPiece);
	}
}
void RetroMap_World::Clipboard::Add(const RetroMap_World &world,FieldHandle fdHd,MarkUpHandle muHd)
{
	auto *markUp=world.GetMarkUp(fdHd,muHd);
	if(nullptr!=markUp)
	{
		this->markUp.push_back(*markUp);
	}
}
YsVec2i RetroMap_World::Clipboard::GetBoundingBoxCenter(void) const
{
	if(0<mapPiece.size() || 0<markUp.size())
	{
		YsBoundingBoxMaker <YsVec2i> bbx;
		for(auto &m : mapPiece)
		{
			auto rect=m.GetBoundingBox2i();
			bbx.Add(rect.Min());
			bbx.Add(rect.Max());
		}
		for(auto &m : markUp)
		{
			auto rect=m.GetBoundingBox2i();
			bbx.Add(rect.Min());
			bbx.Add(rect.Max());
		}
		return bbx.GetCenter();
	}
	return YsVec2i::Origin();
}

////////////////////////////////////////////////////////////

RetroMap_World::RetroMap_World()
{
	undoPtr=nullptr;
	redoPtr=nullptr;
	overlapTexHd=nullptr;
	CleanUp();
}
RetroMap_World::~RetroMap_World()
{
	CleanUp();
}
void RetroMap_World::CleanUp(void)
{
	CleanUpUndoLog();
	modified=YSFALSE;

	allField.ClearDeep();
	allField.EnableSearch();

	texMan.CleanUp();
	trimRect.Set(YsVec2i::Origin(),YsVec2i::Origin());
	defScaling.Set(1,1);
	defFontSize=52;
	defMarkUpFgCol=YsWhite();
	defMarkUpBgCol=YsBlack();
	unit.Set(1,1);
	defArrowSize=MarkUp::ArrowHeadInfo::DefaultSize();
	fileName=L"";

	diffThr=32;
}

void RetroMap_World::CleanUpUndoLog(void)
{
	while(nullptr!=undoPtr)
	{
		auto prev=undoPtr->prev;
		delete undoPtr;
		undoPtr=prev;
	}
	while(nullptr!=redoPtr)
	{
		auto next=redoPtr->next;
		delete redoPtr;
		redoPtr=next;
	}
	lastSavedUndoPtr=nullptr;

	undoCtr=0;
	undoInc=1;
}

void RetroMap_World::Swap(FieldHandle fdHd,MapPieceHandle mpHd1,MapPieceHandle mpHd2)
{
	auto fieldPtr=allField[fdHd];
	fieldPtr->mapPiece[mpHd1]->Swap(*fieldPtr->mapPiece[mpHd2]);
}

void RetroMap_World::CleanUpRedoLogOnly(void)
{
	while(nullptr!=redoPtr)
	{
		auto next=redoPtr->next;
		delete redoPtr;
		redoPtr=next;
	}
	if(nullptr!=undoPtr)
	{
		undoPtr->next=nullptr;
	}
}

RetroMap_World::FieldHandle RetroMap_World::Undo(void)
{
	YSHASHKEY lastFdKey=YSNULLHASHKEY;
	if(nullptr!=undoPtr)
	{
		auto undoCtr=undoPtr->undoCtr;
		while(nullptr!=undoPtr && undoCtr==undoPtr->undoCtr)
		{
			auto lastFdHd=undoPtr->SwitchCurrentFdHd(*this);
			if(nullptr!=lastFdHd)
			{
				lastFdKey=GetSearchKey(lastFdHd);
			}
			undoPtr->Undo(*this);
			redoPtr=undoPtr;
			undoPtr=undoPtr->prev;
		}
	}
	return FindField(lastFdKey);
}
RetroMap_World::FieldHandle RetroMap_World::Redo(void)
{
	FieldHandle lastFdHd=nullptr;
	if(nullptr!=redoPtr)
	{
		auto undoCtr=redoPtr->undoCtr;
		while(nullptr!=redoPtr && undoCtr==redoPtr->undoCtr)
		{
			if(nullptr!=redoPtr->SwitchCurrentFdHd(*this))
			{
				lastFdHd=redoPtr->SwitchCurrentFdHd(*this);
			}
			redoPtr->Redo(*this);
			undoPtr=redoPtr;
			redoPtr=redoPtr->next;
		}
	}
	return lastFdHd;
}

void RetroMap_World::ClearModified(void)
{
	modified=YSFALSE;
	lastSavedUndoPtr=undoPtr;
}
YSBOOL RetroMap_World::IsModified(void) const
{
	return modified;
}

const YsWString &RetroMap_World::GetGameTitle(void) const
{
	return gameTitle;
}

void RetroMap_World::SetGameTitle(const YsWString &wstr)
{
	gameTitle=wstr;
}

YsVec2i RetroMap_World::GetUnit(void) const
{
	return unit;
}

void RetroMap_World::SetUnit(YsVec2i unit)
{
	if(unit.x()<1)
	{
		unit.SetX(1);
	}
	if(unit.y()<1)
	{
		unit.SetY(1);
	}
	this->unit=unit;
}

void RetroMap_World::SetDefaultArrowSize(double arwSize)
{
	this->defArrowSize=arwSize;
}

double RetroMap_World::GetDefaultArrowSize(void) const
{
	return defArrowSize;
}

int RetroMap_World::GetDiffThreshold(void) const
{
	return diffThr;
}
void RetroMap_World::SetDiffThreshold(int diffThr)
{
	this->diffThr=diffThr;
}

int RetroMap_World::GetDefaultFontSize(void) const
{
	return defFontSize;
}
void RetroMap_World::SetDefaultFontSize(int fontSize)
{
	defFontSize=fontSize;
}
YsColor RetroMap_World::GetDefaultMarkUpForegroundColor(void) const
{
	return defMarkUpFgCol;
}
void RetroMap_World::SetDefaultMarkUpForegroundColor(YsColor col)
{
	defMarkUpFgCol=col;
}
YsColor RetroMap_World::GetDefaultMarkUpBackgroundColor(void) const
{
	return defMarkUpBgCol;
}
void RetroMap_World::SetDefaultMarkUpBackgroundColor(YsColor col)
{
	defMarkUpBgCol=col;
}

YsEditArray<RetroMap_World::Field,4>::HandleEnumerator RetroMap_World::AllField(void) const
{
	return allField.AllHandle();
}

YsEditArray<RetroMap_World::MapPiece,4>::HandleEnumerator RetroMap_World::AllMapPiece(FieldHandle fdHd) const
{
	return allField[fdHd]->mapPiece.AllHandle();
}

YSSIZE_T RetroMap_World::GetNumField(void) const
{
	return allField.GetN();
}

YsRect3 RetroMap_World::GetBoundingBox(FieldHandle fdHd) const
{
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->GetBoundingBox();
	}
	YsRect3 rect;
	rect.Set(YsVec3::Origin(),YsVec3::Origin());
	return rect;
}

RetroMap_World::FieldHandle RetroMap_World::GetFirstField(void) const
{
	return allField.First();
}

class RetroMap_World::UndoCreateField : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	virtual void Undo(RetroMap_World &world)
	{
		world.allField.Freeze(fdHd);
	}
	virtual void Redo(RetroMap_World &world)
	{
		world.allField.Melt(fdHd);
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return nullptr;
	}
};

RetroMap_World::FieldHandle RetroMap_World::CreateField(void)
{
	auto fdHd=allField.Create();
	if(nullptr!=fdHd)
	{
		allField[fdHd]->CleanUp();
		allField[fdHd]->owner=this;

		auto undoPtr=NewUndoLog<UndoCreateField>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
		}

		modified=YSTRUE;
		return fdHd;
	}
	return nullptr;

}

class RetroMap_World::UndoDeleteField : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	virtual void Undo(RetroMap_World &world)
	{
		world.allField.Melt(fdHd);
	}
	virtual void Redo(RetroMap_World &world)
	{
		world.allField.Freeze(fdHd);
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return nullptr;
	}
};

void RetroMap_World::DeleteField(FieldHandle fdHd)
{
	allField.Freeze(fdHd);
	modified=YSTRUE;

	auto undoPtr=NewUndoLog<UndoDeleteField>();
	if(nullptr!=undoPtr)
	{
		undoPtr->fdHd=fdHd;
	}
}

RetroMap_World::FieldHandle RetroMap_World::FindNextField(FieldHandle fdHd) const
{
	return allField.FindNext(fdHd);
}
RetroMap_World::FieldHandle RetroMap_World::FindPrevField(FieldHandle fdHd) const
{
	return allField.FindPrev(fdHd);
}

class RetroMap_World::UndoSwapField : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd[2];
	virtual void Undo(RetroMap_World &world)
	{
		auto fieldPtr0=world.allField[fdHd[0]];
		auto fieldPtr1=world.allField[fdHd[1]];
		if(nullptr!=fieldPtr0 && nullptr!=fieldPtr1)
		{
			fieldPtr0->Swap(*fieldPtr1);
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto fieldPtr0=world.allField[fdHd[0]];
		auto fieldPtr1=world.allField[fdHd[1]];
		if(nullptr!=fieldPtr0 && nullptr!=fieldPtr1)
		{
			fieldPtr0->Swap(*fieldPtr1);
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return nullptr;
	}
};

void RetroMap_World::Swap(FieldHandle fdHd1,FieldHandle fdHd2)
{
	if(fdHd1!=fdHd2)
	{
		auto undoLog=NewUndoLog <UndoSwapField>();
		if(nullptr!=undoLog)
		{
			undoLog->fdHd[0]=fdHd1;
			undoLog->fdHd[1]=fdHd2;
		}
		allField[fdHd1]->Swap(*allField[fdHd2]);
	}
}

RetroMap_World::FieldHandle RetroMap_World::FindField(YSHASHKEY key) const
{
	return allField.FindObject(key);
}
const RetroMap_World::Field *RetroMap_World::GetField(FieldHandle fdHd) const
{
	return allField[fdHd];
}
const RetroMap_World::MapPiece *RetroMap_World::GetMapPiece(FieldHandle fdHd,MapPieceHandle mpHd) const
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->GetMapPiece(mpHd);
	}
	return nullptr;
}

const RetroMap_World::MarkUp *RetroMap_World::GetMarkUp(FieldHandle fdHd,MarkUpHandle muHd) const
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->GetMarkUp(muHd);
	}
	return nullptr;
}

RetroMap_World::MapPiece *RetroMap_World::GetEditableMapPiece(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->mapPiece[mpHd];
	}
	return nullptr;
}
RetroMap_World::MarkUp *RetroMap_World::GetEditableMarkUp(FieldHandle fdHd,MarkUpHandle muHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->markUp[muHd];
	}
	return nullptr;
}

YSHASHKEY RetroMap_World::GetSearchKey(FieldHandle fdHd)
{
	return allField.GetSearchKey(fdHd);
}

class RetroMap_World::UndoCreateMapPiece : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MapPieceHandle mpHd;
	void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->mapPiece.Freeze(mpHd);
		}
	}
	void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->mapPiece.Melt(mpHd);
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
RetroMap_World::MapPieceHandle RetroMap_World::LoadImage(FieldHandle fdHd,const YsVec2i pos,YsTextureManager::DATA_FORMAT fom,const YsConstArrayMask <unsigned char> &dat)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		YsString label=texMan.MakeUniqueLabel();
		auto texHd=texMan.AddTexture(label,fom,0,0,dat.GetN(),dat);

		auto texUnitPtr=texMan.GetTexture(texHd);
		if(nullptr!=texUnitPtr)
		{
			auto mpHd=fieldPtr->CreateMapPiece();
			auto mapPiecePtr=GetEditableMapPiece(fdHd,mpHd);
			fieldPtr->SetTexture(mpHd,texHd);
			mapPiecePtr->ReadyTexture();

			fieldPtr->MakeAllVisible(mpHd);

			mapPiecePtr->shape.scaling=defScaling;
			if(0<trimRect.GetWidth() && 0<trimRect.GetHeight())
			{
				mapPiecePtr->shape.visibleArea=trimRect;
			}

			auto visibleAreaCen=mapPiecePtr->shape.visibleArea.GetCenter();
			mapPiecePtr->shape.pos=pos-visibleAreaCen;
			mapPiecePtr->shape.ApplyUnit(this->unit);

			modified=YSTRUE;

			auto undoPtr=NewUndoLog<UndoCreateMapPiece>();
			if(nullptr!=undoPtr)
			{
				undoPtr->fdHd=fdHd;
				undoPtr->mpHd=mpHd;
			}

			return mpHd;
		}

		texMan.Delete(texHd);
	}
	return nullptr;
}

RetroMap_World::MapPieceMarkUpSet RetroMap_World::Paste(FieldHandle fdHd,const Clipboard &clipboard,YsVec2i offset)
{
	MapPieceMarkUpSet pasted;

	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		RetroMap_World::UndoGuard ug(*this);
		for(auto &m : clipboard.mapPiece)
		{
			auto mpHd=CreateMapPiece(fdHd);
			auto *newMapPiece=fieldPtr->mapPiece[mpHd];
			newMapPiece->CopyFrom(m);
			newMapPiece->owner=fieldPtr;
			newMapPiece->shape.pos+=offset;

			pasted.mapPiece.push_back(mpHd);
		}
		for(auto &m : clipboard.markUp)
		{
			auto muHd=CreateMarkUp(fdHd);
			auto newMarkUp=fieldPtr->markUp[muHd];
			newMarkUp->CopyFrom(m);
			newMarkUp->owner=fieldPtr;
			newMarkUp->pos+=offset;
			for(auto &p : newMarkUp->plg)
			{
				p+=offset;
			}

			pasted.markUp.push_back(muHd);
		}
	}
	return pasted;
}

RetroMap_World::MapPieceHandle RetroMap_World::CreateMapPiece(FieldHandle fdHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto mpHd=fieldPtr->CreateMapPiece();
		auto undoPtr=NewUndoLog<UndoCreateMapPiece>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->mpHd=mpHd;
		}
		return mpHd;
	}
	return nullptr;
}

RetroMap_World::MapPiece::Shape RetroMap_World::ExpectedShape(FieldHandle fdHd,const YsVec2i pos) const
{
	MapPiece::Shape shape;
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		shape.scaling=this->defScaling;
		shape.visibleArea=this->trimRect;

		auto visibleAreaCen=shape.visibleArea.GetCenter();
		shape.pos=pos-visibleAreaCen;
		shape.ApplyUnit(this->unit);
	}
	return shape;
}

class RetroMap_World::UndoDeleteMapPiece : public RetroMap_World::UndoLog
{
	FieldHandle fdHd;
	MapPieceHandle mpHd;
	void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->mapPiece.Melt(mpHd);
		}
	}
	void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			fieldPtr->mapPiece.Freeze(mpHd);
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
class RetroMap_World::UndoDeleteMultiMapPiece : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	YsArray <MapPieceHandle> mpHd;
	void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			for(auto hd : mpHd)
			{
				fieldPtr->mapPiece.Melt(hd);
			}
		}
	}
	void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			for(auto hd : mpHd)
			{
				fieldPtr->mapPiece.Freeze(hd);
			}
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::DeleteMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		for(auto hd : mpHd)
		{
			fieldPtr->mapPiece.Freeze(hd);
		}
		auto undoPtr=NewUndoLog<UndoDeleteMultiMapPiece>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->mpHd=mpHd;
		}
		return YSOK;
	}
	return YSERR;
}

RetroMap_World::MarkUp *RetroMap_World::GetMarkUpPtr(FieldHandle fdHd,MarkUpHandle muHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->markUp[muHd];
	}
	return nullptr;
}
const RetroMap_World::MarkUp *RetroMap_World::GetMarkUpPtr(FieldHandle fdHd,MarkUpHandle muHd) const
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->markUp[muHd];
	}
	return nullptr;
}
class RetroMap_World::UndoCreateMarkUp : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	virtual void Undo(RetroMap_World &world)
	{
		world.allField[fdHd]->markUp.Freeze(muHd);
	}
	virtual void Redo(RetroMap_World &world)
	{
		world.allField[fdHd]->markUp.Melt(muHd);
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
RetroMap_World::MarkUpHandle RetroMap_World::CreateMarkUp(FieldHandle fdHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto muHd=fieldPtr->CreateMarkUp();
		auto undoPtr=NewUndoLog<UndoCreateMarkUp>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
		}
		return muHd;
	}
	return nullptr;
}

class RetroMap_World::UndoDeleteMarkUp : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	virtual void Undo(RetroMap_World &world)
	{
		world.allField[fdHd]->markUp.Melt(muHd);
	}
	virtual void Redo(RetroMap_World &world)
	{
		world.allField[fdHd]->markUp.Freeze(muHd);
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};

YSRESULT RetroMap_World::DeleteMarkUp(FieldHandle fdHd,MarkUpHandle muHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		if(YSOK==fieldPtr->markUp.Freeze(muHd))
		{
			auto undoPtr=NewUndoLog<UndoDeleteMarkUp>();
			if(nullptr!=undoPtr)
			{
				undoPtr->fdHd=fdHd;
				undoPtr->muHd=muHd;
			}
			return YSOK;
		}
	}
	return YSERR;
}

class RetroMap_World::UndoDeleteMultiMarkUp : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	YsArray <MarkUpHandle> muHd;
	virtual void Undo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			for(auto hd : muHd)
			{
				fieldPtr->markUp.Melt(hd);
			}
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto fieldPtr=world.allField[fdHd];
		if(nullptr!=fieldPtr)
		{
			for(auto hd : muHd)
			{
				fieldPtr->markUp.Freeze(hd);
			}
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::DeleteMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &muHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto undoPtr=NewUndoLog<UndoDeleteMultiMarkUp>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
		}
		for(auto hd : muHd)
		{
			fieldPtr->markUp.Freeze(hd);
		}
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoMakeStringMarkUp : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	MarkUp::MARKUPTYPE prvMarkUpType,newMarkUpType;
	YsWString prvTxt,newTxt;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->markUpType=prvMarkUpType;
			markUpPtr->txt=prvTxt;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->markUpType=newMarkUpType;
			markUpPtr->txt=newTxt;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::MakeStringMarkUp(FieldHandle fdHd,MarkUpHandle muHd,const YsWString &str)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoMakeStringMarkUp>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->prvMarkUpType=markUpPtr->markUpType;
			undoPtr->prvTxt=markUpPtr->txt;
			undoPtr->newMarkUpType=MarkUp::MARKUP_STRING;
			undoPtr->newTxt=str;
		}
		markUpPtr->MakeStringMarkUp(str);
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}
YSRESULT RetroMap_World::MakeStringMarkUp(FieldHandle fdHd,MarkUpHandle muHd,const YsString &str)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoMakeStringMarkUp>();
		if(nullptr!=undoPtr)
		{
			undoPtr->prvMarkUpType=markUpPtr->markUpType;
			undoPtr->prvTxt=markUpPtr->txt;
			undoPtr->newMarkUpType=MarkUp::MARKUP_STRING;
			undoPtr->newTxt.SetUTF8String(str);
		}
		markUpPtr->MakeStringMarkUp(str);
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoMakePointArrayMarkUp : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	MarkUp::MARKUPTYPE prvMarkUpType,newMarkUpType;
	YsArray <YsVec2i> prvPlg,newPlg;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->markUpType=prvMarkUpType;
			markUpPtr->plg=prvPlg;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->markUpType=newMarkUpType;
			markUpPtr->plg=newPlg;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};

YSRESULT RetroMap_World::MakePointArrayMarkUp(FieldHandle fdHd,MarkUpHandle muHd,const YsConstArrayMask <YsVec2i> &plg)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoMakePointArrayMarkUp>();
		if(nullptr!=undoPtr)
		{
			undoPtr->prvMarkUpType=markUpPtr->markUpType;
			undoPtr->prvPlg=markUpPtr->plg;
			undoPtr->newMarkUpType=MarkUp::MARKUP_POINT_ARRAY;
			undoPtr->newPlg=plg;
		}
		markUpPtr->MakePointArrayMarkUp(plg);
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}


class RetroMap_World::UndoSetMarkUpFgColor : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	YsColor prvFgCol,newFgCol;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->fgCol=prvFgCol;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->fgCol=newFgCol;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::SetMarkUpForegroundColor(FieldHandle fdHd,MarkUpHandle muHd,YsColor col)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMarkUpFgColor>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->prvFgCol=markUpPtr->fgCol;
			undoPtr->newFgCol=col;
		}
		markUpPtr->fgCol=col;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetMarkUpBgColor : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	YsColor prvBgCol,newBgCol;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->bgCol=prvBgCol;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->bgCol=newBgCol;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::SetMarkUpBackgroundColor(FieldHandle fdHd,MarkUpHandle muHd,YsColor col)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMarkUpBgColor>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->prvBgCol=markUpPtr->bgCol;
			undoPtr->newBgCol=col;
		}
		markUpPtr->bgCol=col;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetMarkUpPointArray : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	YsArray <YsVec2i> prvPoint,newPoint;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->plg=prvPoint;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->plg=newPoint;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::SetMarkUpPointArray(FieldHandle fdHd,MarkUpHandle muHd,const YsConstArrayMask <YsVec2i> &pointArray)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMarkUpPointArray>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->prvPoint=markUpPtr->plg;
			undoPtr->newPoint=pointArray;
		}
		markUpPtr->plg=pointArray;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetStringMarkUpFontSize : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	int prvFontSize,newFontSize;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->fontSize=prvFontSize;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->fontSize=newFontSize;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::SetStringMarkUpFontSize(FieldHandle fdHd,MarkUpHandle muHd,int fontSize)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoSetStringMarkUpFontSize>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->prvFontSize=markUpPtr->fontSize;
			undoPtr->newFontSize=fontSize;
		}
		markUpPtr->fontSize=fontSize;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetPointArrayMarkUpArrowHead : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	int frontOrBack;
	MarkUp::ArrowHeadInfo prvArrowInfo,newArrowInfo;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->ArrowInfo(frontOrBack)=prvArrowInfo;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetMarkUpPtr(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->ArrowInfo(frontOrBack)=newArrowInfo;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};

YSRESULT RetroMap_World::SetPointArrayMarkUpArrowHead(FieldHandle fdHd,MarkUpHandle muHd,int frontOrBack,MarkUp::ArrowHeadInfo ahInfo)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoSetPointArrayMarkUpArrowHead>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->frontOrBack=frontOrBack;
			undoPtr->prvArrowInfo=markUpPtr->ArrowInfo(frontOrBack);
			undoPtr->newArrowInfo=ahInfo;
		}
		markUpPtr->ArrowInfo(frontOrBack)=ahInfo;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

YSRESULT RetroMap_World::RemakeStringTexture(FieldHandle fdHd,MarkUpHandle muHd)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		markUpPtr->RemakeStringTexture();
		return YSOK;
	}
	return YSERR;
}
YSRESULT RetroMap_World::ReadyTexture(FieldHandle fdHd,MarkUpHandle muHd)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		return markUpPtr->ReadyTexture();
	}
	return YSERR;
}
YSRESULT RetroMap_World::ReadyVbo(FieldHandle fdHd,MarkUpHandle muHd)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		return markUpPtr->ReadyVbo();
	}
	return YSERR;
}
YSRESULT RetroMap_World::ReadyVbo(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i pos,YsColor hlCol)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		return markUpPtr->ReadyVbo(pos,hlCol);
	}
	return YSERR;
}

YSRESULT RetroMap_World::ReadyVboWithDisplacement(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i disp,YsColor hlCol)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		return markUpPtr->ReadyVboWithDisplacement(disp,hlCol);
	}
	return YSERR;
}

YSRESULT RetroMap_World::ReadyVboWithModifiedPointArray(FieldHandle fdHd,MarkUpHandle muHd,const YsConstArrayMask <YsVec2i> &pointArray,YsColor hlCol)
{
	auto markUpPtr=GetMarkUpPtr(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		return markUpPtr->ReadyVboWithModifiedPointArray(pointArray,hlCol);
	}
	return YSERR;
}

YSRESULT RetroMap_World::ReadyTexture(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto mapPiecePtr=fieldPtr->mapPiece[mpHd];
		if(nullptr!=mapPiecePtr)
		{
			mapPiecePtr->ReadyTexture();
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT RetroMap_World::SetFieldName(FieldHandle fdHd,const YsWString nm)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->fieldName=nm;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetMapPieceTrimRect : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MapPieceHandle mpHd;
	YsRect2i prvTrim,newTrim;
	virtual void Undo(RetroMap_World &world)
	{
		auto mapPiecePtr=world.GetEditableMapPiece(fdHd,mpHd);
		if(nullptr!=mapPiecePtr)
		{
			mapPiecePtr->shape.visibleArea=prvTrim;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto mapPiecePtr=world.GetEditableMapPiece(fdHd,mpHd);
		if(nullptr!=mapPiecePtr)
		{
			mapPiecePtr->shape.visibleArea=newTrim;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
void RetroMap_World::MakeAllVisible(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	auto mapPiecePtr=GetEditableMapPiece(fdHd,mpHd);
	if(nullptr!=fieldPtr && nullptr!=mapPiecePtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMapPieceTrimRect>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->mpHd=mpHd;
			undoPtr->prvTrim=mapPiecePtr->shape.visibleArea;
		}
		fieldPtr->MakeAllVisible(mpHd);
		if(nullptr!=undoPtr)
		{
			undoPtr->newTrim=mapPiecePtr->shape.visibleArea;
		}
		modified=YSTRUE;
	}
}

YSRESULT RetroMap_World::ReadyVbo(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto mapPiecePtr=fieldPtr->mapPiece[mpHd];
		if(nullptr!=mapPiecePtr)
		{
			return mapPiecePtr->ReadyVbo();
		}
	}
	return YSERR;
}

YSRESULT RetroMap_World::ReadyVbo(FieldHandle fdHd,MapPieceHandle mpHd,MapPiece::Shape shape,double alpha)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		auto mapPiecePtr=fieldPtr->mapPiece[mpHd];
		if(nullptr!=mapPiecePtr)
		{
			return mapPiecePtr->ReadyVbo(shape,alpha);
		}
	}
	return YSERR;
}

YSRESULT RetroMap_World::ReadyVboOfAnchoredMarkUp(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		for(auto anHd : fieldPtr->FindAnchorFromBaseMapPiece(mpHd))
		{
			auto muHd=fieldPtr->GetAnchor(anHd)->GetMarkUp(*fieldPtr);
			ReadyVbo(fdHd,muHd);
		}
		return YSOK;
	}
	return YSERR;
}
YSRESULT RetroMap_World::ReadyVboOfAnchoredMarkUp(FieldHandle fdHd,MarkUpHandle baseMuHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		for(auto anHd : fieldPtr->FindAnchorFromBaseMarkUp(baseMuHd))
		{
			auto muHd=fieldPtr->GetAnchor(anHd)->GetMarkUp(*fieldPtr);
			ReadyVbo(fdHd,muHd);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT RetroMap_World::RemakeStringTexture(FieldHandle fdHd)
{
	YSRESULT res=YSOK;
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		for(auto muHd : fieldPtr->AllMarkUp())
		{
			auto markUpPtr=fieldPtr->markUp[muHd];
			markUpPtr->RemakeStringTexture();
		}
	}
	return res;
}
YSRESULT RetroMap_World::ReadyVbo(FieldHandle fdHd)
{
	YSRESULT res=YSOK;
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : fieldPtr->AllMapPiece())
		{
			auto mapPiecePtr=fieldPtr->mapPiece[mpHd];
			if(YSOK!=mapPiecePtr->ReadyVbo())
			{
				res=YSERR;
			}
		}
		for(auto muHd : fieldPtr->AllMarkUp())
		{
			auto markUpPtr=fieldPtr->markUp[muHd];
			markUpPtr->ReadyVbo();
		}
	}
	return res;
}

YSRESULT RetroMap_World::ResetVbo(FieldHandle fdHd)
{
	YSRESULT res=YSERR;
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : fieldPtr->AllMapPiece())
		{
			auto mapPiecePtr=fieldPtr->mapPiece[mpHd];
			if(YSTRUE==mapPiecePtr->usingTentativePosition)
			{
				mapPiecePtr->ReadyVbo();
				res=YSOK;
			}
		}
		for(auto muHd : fieldPtr->AllMarkUp())
		{
			auto markUpPtr=fieldPtr->markUp[muHd];
			if(YSTRUE==markUpPtr->usingTentativePosition)
			{
				markUpPtr->ReadyVbo();
				res=YSOK;
			}
		}
	}
	return res;
}

YSRESULT RetroMap_World::Render(FieldHandle fdHd,YSBOOL drawMapPiece,YSBOOL drawMarkUp) const
{
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		if(YSTRUE==drawMapPiece)
		{
			for(auto mpHd : fieldPtr->AllMapPiece())
			{
				auto mapPiecePtr=fieldPtr->GetMapPiece(mpHd);
				if(YSTRUE==fieldPtr->IsSelected(mpHd))
				{
					mapPiecePtr->RenderFrame(YsBlue());
				}
				mapPiecePtr->Render();
			}
		}
		if(YSTRUE==drawMarkUp)
		{
			for(auto muHd : fieldPtr->AllMarkUp())
			{
				auto markUpPtr=fieldPtr->markUp[muHd];
				if(YSTRUE==fieldPtr->IsSelected(muHd))
				{
					markUpPtr->RenderFrame(YsRed());
				}
				markUpPtr->Render();
			}
		}
		return YSOK;
	}
	return YSERR;
}

void RetroMap_World::MakeUtilityTexture(void)
{
	if(nullptr==overlapTexHd)
	{
		auto label=texMan.MakeUniqueLabel();
		unsigned char pix[4]={0,0,0,0};
		overlapTexHd=texMan.AddTexture(label,texMan.FOM_RAW_RGBA,1,1,4,pix);
		texMan.SetDontSave(overlapTexHd,YSTRUE);
	}
}

RetroMap_World::DiffInfo RetroMap_World::MakeDiff(FieldHandle fdHd,const MapPieceStore &target,const MapPieceStore &exclusion,int thr,YsColor col)
{
	DiffInfo diffInfo;
	diffInfo.totalError=0.0;
	diffInfo.numSample=0;
	diffInfo.numErrorPixel=0;

	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : target)
		{
			auto mpPtr=GetEditableMapPiece(fdHd,mpHd);
			YsBitmap mpBmp;
			auto oneDiff=MakeDiffBitmap(mpBmp,fdHd,mpHd,mpPtr->GetTemporaryShape(),exclusion,thr,col);

			diffInfo.totalError+=oneDiff.totalError;
			diffInfo.numSample+=oneDiff.numSample;
			diffInfo.numErrorPixel+=oneDiff.numErrorPixel;

			mpPtr->SetDiffTexture(mpBmp);
		}
	}

	return diffInfo;
}

RetroMap_World::DiffInfo RetroMap_World::MakeDiffBitmap(YsBitmap &mpBmp,FieldHandle fdHd,MapPieceHandle mpHd,MapPiece::Shape shape,const MapPieceStore &exclusion,int thr,YsColor col) const
{
	YsColor zero;
	zero.SetIntRGBA(0,0,0,0);

	DiffInfo diffInfo;
	diffInfo.totalError=0.0;
	diffInfo.numSample=0;
	diffInfo.numErrorPixel=0;

	auto fieldPtr=GetField(fdHd);
	auto mpPtr=GetMapPiece(fdHd,mpHd);

	auto bgBbx=shape.GetBoundingBox2i();
	if(nullptr!=mpPtr && YSTRUE==CheckOverlappingMapPiece(fdHd,bgBbx,exclusion)) // Excluding self.  Target may be a subset of self (selection)
	{
		auto bgBmp=fieldPtr->MakeBitmap(bgBbx[0],bgBbx.GetSize(),zero,YSTRUE,YSFALSE,exclusion);
		mpBmp=mpPtr->GetBitmap();

		if(bgBmp.GetWidth()==mpBmp.GetWidth() && bgBmp.GetHeight()==mpBmp.GetHeight())
		{
			auto bgBmpPtr=bgBmp.GetRGBABitmapPointer();
			auto mpBmpPtr=mpBmp.GetEditableRGBABitmapPointer();

			auto numPix=bgBmp.GetWidth()*bgBmp.GetHeight();
			for(YSSIZE_T ptr=0; ptr<numPix; ++ptr)
			{
				auto bgPixPtr=bgBmpPtr+ptr*4;
				auto mpPixPtr=mpBmpPtr+ptr*4;
				if(0==bgPixPtr[3] || 0==mpPixPtr[3])
				{
					mpPixPtr[0]=0;
					mpPixPtr[1]=0;
					mpPixPtr[2]=0;
					mpPixPtr[3]=0;
				}
				else
				{
					int diffR=YsAbs((int)bgPixPtr[0]-(int)mpPixPtr[0]);
					int diffG=YsAbs((int)bgPixPtr[1]-(int)mpPixPtr[1]);
					int diffB=YsAbs((int)bgPixPtr[2]-(int)mpPixPtr[2]);

					++diffInfo.numSample;
					diffInfo.totalError+=diffR;
					diffInfo.totalError+=diffG;
					diffInfo.totalError+=diffB;

					if(diffR<thr && diffG<thr && diffB<thr)
					{
						mpPixPtr[0]=0;
						mpPixPtr[1]=0;
						mpPixPtr[2]=0;
						mpPixPtr[3]=0;
					}
					else
					{
						mpPixPtr[0]=(unsigned char)col.Ri();
						mpPixPtr[1]=(unsigned char)col.Gi();
						mpPixPtr[2]=(unsigned char)col.Bi();
						mpPixPtr[3]=(unsigned char)col.Ai();
						++diffInfo.numErrorPixel;
					}
				}
			}
		}
	}
	else
	{
		mpBmp.PrepareBitmap(1,1);
		mpBmp.Clear(0,0,0,0);
	}

	return diffInfo;
}

YSBOOL RetroMap_World::CheckOverlappingMapPiece(
    FieldHandle fdHd,YsRect2i rect0,const MapPieceStore &exclusion) const
{
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : fieldPtr->AllMapPiece())
		{
			if(YSTRUE!=exclusion.IsIncluded(mpHd))
			{
				auto mapPiecePtr=fieldPtr->GetMapPiece(mpHd);
				auto rect=mapPiecePtr->GetBoundingBox2i();
				if(YSTRUE==rect0.DoesOverlap(rect))
				{
					return YSTRUE;
				}
			}
		}
	}
	return YSFALSE;
}

YSRESULT RetroMap_World::RenderDiff(FieldHandle fdHd,const MapPieceStore &target,int thr,YsColor col)
{
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : target)
		{
			auto mpPtr=GetMapPiece(fdHd,mpHd);
			mpPtr->RenderOverlap(mpPtr->GetTemporaryShape(),mpPtr->diffTexHd,YsWhite());
		}
	}
	return YSOK;
}

void RetroMap_World::SelectMapPiece(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->SelectMapPiece(mpHd);
	}
}
void RetroMap_World::SelectMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> &mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->SelectMapPiece(mpHd);
	}
}
void RetroMap_World::AddSelectedMapPiece(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->AddSelectedMapPiece(mpHd);
	}
}
void RetroMap_World::AddSelectedMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> &mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->AddSelectedMapPiece(mpHd);
	}
}
void RetroMap_World::UnselectMapPiece(FieldHandle fdHd,MapPieceHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->UnselectMapPiece(mpHd);
	}
}
void RetroMap_World::UnselectMapPiece(FieldHandle fdHd,const YsConstArrayMask <MapPieceHandle> &mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->UnselectMapPiece(mpHd);
	}
}
void RetroMap_World::UnselectAllMapPiece(FieldHandle fdHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->UnselectAllMapPiece();
	}
}
YSBOOL RetroMap_World::IsSelected(FieldHandle fdHd,MapPieceHandle mpHd) const
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->IsSelected(mpHd);
	}
	return YSFALSE;
}

YsArray <RetroMap_World::MapPieceHandle> RetroMap_World::SelectedMapPiece(FieldHandle fdHd) const
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->SelectedMapPiece();
	}

	YsArray <MapPieceHandle> ary;
	return ary;
}

void RetroMap_World::SelectMarkUp(FieldHandle fdHd,MarkUpHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->SelectMarkUp(mpHd);
	}
}
void RetroMap_World::SelectMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->SelectMarkUp(mpHd);
	}
}
void RetroMap_World::AddSelectedMarkUp(FieldHandle fdHd,MarkUpHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->AddSelectedMarkUp(mpHd);
	}
}
void RetroMap_World::AddSelectedMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->AddSelectedMarkUp(mpHd);
	}
}
void RetroMap_World::UnselectMarkUp(FieldHandle fdHd,MarkUpHandle mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->UnselectMarkUp(mpHd);
	}
}
void RetroMap_World::UnselectMarkUp(FieldHandle fdHd,const YsConstArrayMask <MarkUpHandle> &mpHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->UnselectMarkUp(mpHd);
	}
}
void RetroMap_World::UnselectAllMarkUp(FieldHandle fdHd)
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		fieldPtr->UnselectAllMarkUp();
	}
}
YSBOOL RetroMap_World::IsSelected(FieldHandle fdHd,MarkUpHandle muHd) const
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->IsSelected(muHd);
	}
	return YSFALSE;
}

YsArray <RetroMap_World::MarkUpHandle> RetroMap_World::SelectedMarkUp(FieldHandle fdHd) const
{
	YsArray <MarkUpHandle> selectedMuHd;
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		for(auto key : fieldPtr->selectedMarkUpKey)
		{
			auto muHd=fieldPtr->markUp.FindObject(key);
			if(nullptr!=muHd)
			{
				selectedMuHd.Add(muHd);
			}
		}
	}
	return selectedMuHd;
}

YsRect2i RetroMap_World::GetDefaultTrimRect(void) const
{
	return trimRect;
}
void RetroMap_World::SetDefaultTrimRect(YsRect2i rect)
{
	trimRect=rect;
	modified=YSTRUE;
}

void RetroMap_World::SetDefaultScaling(const YsVec2 &scaling)
{
	defScaling=scaling;
	modified=YSTRUE;
}

YSRESULT RetroMap_World::SetTrimRect(FieldHandle fdHd,MapPieceHandle mpHd,YsRect2i rect)
{
	auto mapPiecePtr=GetEditableMapPiece(fdHd,mpHd);
	if(nullptr!=mapPiecePtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMapPieceTrimRect>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->mpHd=mpHd;
			undoPtr->prvTrim=mapPiecePtr->shape.visibleArea;
			undoPtr->newTrim=rect;
		}
		mapPiecePtr->shape.visibleArea=rect;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetMapPiecePosition : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MapPieceHandle mpHd;
	YsVec2i prvPos,newPos;
	virtual void Undo(RetroMap_World &world)
	{
		auto mapPiecePtr=world.GetEditableMapPiece(fdHd,mpHd);
		if(nullptr!=mapPiecePtr)
		{
			mapPiecePtr->shape.pos=prvPos;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto mapPiecePtr=world.GetEditableMapPiece(fdHd,mpHd);
		if(nullptr!=mapPiecePtr)
		{
			mapPiecePtr->shape.pos=newPos;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::SetPosition(FieldHandle fdHd,MapPieceHandle mpHd,YsVec2i pos)
{
	auto mapPiecePtr=GetEditableMapPiece(fdHd,mpHd);
	if(nullptr!=mapPiecePtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMapPiecePosition>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->mpHd=mpHd;
			undoPtr->prvPos=mapPiecePtr->shape.pos;
			undoPtr->newPos=pos;
		}
		mapPiecePtr->shape.pos=pos;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetMarkUpPosition : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	YsVec2i prvPos,newPos;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetEditableMarkUp(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->pos=prvPos;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetEditableMarkUp(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->pos=newPos;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::SetPosition(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i pos)
{
	auto markUpPtr=GetEditableMarkUp(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMarkUpPosition>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->prvPos=markUpPtr->pos;
			undoPtr->newPos=pos;
		}
		markUpPtr->pos=pos;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoMoveMarkUp : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MarkUpHandle muHd;
	YsVec2i displacement;
	virtual void Undo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetEditableMarkUp(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->Move(-displacement);
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto markUpPtr=world.GetEditableMarkUp(fdHd,muHd);
		if(nullptr!=markUpPtr)
		{
			markUpPtr->Move(displacement);
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::MoveMarkUp(FieldHandle fdHd,MarkUpHandle muHd,YsVec2i displacement)
{
	auto markUpPtr=GetEditableMarkUp(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto undoPtr=NewUndoLog<UndoMoveMarkUp>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->muHd=muHd;
			undoPtr->displacement=displacement;
		}
		markUpPtr->Move(displacement);
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoSetMapPieceScaling : public RetroMap_World::UndoLog
{
public:
	FieldHandle fdHd;
	MapPieceHandle mpHd;
	YsVec2 prvScaling,newScaling;
	virtual void Undo(RetroMap_World &world)
	{
		auto mapPiecePtr=world.GetEditableMapPiece(fdHd,mpHd);
		if(nullptr!=mapPiecePtr)
		{
			mapPiecePtr->shape.scaling=prvScaling;
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		auto mapPiecePtr=world.GetEditableMapPiece(fdHd,mpHd);
		if(nullptr!=mapPiecePtr)
		{
			mapPiecePtr->shape.scaling=newScaling;
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::SetScaling(FieldHandle fdHd,MapPieceHandle mpHd,YsVec2 sca)
{
	auto mapPiecePtr=GetEditableMapPiece(fdHd,mpHd);
	if(nullptr!=mapPiecePtr)
	{
		auto undoPtr=NewUndoLog<UndoSetMapPieceScaling>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->mpHd=mpHd;
			undoPtr->prvScaling=mapPiecePtr->shape.scaling;
			undoPtr->newScaling=sca;
		}
		mapPiecePtr->shape.scaling=sca;
		modified=YSTRUE;
		return YSOK;
	}
	return YSERR;
}

class RetroMap_World::UndoMoveMarkUpPoint : public RetroMap_World::UndoLog
{
public:
	class MarkUpPointIndexAndPos : public MarkUpPointIndex
	{
	public:
		YsVec2i prvPos,newPos;
	};
	FieldHandle fdHd;
	YsArray <MarkUpPointIndexAndPos> posLog;

	virtual void Undo(RetroMap_World &world)
	{
		for(auto p : posLog)
		{
			auto markUpPtr=world.GetEditableMarkUp(fdHd,p.muHd);
			if(nullptr!=markUpPtr && YSTRUE==markUpPtr->plg.IsInRange(p.idx))
			{
				markUpPtr->plg[p.idx]=p.prvPos;
			}
		}
	}
	virtual void Redo(RetroMap_World &world)
	{
		for(auto p : posLog)
		{
			auto markUpPtr=world.GetEditableMarkUp(fdHd,p.muHd);
			if(nullptr!=markUpPtr && YSTRUE==markUpPtr->plg.IsInRange(p.idx))
			{
				markUpPtr->plg[p.idx]=p.newPos;
			}
		}
	}
	virtual FieldHandle SwitchCurrentFdHd(RetroMap_World &) const
	{
		return fdHd;
	}
};
YSRESULT RetroMap_World::ReapplyAnchor(FieldHandle fdHd,MapPieceHandle mpHd)
{
	YsArray <RetroMap_World::UndoMoveMarkUpPoint::MarkUpPointIndexAndPos> posLog;
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto anHd : fieldPtr->FindAnchorFromBaseMapPiece(mpHd))
		{
			auto anchorPtr=fieldPtr->GetAnchor(anHd);
			if(nullptr!=anchorPtr)
			{
				auto muHd=anchorPtr->GetMarkUp(*fieldPtr);
				auto markUpPtr=GetEditableMarkUp(fdHd,muHd);
				if(nullptr!=markUpPtr && YSTRUE==markUpPtr->plg.IsInRange(anchorPtr->markUpPointIdx))
				{
					posLog.Increment();
					posLog.back().muHd=muHd;
					posLog.back().idx=anchorPtr->markUpPointIdx;
					posLog.back().prvPos=markUpPtr->plg[anchorPtr->markUpPointIdx];

					markUpPtr->plg[anchorPtr->markUpPointIdx]=fieldPtr->GetAnchorPosition(anHd);

					posLog.back().newPos=markUpPtr->plg[anchorPtr->markUpPointIdx];
				}
			}
		}

		auto undoPtr=NewUndoLog<UndoMoveMarkUpPoint>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->posLog.MoveFrom(posLog);
		}

		return YSOK;
	}
	return YSERR;
}
YSRESULT RetroMap_World::ReapplyAnchor(FieldHandle fdHd,MarkUpHandle baseMuHd)
{
	YsArray <RetroMap_World::UndoMoveMarkUpPoint::MarkUpPointIndexAndPos> posLog;
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto anHd : fieldPtr->FindAnchorFromBaseMarkUp(baseMuHd))
		{
			auto anchorPtr=fieldPtr->GetAnchor(anHd);
			if(nullptr!=anchorPtr)
			{
				auto muHd=anchorPtr->GetMarkUp(*fieldPtr);
				auto markUpPtr=GetEditableMarkUp(fdHd,muHd);
				if(nullptr!=markUpPtr && YSTRUE==markUpPtr->plg.IsInRange(anchorPtr->markUpPointIdx))
				{
					posLog.Increment();
					posLog.back().muHd=muHd;
					posLog.back().idx=anchorPtr->markUpPointIdx;
					posLog.back().prvPos=markUpPtr->plg[anchorPtr->markUpPointIdx];

					markUpPtr->plg[anchorPtr->markUpPointIdx]=fieldPtr->GetAnchorPosition(anHd);

					posLog.back().newPos=markUpPtr->plg[anchorPtr->markUpPointIdx];
				}
			}
		}

		auto undoPtr=NewUndoLog<UndoMoveMarkUpPoint>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->posLog.MoveFrom(posLog);
		}

		return YSOK;
	}
	return YSERR;
}
YSRESULT RetroMap_World::ReapplyAnchor(FieldHandle fdHd,const YsConstArrayMask <AnchorHandle> &allAnHd)
{
	YsArray <RetroMap_World::UndoMoveMarkUpPoint::MarkUpPointIndexAndPos> posLog;
	auto fieldPtr=GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto anHd : allAnHd)
		{
			auto anchorPtr=fieldPtr->GetAnchor(anHd);
			if(nullptr!=anchorPtr)
			{
				auto muHd=anchorPtr->GetMarkUp(*fieldPtr);
				auto markUpPtr=GetEditableMarkUp(fdHd,muHd);
				if(nullptr!=markUpPtr && YSTRUE==markUpPtr->plg.IsInRange(anchorPtr->markUpPointIdx))
				{
					posLog.Increment();
					posLog.back().muHd=muHd;
					posLog.back().idx=anchorPtr->markUpPointIdx;
					posLog.back().prvPos=markUpPtr->plg[anchorPtr->markUpPointIdx];

					markUpPtr->plg[anchorPtr->markUpPointIdx]=fieldPtr->GetAnchorPosition(anHd);

					posLog.back().newPos=markUpPtr->plg[anchorPtr->markUpPointIdx];
				}
			}
		}
		auto undoPtr=NewUndoLog<UndoMoveMarkUpPoint>();
		if(nullptr!=undoPtr)
		{
			undoPtr->fdHd=fdHd;
			undoPtr->posLog.MoveFrom(posLog);
		}
	}
	return YSOK;
}

void RetroMap_World::SetFileName(const YsWString &fn)
{
	fileName=fn;
}
const YsWString &RetroMap_World::GetFileName(void) const
{
	return fileName;
}

void RetroMap_World::SetLastUsedImageFile(const YsWString fn)
{
	lastImageFileName=fn;
}
YsWString RetroMap_World::GetLastUsedImageFile(void) const
{
	return lastImageFileName;
}

YsWString RetroMap_World::GetFieldName(FieldHandle fdHd) const
{
	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		return fieldPtr->fieldName;
	}
	return L"";
}

YSRESULT RetroMap_World::DataIntegrityCheck(FieldHandle fdHd) const
{
	printf("Checking Integrity.\n");

	YSRESULT res=YSOK;

	auto fieldPtr=allField[fdHd];
	if(nullptr!=fieldPtr)
	{
		YsKeyStore used;
		for(auto mpHd : fieldPtr->mapPiece.AllHandle())
		{
			auto key=fieldPtr->mapPiece.GetSearchKey(mpHd);
			if(used.IsIncluded(key))
			{
				printf("MapPiece Key %d is used multiple times.\n",key);
				res=YSERR;
			}
			used.Add(key);
		}
		used.CleanUp();
		for(auto muHd : fieldPtr->markUp.AllHandle())
		{
			auto key=fieldPtr->markUp.GetSearchKey(muHd);
			if(used.IsIncluded(key))
			{
				printf("MarkUp Key %d is used multiple times.\n",key);
				res=YSERR;
			}
			used.Add(key);
		}
		used.CleanUp();
	}

	return res;
}
