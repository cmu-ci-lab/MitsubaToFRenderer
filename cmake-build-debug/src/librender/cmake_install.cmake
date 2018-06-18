# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/librender

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks" TYPE SHARED_LIBRARY FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "libmitsuba-render.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Developer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Headers/mitsuba/render" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/bsdf.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/common.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/emitter.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/film.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/fwd.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/gatherproc.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/gkdtree.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/imageblock.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/imageproc.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/integrator.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/irrcache.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/medium.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/mipmap.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/noise.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/particleproc.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/phase.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/photon.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/photonmap.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/range.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/records.inl"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/rectwu.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/renderjob.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/renderproc.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/renderqueue.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/sahkdtree2.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/sahkdtree3.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/sampler.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/scene.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/scenehandler.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/sensor.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/shader.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/shape.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/skdtree.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/spiral.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/subsurface.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/testcase.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/texture.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/triaccel.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/triaccel_sse.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/trimesh.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/util.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/volume.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/render/vpl.h"
    )
endif()

