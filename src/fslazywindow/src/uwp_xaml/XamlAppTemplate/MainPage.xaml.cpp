//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <fssimplewindow.h>
#include <fslazywindow.h>
#include "fslazywindow_xaml_bridge.h"

#include <math.h>

using namespace XamlOpenGL;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

////////////////////////////////////////////////////////////

void FsUWPMakeReportWindowSize(void)
{
	// FsSimpleWindow library calls this function to make the application report WindowSize to 
	// FsSimpleWindow library.  But, in this framework, window size is reported repeatedly in the
	// timer event before calling Interval.  This function doesn't have to do anything.
}

////////////////////////////////////////////////////////////

void MainPage::InitializeOpenGLES2(void)
{
	// Cut & Pasted from OpenGL ES2 example.
	// Modified so that it selects the maximum depth bits.
	EGLint configAttributes[] =
	{
		EGL_RED_SIZE,8,
		EGL_GREEN_SIZE,8,
		EGL_BLUE_SIZE,8,
		EGL_ALPHA_SIZE,8,
		EGL_DEPTH_SIZE,24,
		EGL_STENCIL_SIZE,8,
		EGL_NONE
	};

	const EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION,2,
		EGL_NONE
	};

	const EGLint surfaceAttributes[] =
	{
		EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER,EGL_TRUE,
		EGL_NONE
	};

	EGLConfig config = NULL;

	// According to the example, the program should try to initialize OpenGL ES context with
	// EGL to D3D11 Feature Level 10_0+ first.  If it fails, try EGL to D3D11 Feature Level 9_3.
	// If it fails, try EGL to D3D11 Feature Level 11_0 on WARP.
	if (InitializeEGLDisplayDefault() != EGL_TRUE &&
		InitializeEGLDisplay(0) != EGL_TRUE &&
		InitializeEGLDisplay(1) != EGL_TRUE &&
		InitializeEGLDisplay(2) != EGL_TRUE)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to initialize EGL");
	}

	// The sample code given with the OpenGL ES template uses 8-bit Z-buffer, 
	// which is pretty much useless.
	// Ideallly want to have 64-bit (or as precise as coordinates, in many cases 32-bit), 
	// but here get whatever available.
	EGLint numConfigs = 0;
	for (int depthBit = 32; 8 <= depthBit; depthBit -= 8)
	{
		SetUpConfigAttrib(configAttributes, depthBit);
		if (eglChooseConfig(mEglDisplay, configAttributes, &config, 1, &numConfigs) == EGL_TRUE && 0 != numConfigs)
		{
			printf("%d Depth Bits\n", depthBit);
			break;
		}
	}
	if (0 == numConfigs)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to choose first EGLConfig");
	}

	PropertySet^ surfaceCreationProperties = ref new PropertySet();
	surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), swapChainPanel); //  

	mEglSurface = eglCreateWindowSurface(mEglDisplay, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);
	if (mEglSurface == EGL_NO_SURFACE)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to create EGL fullscreen surface");
	}

	mEglContext = eglCreateContext(mEglDisplay, config, EGL_NO_CONTEXT, contextAttributes);
	if (mEglContext == EGL_NO_CONTEXT)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to create EGL context");
	}

	if (eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext) == EGL_FALSE)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to make fullscreen EGLSurface current");
	}

	glClearColor(1, 1, 1, 0);
	printf("OpenGLES2 context has been created.\n");
}

EGLBoolean MainPage::InitializeEGLDisplayDefault(void)
{
	// The sample code tells that eglGetPlatformDisplayEXT is use in place for eglGetDisplay to pass display attributes.
	// But, why not trying eglGetDisplay first?
	mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (EGL_NO_DISPLAY != mEglDisplay)
	{
		auto b = eglInitialize(mEglDisplay, NULL, NULL);
		printf("Looks like the default eglGetDisplay is good enough.  Isn't it?");
		return b;
	}
	return EGL_FALSE;
}

EGLBoolean MainPage::InitializeEGLDisplay(int level)
{
	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
		reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
	if (!eglGetPlatformDisplayEXT)
	{
		throw Exception::CreateException(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
	}

	auto attrib = MakeDisplayAttribute(level);

	mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, attrib.data());
	if (mEglDisplay != EGL_NO_DISPLAY)
	{
		return eglInitialize(mEglDisplay, NULL, NULL);
	}
	return EGL_FALSE;
}

std::vector <EGLint> MainPage::MakeDisplayAttribute(int level) const
{
	const EGLint commonDisplayAttributePre[] =
	{
		EGL_PLATFORM_ANGLE_TYPE_ANGLE,EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
	};
	const EGLint commonDisplayAttributePost[] =
	{
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER,EGL_TRUE,
		EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,EGL_TRUE,
		EGL_NONE,
	};

	std::vector <EGLint> attrib;
	for (auto v : commonDisplayAttributePre)
	{
		attrib.push_back(v);
	}
	switch (level)
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
	for (auto v : commonDisplayAttributePost)
	{
		attrib.push_back(v);
	}
	return attrib;
}

void MainPage::SetUpConfigAttrib(EGLint configAttrib[], int depthBit)
{
	for (int i = 0; configAttrib[i] != EGL_NONE; i += 2)
	{
		if (EGL_DEPTH_SIZE == configAttrib[i])
		{
			configAttrib[i + 1] = depthBit;
		}
	}
}

