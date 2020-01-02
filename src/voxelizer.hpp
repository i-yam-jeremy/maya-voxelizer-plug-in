#ifndef __VOXELIZER_HPP__
#define __VOXELIZER_HPP__

#include <maya/MPxCommand.h>

namespace voxelizer {
  class Voxelizer: public MPxCommand {
  public:
    Voxelizer();
    ~Voxelizer() override;
    MStatus doIt(const MArgList& args) override;
    static MSyntax createSyntax();
    static void* creator();
  private:
    static int voxelMeshNameIndex;
  };
};

#endif
