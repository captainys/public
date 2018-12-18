/* ////////////////////////////////////////////////////////////

File Name: fslazywindow_uwp.cpp
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

#include <stdio.h>
#include <time.h>
#include <vector>
#include <map>
#include <direct.h>

#include <agile.h>

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <angle_windowsstore.h>

#include <fssimplewindow.h>
#include <fslazywindow.h>



using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System;

using namespace Platform;

extern void FsUWPReportWindowSize(int wid,int hei);
extern void FsUWPMakeReportWindowSize(void);
extern void FsUWPSwapBuffers(void);
extern void FsUWPReportMouseEvent(int evt,int lb,int mb,int rb,int mx,int my);
extern void FsUWPReportCharIn(int unicode);


ref class SingleWindowAppView sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
	typedef SingleWindowAppView THISCLASS;

private:  // Cannot have a public member variable?
	bool isWindowClosed,isWindowVisible,first;
	Platform::Agile<CoreWindow> hWnd;

	bool lastKnownLbutton,lastKnownMButton,lastKnownRButton;
	int lastKnownX,lastKnownY;

	static THISCLASS ^CurrentView;

    EGLDisplay mEglDisplay;
    EGLContext mEglContext;
    EGLSurface mEglSurface;

	class KeyMap
	{
	public:
		Windows::System::VirtualKey uwpKey;
		int fsKey;
		KeyMap(){};
		KeyMap(Windows::System::VirtualKey uwpKey,int fsKey)
		{
			this->uwpKey=uwpKey;
			this->fsKey=fsKey;
		}
	};
	std::vector <KeyMap> keyMapTable;

public:
	static THISCLASS ^GetCurrentView(void);

	SingleWindowAppView();

	void ReportWindowSize(void);

	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
	virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
	virtual void Load(Platform::String^ entryPoint);
	virtual void Run();
	virtual void Uninitialize();

	virtual void OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args);
	virtual void OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args);
	virtual void OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args);

	virtual void OnKeyDown(CoreWindow^ window,KeyEventArgs^ args);
	virtual void OnKeyUp(CoreWindow^ window,KeyEventArgs^ args);
	virtual void CharIn(CoreWindow^ window,CharacterReceivedEventArgs^ args);

	virtual void OnMousePointerMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
	virtual void OnMouseWheelMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
	virtual void OnMouseButtonPressed(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
	virtual void OnMouseButtonReleased(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
	void ProcessMouseEvent(Windows::UI::Core::PointerEventArgs^ args);

private:
	void InitializeOpenGLES2(void);
	EGLBoolean InitializeEGLDisplayDefault(void);
	EGLBoolean InitializeEGLDisplay(int level);
	std::vector <EGLint> MakeDisplayAttribute(int level) const;
	std::map <Windows::System::VirtualKey,int> uwpKeyToFsKey;
	void SetUpConfigAttrib(EGLint configAttrib[],int depthBit);
	void CleanUpOpenGLES2(void);
};

/* static */ SingleWindowAppView ^SingleWindowAppView::CurrentView;

/* static */ SingleWindowAppView ^SingleWindowAppView::GetCurrentView(void)
{
	return CurrentView;
}

