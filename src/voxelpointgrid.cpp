#include "voxelpointgrid.hpp"

voxelizer::VoxelPointLine::VoxelPointLine(int depth) {
  for (int i = 0; i < depth; i++) {
    this->voxelOccupancies.push_back(false);
  }
}

bool& voxelizer::VoxelPointLine::operator[](int z) {
  return this->voxelOccupancies[z];
}

const bool& voxelizer::VoxelPointLine::operator[](int z) const {
  return this->voxelOccupancies[z];
}

voxelizer::VoxelPointPlane::VoxelPointPlane(int height, int depth) {
  for (int i = 0; i < height; i++) {
    this->lines.push_back(voxelizer::VoxelPointLine(depth));
  }
}

voxelizer::VoxelPointLine& voxelizer::VoxelPointPlane::operator[](int y) {
  return this->lines[y];
}

const voxelizer::VoxelPointLine& voxelizer::VoxelPointPlane::operator[](int y) const {
  return this->lines[y];
}

voxelizer::VoxelPointGrid::VoxelPointGrid(int width, int height, int depth) {
  for (int i = 0; i < width; i++) {
    this->planes.push_back(voxelizer::VoxelPointPlane(height, depth));
  }
}

voxelizer::VoxelPointPlane& voxelizer::VoxelPointGrid::operator[](int x) {
  return this->planes[x];
}

const voxelizer::VoxelPointPlane& voxelizer::VoxelPointGrid::operator[](int x) const {
  return this->planes[x];
}
