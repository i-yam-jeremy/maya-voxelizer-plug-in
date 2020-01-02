#include "voxelizer.hpp"

#include <iostream>
#include <limits>
#include <cmath>

#include <maya/MIOStream.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MDGModifier.h>
#include <maya/MVector.h>
#include <maya/MTypes.h>
#include <maya/MArgList.h>

#include "voxelpointgrid.hpp"

int voxelizer::Voxelizer::voxelMeshNameIndex = 1;

voxelizer::Voxelizer::Voxelizer() {
  this->meshName = getNextMeshName();
}

MStatus voxelizer::Voxelizer::doIt(const MArgList& args) {
  double resolution;
  MStatus status = args.get(0, resolution);

  MSelectionList selectionList;
  status = MGlobal::getActiveSelectionList(selectionList);
  MItSelectionList selectionListIter(selectionList);
  selectionListIter.setFilter(MFn::kMesh);

  if (selectionList.length() != 1) {
    std::cout << "Must select exactly one object." << std::endl;
  }

  MDagPath dagPath;
  selectionListIter.getDagPath(dagPath);

  MFnMesh mesh(dagPath, &status);
  if (status == MS::kFailure) {
    return status;
  }

  MPoint maxPoint;
  MPoint minPoint;
  status = getMinMaxPoints(mesh, minPoint, maxPoint);

  int voxelCountX = 1 + (maxPoint.x - minPoint.x) / resolution;
  int voxelCountY = 1 + (maxPoint.y - minPoint.y) / resolution;
  int voxelCountZ = 1 + (maxPoint.z - minPoint.z) / resolution;
  voxelizer::VoxelPointGrid voxelPoints(voxelCountX, voxelCountY, voxelCountZ);

  for (int x = 0; x < voxelCountX; x++) {
    for (int y = 0; y < voxelCountY; y++) {
      for (int z = 0; z < voxelCountZ; z++) {
        MPoint p(
          x*resolution + minPoint.x,
          y*resolution + minPoint.y,
          z*resolution + minPoint.z);
        MPoint closestPoint;
        status = mesh.getClosestPoint(p, closestPoint);
        voxelPoints[x][y][z] =
          (std::abs(p.x - closestPoint.x) < resolution) &&
          (std::abs(p.y - closestPoint.y) < resolution) &&
          (std::abs(p.z - closestPoint.z) < resolution);
      }
    }
  }

  // Add voxel cubes
  MPointArray vertexArray;
  MIntArray polygonCounts;
  MIntArray polygonConnects;
  for (int x = 0; x < voxelCountX; x++) {
    for (int y = 0; y < voxelCountY; y++) {
      for (int z = 0; z < voxelCountZ; z++) {
        if (voxelPoints[x][y][z]) {
          MPoint p(
            x*resolution + minPoint.x,
            y*resolution + minPoint.y,
            z*resolution + minPoint.z);
          addVoxel(
            vertexArray,
            polygonCounts,
            polygonConnects,
            p,
            resolution);
        }
      }
    }
  }

  MObject meshTransformObj = dgModifier.createNode("transform");
  MFnDependencyNode dpNode(meshTransformObj);
  dpNode.setName("meshTransform");

  MFnMesh newMesh;
  newMesh.create(vertexArray.length(), polygonCounts.length(), vertexArray, polygonCounts, polygonConnects, meshTransformObj, &status);
  newMesh.setName(meshName.c_str());

  status = setNormals();
  status = setMaterial();

  status = dgModifier.doIt();

  return status;
}

MStatus voxelizer::Voxelizer::redoIt() {
  return dgModifier.doIt();
}

MStatus voxelizer::Voxelizer::undoIt() {
  return dgModifier.undoIt();
}

bool voxelizer::Voxelizer::isUndoable() const {
  return true;
}

MSyntax voxelizer::Voxelizer::createSyntax() {
    MSyntax syntax;

    syntax.setObjectType(MSyntax::kStringObjects, 1, 1);

    syntax.enableQuery(false);
    syntax.enableEdit(false);

    return syntax;
}

