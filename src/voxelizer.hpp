#ifndef __VOXELIZER_HPP__
#define __VOXELIZER_HPP__

#include <string>

#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

#include "voxelpointgrid.hpp"

namespace voxelizer {
/* A MEL command for creating voxel-approximations of input geometry. */
class Voxelizer : public MPxCommand {
public:
  /* Default constructor. */
  Voxelizer();
  /* Creates a voxelized mesh from the selected polygon object. */
  MStatus doIt(const MArgList &args) override;
  /* Re-inserts the voxelized mesh from doIt() into the world. */
  MStatus redoIt() override;
  /* Removes the voxelized mesh from doIt() from the world. */
  MStatus undoIt() override;
  /* Returns true because this command is undoable. */
  bool isUndoable() const override;
  /* Creates an instance of this class using the new keyword. */
  static void *creator();

private:
  /* The handle for the created voxel mesh. */
  MObject voxelObject;
  /* The voxel size in world coordinates. */
  double resolution;
  /* The minimum point of the input mesh's axis-aligned bounding box. */
  MPoint minPoint, maxPoint;
  /* The maximum point of the input mesh's axis-aligned bounding box. */
  int voxelCountX, voxelCountY, voxelCountZ;
  /* The name of the mesh created. */
  std::string meshName;
  /* The global index of mesh names created by this plugin. */
  static int voxelMeshNameIndex;
  /* Calculates the minimum and maximum points of the mesh's axis-aligned
     bounding-box. minPoint and maxPoint are outputs. */
  MStatus getMinMaxPoints(const MFnMesh &mesh, MPoint &minPoint,
                          MPoint &maxPoint);
  /* Calculates the occupancy of points on a voxel grid the for given mesh.
     voxelPoints is the container for the output. */
  MStatus calculateVoxelGridOccupancy(voxelizer::VoxelPointGrid &voxelPoints,
                                      const MFnMesh &mesh);
  /* Populates arrays for the voxel geometry. voxelPoints is the input.
     vertexArray, polygonCounts, polygonConnects are containers for the
     output. */
  void createVoxelGeometryArrays(MPointArray &vertexArray,
                                 MIntArray &polygonCounts,
                                 MIntArray &polygonConnects,
                                 const voxelizer::VoxelPointGrid &voxelPoints);
  /* Appends the geometry of a single voxel to the given arrays. */
  void addVoxel(MPointArray &vertexArray, MIntArray &polygonCounts,
                MIntArray &polygonConnects, MPoint minPoint, double resolution);
  /* Sets the normals for the created voxel mesh. */
  MStatus setNormals();
  /* Sets the material for the created voxel mesh. */
  MStatus setMaterial();
  /* Gets the name of the next voxel mesh created by this command. */
  static std::string getNextMeshName();
};
}; // namespace voxelizer

#endif
