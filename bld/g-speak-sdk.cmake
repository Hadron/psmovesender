
# /* (c) 2012 Oblong Industries */

# find various yobuilds and nobuilds
if (UNIX)
  if (NOT APPLE)
    set (G_SPEAK_DEPS "/opt/oblong/deps")
  else()
    set (G_SPEAK_DEPS "/opt/oblong/deps-64-8")
  endif ()
  set (CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH} ${G_SPEAK_DEPS}")
endif ()

if (NOT G_SPEAK_MIN_VERSION)
  set (G_SPEAK_MIN_VERSION "3.0")
endif ()
if (NOT G_SPEAK_HOME)
  set (G_SPEAK_HOME $ENV{G_SPEAK_HOME}) # blork, $ENV isn't testable.
endif ()
if (NOT G_SPEAK_HOME)
  execute_process(
    COMMAND ${greenhouse_SOURCE_DIR}/bld/find-gspeak-sdk ${G_SPEAK_MIN_VERSION}
    OUTPUT_VARIABLE G_SPEAK_HOME
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()
if (NOT G_SPEAK_HOME)
  set (msg "Can't find g-speak >= ${G_SPEAK_MIN_VERSION}, please set the")
  set (msg "${msg} G_SPEAK_HOME environment variable")
  message (FATAL_ERROR "\n${msg}\n")
endif ()

if (EXISTS ${G_SPEAK_HOME} AND IS_DIRECTORY ${G_SPEAK_HOME})
  message (STATUS "found g-speak: ${G_SPEAK_HOME}")
else ()
  set (msg "I thought I found g-speak at ${G_SPEAK_HOME}, but it's not")
  set (msg "${msg} even a directory")
  message (FATAL_ERROR "\n${msg}\n")
endif ()

if (APPLE)
  set (PKG_CONFIG_EXECUTABLE ${G_SPEAK_DEPS}/bin/pkg-config)
else ()
  set (PKG_CONFIG_EXECUTABLE pkg-config)
endif ()
set (ENV{PKG_CONFIG_PATH} "${G_SPEAK_HOME}/lib/pkgconfig")

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
