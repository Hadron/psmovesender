# Copyright (c) 2012-2013 Hadron Industries, Inc.
#
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 2.8.7)

project (psmovesender)
include (${psmovesender_SOURCE_DIR}/bld/debian-version.cmake)
include (${psmovesender_SOURCE_DIR}/bld/gengetopt.cmake)
include (${psmovesender_SOURCE_DIR}/bld/help2man.cmake)
include (GNUInstallDirs)

READ_DEBIAN_VERSION ()

include_directories (lib hid)
include_directories (/usr/include/hidapi)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math -std=c++0x")

if (TRUE)
set (G_SPEAK_MIN_VERSION "3.10")
set (G_SPEAK_HOME /opt/oblong/g-speak3.11)
include (${psmovesender_SOURCE_DIR}/bld/g-speak-sdk.cmake)
endif (TRUE)

add_custom_command (
  OUTPUT psmove-mouse-options.c psmove-mouse-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBIAN_CHANGELOG_VERSION} -F psmove-mouse-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove/psmove-mouse.ggo
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
install_help2man (psmove-mouse "emulate USB HID mouse/tablet with PlayStation Move")
target_link_libraries (psmove-mouse psmoveapi psmoveapi_tracker CGAL boost_system hidapi-hidraw)
install (TARGETS psmove-mouse RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (G_SPEAK_HOME)

set (CMAKE_INSTALL_RPATH "/opt/oblong/g-speak3.11/lib")

find_gspeak (Loam Loam++ Basement Impetus Noodoo Afferent)
add_custom_command (
  OUTPUT gspeak-uhid-options.c gspeak-uhid-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBIAN_CHANGELOG_VERSION} -F gspeak-uhid-options --input=${CMAKE_CURRENT_SOURCE_DIR}/gspeak/gspeak-uhid.ggo
  DEPENDS gspeak/gspeak-uhid.ggo
  )
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
set_source_files_properties (gspeak/Basic.cpp PROPERTIES OBJECT_DEPENDS gspeak-uhid-options.h)
install_help2man (gspeak-uhid "emulate USB HID mouse/tablet with any g-speak device")
set_target_properties (gspeak-uhid PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${G_SPEAK_CFLAGS}")
target_link_libraries (gspeak-uhid hidapi-hidraw ${G_SPEAK_LIBRARIES})
install (TARGETS gspeak-uhid RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

find_gspeak (Plasma)
add_custom_command (OUTPUT psmove-gspeak-options.c psmove-gspeak-options.h
  COMMAND ${GENGETOPT_EXECUTABLE} ARGS --set-version ${DEBIAN_CHANGELOG_VERSION} -F psmove-gspeak-options --input=${CMAKE_CURRENT_SOURCE_DIR}/psmove/psmove-gspeak.ggo
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
install_help2man (psmove-gspeak "emulate g-speak wand with PlayStation Move")
set_target_properties (psmove-gspeak PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${G_SPEAK_CFLAGS}")
target_link_libraries (psmove-gspeak psmoveapi psmoveapi_tracker CGAL boost_system hidapi-hidraw ${G_SPEAK_LIBRARIES})
install (TARGETS psmove-gspeak RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

endif (G_SPEAK_HOME)

set (CPACK_PACKAGE_NAME "psmovesender")
set (CPACK_PACKAGE_CONTACT "Klee Dienes <klee.dienes@hadronindustries.com>")
include (CPack)
