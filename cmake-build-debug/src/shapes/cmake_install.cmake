# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/shapes

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/obj.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/obj.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/obj.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib" "libmitsuba-hw.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/obj.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/obj.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/obj.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/serialized.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/serialized.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/serialized.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/serialized.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/serialized.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/serialized.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/rectangle.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/rectangle.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/rectangle.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/rectangle.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/rectangle.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/rectangle.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/disk.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/disk.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/disk.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/disk.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/disk.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/disk.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/sphere.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/sphere.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/sphere.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/sphere.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/sphere.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/sphere.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/cylinder.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cylinder.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cylinder.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cylinder.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cylinder.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cylinder.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/cube.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cube.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cube.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cube.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cube.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/cube.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/hair.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/hair.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/hair.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/hair.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/hair.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/hair.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/shapegroup.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/shapegroup.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/shapegroup.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/shapegroup.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/shapegroup.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/shapegroup.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/instance.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/instance.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/instance.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/instance.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/instance.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/instance.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/heightfield.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/heightfield.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/heightfield.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/heightfield.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/heightfield.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/heightfield.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/ply.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/ply.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/ply.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/ply.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/ply.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/ply.dylib")
    endif()
  endif()
endif()

