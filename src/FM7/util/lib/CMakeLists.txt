set(TARGET_NAME fm7lib)

set(SRCS
fm7lib.cpp
cpplib.cpp
)
set(HEADERS
fm7lib.h
cpplib.h
)

add_subdirectory(yscomport)

add_library(${TARGET_NAME} ${SRCS} ${HEADERS})
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
