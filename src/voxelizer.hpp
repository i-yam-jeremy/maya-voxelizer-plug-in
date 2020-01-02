#ifndef __VOXELIZER_HPP__
#define __VOXELIZER_HPP__

#include <string>

#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MSyntax.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>

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
    MDGModifier dgModifier;
    std::string meshName;
    static int voxelMeshNameIndex;

    MStatus getMinMaxPoints(const MFnMesh& mesh, MPoint& minPoint, MPoint& maxPoint);
    void addVoxel(MPointArray& vertexArray, MIntArray& polygonCounts, MIntArray& polygonConnects, MPoint minPoint, double resolution);
    MStatus setNormals();
    static std::string getNextMeshName();
  };
};

#endif