SingleWindowAppView::SingleWindowAppView()
{
	isWindowClosed=false;
	isWindowVisible=false;
	first=true;

	lastKnownLbutton=false;
	lastKnownMButton=false;
	lastKnownRButton=false;
	lastKnownX=0;
	lastKnownY=0;

    mEglDisplay=EGL_NO_DISPLAY;
    mEglContext=EGL_NO_CONTEXT;
    mEglSurface=EGL_NO_SURFACE;

	CurrentView=this;

	auto app=FsLazyWindowApplicationBase::GetApplication();

	char *argv[1];
	argv[0]=(char *)"executable.exe";
	app->BeforeEverything(1,argv);

	// See https://msdn.microsoft.com/en-us/library/system.windows.forms.keys(v=vs.110).aspx
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::A,FSKEY_A));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::B,FSKEY_B));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::C,FSKEY_C));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::D,FSKEY_D));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::E,FSKEY_E));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F,FSKEY_F));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::G,FSKEY_G));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::H,FSKEY_H));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::I,FSKEY_I));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::J,FSKEY_J));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::K,FSKEY_K));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::L,FSKEY_L));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::M,FSKEY_M));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::N,FSKEY_N));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::O,FSKEY_O));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::P,FSKEY_P));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Q,FSKEY_Q));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::R,FSKEY_R));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::S,FSKEY_S));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::T,FSKEY_T));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::U,FSKEY_U));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::V,FSKEY_V));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::W,FSKEY_W));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::X,FSKEY_X));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Y,FSKEY_Y));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Z,FSKEY_Z));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Add,FSKEY_PLUS));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Application,FSKEY_ALT));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Back,FSKEY_BS));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Control,FSKEY_CTRL)); // What's the difference from Windows::System::VirtualKey::Control?
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Decimal,FSKEY_DOT));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Delete,FSKEY_DEL));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Divide,FSKEY_SLASH));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Down,FSKEY_DOWN));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::End,FSKEY_END));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Enter,FSKEY_ENTER));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Escape,FSKEY_ESC));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F1,FSKEY_F1));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F2,FSKEY_F2));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F3,FSKEY_F3));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F4,FSKEY_F4));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F5,FSKEY_F5));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F6,FSKEY_F6));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F7,FSKEY_F7));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F8,FSKEY_F8));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F9,FSKEY_F9));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F10,FSKEY_F10));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F11,FSKEY_F11));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::F12,FSKEY_F12));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Home,FSKEY_HOME));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Insert,FSKEY_INS));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Left,FSKEY_LEFT));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberKeyLock,FSKEY_NUMLOCK));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad0,FSKEY_TEN0));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad1,FSKEY_TEN1));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad2,FSKEY_TEN2));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad3,FSKEY_TEN3));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad4,FSKEY_TEN4));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad5,FSKEY_TEN5));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad6,FSKEY_TEN6));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad7,FSKEY_TEN7));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad8,FSKEY_TEN8));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::NumberPad9,FSKEY_TEN9));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::PageDown,FSKEY_PAGEDOWN));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::PageUp,FSKEY_PAGEUP));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Pause,FSKEY_PAUSEBREAK));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Right,FSKEY_RIGHT));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Scroll,FSKEY_SCROLLLOCK));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Space,FSKEY_SPACE));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Subtract,FSKEY_MINUS));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Tab,FSKEY_TAB));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Up,FSKEY_UP));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::LeftShift,FSKEY_SHIFT));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::RightShift,FSKEY_SHIFT));
	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::Shift,FSKEY_SHIFT));
//	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::,FSKEY_));
//	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::,FSKEY_));
//	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::,FSKEY_));
//	keyMapTable.push_back(KeyMap(Windows::System::VirtualKey::,FSKEY_));

	for(auto km : keyMapTable)
	{
		uwpKeyToFsKey[km.uwpKey]=km.fsKey;
	}
}

void SingleWindowAppView::ReportWindowSize(void)
{
	EGLint w=0,h=0;
	eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &w);
	eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &h);
	FsUWPReportWindowSize(w,h);
}

