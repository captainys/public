set(TARGET_NAME t77Server)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} t77lib comportlib)
