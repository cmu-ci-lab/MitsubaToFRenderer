cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

# Configure the Info.plist to add the version
if (NOT DEFINED MTS_VERSION)
  set (MTS_VERSION "0.5.0")  
endif ()
configure_file("/Users/jiatiansun/Documents/compG/mitsuba/data/darwin/Info.plist.in" "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/./Mitsuba.app/Contents/Info.plist" @ONLY)
# Just copy the PkgInfo file
configure_file("/Users/jiatiansun/Documents/compG/mitsuba/data/darwin/PkgInfo" "/Users/jiatiansun/Documents/compG/mitsuba/cmake-build-debug/binaries/./Mitsuba.app/Contents/PkgInfo" COPYONLY)
