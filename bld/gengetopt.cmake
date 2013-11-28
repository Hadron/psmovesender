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

IF (NOT GENGETOPT_EXECUTABLE)
  FIND_PROGRAM (GENGETOPT_EXECUTABLE gengetopt)
  IF (NOT GENGETOPT_EXECUTABLE)
    MESSAGE (FATAL_ERROR "unable to find gengetopt")
  ENDIF (NOT GENGETOPT_EXECUTABLE)
ENDIF (NOT GENGETOPT_EXECUTABLE)