void MainPage::CleanUpOpenGLES2(void)
{
	// Cut & Pasted from OpenGL ES2 Example.
	if (mEglDisplay != EGL_NO_DISPLAY && mEglSurface != EGL_NO_SURFACE)
	{
		eglDestroySurface(mEglDisplay, mEglSurface);
		mEglSurface = EGL_NO_SURFACE;
	}
	if (mEglDisplay != EGL_NO_DISPLAY && mEglContext != EGL_NO_CONTEXT)
	{
		eglDestroyContext(mEglDisplay, mEglContext);
		mEglContext = EGL_NO_CONTEXT;
	}
	if (mEglDisplay != EGL_NO_DISPLAY)
	{
		eglTerminate(mEglDisplay);
		mEglDisplay = EGL_NO_DISPLAY;
	}
}

void MainPage::ReportWindowSize(void)
{
	EGLint w=0,h=0;
	eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &w);
	eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &h);
	FsUWPReportWindowSize(w,h);
	// printf("%d %d\n",w,h);
}

////////////////////////////////////////////////////////////

MainPage::MainPage()
{
	InitializeComponent();

	isWindowClosed=false;
	isWindowVisible=false;
	needRedraw=false;
	lastKnownLbutton=false;
	lastKnownMButton=false;
	lastKnownRButton=false;
	lastKnownX=0;
	lastKnownY=0;

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

	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &MainPage::OnPageLoaded);

	char *argv[1];
	argv[0]=(char *)"executable.exe";
	FsLazyWindowApplicationBase::GetApplication()->BeforeEverything(1,argv);

	FsOpenWindowOption owo;
	owo.useDoubleBuffer=true;
	FsLazyWindowApplicationBase::GetApplication()->GetOpenWindowOption(owo);
}

void MainPage::OnPageLoaded(Platform::Object ^, Windows::UI::Xaml::RoutedEventArgs^)
{
	//	https://github.com/Microsoft/angle/tree/ms-master/templates

	InitializeOpenGLES2();

	auto window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	// I am not sure which of the window or swapChainPanel should process these input events.

	window->KeyDown+=
		ref new TypedEventHandler<CoreWindow^,KeyEventArgs^>(this,&MainPage::OnKeyDown);
	window->KeyUp+=
		ref new TypedEventHandler<CoreWindow^,KeyEventArgs^>(this,&MainPage::OnKeyUp);
	window->CharacterReceived+=
		ref new TypedEventHandler<CoreWindow^,CharacterReceivedEventArgs^>(this,&MainPage::CharIn);

	window->PointerMoved+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&MainPage::OnMousePointerMoved);
	window->PointerWheelChanged+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&MainPage::OnMouseWheelMoved);
	window->PointerPressed+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&MainPage::OnMouseButtonPressed);
	window->PointerReleased+=
		ref new TypedEventHandler<CoreWindow^,PointerEventArgs^>(this,&MainPage::OnMouseButtonReleased);

    window->VisibilityChanged+=
        ref new TypedEventHandler<CoreWindow^,VisibilityChangedEventArgs^>(this,&MainPage::OnVisibilityChanged);
    window->Closed+=
        ref new TypedEventHandler<CoreWindow^,CoreWindowEventArgs^>(this,&MainPage::OnWindowClosed);

	t=0.0;
	timer=ref new Windows::UI::Xaml::DispatcherTimer();
	TimeSpan s;
	s.Duration=10000; // 10000nanosec=10millisec
	timer->Interval=s;
	timer->Start();
	timer->Tick+=ref new EventHandler<Object^>(this,&MainPage::OnTick);

	char *argv[1];
	argv[0]=(char *)"executable.exe";
	FsLazyWindowApplicationBase::GetApplication()->Initialize(1,argv);
}

void MainPage::OnTick(Platform::Object^,Platform::Object^)
{
	t+=0.1;
	double r=0.5+0.5*sin(t/2.0);
	double g=0.5+0.5*sin(t/3.0);
	double b=0.5+0.5*sin(t/7.0);

	ReportWindowSize();

    if (isWindowVisible)
    {
		auto app=FsLazyWindowApplicationBase::GetApplication();
		app->Interval();
		if(true==app->NeedRedraw() || true==needRedraw)
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
            else
            {
				needRedraw=false;
			}
		}
		if(true==app->MustTerminate())
		{
			// How can I close the window?
		}
	}
}

void MainPage::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	isWindowClosed=true;
}

void MainPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(true!=isWindowVisible && true==args->Visible)
	{
		needRedraw=true;
	}
	isWindowVisible=args->Visible;
}

void MainPage::OnKeyDown(CoreWindow^ window,KeyEventArgs^ args)
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
void MainPage::OnKeyUp(CoreWindow^ window,KeyEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
}
void MainPage::CharIn(CoreWindow^ window,CharacterReceivedEventArgs^ args)
{
	FsUWPReportCharIn(args->KeyCode);
}

void MainPage::OnMousePointerMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
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
void MainPage::OnMouseWheelMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
}
void MainPage::OnMouseButtonPressed(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
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
void MainPage::OnMouseButtonReleased(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args)
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

void MainPage::ProcessMouseEvent(Windows::UI::Core::PointerEventArgs^ args)
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
