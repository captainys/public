/* ////////////////////////////////////////////////////////////

File Name: fsguicallbackconnector.h
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

#ifndef FSGUICALLBACKCONNECTOR_IS_INCLUDED
#define FSGUICALLBACKCONNECTOR_IS_INCLUDED
/* { */

#include "fsguipopupmenu.h"
#include <ysclass.h>



template <class CLIENT_CLASS>
class FsGuiCallBackFunction
{
private:
	CLIENT_CLASS *clientPtr;
	void (CLIENT_CLASS::*funcPtr)(void);
public:
	FsGuiCallBackFunction()
	{
		clientPtr=NULL;
		funcPtr=NULL;
	}
	void Register(CLIENT_CLASS *clientPtr,void (CLIENT_CLASS::*funcPtr)(void))
	{
		this->clientPtr=clientPtr;
		this->funcPtr=funcPtr;
	}
	void Invoke(void)
	{
		if(NULL!=funcPtr)
		{
			(clientPtr->*funcPtr)();
		}
	}
};



template <class CLIENT_CLASS,class PARAMETER_CLASS>
class FsGuiCallBackFunctionWithParameter
{
private:
	CLIENT_CLASS *clientPtr;
	void (CLIENT_CLASS::*funcPtr)(PARAMETER_CLASS param);
public:
	FsGuiCallBackFunctionWithParameter()
	{
		clientPtr=NULL;
		funcPtr=NULL;
	}
	void Register(CLIENT_CLASS *clientPtr,void (CLIENT_CLASS::*funcPtr)(PARAMETER_CLASS param))
	{
		this->clientPtr=clientPtr;
		this->funcPtr=funcPtr;
	}
	void Invoke(PARAMETER_CLASS param)
	{
		if(NULL!=funcPtr)
		{
			(clientPtr->*funcPtr)(param);
		}
	}
};



template <class CLIENT_CLASS,class PARAMETER_CLASS>
class FsGuiDialogCallBackFunction
{
private:
	CLIENT_CLASS *clientPtr;
	void (CLIENT_CLASS::*funcPtr)(class FsGuiDialog *dlgPtr,PARAMETER_CLASS param);
public:
	FsGuiDialogCallBackFunction()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		clientPtr=NULL;
		funcPtr=NULL;
	}
	void Register(CLIENT_CLASS *clientPtr,void (CLIENT_CLASS::*funcPtr)(class FsGuiDialog *,PARAMETER_CLASS))
	{
		this->clientPtr=clientPtr;
		this->funcPtr=funcPtr;
	}
	void Invoke(class FsGuiDialog *dlgPtr,PARAMETER_CLASS param)
	{
		if(NULL!=clientPtr && NULL!=funcPtr)
		{
			(clientPtr->*funcPtr)(dlgPtr,param);
		}
	}
};



