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

# Output variable: DEBIAN_CHANGELOG_VERSION

FUNCTION (READ_DEBIAN_VERSION)
  FILE (READ "${CMAKE_CURRENT_SOURCE_DIR}/debian/changelog" DEBIAN_CHANGELOG_TEXT)
  IF (NOT DEBIAN_CHANGELOG_TEXT)
    MESSAGE (FATAL_ERROR "unable to read text from debian/changelog")
  ENDIF (NOT DEBIAN_CHANGELOG_TEXT)
  STRING (REGEX REPLACE ".*\\((.*)\\).*" "\\1" DEBIAN_CHANGELOG_VERSION "${DEBIAN_CHANGELOG_TEXT}")
  SET (DEBIAN_CHANGELOG_VERSION "${DEBIAN_CHANGELOG_VERSION}" CACHE INTERNAL "Changelog parsed from debian/changelog." FORCE)
  IF (NOT DEBIAN_CHANGELOG_VERSION)
    MESSAGE (FATAL_ERROR "unable to parse version number from debian/changelog")
  ENDIF (NOT DEBIAN_CHANGELOG_VERSION)
ENDFUNCTION (READ_DEBIAN_VERSION)
