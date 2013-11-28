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

FIND_FILE (HELP2MAN_EXECUTABLE help2man HINTS /bin /usr/bin /usr/local/bin)
IF (NOT HELP2MAN_EXECUTABLE)
  MESSAGE (FATAL_ERROR "unable to find help2man")
ENDIF (NOT HELP2MAN_EXECUTABLE)

FIND_FILE (SED_EXECUTABLE sed HINTS /bin /usr/bin /usr/local/bin)
IF (NOT SED_EXECUTABLE)
  message (FATAL_ERROR "unable to find sed")
ENDIF (NOT SED_EXECUTABLE)

MACRO (INSTALL_HELP2MAN program description)
ADD_CUSTOM_COMMAND (
  OUTPUT ${program}.1
  DEPENDS ${program}
  COMMAND ${HELP2MAN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${program}
  -n "${description}"
  --no-info
  --output=${CMAKE_CURRENT_BINARY_DIR}/${program}.1
  COMMAND ${SED_EXECUTABLE} -i "s#${CMAKE_CURRENT_BINARY_DIR}/##g" ${CMAKE_CURRENT_BINARY_DIR}/${program}.1
  )
INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/${program}.1 DESTINATION share/man/man1)
ENDMACRO (INSTALL_HELP2MAN manpage)
