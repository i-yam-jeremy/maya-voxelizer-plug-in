#include <iostream>

#include <maya/MIOStream.h>
#include <maya/MSimple.h>

DeclareSimpleCommand(Retopo, "JeremyBerchtold", "2019");

MStatus Retopo::doIt(const MArgList& args) {
  std::cout << "Retopo executed." << std::endl;
  return MS::kSuccess;
}
