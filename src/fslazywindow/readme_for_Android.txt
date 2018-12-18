(1) Creating a new project in Android Studio
		Check  "Include C++ Support"
		Select Android 6.0 (5.0 might work, not confirmed)
		Empty Activity
		C++ Standard "C++11"
		Check	"Exception Support"
		Check 	"Runtime Type Information (RTTI)"

(2) Copy files in template_addition/android/cpp/* to (Project)/app/src/main/cpp/*

(3) Copy files in template_addition/android/java/* to (Project)/app/src/main/java/*

(4) Open CMakeLists.txt in "External Build Files", and insert following lines in the end.  Replace "somewhere" with an appropriate location.  Replace "sample00_bouncingBall" with your application.  The application project (in this example, sample00_bouncingBall) needs to be after fslazywindow library in target_link_libraries.  Otherwise functions in the application project will not be linked.

add_compile_options(-O2)
set(YS_ANDROID_ASSET_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets")
add_subdirectory(somewhere/public/src ${CMAKE_BINARY_DIR}/public_src)
#add_subdirectory(somewhere/public/tests ${CMAKE_BINARY_DIR}/public_tests)
add_library(ysgles2lib SHARED src/main/cpp/ysgles2view.cpp)
target_link_libraries(ysgles2lib fssimplewindow fslazywindow sample00_bouncingBall ysclass ysgl ysglcpp ysglcpp_gl2 log android)

(5) In Gradle Scripts, open "build.gradle (Module: app)", and find defaultConfig block.  In defaultConfig -> externalNativeBuild -> cmake, insert the following line.

                targets "native-lib","ysgles2lib"

"native-lib" is the name for the default native library.  I hope Google doesn't change it.  If it does, replace as it does.  Without this line, Android Studio will stupidly try to build all the projects in CMake source tree, some of which may not be compiled for Android.


You may also need the following line underneath in the same block.

                abiFilters "x86", "x86_64", "armeabi-v7a", "arm64-v8a"

Gradle seems to be a quite deplorable build system, and it eventually cannot identify what build system it supports.  It ends up trying to set up build for unsupported CPUs such as mips or armabi.  When it happens, you may need to manually specify what CPUs (or ABIs) you want to use by adding this line.

But, once you successfully pass CMake, Android Studio may ask you if you want to update Gradle Plugin to 3.1.3 or newer.  DO IT!  Then you may be able to delete abiFilters line, and move forward.



(6) The FsLazyWindow application needs to be a library rather than an executable.  If you are using add_executable like:

add_executable(sample00_bouncingBall MACOSX_BUNDLE main.cpp)

Replace with:

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
	add_library(sample00_bouncingBall main.cpp)
else()
	add_executable(sample00_bouncingBall MACOSX_BUNDLE main.cpp)
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





ERRORS!  Android Studio is deplorable.

If "undefined reference to std::logic_error::logic_error", try:
   Build -> Refresh Linked C++ Projects

   (https://groups.google.com/forum/#!msg/android-ndk/3iKT-kLEGpY/HkGpp-2gEAAJ)



Unexpected EOF in LogCat

   ON THE DEVICE (Not on the Android Studio)
     Settings -> Developer Options -> Find "Logger Buffer Sizes" and turn to whatever maximum.
   Crap!  Who can live with only 256KB log buffer!?  They do everything to make it difficult to create a useful program.

   (https://stackoverflow.com/questions/48689930/logcat-crashes-with-read-unexpected-eof)