void* voxelizer::Voxelizer::creator() {
  return new voxelizer::Voxelizer();
}

MStatus voxelizer::Voxelizer::getMinMaxPoints(const MFnMesh& mesh, MPoint& minPoint, MPoint& maxPoint) {
  double inf = std::numeric_limits<double>::infinity();
  double negInf = -std::numeric_limits<double>::infinity();
  maxPoint.x = maxPoint.y = maxPoint.z = negInf;
  minPoint.x = minPoint.y = minPoint.z = inf;

  MPointArray vertexArray;
  MStatus status = mesh.getPoints(vertexArray);
  if (status != MS::kSuccess) return status;
  for (unsigned int i = 0; i < vertexArray.length(); i++) {
    MPoint p = vertexArray[i];
    if (p.x < minPoint.x) minPoint.x = p.x;
    if (p.x > maxPoint.x) maxPoint.x = p.x;
    if (p.y < minPoint.y) minPoint.y = p.y;
    if (p.y > maxPoint.y) maxPoint.y = p.y;
    if (p.z < minPoint.z) minPoint.z = p.z;
    if (p.z > maxPoint.z) maxPoint.z = p.z;
  }

  return status;
}

void voxelizer::Voxelizer::addVoxel(MPointArray& vertexArray, MIntArray& polygonCounts, MIntArray& polygonConnects, MPoint minPoint, double resolution) {
  MPoint vertices[] = {
    minPoint,
    MPoint(minPoint.x, minPoint.y, minPoint.z+resolution),
    MPoint(minPoint.x, minPoint.y+resolution, minPoint.z),
    MPoint(minPoint.x, minPoint.y+resolution, minPoint.z+resolution),
    MPoint(minPoint.x+resolution, minPoint.y, minPoint.z),
    MPoint(minPoint.x+resolution, minPoint.y, minPoint.z+resolution),
    MPoint(minPoint.x+resolution, minPoint.y+resolution, minPoint.z),
    MPoint(minPoint.x+resolution, minPoint.y+resolution, minPoint.z+resolution),
  };

  int startVertexIndex = vertexArray.length();

  for (MPoint vertex : vertices) {
    vertexArray.append(MPoint(vertex.x - resolution/2, vertex.y - resolution/2, vertex.z - resolution/2, vertex.w));
  }

  int faceIndices[][4] = {
    {0, 1, 3, 2},
    {4, 5, 7, 6},
    {0, 1, 5, 4},
    {2, 3, 7, 6},
    {0, 2, 6, 4},
    {1, 3, 7, 5}
  };

  for (int i = 0; i < 6; i++) {
    polygonCounts.append(4);
    for (int j = 0; j < 4; j++) {
      polygonConnects.append(startVertexIndex+faceIndices[i][j]);
    }
  }

}

MStatus voxelizer::Voxelizer::setNormals() {
  MStatus status = dgModifier.commandToExecute("select -cl");
  status = dgModifier.commandToExecute(("select " + meshName).c_str());
  status = dgModifier.commandToExecute("polySetToFaceNormal");
  status = dgModifier.commandToExecute("polyNormal -nm 2"); // Conform Normals
  status = dgModifier.commandToExecute("polySetToFaceNormal");
  status = dgModifier.commandToExecute(("select -d " + meshName).c_str());
  return status;
}

MStatus voxelizer::Voxelizer::setMaterial() {
  return dgModifier.commandToExecute((
    "string $shader = `shadingNode -asShader lambert`;"
    "select " + meshName + "; hyperShade -assign $shader;"
    "select -cl; hyperShade -objects $shader;"
    "string $lambert = `createNode lambert`;"
    "select lambert1 $lambert; hyperShade -objects \"\"").c_str());
}

std::string voxelizer::Voxelizer::getNextMeshName() {
  std::string meshName = "voxelizerMesh";
  meshName += std::to_string(voxelMeshNameIndex);
  voxelMeshNameIndex++;
  return meshName;
}
