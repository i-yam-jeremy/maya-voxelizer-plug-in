cmake_minimum_required(VERSION 2.8)
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

project(Voxelizer)
set(PROJECT_NAME Voxelizer)

set(SOURCE_FILES
  src/voxelizer.cpp src/voxelpointgrid.cpp src/voxelpointgrid.hpp
)

set(LIBRARIES OpenMaya Foundation)

build_plugin()
