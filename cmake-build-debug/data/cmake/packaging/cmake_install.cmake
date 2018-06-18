# Install script for directory: /Users/jiatiansun/Documents/compG/mitsuba/data/cmake/packaging

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
  
    message(STATUS "Copying qt_menu.nib into the Bundle resources")
    file(MAKE_DIRECTORY "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Resources/qt_menu.nib")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
      "/Users/jiatiansun/Documents/compG/mitsuba/dependencies/frameworks/QtGui.framework/Resources/qt_menu.nib" 
      "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Resources/qt_menu.nib")
    
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  
message(STATUS "Writing qt.conf")
file(WRITE "${CMAKE_INSTALL_PREFIX}/Mitsuba.app/Contents/Resources/qt.conf"
  "[Paths]
Plugins = qt4plugins
")

endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Runtime" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/data/cmake/packaging/MtsFixupBundle.cmake")
endif()

