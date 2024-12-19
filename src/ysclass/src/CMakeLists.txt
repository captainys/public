set(TARGET_NAME ysclass)
set(IS_LIBRARY_PROJECT 1)
set(LIB_DEPENDENCY "")
set(INCLUDE_DEPENDENCY)
set(OWN_HEADER_PATH .)
set(SINGLE_TARGET 1)
set(SUB_FOLDER "lib")
set(REALLY_NEED_CPP11 0)    # Setting 1 will enable use of C++11 libraries, but lose compatibility with OSX 10.6.



#YSBEGIN "CMake Header" Ver 20170110
# YS CMakeLists Template
# Copyright (c) 2015 Soji Yamakawa.  All rights reserved.
# http://www.ysflight.com
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, 
#    this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice, 
#    this list of conditions and the following disclaimer in the documentation 
#    and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.0.0)



if(MSVC)
	if(NOT WIN_SUBSYSTEM)
		set(WIN_SUBSYSTEM CONSOLE)
	endif()

	if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
		if(EXCLUDE_IN_UNIVERSAL_WINDOWS EQUAL 1)
			return()
		endif()

		add_definitions(-DYS_IS_UNIVERSAL_WINDOWS_APP)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZW")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /ZW")
	endif()

	# I want to keep compatibility with older operating systems, but it's getting difficult.
	# I have to comment out the following lines.
	# if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:${WIN_SUBSYSTEM},5.02 /MACHINE:x64")
	# else()
	# 	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:${WIN_SUBSYSTEM},5.01 /MACHINE:X86")
	# endif()
endif()

if(NOT DEFINED TARGET_NAME)
	message(FATAL_ERROR "TARGET_NAME not defined.")
endif()
if(NOT DEFINED IS_LIBRARY_PROJECT)
	message(FATAL_ERROR "IS_LIBRARY_PROJECT not defined.")
endif()
if(NOT DEFINED SINGLE_TARGET)
	message(FATAL_ERROR "SINGLE_TARGET not defined.")
endif()

# 2016/09/22 Learned a better way than specifying -std=c++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	# 2016/07/22
	#  /MT flags should be set outside the public repository.  It is moved to the higher-level CMakeLists.txt
elseif(APPLE)
	# 2015/07/15
	#   Sorry.  I pulled the plug.  All of my programs, including YS FLIGHT SIMULATOR, won't support 
	#   OSX 10.6 after today.  Apple deliberately disabled C++11 features in the libraries that I need to make my 
	#	programs compatible with OSX 10.6.
	#
	#	I know OSX 10.9 is evil for older models.  My 2008 MacBook Pro flies with OSX 10.6, but becoes
	#	a sloth with OSX 10.9.  Apple used to be a challenger pursuing Microsoft, but it is now an empire
	#	that Microsoft once was, and is doing everything that Microsoft did.  Apple inprison programmers
	#	with Apple-only programming language called Swift (already doing with Objective-C though) and Apple-only
	#	graphics toolkit called Metal, just as Microsoft did with C# and Direct3D.  Apple is making operating
	#	system heavier, slower, and inefficient, just as Microsoft has been doing.  The same thing is going all 
	#	around again.
	#
	#	OK, I warn you.  If you are investing your precious time for learning Swift and/or Metal, you are 
	#	taking a very big gamble.  Apple will throw it away when they get bored of it.  Learning one programming 
	#	language is not just understanding syntax.  You need to write considerable amount of code to learn the 
	#	best practices.  So far, C and C++ have been with for more than 20 years.  Will Swift live that long?
	#	Nobody knows.  I doubt it.  Swift is developed by a closed group.  Maybe one genius is in charge now.
	#	But, when the genius leaves, it could cramble down.  C and C++ are developed by the top computer
	#	scientists of the world.  To me, which is superior is obvious.
	#
	#	No user wants a new operating system.  Everyone wants their system to be cleaner, more stable, more 
	#	secure, and more resource-efficient.  Neither Apple nor Microsoft gets it.  We continue to be forced
	#	to throw away perfectly healthy hardware, and buy new over-spec hardware, which is inefficiently
	#	operated by the wasteful operating systems.
	#
	#	Sad and outrageous.  But, that's what Apple do.  Apple takes C++11 hostage and forces programmers 
	#	to drop support for older but still active-duty operating systems.
	#
	#	Mac is a good computer though.  I am happy with my 2011 MacMini.  I probably would be happy with
	#	my 2008 MacBook Pro if I still can (practically) use it with OSX 10.6, or if 10.9 is as efficient 
	#	as 10.6.

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.9 -Wno-switch")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.9 -Wno-switch")
elseif(UNIX)
	# -Wl,--no-as-needed required for g++ 4.8.4 Confirmed unnecessary with 5.4.0
	#  http://stackoverflow.com/questions/19463602/compiling-multithread-code-with-g
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--no-as-needed")
else()
endif()

