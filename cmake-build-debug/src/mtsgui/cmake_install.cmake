# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/src/mtsgui

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins" TYPE MODULE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/plugins/test_simdtonemap.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_simdtonemap.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_simdtonemap.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_simdtonemap.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_simdtonemap.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/plugins/test_simdtonemap.dylib")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS" TYPE EXECUTABLE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/MacOS/mtsgui")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/mtsgui" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/mtsgui")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-core.dylib" "libmitsuba-core.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-hw.dylib" "libmitsuba-hw.dylib"
      -change "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/Frameworks/libmitsuba-render.dylib" "libmitsuba-render.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/mtsgui")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/mtsgui")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/mtsgui")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS" TYPE EXECUTABLE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/Mitsuba.app/Contents/MacOS/symlinks_install")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/symlinks_install" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/symlinks_install")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/MacOS/symlinks_install")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents" TYPE FILE FILES
    "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/./Mitsuba.app/Contents/Info.plist"
    "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/./Mitsuba.app/Contents/PkgInfo"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Resources" TYPE FILE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/./Mitsuba.app/Contents/Resources/mitsuba.icns")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Resources" TYPE FILE FILES "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/./Mitsuba.app/Contents/Resources/PreviewSettings.nib")
endif()

