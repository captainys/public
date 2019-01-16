cmake_minimum_required(VERSION 3.8)

# Cmake V3.12 add_compile_definitions(FSSIMPLEWINDOW_FOR_WINDOWS_XP)
add_compile_options(-DFSSIMPLEWINDOW_FOR_WINDOWS_XP)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../../../imported/include)

add_subdirectory("../../FM77AVKeyboard" "${CMAKE_BINARY_DIR}/FM77AVKeyboard")
add_subdirectory("../../util/lib/yscomport" "${CMAKE_BINARY_DIR}/yscomport")
add_subdirectory("../../lib/irtoy" "${CMAKE_BINARY_DIR}/irtoy")
add_subdirectory("../../lib/cheaplib" "${CMAKE_BINARY_DIR}/cheaplib")
add_subdirectory("../../lib/cheapgui" "${CMAKE_BINARY_DIR}/cheapgui")
# add_subdirectory("../../RomajiToKanaKey" "${CMAKE_BINARY_DIR}/RomajiToKanaKey")

add_subdirectory("../../../ysgl/src" "${CMAKE_BINARY_DIR}/ysgl")
add_subdirectory("../../../ysbitmapfont/src" "${CMAKE_BINARY_DIR}/ysbitmapfont")
add_subdirectory("../../../fssimplewindow/src" "${CMAKE_BINARY_DIR}/fssimplewindow")
