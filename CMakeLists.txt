cmake_minimum_required (VERSION 2.8.7)

project (psmovesender)

set (G_SPEAK_MIN_VERSION "3.9")
set (G_SPEAK_HOME /opt/oblong/g-speak3.11)

include (${psmovesender_SOURCE_DIR}/bld/g-speak-sdk.cmake)

set (g_speak_deps "")
foreach (g_speak_dep libLoam libLoam++ libPlasma libPlasma++ libBasement
                     libImpetus libAfferent libNoodoo libGestation libMedia
                     libTwillig libPlasmaZeroconf)
  set (g_speak_deps "${g_speak_deps} ${g_speak_dep}")
endforeach ()

execute_process (
  COMMAND ${PKG_CONFIG_EXECUTABLE} ${g_speak_deps} --cflags
  OUTPUT_VARIABLE g_speak_deps_cflags
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

string (REGEX REPLACE " -pthread" "" g_speak_deps_cflags "${g_speak_deps_cflags}")

execute_process (
  COMMAND ${PKG_CONFIG_EXECUTABLE} ${g_speak_deps} --libs --static
  OUTPUT_VARIABLE g_speak_deps_libs
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${g_speak_deps_cflags} -std=c++11 -g -O0")

add_executable (psmovesender
  PSMoveController.cpp
  PSMoveManager.cpp
  main.cpp
)

target_link_libraries (psmovesender ${g_speak_deps_libs})

set (AFRL "${psmovesender_SOURCE_DIR}/..")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/opt/oblong/g-speak3.11/include -I${AFRL}/psmoveapi/src -I${AFRL}/psmoveapi/include -I/usr/include/hidapi")

target_link_libraries (psmovesender psmoveapi psmoveapi_tracker)

include(GNUInstallDirs)

install (TARGETS psmovesender RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

SET(CPACK_PACKAGE_NAME "psmovesender")
SET(CPACK_PACKAGE_CONTACT "Klee Dienes <klee@mit.edu>")
INCLUDE(CPack)
