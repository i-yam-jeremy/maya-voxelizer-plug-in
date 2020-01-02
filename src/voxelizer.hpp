#ifndef __VOXELIZER_HPP__
#define __VOXELIZER_HPP__

#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MSyntax.h>

namespace voxelizer {
  class Voxelizer: public MPxCommand {
  public:
    Voxelizer();
    MStatus doIt(const MArgList& args) override;
    MStatus redoIt() override;
    MStatus undoIt() override;
    static MSyntax createSyntax();
    static void* creator();
  private:
    MDGModifier dgModifier;
    static int voxelMeshNameIndex;
  };
};

#endif
