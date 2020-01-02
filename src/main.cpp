#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>

#include "voxelizer.hpp"

MStatus initializePlugin(MObject obj) {
    MFnPlugin plugin(obj, "Voxelizer", "1.0", "Any");

    MStatus status = plugin.registerCommand("voxelize",
                                voxelizer::Voxelizer::creator,
                                voxelizer::Voxelizer::createSyntax);
    if (!status)
    {
        status.perror("registerCommand");
    }

    return status;
}

MStatus uninitializePlugin(MObject obj) {
    MFnPlugin plugin(obj);

    MStatus status = plugin.deregisterCommand("voxelize");
    if (!status)
    {
        status.perror("deregisterCommand");
    }

    return status;
}
