# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/data/schema/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/data/ior/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/data/microfacet/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/libcore/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/librender/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/libhw/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/libbidir/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/libpython/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/mitsuba/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/converter/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/mtsgui/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/utils/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/bsdfs/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/phase/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/shapes/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/samplers/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/rfilters/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/films/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/sensors/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/emitters/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/medium/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/volume/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/subsurface/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/textures/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/integrators/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/src/tests/cmake_install.cmake")
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/data/cmake/packaging/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
