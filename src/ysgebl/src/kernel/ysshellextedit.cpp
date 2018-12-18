/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit.cpp
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

#include "ysshellextedit.h"
#include "ysshellextio.h"
#include "ysshellextmisc.h"

YsShellExtEdit::EditLog::EditLog()
{
	isRememberSelection=YSFALSE;
	undoCtr=0;
	prev=NULL;
	next=NULL;
}


////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoPrintMessage : public YsShellExtEdit::EditLog
{
public:
	YsString str;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

/* virtual */ void YsShellExtEdit::UndoPrintMessage::Undo(YsShellExtEdit *)
{
	printf("Undoing: %s\n",str.Txt());
}

/* virtual */ void YsShellExtEdit::UndoPrintMessage::Redo(YsShellExtEdit *)
{
	printf("Redoing: %s\n",str.Txt());
}

void YsShellExtEdit::AddUndoMessage(const YsString &str)
{
	NewUndoLog <UndoPrintMessage> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->str=str;
	}
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoRememberSelection : public YsShellExtEdit::EditLog
{
public:
	YsArray <unsigned int> selectedVtKey,selectedPlKey,selectedCeKey,selectedFgKey,selectedVlKey;

	UndoRememberSelection();

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	void SetSelection(YsShellExtEdit *);
	void CaptureSelection(const YsShellExtEdit *);
};

YsShellExtEdit::UndoRememberSelection::UndoRememberSelection()
{
	isRememberSelection=YSTRUE;
}

void YsShellExtEdit::UndoRememberSelection::Undo(YsShellExtEdit *shl)
{
	SetSelection(shl);
}

void YsShellExtEdit::UndoRememberSelection::Redo(YsShellExtEdit *shl)
{
	SetSelection(shl);
}

void YsShellExtEdit::UndoRememberSelection::SetSelection(YsShellExtEdit *shl)
{
	shl->selectedVtKey=selectedVtKey;
	shl->selectedPlKey=selectedPlKey;
	shl->selectedCeKey=selectedCeKey;
	shl->selectedFgKey=selectedFgKey;
	shl->selectedVlKey=selectedVlKey;
}

void YsShellExtEdit::UndoRememberSelection::CaptureSelection(const YsShellExtEdit *shl)
{
	selectedVtKey=shl->selectedVtKey;
	selectedPlKey=shl->selectedPlKey;
	selectedCeKey=shl->selectedCeKey;
	selectedFgKey=shl->selectedFgKey;
	selectedVlKey=shl->selectedVlKey;
}

////////////////////////////////////////////////////////////

YsShellExtEdit::StopIncUndo::StopIncUndo(YsShellExtEdit *shl)
{
	this->shl=shl;
	this->incUndo=shl->PushStopIncUndo();
}

YsShellExtEdit::StopIncUndo::StopIncUndo(YsShellExtEdit &shl)
{
	this->shl=&shl;
	this->incUndo=shl.PushStopIncUndo();
}

YsShellExtEdit::StopIncUndo::~StopIncUndo()
{
	shl->PopIncUndo(incUndo);
}

////////////////////////////////////////////////////////////

YSBOOL YsShellExtEdit::IsEmptyAndHasNeverBeenEdited(void) const
{
	if(0==GetNumVertex() &&
	   0==GetNumPolygon() &&
	   0==GetNumConstEdge() &&
	   0==GetNumFaceGroup() &&
	   0==GetNumVolume() &&
	   NULL==undoPtr &&
	   NULL==redoPtr)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

void YsShellExtEdit::CleanUpEditLog(void)
{
	CleanUpUndoLog();
	CleanUpRedoLog();
}

void YsShellExtEdit::CleanUpUndoLog(void)
{
	YsArray <EditLog *> toDel;
	for(EditLog *ptr=undoPtr; NULL!=ptr; ptr=ptr->prev)
	{
		toDel.Append(ptr);
	}
	for(YSSIZE_T idx=0; idx<toDel.GetN(); ++idx)
	{
		toDel[idx]->NeverBeUndoneAgain(this);
		delete toDel[idx];
	}
	undoPtr=NULL;
	if(NULL!=redoPtr)
	{
		redoPtr->prev=NULL;
	}
}

void YsShellExtEdit::CleanUpRedoLog(void)
{
	YsArray <EditLog *> toDel;
	for(EditLog *ptr=redoPtr; NULL!=ptr; ptr=ptr->next)
	{
		toDel.Append(ptr);
	}
	toDel.Invert();  // Newer entity may be using an older entity.
	for(YSSIZE_T idx=0; idx<toDel.GetN(); ++idx)
	{
		toDel[idx]->NeverBeRedoneAgain(this);
		delete toDel[idx];
	}
	redoPtr=NULL;
	if(NULL!=undoPtr)
	{
		undoPtr->next=NULL;
	}
}

void YsShellExtEdit::AddUndo(EditLog *editLog)
{
	if(YSTRUE==takeEditLog)
	{
		CleanUpRedoLog();

		if(NULL==undoPtr || undoPtr->undoCtr!=undoCtr)
		{
			InsertRememberSelection();
		}

		editLog->undoCtr=undoCtr;
		undoCtr+=incUndo;

		if(NULL!=undoPtr)
		{
			undoPtr->next=editLog;
			editLog->prev=undoPtr;
		}

		undoPtr=editLog;
		redoPtr=NULL;
	}
	else
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  This function must not be called when takeEditLog is YSFALSE.\n");
		delete editLog;
	}
}

void YsShellExtEdit::InsertRememberSelection(void)
{
	// This function is a low-level function.  Never use NewUndoLog class in here.
	UndoRememberSelection *undo=new UndoRememberSelection;
	undo->CaptureSelection(this);
	undo->undoCtr+=undoCtr;
	undoCtr+=incUndo;
	undo->isRememberSelection=YSTRUE;

	if(NULL!=undoPtr)
	{
		undoPtr->next=undo;
		undo->prev=undoPtr;
	}
	undoPtr=undo;
	redoPtr=NULL;
}

YsShellExtEdit::YsShellExtEdit()
{
	CommonInitializer();
}

YsShellExtEdit::YsShellExtEdit(const YsShellExtEdit &shl)
{
	CommonInitializer();
	YsShellExt::CopyFrom((const YsShellExt &)shl);
}
YsShellExtEdit &YsShellExtEdit::operator=(const YsShellExtEdit &shl)
{
	CleanUp();
	YsShellExt::CopyFrom((const YsShellExt &)shl);
	return *this;
}
YsShellExtEdit::YsShellExtEdit(YsShellExtEdit &&shl)
{
	CommonInitializer();
	YsShellExt::MoveFrom((YsShellExt &)shl);
}
YsShellExtEdit &YsShellExtEdit::operator=(YsShellExtEdit &&shl)
{
	CleanUp();
	YsShellExt::MoveFrom((YsShellExt &)shl);
	return *this;
}

YsShellExtEdit::YsShellExtEdit(const YsShellExt &shl)
{
	CommonInitializer();
	YsShellExt::CopyFrom((const YsShellExt &)shl);
}
YsShellExtEdit &YsShellExtEdit::operator=(const YsShellExt &shl)
{
	CleanUp();
	YsShellExt::CopyFrom((const YsShellExt &)shl);
	return *this;
}
YsShellExtEdit::YsShellExtEdit(YsShellExt &&shl)
{
	CommonInitializer();
	YsShellExt::MoveFrom((YsShellExt &)shl);
}
YsShellExtEdit &YsShellExtEdit::operator=(YsShellExt &&shl)
{
	CleanUp();
	YsShellExt::MoveFrom((YsShellExt &)shl);
	return *this;
}

void YsShellExtEdit::CommonInitializer(void)
{
	takeEditLog=YSTRUE;
	undoCtr=0;
	incUndo=1;
	undoPtr=NULL;
	redoPtr=NULL;

	lastSavedUndoCtr=0;
	lastSavedUndoPtr=NULL;

	YsShellExt::EnableSearch();
}

YsShellExtEdit::~YsShellExtEdit()
{
	YsArray <EditLog *> toDel;
	for(EditLog *ptr=undoPtr; NULL!=ptr; ptr=ptr->prev)
	{
		toDel.Append(ptr);
	}
	for(EditLog *ptr=redoPtr; NULL!=ptr; ptr=ptr->next)
	{
		toDel.Append(ptr);
	}
	for(YSSIZE_T idx=0; idx<toDel.GetN(); ++idx)
	{
		delete toDel[idx];
	}
}

void YsShellExtEdit::CleanUp(void)
{
	CleanUpEditLog();
	YsShellExt::CleanUp();
}

void YsShellExtEdit::DeleteAll(void)
{
	StopIncUndo stopIncUndo(*this);
	// DeleteVolume not implemented yet.

	for(auto fgHd : AllFaceGroup())
	{
		DeleteFaceGroup(fgHd);
	}
	for(auto ceHd : AllConstEdge())
	{
		DeleteConstEdge(ceHd);
	}
	for(auto plHd : AllPolygon())
	{
		DeletePolygon(plHd);
	}
	for(auto vtHd : AllVertex())
	{
		DeleteVertex(vtHd);
	}
}

YSRESULT YsShellExtEdit::LoadSrf(YsTextInputStream &inStream)
{
	YsShellExtReader srfReader;
	return srfReader.MergeSrf(*this,inStream);
}

YSRESULT YsShellExtEdit::LoadSrf(const char fn[])
{
	FILE *fp=fopen(fn,"r");
	if(NULL!=fp)
	{
		YsTextFileInputStream inStream(fp);
		YSRESULT res=LoadSrf(inStream);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::LoadObj(const char fn[])
{
	FILE *fp=fopen(fn,"r");
	if(NULL!=fp)
	{
		YsTextFileInputStream inStream(fp);
		YSRESULT res=LoadObj(inStream);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::LoadObj(YsTextInputStream &inStream)
{
	YsShellExtObjReader::ReadOption opt;
	YsShellExtObjReader objReader;
	return objReader.ReadObj(*this,inStream,opt);
}

YSRESULT YsShellExtEdit::LoadOff(const char fn[])
{
	FILE *fp=fopen(fn,"r");
	if(NULL!=fp)
	{
		YsTextFileInputStream inStream(fp);
		YSRESULT res=LoadOff(inStream);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::LoadOff(YsTextInputStream &inStream)
{
	YsShellExtOffReader offReader;
	return offReader.ReadOff(*this,inStream);
}

YSRESULT YsShellExtEdit::LoadDxf(const char fn[])
{
	FILE *fp=fopen(fn,"r");
	if(NULL!=fp)
	{
		YsTextFileInputStream inStream(fp);
		YSRESULT res=LoadDxf(inStream);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::LoadDxf(YsTextInputStream &inStream)
{
	YsShellExtDxfReader dxfReader;
	return dxfReader.ReadDxf(*this,inStream);
}

YSRESULT YsShellExtEdit::LoadStl(const char fn[])
{
	return YsShellExt::LoadStl(fn);
}

YSBOOL YsShellExtEdit::IsModified(void) const
{
	EditLog *effectiveCurrentUndoPtr,*effectiveLastSavedUndoPtr;

	for(effectiveCurrentUndoPtr=undoPtr; 
	    NULL!=effectiveCurrentUndoPtr && YSTRUE==effectiveCurrentUndoPtr->isRememberSelection; 
	    effectiveCurrentUndoPtr=effectiveCurrentUndoPtr->prev);

	for(effectiveLastSavedUndoPtr=lastSavedUndoPtr;
	    NULL!=effectiveLastSavedUndoPtr && YSTRUE==effectiveLastSavedUndoPtr->isRememberSelection;
	    effectiveLastSavedUndoPtr=effectiveLastSavedUndoPtr->prev);

	if(NULL==effectiveCurrentUndoPtr && NULL==effectiveLastSavedUndoPtr)
	{
		return YSFALSE;
	}
	else if(NULL!=effectiveCurrentUndoPtr &&
	        effectiveCurrentUndoPtr==effectiveLastSavedUndoPtr &&
	        lastSavedUndoCtr==effectiveCurrentUndoPtr->undoCtr)
	{
		return YSFALSE;
	}
	return YSTRUE;
}

YsShellExtEdit::SavePoint YsShellExtEdit::GetSavePoint(void) const
{
	SavePoint savePoint;
	if(NULL!=undoPtr)
	{
		savePoint.undoCtr=(int)undoPtr->undoCtr;
	}
	else
	{
		savePoint.undoCtr=0;
	}
	savePoint.undoPtr=undoPtr;
	return savePoint;
}

YSBOOL YsShellExtEdit::IsModified(const YsShellExtEdit::SavePoint &savePoint) const
{
	const EditLog *effectiveCurrentUndoPtr,*effectiveLastSavedUndoPtr;

	for(effectiveCurrentUndoPtr=undoPtr; 
	    NULL!=effectiveCurrentUndoPtr && YSTRUE==effectiveCurrentUndoPtr->isRememberSelection; 
	    effectiveCurrentUndoPtr=effectiveCurrentUndoPtr->prev);

	for(effectiveLastSavedUndoPtr=savePoint.undoPtr;
	    NULL!=effectiveLastSavedUndoPtr && YSTRUE==effectiveLastSavedUndoPtr->isRememberSelection;
	    effectiveLastSavedUndoPtr=effectiveLastSavedUndoPtr->prev);

	if(NULL==effectiveCurrentUndoPtr && NULL==effectiveLastSavedUndoPtr)
	{
		return YSFALSE;
	}
	else if(NULL!=effectiveCurrentUndoPtr &&
	        effectiveCurrentUndoPtr==effectiveLastSavedUndoPtr &&
	        savePoint.undoCtr==effectiveCurrentUndoPtr->undoCtr)
	{
		return YSFALSE;
	}
	return YSTRUE;
}

void YsShellExtEdit::Saved(void) const
{
	EditLog *effectiveCurrentUndoPtr;
	for(effectiveCurrentUndoPtr=undoPtr; 
	    NULL!=effectiveCurrentUndoPtr && YSTRUE==effectiveCurrentUndoPtr->isRememberSelection; 
	    effectiveCurrentUndoPtr=effectiveCurrentUndoPtr->prev);

	lastSavedUndoPtr=effectiveCurrentUndoPtr;
	if(NULL!=lastSavedUndoPtr)
	{
		lastSavedUndoCtr=lastSavedUndoPtr->undoCtr;
	}
	else
	{
		lastSavedUndoCtr=0;
	}
}

void YsShellExtEdit::SetFileName(const wchar_t fn[]) const
{
	this->fn.Set(fn);
}

void YsShellExtEdit::SetFileName(const char fn[]) const
{
	this->fn.SetUTF8String(fn);
}

const wchar_t *YsShellExtEdit::GetFileName(void) const
{
	return fn;
}

const wchar_t *YsShellExtEdit::GetFileName(YsWString &fn) const
{
	fn=this->fn;
	return fn;
}

const char *YsShellExtEdit::GetFileName(YsString &fn) const
{
	fn.EncodeUTF8 <wchar_t> (this->fn);
	return fn;
}

YSRESULT YsShellExtEdit::Undo(void)
{
	if(NULL!=undoPtr && YSTRUE==undoPtr->isRememberSelection)
	{
		SkipUndoOneStep();  // Just skip it.
	}

	if(NULL!=undoPtr)
	{
		const YSSIZE_T undoCtr=undoPtr->undoCtr;
		while(NULL!=undoPtr && undoCtr==undoPtr->undoCtr)
		{
			UndoOneStep();
		}

		if(NULL!=redoPtr && YSTRUE!=redoPtr->isRememberSelection &&
		   NULL!=undoPtr && YSTRUE==undoPtr->isRememberSelection)
		{
			UndoOneStep();
		}

		return YSOK;
	}
	return YSERR;
}

void YsShellExtEdit::UndoOneStep(void)
{
	if(nullptr!=undoPtr)
	{
		undoPtr->Undo(this);
		redoPtr=undoPtr;
		undoPtr=undoPtr->prev;
	}
}

void YsShellExtEdit::SkipUndoOneStep(void)
{
	redoPtr=undoPtr;
	undoPtr=undoPtr->prev;
}

YSRESULT YsShellExtEdit::Redo(void)
{
	if(NULL!=redoPtr && YSTRUE==redoPtr->isRememberSelection)
	{
		SkipRedoOneStep(); // Just skip it.
	}

	if(NULL!=redoPtr)
	{
		const YSSIZE_T undoCtr=redoPtr->undoCtr;
		while(NULL!=redoPtr && undoCtr==redoPtr->undoCtr)
		{
			RedoOneStep();
		}

		if(NULL!=undoPtr && YSTRUE!=undoPtr->isRememberSelection &&
		   NULL!=redoPtr && YSTRUE==redoPtr->isRememberSelection)
		{
			RedoOneStep();
		}

		return YSOK;
	}
	return YSERR;
}

void YsShellExtEdit::RedoOneStep(void)
{
	if(nullptr!=redoPtr)
	{
		redoPtr->Redo(this);
		undoPtr=redoPtr;
		redoPtr=redoPtr->next;
	}
}

void YsShellExtEdit::SkipRedoOneStep(void)
{
	undoPtr=redoPtr;
	redoPtr=redoPtr->next;
}

int YsShellExtEdit::PushStopIncUndo(void)
{
	int prevIncUndo=incUndo;
	incUndo=0;
	return prevIncUndo;
}

void YsShellExtEdit::PopIncUndo(int incUndoIn)
{
	undoCtr+=incUndoIn;
	this->incUndo=incUndoIn;
}

const YsShellExtEdit::EditLog *YsShellExtEdit::GetUndoPointer(void) const
{
	return undoPtr;
}

YSRESULT YsShellExtEdit::UndoUntil(const EditLog *undoPtr)
{
	if(NULL==undoPtr) // Undo all the way
	{
		while(NULL!=this->undoPtr)
		{
			UndoOneStep();
		}
		return YSOK;
	}


	const EditLog *verifyUndoPtr=this->undoPtr;
	while(NULL!=verifyUndoPtr && verifyUndoPtr!=undoPtr)
	{
		verifyUndoPtr=verifyUndoPtr->prev;
	}

	if(NULL!=verifyUndoPtr)
	{
		while(this->undoPtr!=undoPtr && NULL!=this->undoPtr)
		{
			UndoOneStep();
		}
		return YSOK;
	}
	return YSERR;
}

void YsShellExtEdit::DisableEditLog(void)
{
	CleanUpEditLog();
	takeEditLog=YSFALSE;
}
void YsShellExtEdit::EnableEditLog(void)
{
	takeEditLog=YSTRUE;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoAddVertex : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellVertexHandle> vtHdArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoAddVertex::Undo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=vtHdArray.GetN()-1; 0<=idx; --idx)
	{
		shl->FreezeVertex(vtHdArray[idx]);
	}
}

void YsShellExtEdit::UndoAddVertex::Redo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN(); ++idx)
	{
		shl->MeltVertex(vtHdArray[idx]);
	}
}

void YsShellExtEdit::UndoAddVertex::NeverBeUndoneAgain(YsShellExtEdit *)
{
}

void YsShellExtEdit::UndoAddVertex::NeverBeRedoneAgain(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN(); ++idx)
	{
		YsShellExt *shlExt=shl;
		shlExt->DeleteFrozenVertex(vtHdArray[idx]);
	}
}

YsShellVertexHandle YsShellExtEdit::AddVertex(const YsVec3 &pos)
{
	YsShellVertexHandle vtHd=YsShellExt::AddVertex(pos);

	NewUndoLog<UndoAddVertex> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->vtHdArray.MakeUnitLength(vtHd);
	}

	return vtHd;
}

YSRESULT YsShellExtEdit::AddMultiVertex(YsArray <YsShellVertexHandle> &vtHdArray,const YSSIZE_T nVt,const YsVec3 pos[])
{
	vtHdArray.Set(nVt,NULL);
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		vtHdArray[idx]=YsShellExt::AddVertex(pos[idx]);
	}

	NewUndoLog<UndoAddVertex> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->vtHdArray=vtHdArray;
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoChangeVertexAttrib : public YsShellExtEdit::EditLog
{
public:
	YsShellVertexHandle vtHd;
	VertexAttrib prvAttrib,newAttrib;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoChangeVertexAttrib::Undo(YsShellExtEdit *shl)
{
	VertexAttrib *attr=shl->GetVertexAttrib(vtHd);
	attr->CopyFrom(prvAttrib);
}

void YsShellExtEdit::UndoChangeVertexAttrib::Redo(YsShellExtEdit *shl)
{
	VertexAttrib *attr=shl->GetVertexAttrib(vtHd);
	attr->CopyFrom(newAttrib);
}
////////////////////////////////////////////////////////////

YSRESULT YsShellExtEdit::SetVertexAttrib(YsShellVertexHandle vtHd,const VertexAttrib &inAttrib)
{
	if(YSOK==YsShellExt::SetVertexAttrib(vtHd,inAttrib))
	{
		NewUndoLog<UndoChangeVertexAttrib> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->vtHd=vtHd;
			undo.Ptr()->prvAttrib=*GetVertexAttrib(vtHd);
			undo.Ptr()->newAttrib=inAttrib;
		}
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetVertexPosition : public YsShellExtEdit::EditLog
{
public:
	YsShellVertexHandle vtHd;
	YsVec3 prvPos,newPos;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetVertexPosition::Undo(YsShellExtEdit *shl)
{
	YsShell *rawShl=(YsShell *)shl;
	rawShl->ModifyVertexPosition(vtHd,prvPos);
}

void YsShellExtEdit::UndoSetVertexPosition::Redo(YsShellExtEdit *shl)
{
	YsShell *rawShl=(YsShell *)shl;
	rawShl->ModifyVertexPosition(vtHd,newPos);
}

YSRESULT YsShellExtEdit::SetVertexPosition(YsShellVertexHandle vtHd,const YsVec3 &newPos)
{
	if(YSTRUE==takeEditLog)
	{
		NewUndoLog<UndoSetVertexPosition> undo(this);
		undo.Ptr()->vtHd=vtHd;
		GetVertexPosition(undo.Ptr()->prvPos,vtHd);
		undo.Ptr()->newPos=newPos;
	}

	YsShell::ModifyVertexPosition(vtHd,newPos);

	return YSOK;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetMultiVertexPosition : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellVertexHandle> vtHdArray;
	YsArray <YsVec3> prvPosArray,newPosArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetMultiVertexPosition::Undo(YsShellExtEdit *shl)
{
	YsShell *rawShl=(YsShell *)shl;
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN() && idx<prvPosArray.GetN(); ++idx)
	{
		rawShl->ModifyVertexPosition(vtHdArray[idx],prvPosArray[idx]);
	}
}

void YsShellExtEdit::UndoSetMultiVertexPosition::Redo(YsShellExtEdit *shl)
{
	YsShell *rawShl=(YsShell *)shl;
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN() && idx<newPosArray.GetN(); ++idx)
	{
		rawShl->ModifyVertexPosition(vtHdArray[idx],newPosArray[idx]);
	}
}

YSRESULT YsShellExtEdit::SetMultiVertexPosition(YSSIZE_T nVtx,const YsShellVertexHandle vtHdArray[],const YsVec3 vtxPosArray[])
{
	NewUndoLog<UndoSetMultiVertexPosition> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->vtHdArray.Set(nVtx,vtHdArray);
		undo.Ptr()->prvPosArray.Set(nVtx,NULL);
		undo.Ptr()->newPosArray.Set(nVtx,NULL);
		for(YSSIZE_T idx=0; idx<nVtx; ++idx)
		{
			GetVertexPosition(undo.Ptr()->prvPosArray[idx],vtHdArray[idx]);
			undo.Ptr()->newPosArray[idx]=vtxPosArray[idx];
		}
	}

	for(YSSIZE_T idx=0; idx<nVtx; ++idx)
	{
		YsShell::ModifyVertexPosition(vtHdArray[idx],vtxPosArray[idx]);
	}

	return YSOK;
}

YSRESULT YsShellExtEdit::SetMultiVertexPosition(const YsConstArrayMask <YsShellVertexHandle> &vtHdArray,const YsVec3 vtxPosArray[])
{
	return SetMultiVertexPosition(vtHdArray.GetN(),vtHdArray,vtxPosArray);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoVertexAttribChange : public YsShellExtEdit::EditLog
{
public:
	YsShellVertexHandle vtHd;
	VertexAttrib prvAttrib,newAttrib;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoVertexAttribChange::Undo(YsShellExtEdit *shl)
{
	VertexAttrib *attr=shl->GetVertexAttrib(vtHd);
	attr->CopyFrom(prvAttrib);
}

void YsShellExtEdit::UndoVertexAttribChange::Redo(YsShellExtEdit *shl)
{
	VertexAttrib *attr=shl->GetVertexAttrib(vtHd);
	attr->CopyFrom(newAttrib);
}

// YSRESULT YsShellExtEdit::SetVertexSomeAttrib(YsShellVertexHandle vtHd,new attrib)
// {
// 	VertexAttrib *attr=GetVertexAttrib(vtHd);
// 	if(NULL!=attr)
// 	{
//		NewUndoLog<UndoVertexAttribChang undo(this);
// 		if(nullptr!=undo)
// 		{
// 			undo.Ptr()->vtHd=vtHd;
// 			undo.Ptr()->prvAttrib.CopyFrom(*attr);
// 		}
// 
// 		attr->?=?;
// 
// 		if(nullptr!=undo)
// 		{
// 			undo.Ptr()->newAttrib.CopyFrom(*attr);
// 		}
// 		return YSOK;
// 	}
// 	return YSERR;
// }

YSRESULT YsShellExtEdit::SetVertexRoundFlag(YsShellVertexHandle vtHd,YSBOOL round)
{
	VertexAttrib *attr=GetVertexAttrib(vtHd);
	if(NULL!=attr)
	{
		NewUndoLog <UndoVertexAttribChange> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->vtHd=vtHd;
			undo.Ptr()->prvAttrib.CopyFrom(*attr);
		}

		attr->SetRound(round);

		if(nullptr!=undo)
		{
			undo.Ptr()->newAttrib.CopyFrom(*attr);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::SetVertexConstrainedFlag(YsShellVertexHandle vtHd,YSBOOL constFlag)
{
	VertexAttrib *attr=GetVertexAttrib(vtHd);
	if(NULL!=attr)
	{
		NewUndoLog <UndoVertexAttribChange> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->vtHd=vtHd;
			undo.Ptr()->prvAttrib.CopyFrom(*attr);
		}

		attr->SetConstrained(constFlag);

		if(nullptr!=undo)
		{
			undo.Ptr()->newAttrib.CopyFrom(*attr);
		}
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoDeleteVertex : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellVertexHandle> vtHdArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoDeleteVertex::Undo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN(); ++idx)
	{
		shl->MeltVertex(vtHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeleteVertex::Redo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=vtHdArray.GetN()-1; 0<=idx; --idx)
	{
		shl->FreezeVertex(vtHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeleteVertex::NeverBeUndoneAgain(YsShellExtEdit *shl)
{
	// Means, the vertices will never be back alive again.  Can permanently be deleted.
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN(); ++idx)
	{
		YsShellExt *shlExt=shl;
		shlExt->DeleteFrozenVertex(vtHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeleteVertex::NeverBeRedoneAgain(YsShellExtEdit *)
{
}

YSRESULT YsShellExtEdit::DeleteVertex(YsShellVertexHandle vtHd)
{
	const YsShellVertexHandle vtHdArray[1]={vtHd};
	return DeleteMultiVertex(1,vtHdArray);
}

YSRESULT YsShellExtEdit::DeleteMultiVertex(const YSSIZE_T nVt,const YsShellVertexHandle vtHd[])
{
	YSRESULT res=YSOK;

	YsArray <YsShellVertexHandle> actuallyDeleted;
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		if(YSOK==YsShellExt::FreezeVertex(vtHd[idx]))
		{
			actuallyDeleted.Append(vtHd[idx]);
		}
		else
		{
			res=YSERR;
		}
	}

	NewUndoLog<UndoDeleteVertex> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->vtHdArray=actuallyDeleted;
	}

	return res;
}

YSRESULT YsShellExtEdit::DeleteMultiVertex(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray)
{
	return DeleteMultiVertex(vtHdArray.GetN(),vtHdArray);
}

////////////////////////////////////////////////////////////

YSRESULT YsShellExtEdit::SelectVertex(const YSSIZE_T nVt,const YsShellVertexHandle vtHd[])
{
	selectedVtKey.Set(nVt,NULL);
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		selectedVtKey[idx]=GetSearchKey(vtHd[idx]);
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::SelectVertex(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray)
{
	return SelectVertex(vtHdArray.GetN(),vtHdArray);
}

YSRESULT YsShellExtEdit::AddSelectedVertex(const YSSIZE_T nVt,const YsShellVertexHandle vtHd[])
{
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		selectedVtKey.Append(GetSearchKey(vtHd[idx]));
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::AddSelectedVertex(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray)
{
	return AddSelectedVertex(vtHdArray.GetN(),vtHdArray);
}

YsArray <YsShellVertexHandle> YsShellExtEdit::GetSelectedVertex(void) const
{
	YsArray <YsShellVertexHandle> selVtHd;
	GetSelectedVertex(selVtHd);
	return selVtHd;
}

YSRESULT YsShellExtEdit::SelectPolygon(const YSSIZE_T nPl,const YsShellPolygonHandle plHd[])
{
	selectedPlKey.Set(nPl,NULL);
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		selectedPlKey[idx]=GetSearchKey(plHd[idx]);
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::SelectPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray)
{
	return SelectPolygon(plHdArray.GetN(),plHdArray);
}

YSRESULT YsShellExtEdit::AddSelectedPolygon(const YSSIZE_T nPl,const YsShellPolygonHandle plHd[])
{
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		selectedPlKey.Append(GetSearchKey(plHd[idx]));
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::AddSelectedPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray)
{
	return AddSelectedPolygon(plHdArray.GetN(),plHdArray);
}

YsArray <YsShellPolygonHandle> YsShellExtEdit::GetSelectedPolygon(void) const
{
	YsArray <YsShellPolygonHandle> selPlHd;
	GetSelectedPolygon(selPlHd);
	return selPlHd;
}

YSRESULT YsShellExtEdit::SelectConstEdge(const YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[])
{
	selectedCeKey.Set(nVt,NULL);
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		selectedCeKey[idx]=GetSearchKey(ceHd[idx]);
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::SelectConstEdge(const YsConstArrayMask <YsShellExt::ConstEdgeHandle> &ceHdArray)
{
	return SelectConstEdge(ceHdArray.GetN(),ceHdArray);
}

YSRESULT YsShellExtEdit::AddSelectedConstEdge(const YSSIZE_T nVt,const YsShellExt::ConstEdgeHandle ceHd[])
{
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		selectedCeKey.Append(GetSearchKey(ceHd[idx]));
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::AddSelectedConstEdge(const YsConstArrayMask <YsShellExt::ConstEdgeHandle> &ceHdArray)
{
	return AddSelectedConstEdge(ceHdArray.GetN(),ceHdArray);
}

YsArray <YsShellExt::ConstEdgeHandle> YsShellExtEdit::GetSelectedConstEdge(void) const
{
	YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
	GetSelectedConstEdge(selCeHd);
	return selCeHd;
}

YSRESULT YsShellExtEdit::SelectFaceGroup(const YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHd[])
{
	selectedFgKey.Set(nFg,NULL);
	for(YSSIZE_T idx=0; idx<nFg; ++idx)
	{
		selectedFgKey[idx]=GetSearchKey(fgHd[idx]);
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::SelectFaceGroup(const YsConstArrayMask <YsShellExt::FaceGroupHandle> &fgHdArray)
{
	return SelectFaceGroup(fgHdArray.GetN(),fgHdArray);
}

YSRESULT YsShellExtEdit::AddSelectedFaceGroup(const YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHd[])
{
	for(YSSIZE_T idx=0; idx<nFg; ++idx)
	{
		selectedFgKey.Append(GetSearchKey(fgHd[idx]));
	}
	return YSOK;
}

YSRESULT YsShellExtEdit::AddSelectedFaceGroup(const YsConstArrayMask <YsShellExt::FaceGroupHandle> &fgHdArray)
{
	return AddSelectedFaceGroup(fgHdArray.GetN(),fgHdArray);
}

YsArray <YsShellExt::FaceGroupHandle> YsShellExtEdit::GetSelectedFaceGroup(void) const
{
	YsArray <YsShellExt::FaceGroupHandle> selFgHd;
	GetSelectedFaceGroup(selFgHd);
	return selFgHd;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoAddPolygon : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellPolygonHandle> plHdArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoAddPolygon::Undo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=plHdArray.GetN()-1; 0<=idx; --idx)
	{
		YsShellExt *shlExt=shl;
		shlExt->FreezePolygon(plHdArray[idx]);
	}
}

void YsShellExtEdit::UndoAddPolygon::Redo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		YsShellExt *shlExt=shl;
		shlExt->MeltPolygon(plHdArray[idx]);
	}
}

void YsShellExtEdit::UndoAddPolygon::NeverBeUndoneAgain(YsShellExtEdit *)
{
}

void YsShellExtEdit::UndoAddPolygon::NeverBeRedoneAgain(YsShellExtEdit *shl)
{
	// Means that the polygon will never be restored again.  Can be permanently deleted.
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		YsShellExt *shlExt=shl;
		shlExt->DeleteFrozenPolygon(plHdArray[idx]);
	}
}

YsShellPolygonHandle YsShellExtEdit::AddPolygon(YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[])
{
	NewUndoLog<UndoAddPolygon> undo(this);

	YsShellPolygonHandle plHd=YsShellExt::AddPolygon(nVt,vtHdArray);

	if(nullptr!=undo)
	{
		undo.Ptr()->plHdArray.MakeUnitLength(plHd);
	}

	return plHd;
}

YsShell::PolygonHandle YsShellExtEdit::AddPolygon(const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray)
{
	return AddPolygon(vtHdArray.GetN(),vtHdArray);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoDeletePolygon : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellPolygonHandle> plHdArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoDeletePolygon::Undo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		shl->MeltPolygon(plHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeletePolygon::Redo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		shl->FreezePolygon(plHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeletePolygon::NeverBeUndoneAgain(YsShellExtEdit *shl)
{
	// Won't be alive again.
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		YsShellExt *shlExt=shl;
		shlExt->DeleteFrozenPolygon(plHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeletePolygon::NeverBeRedoneAgain(YsShellExtEdit *)
{
}

YSRESULT YsShellExtEdit::DeletePolygon(YsShellPolygonHandle plHd)
{
	const YsShellPolygonHandle plHdArray[1]={plHd};
	return DeleteMultiPolygon(1,plHdArray);
}

YSRESULT YsShellExtEdit::DeleteMultiPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	YSRESULT res=YSOK;

	YsArray <YsShellPolygonHandle> actuallyDeleted;
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		if(YSOK==FreezePolygon(plHdArray[idx]))
		{
			actuallyDeleted.Append(plHdArray[idx]);
		}
		else
		{
			res=YSERR;
		}
	}

	if(0<actuallyDeleted.GetN())
	{
		NewUndoLog<UndoDeletePolygon> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->plHdArray=actuallyDeleted;
		}
	}

	return res;
}

YSRESULT YsShellExtEdit::DeleteMultiPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray)
{
	return DeleteMultiPolygon(plHdArray.GetN(),plHdArray);
}

////////////////////////////////////////////////////////////

YSRESULT YsShellExtEdit::ForceDeletePolygon(YsShellPolygonHandle plHd)
{
	DeleteFaceGroupPolygon(plHd);
	return DeletePolygon(plHd);
}

YSRESULT YsShellExtEdit::ForceDeleteMultiPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		DeleteFaceGroupPolygon(plHdArray[idx]);
	}
	return DeleteMultiPolygon(nPl,plHdArray);
}

YSRESULT YsShellExtEdit::ForceDeleteMultiPolygon(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray)
{
	return ForceDeleteMultiPolygon(plHdArray.GetN(),plHdArray);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetPolygonVertex : public YsShellExtEdit::EditLog
{
public:
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle> prvVtHd,newVtHd;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetPolygonVertex::Undo(YsShellExtEdit *shl)
{
	YsShellExt *rawShl=(YsShellExt *)shl;
	rawShl->SetPolygonVertex(plHd,prvVtHd.GetN(),prvVtHd);
}

void YsShellExtEdit::UndoSetPolygonVertex::Redo(YsShellExtEdit *shl)
{
	YsShellExt *rawShl=(YsShellExt *)shl;
	rawShl->SetPolygonVertex(plHd,newVtHd.GetN(),newVtHd);
}

YSRESULT YsShellExtEdit::SetPolygonVertex(YsShellPolygonHandle plHd,YSSIZE_T nPlVt,const YsShellVertexHandle plVtHd[])
{
	NewUndoLog<UndoSetPolygonVertex> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->plHd=plHd;
		GetVertexListOfPolygon(undo.Ptr()->prvVtHd,plHd);
		undo.Ptr()->newVtHd.Set(nPlVt,plVtHd);
	}

	return YsShellExt::SetPolygonVertex(plHd,nPlVt,plVtHd);
}

YSRESULT YsShellExtEdit::SetPolygonVertex(YsShellPolygonHandle plHd,const YsConstArrayMask <YsShell::VertexHandle> &plVtHd)
{
	return SetPolygonVertex(plHd,plVtHd.GetN(),plVtHd);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetPolygonColor : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellPolygonHandle> plHdArray;
	YsArray <YsColor> prevCol,newCol;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetPolygonColor::Undo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		shl->SetColorOfPolygon(plHdArray[idx],prevCol[idx]);
	}
}

void YsShellExtEdit::UndoSetPolygonColor::Redo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		shl->SetColorOfPolygon(plHdArray[idx],newCol[idx]);
	}
}

YSRESULT YsShellExtEdit::SetPolygonColor(YsShellPolygonHandle plHd,const YsColor &col)
{
	NewUndoLog<UndoSetPolygonColor> undo(this);

	if(nullptr!=undo)
	{
		YsColor prevCol;
		GetColorOfPolygon(prevCol,plHd);
		undo.Ptr()->plHdArray.MakeUnitLength(plHd);
		undo.Ptr()->prevCol.MakeUnitLength(prevCol);
		undo.Ptr()->newCol.MakeUnitLength(col);
	}

	YsShellExt::SetColorOfPolygon(plHd,col);

	return YSOK;
}

YSRESULT YsShellExtEdit::SetPolygonAlpha(YsShellPolygonHandle plHd,const double alpha)
{
	YsColor col;
	GetColorOfPolygon(col,plHd);

	NewUndoLog<UndoSetPolygonColor> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->plHdArray.MakeUnitLength(plHd);
		undo.Ptr()->prevCol.MakeUnitLength(col);
	}

	col.SetAd(alpha);
	YsShellExt::SetColorOfPolygon(plHd,col);

	if(nullptr!=undo)
	{
		undo.Ptr()->newCol.MakeUnitLength(col);
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoChangePolygonAttrib : public YsShellExtEdit::EditLog
{
public:
	YsShellPolygonHandle plHd;
	PolygonAttrib prvAttrib,newAttrib;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoChangePolygonAttrib::Undo(YsShellExtEdit *shl)
{
	PolygonAttrib *attr=shl->GetEditablePolygonAttrib(plHd);
	attr->CopyFrom(prvAttrib);
}

void YsShellExtEdit::UndoChangePolygonAttrib::Redo(YsShellExtEdit *shl)
{
	PolygonAttrib *attr=shl->GetEditablePolygonAttrib(plHd);
	attr->CopyFrom(newAttrib);
}

// YSRESULT YsShellExtEdit::SetPolygonSomething(YsShellPolygonHandle plHd,incoming prop)
// {
// 	PolygonAttrib *attr=GetEditablePolygonAttrib(plHd);
// 	if(NULL!=attr)
// 	{
//		NewUndoLog<UndoChangePolygonAttrib> undo(this);
// 		if(nullptr!=undo)
// 		{
// 			undo.Ptr()->plHd=plHd;
// 			undo.Ptr()->prvAttrib.CopyFrom(*attr);
// 		}
// 
// 		attr->?=?;
// 
// 		if(nullptr!=undo)
// 		{
// 			undo.Ptr()->newAttrib.CopyFrom(*attr);
// 		}
// 
// 		return YSOK;
// 	}
// 	return YSERR;
// }

YSRESULT YsShellExtEdit::SetPolygonDesiredElementSize(YsShellPolygonHandle plHd,const double elemSize)
{
	PolygonAttrib *attr=GetEditablePolygonAttrib(plHd);
	if(NULL!=attr)
	{
		NewUndoLog<UndoChangePolygonAttrib> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->plHd=plHd;
			undo.Ptr()->prvAttrib.CopyFrom(*attr);
		}

		attr->elemSize=elemSize;

		if(nullptr!=undo)
		{
			undo.Ptr()->newAttrib.CopyFrom(*attr);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::SetPolygonNoShadingFlag(YsShellPolygonHandle plHd,YSBOOL noShading)
{
	PolygonAttrib *attr=GetEditablePolygonAttrib(plHd);
	if(NULL!=attr)
	{
		NewUndoLog<UndoChangePolygonAttrib> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->plHd=plHd;
			undo.Ptr()->prvAttrib.CopyFrom(*attr);
		}

		attr->SetNoShading(noShading);

		if(nullptr!=undo)
		{
			undo.Ptr()->newAttrib.CopyFrom(*attr);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::SetPolygonAsLightFlag(YsShellPolygonHandle plHd,YSBOOL asLight)
{
	PolygonAttrib *attr=GetEditablePolygonAttrib(plHd);
	if(NULL!=attr)
	{
		NewUndoLog<UndoChangePolygonAttrib> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->plHd=plHd;
			undo.Ptr()->prvAttrib.CopyFrom(*attr);
		}

		attr->SetAsLight(asLight);

		if(nullptr!=undo)
		{
			undo.Ptr()->newAttrib.CopyFrom(*attr);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit::SetPolygonTwoSidedFlag(YsShellPolygonHandle plHd,YSBOOL twoSided)
{
	PolygonAttrib *attr=GetEditablePolygonAttrib(plHd);
	if(NULL!=attr)
	{
		NewUndoLog<UndoChangePolygonAttrib> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->plHd=plHd;
			undo.Ptr()->prvAttrib.CopyFrom(*attr);
		}

		attr->SetTwoSided(twoSided);

		if(nullptr!=undo)
		{
			undo.Ptr()->newAttrib.CopyFrom(*attr);
		}

		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

YSRESULT YsShellExtEdit::SetPolygonAttrib(YsShellPolygonHandle plHd,const PolygonAttrib &inAttrib)
{
	if(YSOK==YsShellExt::SetPolygonAttrib(plHd,inAttrib))
	{
		NewUndoLog<UndoChangePolygonAttrib> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->plHd=plHd;
			undo.Ptr()->prvAttrib=*GetEditablePolygonAttrib(plHd);
			undo.Ptr()->newAttrib=inAttrib;
		}
		return YSOK;
	}
	return YSERR;
}


////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetPolygonNormal : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellPolygonHandle> plHdArray;
	YsArray <YsVec3> prvNom,newNom;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetPolygonNormal::Undo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		shl->SetNormalOfPolygon(plHdArray[idx],prvNom[idx]);
	}
}

void YsShellExtEdit::UndoSetPolygonNormal::Redo(YsShellExtEdit *shl)
{
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		shl->SetNormalOfPolygon(plHdArray[idx],newNom[idx]);
	}
}

YSRESULT YsShellExtEdit::SetPolygonNormal(YsShellPolygonHandle plHd,const YsVec3 &nom)
{
	const YsShellPolygonHandle plHdArray[1]={plHd};
	const YsVec3 nomArray[1]={nom};
	return SetPolygonNormalMulti(1,plHdArray,nomArray);
}

YSRESULT YsShellExtEdit::SetPolygonNormalMulti(const YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[],const YsVec3 nomArray[])
{
	NewUndoLog<UndoSetPolygonNormal> undo(this);

	if(nullptr!=undo)
	{
		undo.Ptr()->plHdArray.Set(nPl,plHdArray);
		undo.Ptr()->prvNom.Set(nPl,NULL);
		for(YSSIZE_T idx=0; idx<nPl; ++idx)
		{
			GetNormalOfPolygon(undo.Ptr()->prvNom[idx],plHdArray[idx]);
		}
		undo.Ptr()->newNom.Set(nPl,nomArray);
	}

	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		SetNormalOfPolygon(plHdArray[idx],nomArray[idx]);
	}

	return YSOK;
}

YSRESULT YsShellExtEdit::SetPolygonNormalMulti(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray,const YsVec3 nomArray[])
{
	return SetPolygonNormalMulti(plHdArray.GetN(),plHdArray,nomArray);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoAddConstEdge : public YsShellExtEdit::EditLog
{
public:
	YsShellExt::ConstEdgeHandle ceHd;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoAddConstEdge::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=shl;
	shlExt->FreezeConstEdge(ceHd);
}

void YsShellExtEdit::UndoAddConstEdge::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=shl;
	shlExt->MeltConstEdge(ceHd);
}

void YsShellExtEdit::UndoAddConstEdge::NeverBeUndoneAgain(YsShellExtEdit *)
{
}

void YsShellExtEdit::UndoAddConstEdge::NeverBeRedoneAgain(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=shl;
	shlExt->DeleteFrozenConstEdge(ceHd);
}

YsShellExt::ConstEdgeHandle YsShellExtEdit::AddConstEdge(YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[],YSBOOL isLoop)
{
	YsShellExt *shlExt=this;
	YsShellExt::ConstEdgeHandle ceHd=shlExt->AddConstEdge(nVt,ceVtHd,isLoop);

	if(nullptr!=ceHd)
	{
		NewUndoLog<UndoAddConstEdge> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->ceHd=ceHd;
		}
	}

	return ceHd;
}

YsShellExt::ConstEdgeHandle YsShellExtEdit::AddConstEdge(const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,YSBOOL isLoop)
{
	return AddConstEdge(ceVtHd.GetN(),ceVtHd,isLoop);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetConstEdgeIsLoop : public YsShellExtEdit::EditLog
{
public:
	YsShellExtEdit::ConstEdgeHandle ceHd;
	YSBOOL prvIsLoop,newIsLoop;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetConstEdgeIsLoop::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeIsLoop(ceHd,prvIsLoop);
}

void YsShellExtEdit::UndoSetConstEdgeIsLoop::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeIsLoop(ceHd,newIsLoop);
}

YSRESULT YsShellExtEdit::SetConstEdgeIsLoop(ConstEdgeHandle ceHd,YSBOOL isLoop)
{
	YsShellExt *shlExt=this;
	if(isLoop!=shlExt->IsConstEdgeLoop(ceHd))
	{
		NewUndoLog<UndoSetConstEdgeIsLoop> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->prvIsLoop=shlExt->IsConstEdgeLoop(ceHd);
			undo.Ptr()->newIsLoop=isLoop;
		}
		shlExt->SetConstEdgeIsLoop(ceHd,isLoop);
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetConstEdgeIsCrease : public YsShellExtEdit::EditLog
{
public:
	YsShellExtEdit::ConstEdgeHandle ceHd;
	YSBOOL prvIsCrease,newIsCrease;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetConstEdgeIsCrease::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeIsCrease(ceHd,prvIsCrease);
}

void YsShellExtEdit::UndoSetConstEdgeIsCrease::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeIsCrease(ceHd,newIsCrease);
}

YSRESULT YsShellExtEdit::SetConstEdgeIsCrease(ConstEdgeHandle ceHd,YSBOOL isCrease)
{
	YsShellExt *shlExt=this;
	if(isCrease!=shlExt->IsCreaseConstEdge(ceHd))
	{
		NewUndoLog<UndoSetConstEdgeIsCrease> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->ceHd=ceHd;
			undo.Ptr()->prvIsCrease=shlExt->IsCreaseConstEdge(ceHd);
			undo.Ptr()->newIsCrease=isCrease;
		}
		shlExt->SetConstEdgeIsCrease(ceHd,isCrease);
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoModifyConstEdge : public YsShellExtEdit::EditLog
{
public:
	YsShellExtEdit::ConstEdgeHandle ceHd;
	YsArray <YsShellVertexHandle> prvCeVtHd,newCeVtHd;
	YSBOOL prvIsLoop,newIsLoop;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoModifyConstEdge::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->ModifyConstEdge(ceHd,prvCeVtHd,prvIsLoop);
}

void YsShellExtEdit::UndoModifyConstEdge::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->ModifyConstEdge(ceHd,newCeVtHd,newIsLoop);
}

YSRESULT YsShellExtEdit::ModifyConstEdge(ConstEdgeHandle ceHd,YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[],YSBOOL isLoop)
{
	if(YSTFUNKNOWN==isLoop)
	{
		isLoop=IsConstEdgeLoop(ceHd);
	}

	NewUndoLog<UndoModifyConstEdge> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->ceHd=ceHd;
		GetConstEdge(undo.Ptr()->prvCeVtHd,undo.Ptr()->prvIsLoop,ceHd);
		undo.Ptr()->newCeVtHd.Set(nVt,ceVtHd);
		undo.Ptr()->newIsLoop=isLoop;
	}

	return YsShellExt::ModifyConstEdge(ceHd,nVt,ceVtHd,isLoop);
}

YSRESULT YsShellExtEdit::ModifyConstEdge(ConstEdgeHandle ceHd,const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,YSBOOL isLoop)
{
	return ModifyConstEdge(ceHd,ceVtHd.GetN(),ceVtHd,isLoop);
}

////////////////////////////////////////////////////////////

// YSRESULT YsShellExtEdit::AddConstEdgeVertex(ConstEdgeHandle ceHd,const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd)
// {
// 	return AddConstEdgeVertex(ceHd,ceVtHd.GetN(),ceVtHd);
// }

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoDeleteMultiConstEdge : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellExt::ConstEdgeHandle> ceHdArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoDeleteMultiConstEdge::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(YSSIZE_T idx=0; idx<ceHdArray.GetN(); ++idx)
	{
		shlExt->MeltConstEdge(ceHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeleteMultiConstEdge::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(YSSIZE_T idx=0; idx<ceHdArray.GetN(); ++idx)
	{
		shlExt->FreezeConstEdge(ceHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeleteMultiConstEdge::NeverBeUndoneAgain(YsShellExtEdit *shl)
{
	// Means const edges will never be alive again.
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(YSSIZE_T idx=0; idx<ceHdArray.GetN(); ++idx)
	{
		shlExt->DeleteFrozenConstEdge(ceHdArray[idx]);
	}
}

void YsShellExtEdit::UndoDeleteMultiConstEdge::NeverBeRedoneAgain(YsShellExtEdit *)
{
}

YSRESULT YsShellExtEdit::DeleteMultiConstEdge(YSSIZE_T nCe,const ConstEdgeHandle ceHdArray[])
{
	NewUndoLog<UndoDeleteMultiConstEdge> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->ceHdArray.Set(nCe,ceHdArray);
	}

	for(YSSIZE_T idx=0; idx<nCe; ++idx)
	{
		YsShellExt::FreezeConstEdge(ceHdArray[idx]);
	}

	return YSOK;
}

YSRESULT YsShellExtEdit::DeleteMultiConstEdge(const YsConstArrayMask <ConstEdgeHandle> &ceHdArray)
{
	return DeleteMultiConstEdge(ceHdArray.GetN(),ceHdArray);
}

YSRESULT YsShellExtEdit::DeleteConstEdge(ConstEdgeHandle ceHd)
{
	return DeleteMultiConstEdge(1,&ceHd);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetConstEdgeLabel : public YsShellExtEdit::EditLog
{
public:
	YsShellExt::ConstEdgeHandle ceHd;
	YsString prvLabel,newLabel;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetConstEdgeLabel::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeLabel(ceHd,prvLabel);
}

void YsShellExtEdit::UndoSetConstEdgeLabel::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeLabel(ceHd,newLabel);
}

YSRESULT YsShellExtEdit::SetConstEdgeLabel(YsShellExt::ConstEdgeHandle ceHd,const char label[])
{
	YsShellExt *shlExt=(YsShellExt *)this;

	NewUndoLog<UndoSetConstEdgeLabel> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->ceHd=ceHd;
		undo.Ptr()->prvLabel=shlExt->GetConstEdgeLabel(ceHd);
	}

	if(YSOK==shlExt->SetConstEdgeLabel(ceHd,label))
	{
		if(nullptr!=undo)
		{
			undo.Ptr()->newLabel=label;
		}
		return YSOK;
	}
	else
	{
		undo.Discard();
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetConstEdgeAttrib : public YsShellExtEdit::EditLog
{
public:
	YsShellExtEdit::ConstEdgeHandle ceHd;
	YsShellExt::ConstEdgeAttrib prvAttrib,newAttrib;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetConstEdgeAttrib::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeAttrib(ceHd,prvAttrib);
}

void YsShellExtEdit::UndoSetConstEdgeAttrib::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetConstEdgeAttrib(ceHd,newAttrib);
}

YSRESULT YsShellExtEdit::SetConstEdgeAttrib(YsShellExt::ConstEdgeHandle ceHd,const YsShellExt::ConstEdgeAttrib &attrib)
{
	YsShellExt *shlExt=(YsShellExt *)this;

	NewUndoLog<UndoSetConstEdgeAttrib> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->ceHd=ceHd;
		undo.Ptr()->prvAttrib=shlExt->GetConstEdgeAttrib(ceHd);
		undo.Ptr()->newAttrib=attrib;
	}

	if(YSOK==shlExt->SetConstEdgeAttrib(ceHd,attrib))
	{
		return YSOK;
	}
	else
	{
		undo.Discard();
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoAddFaceGroup : public YsShellExtEdit::EditLog
{
public:
	YsShellExt::FaceGroupHandle fgHd;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoAddFaceGroup::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->FreezeFaceGroup(fgHd);
}

void YsShellExtEdit::UndoAddFaceGroup::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->MeltFaceGroup(fgHd);
}

void YsShellExtEdit::UndoAddFaceGroup::NeverBeUndoneAgain(YsShellExtEdit *)
{
}

void YsShellExtEdit::UndoAddFaceGroup::NeverBeRedoneAgain(YsShellExtEdit *shl)
{
	// Means never be melted again.
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->DeleteFrozenFaceGroup(fgHd);
}

YsShellExt::FaceGroupHandle YsShellExtEdit::AddFaceGroup(YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[])
{
	for(YSSIZE_T plIdx=0; plIdx<nPl; ++plIdx)
	{
		if(NULL!=FindFaceGroupFromPolygon(fgPlHd[plIdx]))
		{
			return NULL;  // A polygon already used.
		}
	}

	YsShellExt::FaceGroupHandle fgHd=YsShellExt::AddFaceGroup(nPl,fgPlHd);

	NewUndoLog<UndoAddFaceGroup> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->fgHd=fgHd;
	}

	return fgHd;
}

YsShellExt::FaceGroupHandle YsShellExtEdit::AddFaceGroup(const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd)
{
	return AddFaceGroup(fgPlHd.GetN(),fgPlHd);
}


////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoModifyFaceGroup : public YsShellExtEdit::EditLog
{
public:
	YsShellExt::FaceGroupHandle fgHd;
	YsArray <YsShellPolygonHandle> orgPlHd,newPlHd;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoModifyFaceGroup::Undo(YsShellExtEdit *shl)
{
	YsShellExt &shlExt=(YsShellExt &)(*shl);
	shlExt.ModifyFaceGroup(fgHd,orgPlHd);
}

void YsShellExtEdit::UndoModifyFaceGroup::Redo(YsShellExtEdit *shl)
{
	YsShellExt &shlExt=(YsShellExt &)(*shl);
	shlExt.ModifyFaceGroup(fgHd,newPlHd);
}

YSRESULT YsShellExtEdit::AddFaceGroupPolygon(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[])
{
	if(0>=nPl)
	{
		return YSOK;
	}

	NewUndoLog<UndoModifyFaceGroup> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->fgHd=fgHd;
		GetFaceGroup(undo.Ptr()->orgPlHd,fgHd);
	}

	auto res=YsShellExt::AddFaceGroupPolygon(fgHd,nPl,fgPlHd);

	if(YSOK==res)
	{
		if(nullptr!=undo)
		{
			GetFaceGroup(undo.Ptr()->newPlHd,fgHd);
		}
	}
	else
	{
		undo.Discard();
	}
	return res;
}

YSRESULT YsShellExtEdit::AddFaceGroupPolygon(FaceGroupHandle fgHd,const YsConstArrayMask <YsShell::PolygonHandle> &fgPlHd)
{
	return AddFaceGroupPolygon(fgHd,fgPlHd.GetN(),fgPlHd);
}


YSRESULT YsShellExtEdit::ModifyFaceGroup(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[])
{
	NewUndoLog<UndoModifyFaceGroup> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->fgHd=fgHd;
		GetFaceGroup(undo.Ptr()->orgPlHd,fgHd);
		undo.Ptr()->newPlHd.Set(nPl,fgPlHd);
	}

	auto res=YsShellExt::ModifyFaceGroup(fgHd,nPl,fgPlHd);
	if(YSOK!=res)
	{
		undo.Discard();
	}
	return res;
}

YSRESULT YsShellExtEdit::ModifyFaceGroup(FaceGroupHandle fgHd,const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd)
{
	return ModifyFaceGroup(fgHd,fgPlHd.GetN(),fgPlHd);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoDeleteFaceGroupPolygon : public YsShellExtEdit::EditLog
{
public:
	class FaceGroupPolygonPair
	{
	public:
		YsShellExt::FaceGroupHandle fgHd;
		YsShellPolygonHandle plHd;
	};

	YsArray <FaceGroupPolygonPair> fgPlPairArray;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoDeleteFaceGroupPolygon::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(auto &fgPlPair : fgPlPairArray)
	{
		const YsShellPolygonHandle plHdArray[1]={fgPlPair.plHd};
		shlExt->AddFaceGroupPolygon(fgPlPair.fgHd,1,plHdArray);
	}
}

void YsShellExtEdit::UndoDeleteFaceGroupPolygon::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	YsArray <PolygonHandle> plHd;
	for(auto &fgPlPair : fgPlPairArray)
	{
		plHd.Add(fgPlPair.plHd);
	}
	shlExt->DeleteFaceGroupPolygonMulti(plHd);
}

YsShellExt::FaceGroupHandle YsShellExtEdit::DeleteFaceGroupPolygon(YsShellPolygonHandle plHd)
{
	YsShellExt::FaceGroupHandle fgHd=YsShellExt::DeleteFaceGroupPolygon(plHd);
	if(NULL!=fgHd)
	{
		NewUndoLog<UndoDeleteFaceGroupPolygon> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->fgPlPairArray.Set(1,NULL);
			undo.Ptr()->fgPlPairArray[0].fgHd=fgHd;
			undo.Ptr()->fgPlPairArray[0].plHd=plHd;
		}
		return fgHd;
	}
	return NULL;
}

YSRESULT YsShellExtEdit::DeleteFaceGroupPolygonMulti(YSSIZE_T nPl,const YsShellPolygonHandle plHd[])
{
	YSRESULT res=YSERR;

	NewUndoLog<UndoDeleteFaceGroupPolygon> undo(this);
	if(nullptr!=undo)
	{
		for(YSSIZE_T plIdx=0; plIdx<nPl; ++plIdx)
		{
			YsShellExt::FaceGroupHandle fgHd=YsShellExt::FindFaceGroupFromPolygon(plHd[plIdx]);
			if(nullptr!=fgHd)
			{
				undo.Ptr()->fgPlPairArray.Increment();
				undo.Ptr()->fgPlPairArray.Last().plHd=plHd[plIdx];
				undo.Ptr()->fgPlPairArray.Last().fgHd=fgHd;
			}
		}
	}

	res=YsShellExt::DeleteFaceGroupPolygonMulti(nPl,plHd);
	if(YSOK!=res)
	{
		undo.Discard();
	}
	return res;
}

YSRESULT YsShellExtEdit::DeleteFaceGroupPolygonMulti(const YsConstArrayMask <YsShell::PolygonHandle> &plHdArray)
{
	return DeleteFaceGroupPolygonMulti(plHdArray.GetN(),plHdArray);
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetFaceGroupAttrib : public YsShellExtEdit::EditLog
{
public:
	YsShellExt::FaceGroupHandle fgHd;
	YsShellExt::FaceGroupAttrib prvAttrib,newAttrib;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetFaceGroupAttrib::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetFaceGroupAttrib(fgHd,prvAttrib);
}

void YsShellExtEdit::UndoSetFaceGroupAttrib::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetFaceGroupAttrib(fgHd,newAttrib);
}

YSRESULT YsShellExtEdit::SetFaceGroupAttrib(YsShellExt::FaceGroupHandle fgHd,const YsShellExt::FaceGroupAttrib &attrib)
{
	YsShellExt *shlExt=(YsShellExt *)this;

	NewUndoLog<UndoSetFaceGroupAttrib> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->fgHd=fgHd;
		undo.Ptr()->prvAttrib=shlExt->GetFaceGroupAttrib(fgHd);
		undo.Ptr()->newAttrib=attrib;
	}

	auto res=shlExt->SetFaceGroupAttrib(fgHd,attrib);
	if(YSOK!=res)
	{
		undo.Discard();
	}
	return res;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetFaceGroupLabel : public YsShellExtEdit::EditLog
{
public:
	YsShellExt::FaceGroupHandle fgHd;
	YsString prvLabel,newLabel;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
};

void YsShellExtEdit::UndoSetFaceGroupLabel::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetFaceGroupLabel(fgHd,prvLabel);
}

void YsShellExtEdit::UndoSetFaceGroupLabel::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->SetFaceGroupLabel(fgHd,newLabel);
}

YSRESULT YsShellExtEdit::SetFaceGroupLabel(YsShellExt::FaceGroupHandle fgHd,const char label[])
{
	YsShellExt *shlExt=(YsShellExt *)this;

	NewUndoLog<UndoSetFaceGroupLabel> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->prvLabel=shlExt->GetFaceGroupLabel(fgHd);
		undo.Ptr()->newLabel=label;
	}

	auto res=shlExt->SetFaceGroupLabel(fgHd,label);
	if(YSOK!=res)
	{
		undo.Discard();
	}
	return res;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoDeleteFaceGroup : public YsShellExtEdit::EditLog
{
public:
	YsShellExt::FaceGroupHandle fgHd;

	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *);
};

void YsShellExtEdit::UndoDeleteFaceGroup::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->MeltFaceGroup(fgHd);
}

void YsShellExtEdit::UndoDeleteFaceGroup::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->FreezeFaceGroup(fgHd);
}

void YsShellExtEdit::UndoDeleteFaceGroup::NeverBeUndoneAgain(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	shlExt->DeleteFrozenFaceGroup(fgHd);
}

void YsShellExtEdit::UndoDeleteFaceGroup::NeverBeRedoneAgain(YsShellExtEdit *)
{
}

YSRESULT YsShellExtEdit::DeleteFaceGroup(FaceGroupHandle fgHd)
{
	YsShellExt *shlExt=(YsShellExt *)this;

	if(YSOK==shlExt->FreezeFaceGroup(fgHd))
	{
		NewUndoLog<UndoDeleteFaceGroup> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->fgHd=fgHd;
		}
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoAddMultiConstEdge : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellExtEdit::ConstEdgeHandle> ceHdArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};
void YsShellExtEdit::UndoAddMultiConstEdge::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(auto ceHd : ceHdArray)
	{
		shlExt->FreezeConstEdge(ceHd);
	}
}

void YsShellExtEdit::UndoAddMultiConstEdge::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(auto ceHd : ceHdArray)
	{
		shlExt->MeltConstEdge(ceHd);
	}
}

void YsShellExtEdit::UndoAddMultiConstEdge::NeverBeUndoneAgain(YsShellExtEdit *)
{
}

void YsShellExtEdit::UndoAddMultiConstEdge::NeverBeRedoneAgain(YsShellExtEdit *shl)
{
	// Means never be melted again.
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(auto ceHd : ceHdArray)
	{
		shlExt->DeleteFrozenConstEdge(ceHd);
	}
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoAddMultiFaceGroup : public YsShellExtEdit::EditLog
{
public:
	YsArray <YsShellExtEdit::FaceGroupHandle> fgHdArray;
	virtual void Undo(YsShellExtEdit *shl);
	virtual void Redo(YsShellExtEdit *shl);
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl);
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl);
};
void YsShellExtEdit::UndoAddMultiFaceGroup::Undo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(auto fgHd : fgHdArray)
	{
		shlExt->FreezeFaceGroup(fgHd);
	}
}

void YsShellExtEdit::UndoAddMultiFaceGroup::Redo(YsShellExtEdit *shl)
{
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(auto fgHd : fgHdArray)
	{
		shlExt->MeltFaceGroup(fgHd);
	}
}

void YsShellExtEdit::UndoAddMultiFaceGroup::NeverBeUndoneAgain(YsShellExtEdit *)
{
}

void YsShellExtEdit::UndoAddMultiFaceGroup::NeverBeRedoneAgain(YsShellExtEdit *shl)
{
	// Means never be melted again.
	YsShellExt *shlExt=(YsShellExt *)shl;
	for(auto fgHd : fgHdArray)
	{
		shlExt->DeleteFrozenFaceGroup(fgHd);
	}
}

class YsShellExtEdit::UndoAddMetaData : public YsShellExtEdit::EditLog
{
public:
	std::vector <YsShell::MetaDataHandle> mdHd;

	virtual void Undo(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		for(auto hd : mdHd)
		{
			shlExt->FreezeMetaData(hd);
		}
	}
	virtual void Redo(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		for(auto hd : mdHd)
		{
			shlExt->MeltMetaData(hd);
		}
	}
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl)
	{
	}
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		for(auto hd : mdHd)
		{
			shlExt->DeleteFrozenMetaData(hd);
		}
	}
};

YsShell::MetaDataHandle YsShellExtEdit::AddMetaData(const char key[],const char value[])
{
	auto mdHd=YsShell::AddMetaData(key,value);
	NewUndoLog<UndoAddMetaData> undo(this);
	if(nullptr!=undo)
	{
		undo.Ptr()->mdHd.push_back(mdHd);
	}
	return mdHd;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoSetMetaDataValue : public YsShellExtEdit::EditLog
{
public:
	YsShell::MetaDataHandle mdHd;
	YsString prvValue,newValue;

	virtual void Undo(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		shlExt->SetMetaDataValue(mdHd,prvValue);
	}
	virtual void Redo(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		shlExt->SetMetaDataValue(mdHd,newValue);
	}
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl)
	{
	}
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl)
	{
	}
};

YSRESULT YsShellExtEdit::SetMetaDataValue(MetaDataHandle mdHd,const char value[])
{
	if(nullptr!=mdHd)
	{
		auto md=GetMetaData(mdHd);
		if(YSOK==YsShell::SetMetaDataValue(mdHd,value))
		{
			NewUndoLog<UndoSetMetaDataValue> undo(this);
			if(nullptr!=undo)
			{
				undo.Ptr()->mdHd=mdHd;
				undo.Ptr()->newValue=value;
				undo.Ptr()->prvValue=md.value;
			}
			return YSOK;
		}
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

class YsShellExtEdit::UndoDeleteMetaData : public YsShellExtEdit::EditLog
{
public:
	std::vector <YsShell::MetaDataHandle> mdHd;

	virtual void Undo(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		for(auto hd : mdHd)
		{
			shlExt->MeltMetaData(hd);
		}
	}
	virtual void Redo(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		for(auto hd : mdHd)
		{
			shlExt->FreezeMetaData(hd);
		}
	}
	virtual void NeverBeUndoneAgain(YsShellExtEdit *shl)
	{
		YsShellExt *shlExt=(YsShellExt *)shl;
		for(auto hd : mdHd)
		{
			shlExt->DeleteFrozenMetaData(hd);
		}
	}
	virtual void NeverBeRedoneAgain(YsShellExtEdit *shl)
	{
	}
};

YSRESULT YsShellExtEdit::DeleteMetaData(MetaDataHandle mdHd)
{
	if(YSOK==YsShell::FreezeMetaData(mdHd))
	{
		NewUndoLog<UndoDeleteMetaData> undo(this);
		if(nullptr!=undo)
		{
			undo.Ptr()->mdHd.push_back(mdHd);
		}
		return YSOK;
	}
	return YSERR;
}




YSRESULT YsShellExtEdit::MergeSrf(YsTextInputStream &inStream)
{
	StopIncUndo incUndo(this);

	YsShellVertexStore existingVtx(Conv());
	YsShellPolygonStore existingPlg(Conv());
	ConstEdgeStore existingCe(Conv());
	FaceGroupStore existingFg(Conv());
	for(auto vtHd : AllVertex())
	{
		existingVtx.AddVertex(vtHd);
	}
	for(auto plHd : AllPolygon())
	{
		existingPlg.AddPolygon(plHd);
	}
	for(auto ceHd : AllConstEdge())
	{
		existingCe.AddConstEdge(ceHd);
	}
	for(auto fgHd : AllFaceGroup())
	{
		existingFg.AddFaceGroup(fgHd);
	}

	YsShellExtReader srfReader;
	auto res=srfReader.MergeSrf(*this,inStream);

	// Must enforce the order that undo logs are added.
	{
		NewUndoLog<UndoAddVertex> undoVtx(this);
		if(nullptr!=undoVtx)
		{
			for(auto vtHd : AllVertex())
			{
				if(YSTRUE!=existingVtx.IsIncluded(vtHd))
				{
					undoVtx.Ptr()->vtHdArray.Append(vtHd);
				}
			}
		}
	}
	{
		NewUndoLog<UndoAddPolygon> undoPlg(this);
		if(nullptr!=undoPlg)
		{
			for(auto plHd : AllPolygon())
			{
				if(YSTRUE!=existingPlg.IsIncluded(plHd))
				{
					undoPlg.Ptr()->plHdArray.Append(plHd);
				}
			}
		}
	}
	{
		NewUndoLog<UndoAddMultiConstEdge> undoConstEdge(this);
		if(nullptr!=undoConstEdge)
		{
			for(auto ceHd : AllConstEdge())
			{
				if(YSTRUE!=existingCe.IsIncluded(ceHd))
				{
					undoConstEdge.Ptr()->ceHdArray.Add(ceHd);
				}
			}
		}
	}
	{
		NewUndoLog<UndoAddMultiFaceGroup> undoFaceGroup(this);
		if(nullptr!=undoFaceGroup)
		{
			for(auto fgHd : AllFaceGroup())
			{
				if(YSTRUE!=existingFg.IsIncluded(fgHd))
				{
					undoFaceGroup.Ptr()->fgHdArray.Add(fgHd);
				}
			}
		}
	}
	return res;
}

