cmake_minimum_required(VERSION 3.0...3.29)

set(PROFIT_MAJOR_VERSION 0)
set(PROFIT_MINOR_VERSION 9)
set(PROFIT_MICRO_VERSION 1)
set(PROFIT_BETA_VERSION )
set(PROFIT_VERSION ${PROFIT_MAJOR_VERSION}.${PROFIT_MINOR_VERSION}.${PROFIT_MICRO_VERSION}${PROFIT_BETA_VERSION})

project(profit VERSION ${PROFIT_MAJOR_VERSION}.${PROFIT_MINOR_VERSION}.${PROFIT_MICRO_VERSION})
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER LANGUAGES C)

string(TIMESTAMP PROFIT_BUILD_YEAR "%Y")
math(EXPR PROFIT_SO_VERSION ${PROJECT_VERSION_MAJOR}*20)
set(PROFIT_INTERFACE_VERSION 0)
set(PROFIT_INTERFACE_REVISION 0)
set(PROFIT_INTERFACE_AGE 0)

# ############################################################################
# these will be removed after upgrading CMake minimum version
set(PROJECT_DESCRIPTION   "Portable C library for handling MultiGen Open Flight 3D model files")
# ############################################################################

# ############################################################################
# Prevent in-source builds, as they often cause severe build problems
# ############################################################################

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "${CMAKE_PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake <path_to_${CMAKE_PROJECT_NAME}> [options]' from there.")
endif()

# ############################################################################
# Include necessary submodules
# ############################################################################

include(CheckIncludeFile)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(TestBigEndian)

# ############################################################################
# Provide options to customise the build
# ############################################################################

option(PROFIT_BUILD_SHARED_LIBS "Build shared library when ON, static when OFF (default)." OFF)
#option(PROFIT_BUILD_TESTS "Build unit tests when ON (default), skips them when OFF." ON)

# ##########################################################################
# Setup build environment
# ##########################################################################

if(NOT CMAKE_BUILD_TYPE)
  # Has no effect for multi configuration generators (VisualStudio, Xcode).
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose type of build, options are Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()
# Set common output directories for all targets built.
# First for the generic no-config case (e.g. with mingw)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# Second, for multi-config builds (e.g. msvc)
foreach (_config ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${_config} _config)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_config} "${CMAKE_BINARY_DIR}/lib")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_config} "${CMAKE_BINARY_DIR}/lib")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_config} "${CMAKE_BINARY_DIR}/bin")
endforeach()

check_include_file(string.h HAVE_STRING_H)

TEST_BIG_ENDIAN(WORDS_BIGENDIAN)

# Get all compilation units.
set(COMMON_SRCS
  src/basics.c
  src/cruft.c
  src/debug.c
  src/messages.c
  src/model.c
  src/node.c
  src/nodeinfo.c
  src/profit.c
  src/state.c
  src/vertex.c
  src/nodes/color_palette.c
  src/nodes/external_reference.c
  src/nodes/face.c
  src/nodes/group.c
  src/nodes/header.c
  src/nodes/instance_definition.c
  src/nodes/instance_reference.c
  src/nodes/level_of_detail.c
  src/nodes/material.c
  src/nodes/material_palette.c
  src/nodes/matrix.c
  src/nodes/morph_vertex_list.c
  src/nodes/object.c
  src/nodes/obsolete_vertex_with_color.c
  src/nodes/obsolete_vertex_with_normal.c
  src/nodes/pop_attribute.c
  src/nodes/pop_extension.c
  src/nodes/pop_level.c
  src/nodes/pop_subface.c
  src/nodes/push_attribute.c
  src/nodes/push_extension.c
  src/nodes/push_level.c
  src/nodes/push_subface.c
  #src/nodes/replicate.c
  src/nodes/texture.c
  src/nodes/vertex_list.c
  src/nodes/vertex_palette.c
  src/nodes/vertex_with_color.c
  src/nodes/vertex_with_normal.c
  src/nodes/vertex_with_normal_and_texture.c
  src/nodes/vertex_with_texture.c
  src/util/array.c
  src/util/bfile.c
  src/util/mempool.c
)

