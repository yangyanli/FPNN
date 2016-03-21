#pragma once
#ifndef OSG_UTILITY_H
#define OSG_UTILITY_H

#include <osg/Node>
#include <osg/Array>
#include <osg/ref_ptr>
#include <osgViewer/View>
#include <osg/BoundingBox>
#include "point_intersector.h"
#include <osgViewer/ViewerEventHandlers>

namespace osg {
class Geode;
class LineSegment;
}

namespace OSGUtility {
void applyShaderFancy(osg::Node* node);

void applyShaderNormal(osg::Node* node);

osg::Geode* drawCylinder(const osg::LineSegment& axis, double thickness, const osg::Vec4& color);
osg::Geode* drawCylinder(const osg::Vec3& base, const osg::Vec3& top, double thickness, const osg::Vec4& color);
osg::Geode* drawCylinders(const osg::Vec3Array* bases, const osg::Vec3Array* tops, const std::vector<double>& radii, const osg::Vec4Array* colors,
    int subdivide_level = 2);

osg::Geode* drawCone(const osg::LineSegment& axis, double radius, const osg::Vec4& color);
osg::Geode* drawCone(const osg::Vec3& base, const osg::Vec3& top, double radius, const osg::Vec4& color);

osg::Group* drawArrow(const osg::Vec3& base, const osg::Vec3& top, double radius, double head_length, const osg::Vec4& color);

osg::Geode* drawVertex(const osg::Vec3& center, double thickness, const osg::Vec4& color);
osg::Geode* drawSphere(const osg::Vec3& center, double radius, const osg::Vec4& color);
osg::Geode* drawSpheres(const osg::Vec3Array* centers, const std::vector<double>& radii, const osg::Vec4Array* colors, int subdivide_level = 2);
osg::Geode* drawSpheres(const osg::Vec3Array* centers, double radius, const osg::Vec4Array* colors, int subdivide_level = 2);

osg::Geode* drawDisk(const osg::Vec3& center, double radius, const osg::Vec3& normal, const osg::Vec4& color);

osg::Geode* drawBox(const osg::Vec3& center, double width, const osg::Vec4& color);
osg::Geode* drawBox(const osg::Vec3& center, double length_x, double length_y, double length_z, const osg::Vec4& color);
osg::Geode* drawTetrahedron(const osg::Vec3& center, double radius, const osg::Vec4& color);
osg::Geode* drawPolygon(osg::Vec3Array* polygon, const osg::Vec4& color);
osg::Geode* drawBBox(osg::BoundingBox* bbox, double thickness, const osg::Vec4& color);

void sampleOnSphere(osg::Vec3Array* vertices, osg::Vec3Array* normals, int subdivide_level = 2);

void computeNodePathToRoot(osg::Node& node, osg::NodePath& np);

template<class T>
T* computeIntersection(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osgUtil::LineSegmentIntersector::Intersection& intersection,
    osg::NodePath& node_path) {
  T* intersection_object = nullptr;

  osgUtil::LineSegmentIntersector::Intersections intersections;

  float x = ea.getX();
  float y = ea.getY();

  if (!view->computeIntersections(x, y, intersections)) {
    return intersection_object;
  }

  intersection = *(intersections.begin());
  node_path = intersection.nodePath;

  while (!node_path.empty()) {
    intersection_object = dynamic_cast<T*>(node_path.back());
    if (intersection_object != nullptr) {
      break;
    }
    node_path.pop_back();
  }

  return intersection_object;
}

template<class T>
T* computePointIntersection(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osgUtil::LineSegmentIntersector::Intersection& intersection,
    osg::NodePath& node_path) {
  T* intersection_object = nullptr;

  float x = ea.getX();
  float y = ea.getY();

  osg::ref_ptr<PointIntersector> intersector = new PointIntersector(osgUtil::Intersector::WINDOW, x, y);
  osgUtil::IntersectionVisitor intersection_visitor(intersector.get());
  view->getCamera()->accept(intersection_visitor);

  if (!intersector->containsIntersections())
    return intersection_object;

  intersection = *(intersector->getIntersections().begin());
  node_path = intersection.nodePath;

  while (!node_path.empty()) {
    intersection_object = dynamic_cast<T*>(node_path.back());
    if (intersection_object != nullptr) {
      break;
    }
    node_path.pop_back();
  }

  return intersection_object;
}
}
;

#endif // OSG_UTILITY_H
