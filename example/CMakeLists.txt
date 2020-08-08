cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

add_executable(example example.c)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set_property(TARGET example APPEND_STRING PROPERTY COMPILE_FLAGS " /W3")
else()
  set_property(TARGET example APPEND_STRING PROPERTY COMPILE_FLAGS " -Wall")
endif()

target_link_libraries(example svgplot)
