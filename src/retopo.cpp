#include <iostream>
#include <limits>

#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>

DeclareSimpleCommand(Retopo, "Jeremy Berchtold", "2019");

double squareDistanceBetweenPoints(const MPoint& a, const MPoint& b) {
  return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z);
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
  MObject component;
  selectionListIter.getDagPath(dagPath, component);

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
  auto vertices = new double[vertexArray.length()][4];
  vertexArray.get(vertices);
  for (unsigned int i = 0; i < vertexArray.length(); i++) {
    MPoint p(vertices[i][0], vertices[i][1], vertices[i][2], vertices[i][3]);
    if (p.x < minPoint.x) minPoint.x = p.x;
    if (p.x > maxPoint.x) maxPoint.x = p.x;
    if (p.y < minPoint.y) minPoint.y = p.y;
    if (p.y > maxPoint.y) maxPoint.y = p.y;
    if (p.z < minPoint.z) minPoint.z = p.z;
    if (p.z > maxPoint.z) maxPoint.z = p.z;
  }
  delete[] vertices;

  std::cout << "(" << minPoint.x << ", " << minPoint.y << ", " << minPoint.z << ")" << std::endl;
  std::cout << "(" << maxPoint.x << ", " << maxPoint.y << ", " << maxPoint.z << ")" << std::endl;

  double resolution = 0.1;

  int voxelCountX = 1 + (maxPoint.x - minPoint.y) / resolution;
  int voxelCountY = 1 + (maxPoint.x - minPoint.y) / resolution;
  int voxelCountZ = 1 + (maxPoint.x - minPoint.y) / resolution;
  auto voxelPoints = new bool[voxelCountX][voxelCountY][voxelCountZ];

  for (int x = 0; x < voxelCountX; x++) {
    for (int x = 0; x < voxelCountX; x++) {
      for (int x = 0; x < voxelCountX; x++) {
        MPoint p(
          x*resolution + minPoint.x,
          y*resolution + minPoint.y,
          z*resolution + minPoint.z);
        MPoint closestPoint;
        status = mesh.getClosestPoint(p, closestPoint);
        voxelPoints[squareDistanceBetweenPoints(p, closestPoint) < resolution*resolution) {
      }
    }
  }

  delete[] voxelPoints;

  std::cout << "Polygon count: " << mesh.numPolygons() << std::endl;

  return status;
}
