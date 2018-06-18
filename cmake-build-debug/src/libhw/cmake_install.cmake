# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/libhw

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks" TYPE SHARED_LIBRARY FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "libmitsuba-hw.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Developer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Headers/mitsuba/hw" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/basicshader.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/device.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/font.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/glgeometry.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/glprogram.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/glrenderer.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/glsync.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/gltexture.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/gpugeometry.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/gpuprogram.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/gpusync.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/gputexture.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/renderer.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/shadow.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/session.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/viewer.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/vpl.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/nsgldevice.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/nsglkeys.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/nsglrenderer.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/hw/nsglsession.h"
    )
endif()

