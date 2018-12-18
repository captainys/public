//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <angle_windowsstore.h>

namespace XamlOpenGL
{
	using namespace Windows::UI::Core;

	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	private:
		EGLDisplay mEglDisplay;
		EGLContext mEglContext;
		EGLSurface mEglSurface;
		Windows::UI::Xaml::DispatcherTimer ^timer;

		bool isWindowVisible,isWindowClosed,needRedraw;
		bool lastKnownLbutton,lastKnownMButton,lastKnownRButton;
		int lastKnownX,lastKnownY;

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

	private:
		void InitializeOpenGLES2(void);
		EGLBoolean InitializeEGLDisplayDefault(void);
		EGLBoolean InitializeEGLDisplay(int level);
		std::vector <EGLint> MakeDisplayAttribute(int level) const;
		std::map <Windows::System::VirtualKey, int> uwpKeyToFsKey;
		void SetUpConfigAttrib(EGLint configAttrib[], int depthBit);
		void CleanUpOpenGLES2(void);

		void ReportWindowSize(void);

		double t;
		void OnPageLoaded(Platform::Object ^, Windows::UI::Xaml::RoutedEventArgs^);
		void OnTick(Platform::Object^,Platform::Object^);
		void OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args);
		void OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args);

		void OnKeyDown(CoreWindow^ window,KeyEventArgs^ args);
		void OnKeyUp(CoreWindow^ window,KeyEventArgs^ args);
		void CharIn(CoreWindow^ window,CharacterReceivedEventArgs^ args);

		void OnMousePointerMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
		void OnMouseWheelMoved(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
		void OnMouseButtonPressed(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
		void OnMouseButtonReleased(Windows::UI::Core::CoreWindow^ window,Windows::UI::Core::PointerEventArgs^ args);
		void ProcessMouseEvent(Windows::UI::Core::PointerEventArgs^ args);

	public:
		MainPage();
	};
}