template <class CLIENT_CLASS>
class FsGuiMenuCallBackConnector
{
private:
	class MenuCallBackFunction : public FsGuiCallBackFunctionWithParameter <CLIENT_CLASS,class FsGuiPopUpMenuItem *>
	{
	};
	YsHashTable <MenuCallBackFunction> menuIdToCallBack;
	YsHashTable <MenuCallBackFunction> fsKeyToCallBack[8];

private:
	int ShiftCtrlAltToTableIndex(YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
	{
		int idx=0;
		if(YSTRUE==shift)
		{
			idx+=1;
		}
		if(YSTRUE==ctrl)
		{
			idx+=2;
		}
		if(YSTRUE==alt)
		{
			idx+=4;
		}
		return idx;
	}
	YSRESULT Register(YsHashTable <MenuCallBackFunction> &hash,YSHASHKEY hashKey,CLIENT_CLASS *clientPtr,void (CLIENT_CLASS::*funcPtr)(class FsGuiPopUpMenuItem *))
	{
		MenuCallBackFunction func;
		func.Register(clientPtr,funcPtr);
		return hash.UpdateElement(hashKey,func);
	}
public:
	void CleanUp(void)
	{
		menuIdToCallBack.CleanUp();
		for(auto &table : fsKeyToCallBack)
		{
			table.CleanUp();
		}
	}
	YSRESULT Register(int menuId,CLIENT_CLASS *clientPtr,void (CLIENT_CLASS::*funcPtr)(class FsGuiPopUpMenuItem *))
	{
		return Register(menuIdToCallBack,(YSHASHKEY)menuId,clientPtr,funcPtr);
	}
	YSRESULT RegisterShortCutKey(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt,CLIENT_CLASS *clientPtr,void (CLIENT_CLASS::*funcPtr)(class FsGuiPopUpMenuItem *))
	{
		auto &table=fsKeyToCallBack[ShiftCtrlAltToTableIndex(shift,ctrl,alt)];
		return Register(table,(YSHASHKEY)fsKey,clientPtr,funcPtr);
	}

public:
	YSRESULT Invoke(class FsGuiPopUpMenuItem *menuPtr)
	{
		MenuCallBackFunction func;
		if(YSOK==menuIdToCallBack.FindElement(func,(YSHASHKEY)(menuPtr->GetId())))
		{
			func.Invoke(menuPtr);
			return YSOK;
		}
		return YSERR;
	}
	YSRESULT InvokeFromShortCut(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
	{
		auto &table=fsKeyToCallBack[ShiftCtrlAltToTableIndex(shift,ctrl,alt)];
		MenuCallBackFunction func;
		if(YSOK==table.FindElement(func,(YSHASHKEY)fsKey))
		{
			func.Invoke(NULL);
			return YSOK;
		}
		return YSERR;
	}
};



template <class CLIENT_CLASS>
class FsGuiMouseButtonCallBackFunction
{
private:
	CLIENT_CLASS *clientPtr;
	YSRESULT (CLIENT_CLASS::*funcPtr)(YSBOOL,YSBOOL,YSBOOL,int,int);
public:
	FsGuiMouseButtonCallBackFunction()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		clientPtr=NULL;
		funcPtr=NULL;
	}
	void Register(CLIENT_CLASS *clientPtr,YSRESULT (CLIENT_CLASS::*funcPtr)(YSBOOL,YSBOOL,YSBOOL,int,int))
	{
		this->clientPtr=clientPtr;
		this->funcPtr=funcPtr;
	}
	YSRESULT Invoke(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
	{
		if(NULL!=clientPtr && NULL!=funcPtr)
		{
			return (clientPtr->*funcPtr)(lb,mb,rb,mx,my);
		}
		return YSERR;
	}
};



template <class CLIENT_CLASS>
class FsGuiBoxCallBackFunction
{
private:
	CLIENT_CLASS *clientPtr;
	YSRESULT (CLIENT_CLASS::*funcPtr)(int,int,int,int);
public:
	FsGuiBoxCallBackFunction()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		clientPtr=NULL;
		funcPtr=NULL;
	}
	YSBOOL IsSet(void) const
	{
		if(NULL!=clientPtr && NULL!=funcPtr)
		{
			return YSTRUE;
		}
		return YSFALSE;
	}
	void Register(CLIENT_CLASS *clientPtr,YSRESULT (CLIENT_CLASS::*funcPtr)(int,int,int,int))
	{
		this->clientPtr=clientPtr;
		this->funcPtr=funcPtr;
	}
	YSRESULT Invoke(int x1,int y1,int x2,int y2)
	{
		if(NULL!=clientPtr && NULL!=funcPtr)
		{
			return (clientPtr->*funcPtr)(x1,y1,x2,y2);
		}
		return YSERR;
	}
};



template <class CLIENT_CLASS>
class FsGuiKeyCallBackFunction
{
private:
	CLIENT_CLASS *clientPtr;
	YSRESULT (CLIENT_CLASS::*funcPtr)(YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
public:
	FsGuiKeyCallBackFunction()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		clientPtr=NULL;
		funcPtr=NULL;
	}
	void Register(CLIENT_CLASS *clientPtr,YSRESULT (CLIENT_CLASS::*funcPtr)(YSBOOL,YSBOOL,YSBOOL))
	{
		this->clientPtr=clientPtr;
		this->funcPtr=funcPtr;
	}
	YSRESULT Invoke(YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
	{
		if(NULL!=clientPtr && NULL!=funcPtr)
		{
			return (clientPtr->*funcPtr)(shift,ctrl,alt);
		}
		return YSERR;
	}
};



template <class CLIENT_CLASS>
class FsGuiMouseMoveCallBackFunction
{
private:
	CLIENT_CLASS *clientPtr;
	YSRESULT (CLIENT_CLASS::*funcPtr)(YSBOOL,YSBOOL,YSBOOL,int,int,int,int);
public:
	FsGuiMouseMoveCallBackFunction()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		clientPtr=NULL;
		funcPtr=NULL;
	}
	YSBOOL IsSet(void) const
	{
		if(NULL!=clientPtr && NULL!=funcPtr)
		{
			return YSTRUE;
		}
		return YSFALSE;
	}
	void Register(CLIENT_CLASS *clientPtr,YSRESULT (CLIENT_CLASS::*funcPtr)(YSBOOL,YSBOOL,YSBOOL,int,int,int,int))
	{
		this->clientPtr=clientPtr;
		this->funcPtr=funcPtr;
	}
	YSRESULT Invoke(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my,int prevMx,int prevMy)
	{
		if(NULL!=clientPtr && NULL!=funcPtr)
		{
			return (clientPtr->*funcPtr)(lb,mb,rb,mx,my,prevMx,prevMy);
		}
		return YSERR;
	}
};



/* } */
#endif
