#ifndef __VOXELIZER_HPP__
#define __VOXELIZER_HPP__

#include <string>

#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MSyntax.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>

#include "voxelpointgrid.hpp"

namespace voxelizer {
  class Voxelizer: public MPxCommand {
  public:
    Voxelizer();
    MStatus doIt(const MArgList& args) override;
    MStatus redoIt() override;
    MStatus undoIt() override;
    bool isUndoable() const override;
    static MSyntax createSyntax();
    static void* creator();
  private:
    double resolution;
    MPoint minPoint, maxPoint;
    int voxelCountX, voxelCountY, voxelCountZ;
    MDGModifier dgModifier;
    std::string meshName;
    static int voxelMeshNameIndex;

    MStatus getMinMaxPoints(const MFnMesh& mesh, MPoint& minPoint, MPoint& maxPoint);
    MStatus calculateVoxelGridOccupancy(voxelizer::VoxelPointGrid& voxelPoints, const MFnMesh& mesh);
    void createVoxelGeometryArrays(MPointArray& vertexArray, MIntArray& polygonCounts, MIntArray& polygonConnects, const voxelizer::VoxelPointGrid& voxelPoints);
    void addVoxel(MPointArray& vertexArray, MIntArray& polygonCounts, MIntArray& polygonConnects, MPoint minPoint, double resolution);
    MStatus setNormals();
    MStatus setMaterial();
    static std::string getNextMeshName();
  };
};

#endif