/* virtual */ void SingleWindowAppView::Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView)
{
    applicationView->Activated+=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this,&THISCLASS::OnActivated);
}
/* virtual */ void SingleWindowAppView::SetWindow(Windows::UI::Core::CoreWindow^ window)
{
	hWnd=window;

	window->KeyDown+=
		ref new TypedEventHandler<CoreWindow^,KeyEventArgs^>(this,&THISCLASS::OnKeyDown);
	window->KeyUp+=
		ref new TypedEventHandler<CoreWindow^,KeyEventArgs^>(this,&THISCLASS::OnKeyUp);
	window->CharacterReceived+=
		ref new TypedEventHandler<CoreWindow^,CharacterReceivedEventArgs^>(this,&THISCLASS::CharIn);

	window->PointerMoved+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&THISCLASS::OnMousePointerMoved);
	window->PointerWheelChanged+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&THISCLASS::OnMouseWheelMoved);
	window->PointerPressed+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&THISCLASS::OnMouseButtonPressed);
	window->PointerReleased+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&THISCLASS::OnMouseButtonReleased);

    window->VisibilityChanged+=
        ref new TypedEventHandler<CoreWindow^,VisibilityChangedEventArgs^>(this,&THISCLASS::OnVisibilityChanged);
    window->Closed+=
        ref new TypedEventHandler<CoreWindow^,CoreWindowEventArgs^>(this,&THISCLASS::OnWindowClosed);

	InitializeOpenGLES2(); // Example initializes OpenGL ES context in here.

	// 2017/02/16
	// To work with abusive use of asynchronous operation in UWP API, I need to run an 
	// event loop when I need to block until an IAsyncOperation is over.
	// But, calling
	//    CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
	// from here looks to break something and the program crashes on exit.
	// In today's case, calling YsFileInfo of YsPort library from Initialize cause the program to crasn on exit.
	// Instead, Initialize is called in Run, just before the first call to Interval.
}
/* virtual */ void SingleWindowAppView::Load(Platform::String^ entryPoint)
{
	// Example creates renderers here.
}
/* virtual */ void SingleWindowAppView::Run()
{
    while(!isWindowClosed)
    {
		// See https://blogs.msdn.microsoft.com/oldnewthing/20041130-00/?p=37173 for difference between GetKeyState and GetAsyncKeyState.
		// Go with GetAsyncKeyState.  GetKeyState may be useless.
		if(Windows::UI::Core::CoreVirtualKeyStates::Down==hWnd->GetKeyState(Windows::System::VirtualKey::Escape))
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
		}
		if(Windows::UI::Core::CoreVirtualKeyStates::Down==hWnd->GetAsyncKeyState(Windows::System::VirtualKey::Escape))
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			char dir[256];
			_getcwd(dir,255);
			printf("%s\n",dir);
		}

        if (isWindowVisible)
        {
			// 2017/02/16 See explanation above.
			//   To work with ridiculous abusive use of asynchronous operation in Universal Windows API,
			//   I need to call Initialize from here.
			if(true==first)
			{
				auto app=FsLazyWindowApplicationBase::GetApplication();
				FsOpenWindowOption opt;
				app->GetOpenWindowOption(opt);

				char *argv[1];
				argv[0]=(char *)"executable.exe";
				app->Initialize(1,argv);
				first=false;
			}

			// This doesn't seem to wait for an event.  Like a PeekMessage
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			auto app=FsLazyWindowApplicationBase::GetApplication();
			app->Interval();
			if(true==app->NeedRedraw())
			{
				app->Draw();

				if (eglSwapBuffers(mEglDisplay, mEglSurface) != GL_TRUE)
				{
					// Device lost?  So, you mean I need to re-create all the resources?
					// No way.  Why do you make it as bad as Direct 3D9.
					CleanUpOpenGLES2();
					InitializeOpenGLES2();
					app->ContextLostAndRecreated();
	            }
			}
			if(true==app->MustTerminate())
			{
				// How can I close the window?
			}

			// sleep for app->GetMinimumSleepPerInterval();
        }
        else
        {
			// This seems to wait until an event is thrown.
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }

	auto app=FsLazyWindowApplicationBase::GetApplication();
	app->BeforeTerminate();

	CleanUpOpenGLES2(); // Example deletes OpenGL context here.
}
/* virtual */ void SingleWindowAppView::Uninitialize()
{
}


/* virtual */ void SingleWindowAppView::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    CoreWindow::GetForCurrentThread()->Activate();
}
/* virtual */ void SingleWindowAppView::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	isWindowVisible=args->Visible;
}
/* virtual */ void SingleWindowAppView::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	isWindowClosed=true;
}

/* virtual */ void SingleWindowAppView::OnKeyDown(CoreWindow^ window,KeyEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	auto Lshift=window->GetAsyncKeyState(Windows::System::VirtualKey::LeftShift);
	auto Rshift=window->GetAsyncKeyState(Windows::System::VirtualKey::RightShift);
	if(Lshift==Windows::UI::Core::CoreVirtualKeyStates::Down)
	{
		printf("Lshift\n");
	}
	if(Rshift==Windows::UI::Core::CoreVirtualKeyStates::Down)
	{
		printf("Rshift\n");
	}
	

	auto found=uwpKeyToFsKey.find(args->VirtualKey);
	if(uwpKeyToFsKey.end()!=found)
	{
		FsPushKey(found->second);
	}
}
/* virtual */ void SingleWindowAppView::OnKeyUp(CoreWindow^ window,KeyEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
}
/* virtual */ void SingleWindowAppView::CharIn(CoreWindow^ window,CharacterReceivedEventArgs^ args)
{
	FsUWPReportCharIn(args->KeyCode);
}

/* virtual */ void SingleWindowAppView::OnMousePointerMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
{
	auto point=args->CurrentPoint;
	if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Mouse)
	{
		ProcessMouseEvent(args);
	}
	else if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Pen)
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
	}
	else if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Touch)
	{
printf("%s %d (ID %d)\n",__FUNCTION__,__LINE__,point->PointerId);
	}
}
/* virtual */ void SingleWindowAppView::OnMouseWheelMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
}
/* virtual */ void SingleWindowAppView::OnMouseButtonPressed(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
{
	auto point=args->CurrentPoint;
	if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Mouse)
	{
		ProcessMouseEvent(args);
	}
	else if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Pen)
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
	}
	else if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Touch)
	{
printf("%s %d (ID %d)\n",__FUNCTION__,__LINE__,point->PointerId);
	}
}
/* virtual */ void SingleWindowAppView::OnMouseButtonReleased(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
{
	auto point=args->CurrentPoint;
	if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Mouse)
	{
		ProcessMouseEvent(args);
	}
	else if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Pen)
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
	}
	else if(point->PointerDevice->PointerDeviceType==Windows::Devices::Input::PointerDeviceType::Touch)
	{
printf("%s %d (ID %d)\n",__FUNCTION__,__LINE__,point->PointerId);
	}
}

