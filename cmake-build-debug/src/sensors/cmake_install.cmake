# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/sensors

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/radiancemeter.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/radiancemeter.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/radiancemeter.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/radiancemeter.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/radiancemeter.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/radiancemeter.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/irradiancemeter.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/irradiancemeter.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/irradiancemeter.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/irradiancemeter.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/irradiancemeter.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/irradiancemeter.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/fluencemeter.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/fluencemeter.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/fluencemeter.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/fluencemeter.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/fluencemeter.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/fluencemeter.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/perspective.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/perspective_rdist.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective_rdist.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective_rdist.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective_rdist.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective_rdist.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/perspective_rdist.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/thinlens.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/thinlens.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/thinlens.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/thinlens.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/thinlens.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/thinlens.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/orthographic.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/orthographic.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/orthographic.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/orthographic.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/orthographic.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/orthographic.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/telecentric.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/telecentric.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/telecentric.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/telecentric.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/telecentric.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/telecentric.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/spherical.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/spherical.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/spherical.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/spherical.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/spherical.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/spherical.dylib")
    endif()
  endif()
endif()

