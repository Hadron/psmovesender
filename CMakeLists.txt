cmake_minimum_required (VERSION 2.8.7)

project (psmovesender)

include (${psmovesender_SOURCE_DIR}/bld/debian-version.cmake)
include (${psmovesender_SOURCE_DIR}/bld/gengetopt.cmake)
include (${psmovesender_SOURCE_DIR}/bld/help2man.cmake)

if (TRUE)
set (G_SPEAK_MIN_VERSION "3.10")
set (G_SPEAK_HOME /opt/oblong/g-speak3.11)
include (${psmovesender_SOURCE_DIR}/bld/g-speak-sdk.cmake)
else ()
endif ()

set (SRCS
  PSMoveController.cpp
  PSMoveManager.cpp
  FGParser.cpp
  quaternion.cpp
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${g_speak_deps_cflags} -frounding-math -std=c++0x -g -O0")

set (AFRL "${psmovesender_SOURCE_DIR}/..")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_CURRENT_BINARY_DIR} -I${AFRL}/psmoveapi/src -I${AFRL}/psmoveapi/include -I/usr/include/hidapi")

include(GNUInstallDirs)

ADD_CUSTOM_COMMAND(
  OUTPUT psmove-mouse-options.c psmove-mouse-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBFULLVERSION} -F psmove-mouse-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove-mouse.ggo
  DEPENDS psmove-mouse.ggo
  )

add_executable (psmove-mouse
  ${SRCS}
  PSMoveHIDMouseController.cpp
  PSMoveHIDMouseManager.cpp
  HIDSender.cpp
  psmove-mouse-options.c
  psmove-mouse.cpp
  psmove-mouse.1
)
manpage(psmove-mouse "emulate USB HID mouse/tablet with PlayStation Move")
target_link_libraries (psmove-mouse psmoveapi psmoveapi_tracker CGAL boost_system hidapi-hidraw)
install (TARGETS psmove-mouse RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (G_SPEAK_HOME)

ADD_CUSTOM_COMMAND(
  OUTPUT psmove-gspeak-options.c psmove-gspeak-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBFULLVERSION} -F psmove-gspeak-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove-gspeak.ggo
  DEPENDS psmove-gspeak.ggo
  )

add_executable (psmove-gspeak
  ${SRCS}
  PSMoveWandController.cpp
  PSMoveWandManager.cpp
  psmove-gspeak-options.c
  psmove-gspeak.cpp
  psmove-gspeak.1
)
manpage(psmove-gspeak "emulate g-speak wand with PlayStation Move")
target_link_libraries (psmove-gspeak psmoveapi psmoveapi_tracker CGAL boost_system hidapi-hidraw)
target_link_libraries (psmove-gspeak ${g_speak_deps_libs})
install (TARGETS psmove-gspeak RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

endif (G_SPEAK_HOME)

add_executable (test
  FGParser.cpp
  test.cpp
)
target_link_libraries (test boost_system CGAL)

SET(CPACK_PACKAGE_NAME "psmovesender")
SET(CPACK_PACKAGE_CONTACT "Klee Dienes <klee@mit.edu>")
INCLUDE(CPack)
