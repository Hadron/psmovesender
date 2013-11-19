cmake_minimum_required (VERSION 2.8.7)

project (psmovesender)

FILE (READ "${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog" DEBCHANGELOG)
STRING (REGEX REPLACE ".*\\((.*)\\).*" "\\1" DEBFULLVERSION "${DEBCHANGELOG}")
message(STATUS "Building Debian version: " ${DEBFULLVERSION})

SET(DEBFULLVERSION ${DEBFULLVERSION} CACHE INTERNAL "Full package version." FORCE)

if(NOT DEBFULLVERSION)
  message(FATAL_ERROR "unable to find debian version")
endif(NOT DEBFULLVERSION)

find_file(HELP2MAN_EXECUTABLE help2man HINTS /bin /usr/bin /usr/local/bin)
if(NOT HELP2MAN_EXECUTABLE)
  message(FATAL_ERROR "unable to find help2man")
endif(NOT HELP2MAN_EXECUTABLE)

find_file(SED_EXECUTABLE sed HINTS /bin /usr/bin /usr/local/bin)
if(NOT SED_EXECUTABLE)
  message(FATAL_ERROR "unable to find sed")
endif(NOT SED_EXECUTABLE)

if (TRUE)
set (G_SPEAK_MIN_VERSION "3.10")
set (G_SPEAK_HOME /opt/oblong/g-speak3.10)
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

IF(NOT GENGETOPT_EXECUTABLE)
  FIND_PROGRAM(GENGETOPT_EXECUTABLE gengetopt)
  IF (NOT GENGETOPT_EXECUTABLE)
    MESSAGE(FATAL_ERROR "gengetopt not found - aborting")
  ENDIF (NOT GENGETOPT_EXECUTABLE)
ENDIF(NOT GENGETOPT_EXECUTABLE)

ADD_CUSTOM_COMMAND(
  OUTPUT psmove-mouse-options.c psmove-mouse-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version v${DEBFULLVERSION} -F psmove-mouse-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove-mouse.ggo
  DEPENDS psmove-mouse.ggo
  )

ADD_CUSTOM_COMMAND(
  OUTPUT psmove-gspeak-options.c psmove-gspeak-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version v${DEBFULLVERSION} -F psmove-gspeak-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove-gspeak.ggo
  DEPENDS psmove-gspeak.ggo
  )

macro(manpage program)
ADD_CUSTOM_COMMAND (
  OUTPUT ${program}.1
  DEPENDS ${program}
  COMMAND ${HELP2MAN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${program}
  --no-info
  --output=${CMAKE_CURRENT_BINARY_DIR}/${program}.1
  COMMAND ${SED_EXECUTABLE} -i "s#${CMAKE_CURRENT_BINARY_DIR}/##g" ${CMAKE_CURRENT_BINARY_DIR}/${program}.1
  )
INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/${program}.1 DESTINATION share/man/man1)
endmacro(manpage)

add_executable (psmove-mouse
  ${SRCS}
  PSMoveHIDMouseController.cpp
  PSMoveHIDMouseManager.cpp
  HIDSender.cpp
  psmove-mouse-options.c
  psmove-mouse.cpp
  psmove-mouse.1
)
manpage(psmove-mouse)
target_link_libraries (psmove-mouse psmoveapi psmoveapi_tracker CGAL)
install (TARGETS psmove-mouse RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (G_SPEAK_HOME)
add_executable (psmove-gspeak
  ${SRCS}
  PSMoveWandController.cpp
  PSMoveWandManager.cpp
  psmove-gspeak-options.c
  psmove-gspeak.cpp
  psmove-gspeak.1
)
manpage(psmove-gspeak)
target_link_libraries (psmove-gspeak psmoveapi psmoveapi_tracker CGAL)
target_link_libraries (psmove-gspeak ${g_speak_deps_libs})
install (TARGETS psmove-gspeak RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

add_executable (test
  FGParser.cpp
  test.cpp
)
target_link_libraries (test boost_system CGAL)

SET(CPACK_PACKAGE_NAME "psmovesender")
SET(CPACK_PACKAGE_CONTACT "Klee Dienes <klee@mit.edu>")
INCLUDE(CPack)