if(IS_LIBRARY_PROJECT)
	#set(YS_LIBRARY_LIST ${YS_LIBRARY_LIST} ${TARGET_NAME} PARENT_SCOPE)
	# Modified as suggested in CMake performance tips.
	list(APPEND YS_LIBRARY_LIST ${TARGET_NAME})
	set(YS_LIBRARY_LIST ${YS_LIBRARY_LIST} PARENT_SCOPE)
endif()

#YSEND



set(SRCS
ysadvgeometry.cpp
ysadvgeometryx.cpp
ysargs.cpp
ysbase.cpp
ysbase64.cpp
ysclassmgr.cpp
ysclip.cpp
ysequation.cpp
ysexception.cpp
ysfilename.cpp
ysgeometry.cpp
yshash.cpp
yskdtree.cpp
yskeyword.cpp
yslist.cpp
ysmath.cpp
ysmatrix.cpp
ysmatrix3x3.cpp
ysmatrix4x4.cpp
yspixelmap.cpp
yspositivevector.cpp
yspredefined.cpp
ysprintf.cpp
ysproperty.cpp
ysreldir.cpp
ysshell.cpp
ysshell2d.cpp
ysshellblend.cpp
ysshellblend2.cpp
ysshellfileio.cpp
ysshelllattice.cpp
ysshelllattice_singleThread.cpp
ysshellsearch.cpp
ysshellstl.cpp
ysshellutil.cpp
ysstring.cpp
yssword.cpp
ystask.cpp
ystextresource.cpp
ystextstream.cpp
ysveci.cpp
ysviewcontrol.cpp
yswizard.cpp
ysinterpolation.cpp
ysdelaunaytri.cpp
yspropertyfile.cpp
ystextdata.cpp
yspositivearea.cpp
./yshastextmetadata.cpp
./yscommandline.cpp
)

set(HEADERS
ys2darray.h
ysadvgeometry.h
ysargs.h
ysarray.h
ysbase.h
ysbase64.h
ysbdhandleiterator.h
ysbigint.h
ysbinarytree.h
ysclass.h
ysclassmgr.h
ysclip.h
ysdef.h
ysdict.h
ysedgeloop.h
yseditarray.h
ysequation.h
ysexception.h
ysfilename.h
ysgeometry.h
yshash.h
yshash_.h
ysindexiterator.h
ysinterpolation.h
yskdtree.h
yskeyword.h
yslattice.h
yslist.h
yslist2.h
ysmalloc.h
ysmath.h
ysmatrix.h
ysmatrixtemplate.h
ysneverforget.h
ysoctree.h
yspixelmap.h
yspositivevector.h
yspredefined.h
ysprintf.h
ysprogressivetask.h
ysproperty.h
ysrange.h
ysreldir.h
yssegarray.h
ysshell.h
ysshell2d.h
ysshellblend2.h
ysshellkdtree.h
ysshelllattice.h
ysshelloctree.h
ysshellsearch.h
ysshelltype.h
ysshellutil.h
yssort.h
ysstring.h
ysstringutil.h
ystask.h
ystempobj.h
ystextresource.h
ystextstream.h
ystightarray.h
ysudhandleiterator.h
ysunitconv.h
ysveci.h
ysvectorhistogram.h
ysviewcontrol.h
yswizard.h
ysmergesort.h
yscompilerwarning.h
ysfitcircle.h
ysstringparser.h
ysrect.h
yspropertyfile.h
ystextdata.h
ysarraymask.h
yspositivearea.h
yshastextmetadata.h
yscommandline.h
)

add_library(ysclass ${SRCS} ${HEADERS})
target_include_directories(ysclass PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
