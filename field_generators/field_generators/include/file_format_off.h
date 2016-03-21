#pragma once
#ifndef FILE_FORMAT_OFF_H
#define FILE_FORMAT_OFF_H

#include <string>
#include <osg/Vec3d>
#include <osg/Vec4>
#include <osg/Array>

struct OffFaceVertex {
  size_t vertex_idx;
};

typedef std::vector<OffFaceVertex> OffFaceVertexVector;

struct OffFace {
  OffFace() :
      color(0.0, 0.0, 0.0, 1.0) {
  }
  OffFaceVertexVector facevertices;
  osg::Vec4d color;
};

typedef std::vector<OffFace> OffFaceVector;

struct OffFileContent {
  OffFileContent(void) :
      vertices(new osg::Vec3dArray), colors(new osg::Vec4dArray) {
  }
  virtual ~OffFileContent(void) {
  }

  std::string filename;

  osg::ref_ptr<osg::Vec3dArray> vertices;
  osg::ref_ptr<osg::Vec4dArray> colors;

  OffFaceVector faces;
};

class FileFormatOff {
public:
  FileFormatOff(void);
  virtual ~FileFormatOff(void);

  // off
  static int addVertex(double x, double y, double z, double r, double g, double b, double a, void *userData);
  static int startFace(void *userData);
  static int addToFace(size_t v, void *userData);
  static int endFace(double r, double g, double b, double a, void *userData);
};

#endif // FILE_FORMAT_OFF_H
