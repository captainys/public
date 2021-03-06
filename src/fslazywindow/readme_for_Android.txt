Update 2021/05/03
   *** **** it!  Why does Android Studio change every time I look at it!



Pre-requisite:
  Ninja build system seems to be "separately" installed from Android Studio.
  There is absolutely zero description about this pre-requisite.
  Good job, Google.

  Update 2021/05/03:
    In the new Android Studio
    Ninja build doesn't seem to be separately installed, or may not be used at all.
    NDK needs to be installed from:
    1 Tools->SDK Management
    2 In the dialog: Appearance & Behavior -> System Settings -> Android SDK
    3 In the right pane: Click on SDK Tools tab.
    4 Check "NDK (Side by side)" and "CMake".
    5 "Apply" and "OK"



(1) Creating a new project in Android Studio
		Check  "Include C++ Support"
		Select Android 6.0 (5.0 might work, not confirmed)
		Empty Activity
		C++ Standard "C++11"
		Check	"Exception Support"
		Check 	"Runtime Type Information (RTTI)"
	Update 2021/05/03
		Why the **** does it change every time I look at it!?
		File -> New Project -> Scroll Down to select "Native C++"
		"Next"
		C++ Standard "C++11" or whatever needed.
		"Finish"
	Wait very long time until the project loads.
	For the first time, it takes minutes to load a project.



