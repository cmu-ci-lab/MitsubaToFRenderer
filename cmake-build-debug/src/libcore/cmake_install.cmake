# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/libcore

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks" TYPE SHARED_LIBRARY FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "libmitsuba-core.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Developer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Headers/mitsuba" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/mitsuba.h")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Developer" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Headers/mitsuba/core" TYPE FILE PERMISSIONS OWNER_READ GROUP_READ WORLD_READ FILES
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/aabb.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/aabb_sse.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/appender.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/atomic.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/barray.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/bitmap.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/brent.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/bsphere.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/simplecache.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/chisquare.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/class.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/cobject.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/constants.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/cstream.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/formatter.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/frame.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/fresolver.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/fstream.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/fwd.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/half.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/kdtree.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/lock.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/logger.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/lrucache.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/math.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/matrix.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/matrix.inl"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/mempool.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/mmap.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/mstream.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/netobject.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/normal.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/object.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/octree.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/platform.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/plugin.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/pmf.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/point.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/properties.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/qmc.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/quad.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/quat.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/random.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/ray.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/ray_sse.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/ref.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/rfilter.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/sched.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/sched_remote.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/serialization.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/sfcurve.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/shvector.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/spectrum.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/spline.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/sse.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/ssemath.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/ssevector.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/sshstream.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/sstream.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/statistics.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/stream.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/thread.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/timer.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/tls.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/track.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/transform.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/triangle.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/util.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/vector.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/version.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/vmf.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/warp.h"
    "/Users/jiatiansun/Documents/compG/mitsuba/include/mitsuba/core/zstream.h"
    )
endif()

