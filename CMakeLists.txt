cmake_minimum_required (VERSION 2.8.7)

project (psmovesender)

include_directories (lib hid)

include (${psmovesender_SOURCE_DIR}/bld/debian-version.cmake)
include (${psmovesender_SOURCE_DIR}/bld/gengetopt.cmake)
include (${psmovesender_SOURCE_DIR}/bld/help2man.cmake)

if (TRUE)
set (G_SPEAK_MIN_VERSION "3.10")
set (G_SPEAK_HOME /opt/oblong/g-speak3.11)
include (${psmovesender_SOURCE_DIR}/bld/g-speak-sdk.cmake)
else ()
endif ()

set (AFRL "${psmovesender_SOURCE_DIR}/..")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math -std=c++0x")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_CURRENT_BINARY_DIR} -I${AFRL}/psmoveapi/src -I${AFRL}/psmoveapi/include -I/usr/include/hidapi")

include (GNUInstallDirs)

add_custom_command (
  OUTPUT psmove-mouse-options.c psmove-mouse-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBFULLVERSION} -F psmove-mouse-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove/psmove-mouse.ggo
  DEPENDS psmove/psmove-mouse.ggo
  )
set_source_files_properties (psmove/psmove-mouse.cpp PROPERTIES OBJECT_DEPENDS psmove-mouse-options.h)
add_executable (psmove-mouse
  psmove/PSMoveController.cpp
  psmove/PSMoveManager.cpp
  lib/FGParser.cpp
  lib/quaternion.cpp
  hid/HIDSender.cpp
  psmove/PSMoveHIDMouseController.cpp
  psmove/PSMoveHIDMouseManager.cpp
  psmove/psmove-mouse.cpp
  psmove-mouse-options.c
  psmove-mouse-options.h
  psmove-mouse.1
)
manpage (psmove-mouse "emulate USB HID mouse/tablet with PlayStation Move")
target_link_libraries (psmove-mouse psmoveapi psmoveapi_tracker CGAL boost_system hidapi-hidraw)
install (TARGETS psmove-mouse RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (FALSE)
add_executable (test
  FGParser.cpp
  test.cpp
)
target_link_libraries (test boost_system CGAL)
endif (FALSE)

if (G_SPEAK_HOME)

set_gspeak_flags ("libLoam libLoam++ libBasement libImpetus libNoodoo libAfferent")
add_custom_command (
  OUTPUT gspeak-uhid-options.c gspeak-uhid-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBFULLVERSION} -F gspeak-uhid-options --input=${CMAKE_CURRENT_SOURCE_DIR}/gspeak/gspeak-uhid.ggo
  DEPENDS gspeak/gspeak-uhid.ggo
  )
set_source_files_properties (gspeak/Basic.cpp PROPERTIES OBJECT_DEPENDS gspeak-uhid-options.h)
add_executable (gspeak-uhid
  gspeak/WindowedNullVisiFeld.cpp
  gspeak/BasicDrome.cpp
  gspeak/BasicApp.cpp
  gspeak/Basic.cpp
  gspeak/HIDPointer.cpp
  hid/HIDSender.cpp
  gspeak-uhid-options.c
  gspeak-uhid-options.h
  gspeak-uhid.1
)
manpage (gspeak-uhid "emulate USB HID mouse/tablet with any g-speak device")
set_target_properties (gspeak-uhid PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${g_speak_deps_cflags}")
target_link_libraries (gspeak-uhid hidapi-hidraw ${g_speak_deps_libs})
install (TARGETS gspeak-uhid RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set_gspeak_flags ("libLoam libLoam++ libPlasma libPlasma++ libBasement")
add_custom_command (OUTPUT psmove-gspeak-options.c psmove-gspeak-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBFULLVERSION} -F psmove-gspeak-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove/psmove-gspeak.ggo
  DEPENDS psmove/psmove-gspeak.ggo
  )
set_source_files_properties (psmove/psmove-gspeak.cpp PROPERTIES OBJECT_DEPENDS psmove-gspeak-options.h)
add_executable (psmove-gspeak
  psmove/PSMoveController.cpp
  psmove/PSMoveManager.cpp
  lib/quaternion.cpp
  psmove/PSMoveWandController.cpp
  psmove/PSMoveWandManager.cpp
  psmove/psmove-gspeak.cpp
  psmove-gspeak-options.c
  psmove-gspeak-options.h
  psmove-gspeak.1
)
manpage (psmove-gspeak "emulate g-speak wand with PlayStation Move")
set_target_properties (psmove-gspeak PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${g_speak_deps_cflags}")
target_link_libraries (psmove-gspeak psmoveapi psmoveapi_tracker CGAL boost_system hidapi-hidraw ${g_speak_deps_libs})
install (TARGETS psmove-gspeak RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

endif (G_SPEAK_HOME)

set (CPACK_PACKAGE_NAME "psmovesender")
set (CPACK_PACKAGE_CONTACT "Klee Dienes <klee@mit.edu>")
include (CPack)