(2) Copy files in template_addition/android/cpp/* to (Project)/app/src/main/cpp/*

(3) Copy files in template_addition/android/java/* to (Project)/app/src/main/java/*

(4) Open CMakeLists.txt in "External Build Files", and insert following lines in the end.  Replace "somewhere" with an appropriate location.  Make sure to use slash instead of backslash if copy & paste in Windows.  Replace "AndroidProgram" with your application.  The application project (in this example, AndroidProgram) needs to be after fslazywindow library in target_link_libraries.  Otherwise functions in the application project will not be linked.

set(TARGET_NAME AndroidProgram)
add_compile_options(-O2)
set(YS_ANDROID_ASSET_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets")
add_subdirectory(somewhere/public/src ${CMAKE_BINARY_DIR}/public_src)
#add_subdirectory(somewhere/public/tests ${CMAKE_BINARY_DIR}/public_tests)
add_library(ysgles2lib SHARED src/main/cpp/ysgles2view.cpp)
target_link_libraries(ysgles2lib fssimplewindow fslazywindow ${TARGET_NAME} ysclass ysgl ysglcpp ysglcpp_gl2 log android)

Update 2021/05/03
Aparently the new Android Studio makes CMakeLists.cpp under cpp subdirectory.  I don't know if it always is the case.  (Why the **** does it change every time I look at it!?)  If that's the case, remove "src/main/cpp" in the above CMake-let.


(5) In Gradle Scripts, open "build.gradle (Module: app)", and find defaultConfig block.  In defaultConfig -> externalNativeBuild -> cmake, insert the following line.

                targets "native-lib","ysgles2lib"

Update 2021/05/03.  It changes every time I look at it.  It is a sign that Android Studio is a bad program.  There are two "build.gradle"s, and the one you need to open is "build.gradle (Module: *****.app)"

"native-lib" is the name for the default native library.  I hope Google doesn't change it.  If it does, replace as it does.  Without this line, Android Studio will stupidly try to build all the projects in CMake source tree, some of which may not be compiled for Android.


You may also need the following line underneath in the same block.

                abiFilters "x86", "x86_64", "armeabi-v7a", "arm64-v8a"

Gradle seems to be a quite deplorable build system, and it eventually cannot identify what build system it supports.  It ends up trying to set up build for unsupported CPUs such as mips or armabi.  When it happens, you may need to manually specify what CPUs (or ABIs) you want to use by adding this line.

But, once you successfully pass CMake, Android Studio may ask you if you want to update Gradle Plugin to 3.1.3 or newer.  DO IT!  Then you may be able to delete abiFilters line, and move forward.



(6) The FsLazyWindow application needs to be a library rather than an executable.  If you are using add_executable like:

add_executable(AndroidProgram MACOSX_BUNDLE main.cpp)

Replace with:

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
	add_library(AndroidProgram main.cpp)
else()
	add_executable(AndroidProgram MACOSX_BUNDLE main.cpp)
endif()

Or, if you are using my template, add:

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
	set(IS_LIBRARY_PROJECT 1)
endif()

in an appropriate location.



(7) Open MainActivity.java, and add 
import lib.ysflight.YsAndroidConnector;
import lib.ysflight.YsGLES2View;
import lib.ysflight.YsAsset;


	Then add a member variable and constructor:

    YsGLES2View ysView;
    YsAsset ysAsm;
    YsAndroidConnector ysAndroid;


	Then delete lines in onCreate and replace with:

        super.onCreate(savedInstanceState);
        ysAsm=new YsAsset(this.getAssets(),this.getBaseContext());
        ysView=new YsGLES2View(getApplication());
        ysAndroid=new YsAndroidConnector(this.getBaseContext());
        setContentView(ysView);



( ) File -> Settings -> Appearance & Behavior -> System Settings
    Uncheck "Synchronize Files on Frame or Editor Tab Activation"
    
    As suggested in:
    https://stackoverflow.com/questions/34402973/android-studio-disable-gradle-sync

    Android Studio is a horribly slow IDE.
    When it starts synchronization it easily stops 10 minutes.
    Maybe written in Java?
    Why don't they write in C++ if they are incapable of making it faster?



( ) Using external storage: https://developer.android.com/training/data-storage/files#WriteExternalStorage




Memo to myself:
OpenGL ES GLSL requires an infamous precision specifier.

	varying vec4 varyingColor;

needs to be written as:

	varying highp vec4 varyingColor;

It only breaks compatibility between desktop systems and portable devices.  In desktop systems,
the compile function can insert:

#define highp
#define mediump
#define lowp

to let GLSL compiler ignore precision prefixes.





ERRORS!  Android Studio is deplorable.

If "undefined reference to std::logic_error::logic_error", try:
   Build -> Refresh Linked C++ Projects

   (https://groups.google.com/forum/#!msg/android-ndk/3iKT-kLEGpY/HkGpp-2gEAAJ)



Unexpected EOF in LogCat

   ON THE DEVICE (Not on the Android Studio)
     Settings -> Developer Options -> Find "Logger Buffer Sizes" and turn to whatever maximum.
   Crap!  Who can live with only 256KB log buffer!?  They do everything to make it difficult to create a useful program.

   (https://stackoverflow.com/questions/48689930/logcat-crashes-with-read-unexpected-eof)



Showing CMake Errors.
  Android Studio succeeds in staying deplorable as of version 3.3.1.
  It is incredibly difficult to find the way to show CMake errors.

  To see CMake errors:
  Make sure to open (may already be open):
    View -> Tools -> Build

  Then, click on this nonsense icon (two overlapping box with ">") underneath the hammer icon on the left.

  People should grow out of hieroglyphy.
  This two boxes with ">" did not make any sense to me.

  Icons!  Icons we once thought there might be pictures that explains to the people who speaks
  different languages.  That was a utterly wrong assumption.  Words explain much better than
  senseless icons.  Icons failed.  Why those developers cannot learn?

  This uninterpretable hieroglyph does give absolutely zero impression that it shows CMake errors.
  A simple text menu "Show Build Log" or "Show Build Output" makes 2^64 times more sense.

  If they want to live in the time of Egypitian gods, they should get out of my world.
  I appreciate them travel back in time and never come back.




Console Output
Include the following headers, and

#include <android/log.h>
#include <android/fssimplewindowconnector.h>
#include <android/fslazywindowconnector.h>
#include <android/fslazywindow_android.h>
#include <android/fssimplewindow_android.h>

replace printf with something like:

        __android_log_print(ANDROID_LOG_INFO,"Ys","%s %d",__FUNCTION__,__LINE__);