set(COMMON_HDRS
  include/profit/basics.h
  include/profit/callback.h
  include/profit/debug.h
  include/profit/material.h
  include/profit/messages.h
  include/profit/model.h
  include/profit/node.h
  include/profit/nodeinfo.h
  include/profit/profit.h
  include/profit/state.h
  include/profit/texture.h
  include/profit/vertex.h
  include/profit/nodes/color_palette.h
  include/profit/nodes/external_reference.h
  include/profit/nodes/face.h
  include/profit/nodes/group.h
  include/profit/nodes/header.h
  include/profit/nodes/instance_definition.h
  include/profit/nodes/instance_reference.h
  include/profit/nodes/level_of_detail.h
  include/profit/nodes/material.h
  include/profit/nodes/material_palette.h
  include/profit/nodes/matrix.h
  include/profit/nodes/morph_vertex_list.h
  include/profit/nodes/object.h
  include/profit/nodes/obsolete_vertex_with_color.h
  include/profit/nodes/obsolete_vertex_with_normal.h
  include/profit/nodes/pop_attribute.h
  include/profit/nodes/pop_extension.h
  include/profit/nodes/pop_level.h
  include/profit/nodes/pop_subface.h
  include/profit/nodes/push_attribute.h
  include/profit/nodes/push_extension.h
  include/profit/nodes/push_level.h
  include/profit/nodes/push_subface.h
  #include/profit/nodes/replicate.h
  include/profit/nodes/texture.h
  include/profit/nodes/vertex_list.h
  include/profit/nodes/vertex_palette.h
  include/profit/nodes/vertex_with_color.h
  include/profit/nodes/vertex_with_normal.h
  include/profit/nodes/vertex_with_normal_and_texture.h
  include/profit/nodes/vertex_with_texture.h
  include/profit/util/array.h
  include/profit/util/bfile.h
  include/profit/util/mempool.h
)

# Setup the library.
if(WIN32)
  if(MINGW)
    set(PROFIT_DEFAULT_SHARED_POSTFIX "")
    set(PROFIT_DEFAULT_STATIC_POSTFIX "")
  else()
    # On Windows the major version number is part of the library name
    set(PROFIT_DEFAULT_SHARED_POSTFIX ${PROJECT_VERSION_MAJOR})
    set(PROFIT_DEFAULT_STATIC_POSTFIX ${PROJECT_VERSION_MAJOR}s)
  endif()
  if(PROFIT_BUILD_SHARED_LIBS)
    set(PROFIT_DEFAULT_POSTFIX ${PROFIT_DEFAULT_SHARED_POSTFIX})
    set(PROFIT_LIBRARY_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
  else()
    set(PROFIT_DEFAULT_POSTFIX ${PROFIT_DEFAULT_STATIC_POSTFIX})
    set(PROFIT_LIBRARY_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
  endif()
  set(CMAKE_RELEASE_POSTFIX ${PROFIT_DEFAULT_POSTFIX})
  set(CMAKE_MINSIZEREL_POSTFIX ${PROFIT_DEFAULT_POSTFIX})
  set(CMAKE_RELWITHDEBINFO_POSTFIX ${PROFIT_DEFAULT_POSTFIX})
  set(CMAKE_DEBUG_POSTFIX ${PROFIT_DEFAULT_POSTFIX}d)

  set(PROFIT_RELEASE_SYSTEM_LIBRARY_NAME profit${CMAKE_RELEASE_POSTFIX}${PROFIT_LIBRARY_SUFFIX})
  set(PROFIT_DEBUG_SYSTEM_LIBRARY_NAME profit${CMAKE_DEBUG_POSTFIX}${PROFIT_LIBRARY_SUFFIX})
elseif(APPLE)
else()
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  set(PROFIT_RELEASE_SYSTEM_LIBRARY_NAME "$<TARGET_FILE_NAME:profit>")
  set(PROFIT_DEBUG_SYSTEM_LIBRARY_NAME "$<TARGET_FILE_NAME:profit>")
endif()

if(PROFIT_BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED ${COMMON_SRCS} ${COMMON_HDRS})
else()
  add_library(${PROJECT_NAME} STATIC ${COMMON_SRCS} ${COMMON_HDRS})
endif()

if(WIN32 AND NOT MINGW)
  set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROFIT_SO_VERSION})
