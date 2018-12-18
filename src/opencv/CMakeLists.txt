# Use parameter -DYS_USE_OPENCV=1 to enable OpenCV-related projects.
if(NOT YS_USE_OPENCV EQUAL 1)
	message("YS_USE_OPENCV is OFF.  Skipping OpenCV")
	return()
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BITNESS 64)
else()
	set(BITNESS 32)
endif()



if(NOT YS_POSSIBLE_OPENCV_LOCATION)
	if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
		set(DIRSUFFIX uwp)
	else()
		set(DIRSUFFIX ${BITNESS})
	endif()

	set(YS_POSSIBLE_OPENCV_LOCATION
		D:/OpenCV/build${DIRSUFFIX}
		C:/OpenCV/build${DIRSUFFIX}
		D:/OpenCV/build
		C:/OpenCV/build
		$ENV{HOME}/OpenCV/build
		/usr/lib/opencv
	)
endif()
find_package(OpenCV PATHS ${YS_POSSIBLE_OPENCV_LOCATION})

if(OpenCV_FOUND)
	# I leave the following CMake code for UWP hoping that the current useless implementation of OpenCV is
	# replaced with more useful implementation in the future.
	# At this moment, to use OpenCV Video Capture from UWP app, the program needs to use a special running pattern,
	# and there seems to be no way of re-using the same code written for other platforms.
	# 
	# Make a list of import files so that UWP app can make them VS_DEPLOYMENT_CONTENTS
	# OpenCV_INSTALL_PATH=D:/OpenCV/builduwp
	if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
		set(OpenCV_VERSIONSTR "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}")
		set(RELEASE_DLL )
		set(DEBUG_DLL )
		foreach(LIB ${OpenCV_LIBS})
			list(APPEND RELEASE_DLL 
				${OpenCV_INSTALL_PATH}/bin/Release/${LIB}${OpenCV_VERSIONSTR}.dll
				${OpenCV_INSTALL_PATH}/bin/Release/${LIB}${OpenCV_VERSIONSTR}.manifest
				${OpenCV_INSTALL_PATH}/bin/Release/${LIB}${OpenCV_VERSIONSTR}.pdb
				${OpenCV_INSTALL_PATH}/bin/Release/${LIB}${OpenCV_VERSIONSTR}.pri)
			list(APPEND DEBUG_DLL 
				${OpenCV_INSTALL_PATH}/bin/Debug/${LIB}${OpenCV_VERSIONSTR}.dll
				${OpenCV_INSTALL_PATH}/bin/Debug/${LIB}${OpenCV_VERSIONSTR}.manifest
				${OpenCV_INSTALL_PATH}/bin/Debug/${LIB}${OpenCV_VERSIONSTR}.pdb
				${OpenCV_INSTALL_PATH}/bin/Debug/${LIB}${OpenCV_VERSIONSTR}.pri)
		endforeach(LIB)

		foreach(DLLFILE ${RELEASE_DLL})
			if(EXISTS ${DLLFILE})
				list(APPEND VS_OpenCV_RELEASE_DLL ${DLLFILE})
			endif()
		endforeach(DLLFILE)

		foreach(DLLFILE ${DEBUG_DLL})
			if(EXISTS ${DLLFILE})
				list(APPEND VS_OpenCV_DEBUG_DLL ${DLLFILE})
			endif()
		endforeach(DLLFILE)
	endif()

	include_directories(${OpenCV_INCLUDE_DIRS})
	add_subdirectory(videoCapture)
	add_subdirectory(drawMarker)
	add_subdirectory(arucoMarkerDetection)
	message("OpenCV found at ${OpenCV_INSTALL_PATH}")
	message("Make sure to add paths to the DLL directories.")

else()
	message("OpenCV not found.  Skipping OpenCV projects.")
endif()

