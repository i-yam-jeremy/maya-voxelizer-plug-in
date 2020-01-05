#ifndef __VOXEL_POINT_GRID_HPP__
#define __VOXEL_POINT_GRID_HPP__

#include <deque>
#include <vector>

/**

  VoxelPointGrid, VoxelPointPlane, and VoxelPointLine store 3-, 2-, and
  1-dimensional occupancy data.

  VoxelPointGrid can be accessed using multiple [] operators. This provides
  a clean and easy to read syntax.

  Example:
    VoxelPointGrid grid;
    ...
    grid[x][y][z] = false;
    ...
    std::cout << grid[x][y][z] << std::endl;

*/

namespace voxelizer {
/* Stores 1-dimensional voxel occupancy data. */
class VoxelPointLine {
public:
  /* Creates an empty voxel point line with the given size. */
  VoxelPointLine(int depth);
  bool &operator[](int z);
  const bool &operator[](int z) const;

private:
  /* The occupancies for each point on the line. */
  std::deque<bool> voxelOccupancies;
};

class VoxelPointPlane {
public:
  /* Creates an empty voxel point plane with the given dimensions. */
  VoxelPointPlane(int height, int depth);
  VoxelPointLine &operator[](int y);
  const VoxelPointLine &operator[](int y) const;

private:
  /* The 1-dimensional lines that make up the 2-dimensional plane. */
  std::vector<VoxelPointLine> lines;
};

/* Stores voxel point occupancy data. */
class VoxelPointGrid {
public:
  /* Creates an empty voxel point grid with the given dimensions. */
  VoxelPointGrid(int width, int height, int depth);
  VoxelPointPlane &operator[](int x);
  const VoxelPointPlane &operator[](int x) const;

private:
  /* The 2-dimensional cross-sections that make up the 3-dimensional grid. */
  std::vector<VoxelPointPlane> planes;
};
}; // namespace voxelizer

#endif
