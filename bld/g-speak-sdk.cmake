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

find_package (PkgConfig)

set (CMAKE_INSTALL_RPATH "/opt/oblong/g-speak3.11/lib")

set (ENV{PKG_CONFIG_PATH} "${G_SPEAK_HOME}/lib/pkgconfig")

macro (FIND_GSPEAK)
  
  foreach (lib ${ARGN})
    
    string (TOUPPER ${lib} var)

    pkg_check_modules (${var} lib${lib})
    
    include_directories (${${var}_INCLUDE_DIRS})
    link_directories (${${var}_LIBRARY_DIRS})
    
    set (G_SPEAK_LIBRARIES "${G_SPEAK_LIBRARIES};${${var}_LIBRARIES}")

  endforeach ()
endmacro (FIND_GSPEAK)
