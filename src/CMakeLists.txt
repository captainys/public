cmake_minimum_required(VERSION 3.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 2016/09/18 Some functions does not seem to work or difficult to use with non-default CMAKE_BINARY_DIR.  Commented out.
##Cmake is supposed to set these two variables.  However, it doesn't.
##Looks to be something to do with variable cache.  Variable cache is one of the design failures of cmake.
#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/exe)

set(YS_COMMAND_BINARY_DIR ${CMAKE_BINARY_DIR}/exe)
set(YS_COMMAND_BINARY_DIR ${YS_COMMAND_BINARY_DIR} PARENT_SCOPE)



# OpenGL ES 2.0 for Windows required for compiling for Universal Windows Platform.
# It's great!  Finally Microsoft officially supports OpenGL ES 2.0!
# I'd be happier if full-scale OpenGL 2.0 is supported, but ES 2.0 is good!
if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
	add_definitions(-DYS_IS_UNIVERSAL_WINDOWS_APP)

	find_program(NUGET_EXE nuget)
	if(NOT NUGET_EXE)
		message("NUGET.EXE not found.")
		message("Unfortunatelly as of 09/13/2016, you cannot install NUGET.EXE as a globally-available command")
		message("from Visual Studio 2015, or from Add/Remove Programs.  I spent about 30 minutes to search for")
		message("the way, but didn't find one.")
		message("You can download NUGET.EXE from:")
		message("    https://dist.nuget.org/index.html")
		message("and copy it to a directory where PATH is pointing.")
		message(FATAL_ERROR "Please install this executable, and run CMake again.")
	endif()
	message("NUGET.EXE found at ${NUGET_EXE}")

	# What I want to run here is:
	#   nuget install "ANGLE.WindowsStore" -OutputDirectory (destination dir) -ExcludeVersion
	# And then I want to know exactly the directory name.  Can be "ANGLE.WindowsStore"
	exec_program(${NUGET_EXE}
	             ARGS install "ANGLE.WindowsStore" -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/nuget_packages)
	set(UWP_GLES2_TARGETS "${CMAKE_BINARY_DIR}/nuget_packages/ANGLE.WindowsStore/build/native/ANGLE.WindowsStore.targets")
	set(UWP_GLES2_TARGETS "${UWP_GLES2_TARGETS}" PARENT_SCOPE)


	set(YS_OPENGL_VERSION "OpenGLES2")
	set(YS_OPENGL_VERSION "OpenGLES2" PARENT_SCOPE)



	exec_program(${NUGET_EXE}
	             ARGS install "Win2D.uwp" -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/nuget_packages)
	set(UWP_WIN2D_TARGETS "${CMAKE_BINARY_DIR}/nuget_packages/Win2D.uwp/build/native/Win2D.uwp.targets")
	set(UWP_WIN2D_TARGETS "${UWP_WIN2D_TARGETS}" PARENT_SCOPE)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
	set(YS_OPENGL_VERSION "OpenGLES2")
	set(YS_OPENGL_VERSION "OpenGLES2" PARENT_SCOPE)
endif()



# If ysgl source files may not be placed under ${CMAKE_CURRENT_SOURCE_DIR}/src/ysgl/src, include_directories must be added here.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imported/include)


# Capture global-YS_LIBRARY_LIST into local YS_LIBRARY_LIST
set(YS_LIBRARY_LIST ${YS_LIBRARY_LIST})

add_subdirectory("ysclass/src")
add_subdirectory("ysclass11/src")
add_subdirectory("ysport/src")

add_subdirectory("ysgl/src")
add_subdirectory("ysglcpp/src")
add_subdirectory("ysglcpp/src/gl2")
if(NOT ANDROID)
	add_subdirectory("ysglcpp/src/gl1")
	add_subdirectory("ysglcpp/src/nownd")
endif()

add_subdirectory("ysbitmap/src")
if(NOT ANDROID)
	add_subdirectory("ysbitmap/cmdtools")
endif()
add_subdirectory("ysbitmaputil/src")
add_subdirectory("ysbitmapfont/src")
if(NOT ANDROID)
	add_subdirectory("ysbitmapfont/cmdtools")
endif()
add_subdirectory("yssystemfont/src")

add_subdirectory("fssimplewindow/src")
if(NOT ANDROID)
	add_subdirectory("fssimplewindow/template")
	add_subdirectory("fssimplewindow/samples")
endif()

add_subdirectory("yssimplesound/src")
add_subdirectory("yssimplesound/src/nownd")
if(NOT ANDROID)
	add_subdirectory("yssimplesound/test")
	add_subdirectory("yssimplesound/sample")
	add_subdirectory("yssimplesound/sample_fssimplewindow")
endif()

add_subdirectory("fslazywindow/src")
add_subdirectory("fslazywindow/template")
add_subdirectory("fslazywindow/samples/sample00-bouncingBall")
add_subdirectory("fssimplefiledialog/src")
add_subdirectory("fssimplefiledialog/sample")

add_subdirectory("ysfontrenderer/src")

add_subdirectory("ystexturemanager/src")
add_subdirectory("ystexturemanager/src/gl")
if(NOT ANDROID)
	add_subdirectory("ystexturemanager/src/null")
endif()

add_subdirectory("fsguilib/src")
add_subdirectory("fsguilib/src/gl2")
if(NOT ANDROID)
	add_subdirectory("fsguilib/src/gl1")
	add_subdirectory("fsguilib/src/nownd")
endif()
add_subdirectory("fsguilib/filedialog")
add_subdirectory("fsguilib/template")
add_subdirectory("fsguilib/template_with_filedialog")
add_subdirectory("fsgui3d/src")
if(NOT ANDROID)
	add_subdirectory("fsgui3d/src/gl1")
endif()
add_subdirectory("fsgui3d/src/gl2")
add_subdirectory("fsgui3d/template")
add_subdirectory("fsgui3d/template_with_filedialog")
if(NOT ANDROID)
	add_subdirectory("fsgui3d/template-easy")
endif()

add_subdirectory("yssocket/src")

add_subdirectory("ysgebl/src/kernel")
add_subdirectory("ysgebl/src/kernelutil")
add_subdirectory("ysgebl/src/shellrender")
add_subdirectory("ysgebl/src/shellrender/gl2")
if(NOT ANDROID)
	add_subdirectory("ysgebl/src/shellrender/gl1")
	add_subdirectory("ysgebl/src/shellrender/nownd")
endif()
add_subdirectory("ysgebl/src/gui_foundation")
add_subdirectory("ysgebl/src/gui")
add_subdirectory("ysgebl/src/main")
if(NOT ANDROID)
	add_subdirectory("ysgebl/src/cmdlib")
	add_subdirectory("ysgebl/src/cmdmain")
	add_subdirectory("ysgebl/src/samples")
endif()

#Must be commented out, or someone who works on the tutorial cannot create
#a project in the same name.
#add_subdirectory("ysgebl/tutorial/kernel/tutorial01")
#add_subdirectory("ysgebl/tutorial/kernel/tutorial02")
#add_subdirectory("ysgebl/tutorial/kernel/tutorial03")
#add_subdirectory("ysgebl/tutorial/kernel/tutorial04_makedatafile")
#add_subdirectory("ysgebl/tutorial/kernel/tutorial04")
#add_subdirectory("ysgebl/tutorial/kernel/tutorial05")
#add_subdirectory("ysgebl/tutorial/kernel/tutorial06")

add_subdirectory("2DRetroMapTool/src")

if(NOT ANDROID)
	add_subdirectory("FM7")
endif()

add_subdirectory(opencv)

# Export include paths
foreach(ONE_LIBRARY ${YS_LIBRARY_LIST})
set(${ONE_LIBRARY}_INCLUDE_DIR ${${ONE_LIBRARY}_INCLUDE_DIR} PARENT_SCOPE)
endforeach(ONE_LIBRARY)

# Copy local-YS_LIBRARY_LIST back to global-YS_LIBRARY_LIST
set(YS_LIBRARY_LIST ${YS_LIBRARY_LIST} PARENT_SCOPE)
