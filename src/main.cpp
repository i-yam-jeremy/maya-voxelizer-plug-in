#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>

#include "voxelizer.hpp"

MStatus initializePlugin(MObject obj) {
  MFnPlugin plugin(obj, "Voxelizer", "1.0", "Any");

  MStatus status =
      plugin.registerCommand("voxelize", voxelizer::Voxelizer::creator,
                             voxelizer::Voxelizer::createSyntax);
  if (!status) {
    MGlobal::displayError("Unable to register 'voxelize' command.");
    return status;
  }

  return status;
}

MStatus uninitializePlugin(MObject obj) {
  MFnPlugin plugin(obj);

  MStatus status = plugin.deregisterCommand("voxelize");
  if (!status) {
    MGlobal::displayError("Unable to deregister 'voxelize' command.");
    return status;
  }

  return status;
}
