# Copyright (c) 2003-2010 by FlashCode <flashcode@flashtux.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# - Find Gettext
# This module finds if gettext is installed and determines where 
# the include files and libraries are.
#
# This code sets the following variables:
#
#  GETTEXT_FOUND = is gettext usable on system?

IF(GETTEXT_FOUND)
   # Already in cache, be silent
   SET(GETTEXT_FIND_QUIETLY TRUE)
ENDIF(GETTEXT_FOUND)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)

FIND_PATH(LIBINTL_INCLUDE
  NAMES libintl.h
  PATH /usr/local/include /usr/pkg/include /usr/include
)

SET(CMAKE_REQUIRED_INCLUDES ${LIBINTL_INCLUDE})

CHECK_INCLUDE_FILES(libintl.h HAVE_LIBINTL_H)

IF(HAVE_LIBINTL_H)

  CHECK_FUNCTION_EXISTS(dgettext LIBC_HAS_DGETTEXT)
  IF(LIBC_HAS_DGETTEXT)
    SET(GETTEXT_FOUND TRUE)
  ELSE(LIBC_HAS_DGETTEXT)
    FIND_LIBRARY(LIBINTL_LIBRARY NAMES intl
      PATHS
      /usr/local/lib
      /usr/lib
      )
    IF(LIBINTL_LIBRARY)
      IF(${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
        SET(CMAKE_REQUIRED_LIBRARIES "iconv")
        CHECK_LIBRARY_EXISTS(${LIBINTL_LIBRARY} "libintl_dgettext" "" LIBINTL_HAS_DGETTEXT)
      ELSE(${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
        CHECK_LIBRARY_EXISTS(${LIBINTL_LIBRARY} "dgettext" "" LIBINTL_HAS_DGETTEXT)
      ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
      
      IF(LIBINTL_HAS_DGETTEXT)
	SET(GETTEXT_FOUND TRUE)
      ENDIF(LIBINTL_HAS_DGETTEXT)
    ENDIF(LIBINTL_LIBRARY)
  ENDIF(LIBC_HAS_DGETTEXT)
ENDIF(HAVE_LIBINTL_H)