else()
  set_target_properties(${PROJECT_NAME} PROPERTIES VERSION "${PROFIT_SO_VERSION}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" SOVERSION ${PROFIT_SO_VERSION})
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_CONFIG_H PROFIT_INTERNAL PROFIT_DEBUG=$<CONFIG:Debug>)

if(WIN32)
  if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES)
  endif()
  if(PROFIT_BUILD_SHARED_LIBS)
    configure_file(${PROJECT_NAME}.rc.cmake.in ${PROJECT_NAME}.rc)
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PROFIT_MAKE_DLL INTERFACE PROFIT_DLL)
  else()
    target_compile_definitions(${PROJECT_NAME} INTERFACE PROFIT_NOT_DLL)
    if(MSVC)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")                       # Override default /Zi to embed
      set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Z7")     # debugging info in the .lib.
    endif()
  endif()
else()
  target_link_libraries(${PROJECT_NAME} m)
endif()

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ############################################################################
# Packaging prerequisites
# ############################################################################

set(PACKAGE ${PROJECT_NAME})
set(PACKAGE_DESCRIPTION "Portable C library for handling MultiGen Open Flight 3D model files")
set(PACKAGE_BUGREPORT "coin-support@coin3d.org")
set(PACKAGE_NAME ${PROJECT_NAME})
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
set(PACKAGE_TARNAME ${PROJECT_NAME_LOWER})
set(PACKAGE_URL "https://github.com/coin3d/${PROJECT_NAME_LOWER}")
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(PACKAGE_HOST ${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME})
set(PACKAGE_COMPILER ${CMAKE_CXX_COMPILER})
set(PACKAGE_REQUIREMENTS "profit")

configure_file(include/config.h.cmake.in config.h)

# ############################################################################
# Setup targets in subdirectories
# ############################################################################

add_subdirectory(examples)

# ############################################################################
# Install headers and binaries
# ############################################################################

install(DIRECTORY include/${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT development FILES_MATCHING PATTERN "*.h")

if(NOT CMAKE_VERSION VERSION_LESS 3.12)
  install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime NAMELINK_COMPONENT development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
  )
  if(PROFIT_BUILD_SHARED_LIBS)
    if(MSVC)
      install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR} CONFIGURATIONS Debug RelWithDebInfo COMPONENT debug)
    endif()
  endif()
else()
  install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime NAMELINK_SKIP
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
  )
  if(PROFIT_BUILD_SHARED_LIBS)
    install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development NAMELINK_ONLY)
    if(MSVC)
      install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR} CONFIGURATIONS Debug RelWithDebInfo COMPONENT debug)
    endif()
  endif()
endif()

# ############################################################################
# Install pkgconfig file and CMake config package
# ############################################################################

if(UNIX OR MINGW)
  configure_file(${CMAKE_SOURCE_DIR}/${PROJECT_NAME_LOWER}.pc.cmake.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME_LOWER}.pc @ONLY)
  install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME_LOWER}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

configure_package_config_file(${PROJECT_NAME_LOWER}-config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}
  PATH_VARS CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# To make the component usable not only from the install directory but also from the build directory
export(
  TARGETS ${PROJECT_NAME}
  FILE ${PROJECT_NAME_LOWER}-export.cmake
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}
  COMPONENT development
)

install(EXPORT ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}::
  FILE ${PROJECT_NAME_LOWER}-export.cmake
  COMPONENT development
)
