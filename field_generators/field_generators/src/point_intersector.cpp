#include <osg/Version>
#include <osg/Geometry>
#include <osg/TemplatePrimitiveFunctor>

#include "point_intersector.h"

PointIntersector::PointIntersector() :
    osgUtil::LineSegmentIntersector(MODEL, 0.0, 0.0), _pick_bias(2.0f) {
}

PointIntersector::PointIntersector(const osg::Vec3& start, const osg::Vec3& end) :
    osgUtil::LineSegmentIntersector(start, end), _pick_bias(2.0f) {
}

PointIntersector::PointIntersector(CoordinateFrame cf, double x, double y) :
    osgUtil::LineSegmentIntersector(cf, x, y), _pick_bias(2.0f) {
}

osgUtil::Intersector* PointIntersector::clone(osgUtil::IntersectionVisitor& iv) {
  if (_coordinateFrame == MODEL && iv.getModelMatrix() == 0) {
    osg::ref_ptr<PointIntersector> cloned = new PointIntersector(_start, _end);
    cloned->_parent = this;
    cloned->_pick_bias = _pick_bias;
    return cloned.release();
  }

  osg::Matrix matrix;
  switch (_coordinateFrame) {
  case WINDOW:
    if (iv.getWindowMatrix())
      matrix.preMult(*iv.getWindowMatrix());
    if (iv.getProjectionMatrix())
      matrix.preMult(*iv.getProjectionMatrix());
    if (iv.getViewMatrix())
      matrix.preMult(*iv.getViewMatrix());
    if (iv.getModelMatrix())
      matrix.preMult(*iv.getModelMatrix());
    break;
  case PROJECTION:
    if (iv.getProjectionMatrix())
      matrix.preMult(*iv.getProjectionMatrix());
    if (iv.getViewMatrix())
      matrix.preMult(*iv.getViewMatrix());
    if (iv.getModelMatrix())
      matrix.preMult(*iv.getModelMatrix());
    break;
  case VIEW:
    if (iv.getViewMatrix())
      matrix.preMult(*iv.getViewMatrix());
    if (iv.getModelMatrix())
      matrix.preMult(*iv.getModelMatrix());
    break;
  case MODEL:
    if (iv.getModelMatrix())
      matrix = *iv.getModelMatrix();
    break;
  }

  osg::Matrix inverse = osg::Matrix::inverse(matrix);
  osg::ref_ptr<PointIntersector> cloned = new PointIntersector(_start * inverse, _end * inverse);
  cloned->_parent = this;
  cloned->_pick_bias = _pick_bias;
  return cloned.release();
}

void PointIntersector::intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable) {
#if OSG_VERSION_LESS_THAN(3,3,0)
  osg::BoundingBox bb = drawable->getBound();
#else
  osg::BoundingBox bb = drawable->getBoundingBox();
#endif
  bb.xMin() -= _pick_bias;
  bb.xMax() += _pick_bias;
  bb.yMin() -= _pick_bias;
  bb.yMax() += _pick_bias;
  bb.zMin() -= _pick_bias;
  bb.zMax() += _pick_bias;

  osg::Vec3d s(_start), e(_end);
  if (!intersectAndClip(s, e, bb))
    return;
  if (iv.getDoDummyTraversal())
    return;

  osg::Geometry* geometry = drawable->asGeometry();
  if (geometry) {
    osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
    if (!vertices)
      return;

    osg::Vec3d dir = e - s;
    double invLength = 1.0 / dir.length();
    for (unsigned int i = 0; i < vertices->size(); ++i) {
      double distance = fabs((((*vertices)[i] - s) ^ dir).length());
      distance *= invLength;
      if (_pick_bias < distance)
        continue;

      Intersection hit;
      hit.ratio = distance;
      hit.nodePath = iv.getNodePath();
      hit.drawable = drawable;
      hit.matrix = iv.getModelMatrix();
      hit.localIntersectionPoint = (*vertices)[i];
      insertIntersection(hit);
    }
  }

  return;
}
