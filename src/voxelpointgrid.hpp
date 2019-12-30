#ifndef __VOXEL_POINT_GRID_HPP__
#define __VOXEL_POINT_GRID_HPP__

#include <vector>
#include <deque>

namespace voxelizer {
  class VoxelPointLine {
  public:
    VoxelPointLine(int depth);
    bool& operator[](int z);
  private:
    std::deque<bool> voxelOccupancies;
  };

  class VoxelPointPlane {
  public:
    VoxelPointPlane(int height, int depth);
    VoxelPointLine& operator[](int y);
  private:
    std::vector<VoxelPointLine> lines;
  };

  class VoxelPointGrid {
  public:
    VoxelPointGrid(int width, int height, int depth);
    VoxelPointPlane& operator[](int x);
  private:
    std::vector<VoxelPointPlane> planes;
  };
};

#endif
