cmake_minimum_required(VERSION 2.8)
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

project(Retopo)
set(PROJECT_NAME Retopo)

set(SOURCE_FILES
  src/retopo.cpp
)

set(LIBRARIES OpenMaya)

build_plugin()
