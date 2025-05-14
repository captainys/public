cmake_minimum_required(VERSION 3.0)
project(YS_PUBLIC)

if(UNIX)
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -Wno-unused-variable)
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-unused-variable)
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(experimental)

set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)

set(YS_PUBLIC_LIBRARIES 1 PARENT_SCOPE)
