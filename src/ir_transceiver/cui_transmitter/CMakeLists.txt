set(TARGET_NAME irsend)

add_executable(${TARGET_NAME} main.cpp ../gui/irdata.cpp ../gui/irdata.h)
target_link_libraries(${TARGET_NAME} cheaplib comportlib irtoylib)
