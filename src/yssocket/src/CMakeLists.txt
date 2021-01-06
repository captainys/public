set(TARGET_NAME yssocket)
add_library(${TARGET_NAME} yssocket.cpp)
target_link_libraries(${TARGET_NAME} ysclass)