void SingleWindowAppView::ProcessMouseEvent(Windows::UI::Core::PointerEventArgs^ args)
{
	auto point=args->CurrentPoint;
	auto prop=point->Properties;
	int Lbutton=(int)prop->IsLeftButtonPressed;
	int Mbutton=(int)prop->IsMiddleButtonPressed;
	int Rbutton=(int)prop->IsRightButtonPressed;

	auto dispInfo=Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
	auto dpiX=dispInfo->LogicalDpi; // Should I go with LogicalDpi or RawDpi?
	auto dpiY=dispInfo->LogicalDpi; // Should I go with LogicalDpi or RawDpi?

	int x=(int)(point->Position.X*dpiX/96.0);
	int y=(int)(point->Position.Y*dpiY/96.0);

	if(!lastKnownLbutton && Lbutton)
	{
		FsUWPReportMouseEvent(FSMOUSEEVENT_LBUTTONDOWN,Lbutton,Mbutton,Rbutton,x,y);
	}
	if(lastKnownX!=x || lastKnownY!=y)
	{
		FsUWPReportMouseEvent(FSMOUSEEVENT_MOVE,Lbutton,Mbutton,Rbutton,x,y);
	}
	if(lastKnownLbutton && !Lbutton)
	{
		FsUWPReportMouseEvent(FSMOUSEEVENT_LBUTTONUP,Lbutton,Mbutton,Rbutton,x,y);
	}

	lastKnownLbutton=Lbutton;
	lastKnownMButton=Mbutton;
	lastKnownRButton=Rbutton;
	lastKnownX=x;
	lastKnownY=y;
}

void SingleWindowAppView::InitializeOpenGLES2(void)
{
	// Cut & Pasted from OpenGL ES2 example.
	// Modified so that it selects the maximum depth bits.
	EGLint configAttributes[]=
	{
		EGL_RED_SIZE,8,
		EGL_GREEN_SIZE,8,
		EGL_BLUE_SIZE,8,
		EGL_ALPHA_SIZE,8,
		EGL_DEPTH_SIZE,24,
		EGL_STENCIL_SIZE,8,
		EGL_NONE
	};

	const EGLint contextAttributes[]=
	{
		EGL_CONTEXT_CLIENT_VERSION,2,
		EGL_NONE
	};

	const EGLint surfaceAttributes[]=
	{
		EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER,EGL_TRUE,
		EGL_NONE
	};

	EGLConfig config = NULL;

	// According to the example, the program should try to initialize OpenGL ES context with
	// EGL to D3D11 Feature Level 10_0+ first.  If it fails, try EGL to D3D11 Feature Level 9_3.
	// If it fails, try EGL to D3D11 Feature Level 11_0 on WARP.
	if(InitializeEGLDisplayDefault()!=EGL_TRUE &&
	   InitializeEGLDisplay(0)!=EGL_TRUE &&
	   InitializeEGLDisplay(1)!=EGL_TRUE &&
	   InitializeEGLDisplay(2)!=EGL_TRUE)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to initialize EGL");
	}

	// The sample code given with the OpenGL ES template uses 8-bit Z-buffer, 
	// which is pretty much useless.
	// Ideallly want to have 64-bit (or as precise as coordinates, in many cases 32-bit), 
	// but here get whatever available.
	EGLint numConfigs = 0;
	for(int depthBit=32; 8<=depthBit; depthBit-=8)
	{
		SetUpConfigAttrib(configAttributes,depthBit);
		if(eglChooseConfig(mEglDisplay,configAttributes,&config,1,&numConfigs)==EGL_TRUE && 0!=numConfigs)
		{
			printf("%d Depth Bits\n",depthBit);
			break;
		}
	}
	if(0==numConfigs)
	{
		throw Exception::CreateException(E_FAIL,L"Failed to choose first EGLConfig");
	}

	PropertySet^ surfaceCreationProperties = ref new PropertySet();
	surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), hWnd.Get());

	mEglSurface=eglCreateWindowSurface(mEglDisplay,config,reinterpret_cast<IInspectable*>(surfaceCreationProperties),surfaceAttributes);
	if(mEglSurface==EGL_NO_SURFACE)
	{
		throw Exception::CreateException(E_FAIL,L"Failed to create EGL fullscreen surface");
	}

	mEglContext=eglCreateContext(mEglDisplay,config,EGL_NO_CONTEXT,contextAttributes);
	if(mEglContext==EGL_NO_CONTEXT)
	{
		throw Exception::CreateException(E_FAIL,L"Failed to create EGL context");
	}

	if(eglMakeCurrent(mEglDisplay,mEglSurface,mEglSurface,mEglContext)==EGL_FALSE)
	{
		throw Exception::CreateException(E_FAIL,L"Failed to make fullscreen EGLSurface current");
	}

	glClearColor(1,1,1,0);
}

