set(TARGET_NAME comportlib)
set(WIN_SUBSYSTEM WINDOWS)



if(MSVC)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:${WIN_SUBSYSTEM},5.02 /MACHINE:x64")
	else()
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:${WIN_SUBSYSTEM},5.01 /MACHINE:X86")
	endif()
	set(COMPORT_SOURCE comport.cpp comport_win.cpp comport.h)
else()
	set(COMPORT_SOURCE comport.cpp comport_null.cpp comport.h)
endif()



add_library(${TARGET_NAME} ${COMPORT_SOURCE})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})