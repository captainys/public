cmake_minimum_required(VERSION 3.0)
project(YS_PUBLIC)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(experimental)

set(YS_ALL_BATCH_TEST ${YS_ALL_BATCH_TEST} PARENT_SCOPE)
