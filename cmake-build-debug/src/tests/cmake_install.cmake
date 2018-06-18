# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/tests

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_chisquare.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_chisquare.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_chisquare.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_chisquare.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_chisquare.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_chisquare.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_dgeom.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_dgeom.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_dgeom.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_dgeom.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_dgeom.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_dgeom.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_kd.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_kd.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_kd.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_kd.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_kd.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_kd.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_la.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_la.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_la.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_la.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_la.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_la.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_quad.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_quad.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_quad.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_quad.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_quad.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_quad.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_random.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_random.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_random.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_random.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_random.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_random.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_rtrans.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_rtrans.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_rtrans.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_rtrans.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_rtrans.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_rtrans.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_samplers.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_samplers.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_samplers.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_samplers.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_samplers.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_samplers.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_sh.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_sh.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_sh.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_sh.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_sh.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_sh.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_spectrum.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_spectrum.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_spectrum.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_spectrum.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_spectrum.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_spectrum.dylib")
    endif()
  endif()
endif()