EGLBoolean SingleWindowAppView::InitializeEGLDisplayDefault(void)
{
	// The sample code tells that eglGetPlatformDisplayEXT is use in place for eglGetDisplay to pass display attributes.
	// But, why not trying eglGetDisplay first?
	mEglDisplay=eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(EGL_NO_DISPLAY!=mEglDisplay)
	{
		auto b=eglInitialize(mEglDisplay,NULL,NULL);
		printf("Looks like the default eglGetDisplay is good enough.  Isn't it?");
		return b;
	}
	return EGL_FALSE;
}

EGLBoolean SingleWindowAppView::InitializeEGLDisplay(int level)
{
	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT=
	    reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
	if (!eglGetPlatformDisplayEXT)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
	}

	auto attrib=MakeDisplayAttribute(level);

	mEglDisplay=eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE,EGL_DEFAULT_DISPLAY,attrib.data());
	if(mEglDisplay!=EGL_NO_DISPLAY)
	{
		return eglInitialize(mEglDisplay,NULL,NULL);
	}
	return EGL_FALSE;
}

std::vector <EGLint> SingleWindowAppView::MakeDisplayAttribute(int level) const
{
	const EGLint commonDisplayAttributePre[]=
	{
		EGL_PLATFORM_ANGLE_TYPE_ANGLE,EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
	};
	const EGLint commonDisplayAttributePost[]=
	{
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER,EGL_TRUE,
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,EGL_TRUE,
		EGL_NONE,
	};

	std::vector <EGLint> attrib;
	for(auto v : commonDisplayAttributePre)
	{
		attrib.push_back(v);
	}
	switch(level)
	{
	case 0:
		break;
	case 1:  // FL9.  I don't know exactly what FL9 stands for, but as example says.
		attrib.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE);
		attrib.push_back(9);
		attrib.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE);
		attrib.push_back(3);
		break;
	case 2:  // warp.  I don't know exactly what it means by "warp", but as example says.
		attrib.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE);
		attrib.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE);
		break;
	}
	for(auto v : commonDisplayAttributePost)
	{
		attrib.push_back(v);
	}
	return attrib;
}

void SingleWindowAppView::SetUpConfigAttrib(EGLint configAttrib[],int depthBit)
{
	for(int i=0; configAttrib[i]!=EGL_NONE; i+=2)
	{
		if(EGL_DEPTH_SIZE==configAttrib[i])
		{
			configAttrib[i+1]=depthBit;
		}
	}
}

void SingleWindowAppView::CleanUpOpenGLES2(void)
{
	// Cut & Pasted from OpenGL ES2 Example.
	if(mEglDisplay!=EGL_NO_DISPLAY && mEglSurface!=EGL_NO_SURFACE)
	{
		eglDestroySurface(mEglDisplay,mEglSurface);
		mEglSurface=EGL_NO_SURFACE;
	}
	if(mEglDisplay!=EGL_NO_DISPLAY && mEglContext!=EGL_NO_CONTEXT)
	{
		eglDestroyContext(mEglDisplay,mEglContext);
		mEglContext=EGL_NO_CONTEXT;
	}
	if(mEglDisplay!=EGL_NO_DISPLAY)
	{
		eglTerminate(mEglDisplay);
		mEglDisplay=EGL_NO_DISPLAY;
	}
}

void FsUWPMakeReportWindowSize(void)
{
	auto view=SingleWindowAppView::GetCurrentView();
	view->ReportWindowSize();
}

void FsUWPSwapBuffers(void)
{
	// auto view=SingleWindowAppView::GetCurrentView();
	// view->SwapBuffers();
}




ref class SingleWindowAppSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
    {
        return ref new SingleWindowAppView();
    }
};


[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto singleWindowAppSource = ref new SingleWindowAppSource();
    CoreApplication::Run(singleWindowAppSource);
    return 0;
}

