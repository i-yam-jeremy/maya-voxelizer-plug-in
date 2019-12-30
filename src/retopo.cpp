#include <iostream>
#include <limits>

#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>

#include "voxelpointgrid.hpp"

DeclareSimpleCommand(Retopo, "Jeremy Berchtold", "2019");

double squareDistanceBetweenPoints(const MPoint& a, const MPoint& b) {
  return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z);
}

MStatus addVoxel(MFnMesh& mesh, MPoint minPoint, double resolution) {
  MPoint v[] = {
    minPoint,
    MPoint(minPoint.x, minPoint.y, minPoint.z+resolution),
    MPoint(minPoint.x, minPoint.y+resolution, minPoint.z),
    MPoint(minPoint.x, minPoint.y+resolution, minPoint.z+resolution),
    MPoint(minPoint.x+resolution, minPoint.y, minPoint.z),
    MPoint(minPoint.x+resolution, minPoint.y, minPoint.z+resolution),
    MPoint(minPoint.x+resolution, minPoint.y+resolution, minPoint.z),
    MPoint(minPoint.x+resolution, minPoint.y+resolution, minPoint.z+resolution),
  };

  MPoint faceVerts[][4] = {
    {v[0],v[1],v[3],v[2]},
    {v[4],v[5],v[7],v[6]},
    {v[0],v[1],v[5],v[4]},
    {v[2],v[3],v[7],v[6]},
    {v[0],v[2],v[6],v[4]},
    {v[1],v[3],v[7],v[5]}
  };

  for (int i = 0; i < 6; i++) {
    MStatus status;
    mesh.addPolygon(MPointArray(faceVerts[i], 4), true, 1.0e-10, MObject::kNullObj, &status);
    if (status != MS::kSuccess) return status;
  }

  return MS::kSuccess;
}

MStatus Retopo::doIt(const MArgList& args) {
  MSelectionList selectionList;
  MStatus status = MGlobal::getActiveSelectionList(selectionList);
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

  double inf = std::numeric_limits<double>::infinity();
  double negInf = -std::numeric_limits<double>::infinity();
  MPoint maxPoint(negInf, negInf, negInf);
  MPoint minPoint(inf, inf, inf);

  MPointArray vertexArray;
  status = mesh.getPoints(vertexArray);
  for (unsigned int i = 0; i < vertexArray.length(); i++) {
    MPoint p = vertexArray[i];
    if (p.x < minPoint.x) minPoint.x = p.x;
    if (p.x > maxPoint.x) maxPoint.x = p.x;
    if (p.y < minPoint.y) minPoint.y = p.y;
    if (p.y > maxPoint.y) maxPoint.y = p.y;
    if (p.z < minPoint.z) minPoint.z = p.z;
    if (p.z > maxPoint.z) maxPoint.z = p.z;
  }

  std::cout << "(" << minPoint.x << ", " << minPoint.y << ", " << minPoint.z << ")" << std::endl;
  std::cout << "(" << maxPoint.x << ", " << maxPoint.y << ", " << maxPoint.z << ")" << std::endl;

  double resolution = 0.1;

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
          squareDistanceBetweenPoints(p, closestPoint) < resolution*resolution;
      }
    }
  }

  //Delete existing faces
  std::cout << "Num polygons: " << mesh.numPolygons() << std::endl;
  /*for (int i = 0; i < mesh.numPolygons(); i++) {
    status = mesh.deleteFace(i);
    if (status != MS::kSuccess) return status;
    std::cout << "Polygon count: " << mesh.numPolygons() << std::endl;
  }

  status = mesh.updateSurface();*/

  // Add voxel cubes
  /*for (int x = 1; x < voxelCountX; x++) {
    for (int y = 1; y < voxelCountY; y++) {
      for (int z = 1; z < voxelCountZ; z++) {
        if (voxelPoints[x][y][z]/* && voxelPoints[x-1][y][z] &&
            voxelPoints[x][y-1][z] && voxelPoints[x-1][y-1][z] &&
            voxelPoints[x][y][z-1] && voxelPoints[x-1][y][z-1] &&
            voxelPoints[x][y-1][z-1] && voxelPoints[x-1][y-1][z-1]*//*) {
          MPoint p(
            x*resolution + minPoint.x,
            y*resolution + minPoint.y,
            z*resolution + minPoint.z);
          status = addVoxel(mesh, p, resolution);
          if (status != MS::kSuccess) return status;
        }
      }
    }
  }*/

  std::cout << "Polygon count: " << mesh.numPolygons() << std::endl;

  return status;
}
