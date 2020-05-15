cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
  if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting CMAKE_BUILD_TYPE type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
  endif()
endif()

project(svgplot VERSION 0.1.0 LANGUAGES C)

set(SVGPLOT_PUBLIC_INCLUDES svgplot.h)

add_library(svgplot STATIC src/svgplot.c src/space_iter.h ${SVGPLOT_PUBLIC_INCLUDES})
set_property(TARGET svgplot APPEND PROPERTY PUBLIC_HEADER ${SVGPLOT_PUBLIC_INCLUDES})
set_property(TARGET svgplot PROPERTY ARCHIVE_OUTPUT_DIRECTORY "$<$<NOT:$<CONFIG:Release>>:$<CONFIG>>")

if (x${CMAKE_CXX_COMPILER_ID} STREQUAL "xMSVC")
  set_property(TARGET svgplot APPEND_STRING PROPERTY COMPILE_FLAGS " /W3")
else()
  set_property(TARGET svgplot APPEND_STRING PROPERTY COMPILE_FLAGS " -Wall")
endif()

target_include_directories(svgplot PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>")

if (UNIX)
  target_link_libraries(svgplot pthread)
endif()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_LIST_DIR})
  include(CMakePackageConfigHelpers)
  configure_package_config_file(cmake_support/svgplotConfig.cmake.in ${CMAKE_BINARY_DIR}/cmake/svgplot/svgplotConfig.cmake
    INSTALL_DESTINATION "lib/cmake/svgplot"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  write_basic_package_version_file(${CMAKE_BINARY_DIR}/cmake/svgplot/svgplotConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

  export(TARGETS svgplot FILE "${PROJECT_BINARY_DIR}/cmake/svgplot/svgplotTargets.cmake")

  install(TARGETS svgplot
    EXPORT svgplot_targets
    RUNTIME DESTINATION "bin$<$<NOT:$<CONFIG:Release>>:/$<CONFIG>>"
    ARCHIVE DESTINATION "lib$<$<NOT:$<CONFIG:Release>>:/$<CONFIG>>"
    LIBRARY DESTINATION "lib$<$<NOT:$<CONFIG:Release>>:/$<CONFIG>>"
    PUBLIC_HEADER DESTINATION include/svgplot
    INCLUDES DESTINATION include)
  install(EXPORT svgplot_targets DESTINATION lib/cmake/svgplot FILE "svgplotTargets.cmake")
  install(FILES "${CMAKE_BINARY_DIR}/cmake/svgplot/svgplotConfig.cmake"
                "${CMAKE_BINARY_DIR}/cmake/svgplot/svgplotConfigVersion.cmake"
          DESTINATION "lib/cmake/svgplot")
  install(FILES LICENSE DESTINATION share/doc/svgplot)
  install(FILES README.md DESTINATION share/doc/svgplot)
endif()



