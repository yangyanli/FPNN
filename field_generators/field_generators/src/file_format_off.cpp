#include "mesh_io.h"
#include "file_format_off.h"

FileFormatOff::FileFormatOff(void) {

}

FileFormatOff::~FileFormatOff(void) {

}

int FileFormatOff::addVertex(double x, double y, double z, double r, double g, double b, double a, void *userData) {
  OffFileContent *ofc = (OffFileContent*) userData;
  ofc->vertices->push_back(osg::Vec3d(x, y, z));
  ofc->colors->push_back(osg::Vec4d(r, g, b, a));

  return 0;
}

int FileFormatOff::startFace(void *userData) {
  OffFileContent *ofc = (OffFileContent*) userData;
  ofc->faces.push_back(OffFace());
  return 0;
}

int FileFormatOff::addToFace(size_t v, void *userData) {
  OffFileContent *ofc = (OffFileContent*) userData;
  OffFaceVertex fv = { v };
  ofc->faces.rbegin()->facevertices.push_back(fv);
  return 0;
}

int FileFormatOff::endFace(double r, double g, double b, double a, void *userData) {
  OffFileContent *ofc = (OffFileContent*) userData;
  ofc->faces.rbegin()->color = osg::Vec4(r, g, b, a);
  return 0;
}
