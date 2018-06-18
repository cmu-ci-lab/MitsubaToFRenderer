# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/libbidir

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks" TYPE SHARED_LIBRARY FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-bidir.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-bidir.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-bidir.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "libmitsuba-bidir.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-bidir.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-bidir.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-bidir.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Developer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Headers/mitsuba/bidir" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/common.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/edge.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/geodist2.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/manifold.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/mempool.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/mut_bidir.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/mut_caustic.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/mut_lens.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/mut_manifold.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/mut_mchain.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/mutator.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/path.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/pathsampler.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/rsampler.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/util.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/bidir/vertex.h"
    )
endif()

