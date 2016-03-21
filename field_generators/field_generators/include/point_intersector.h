#ifndef POINT_INTERSECTOR_H_
#define POINT_INTERSECTOR_H_

#include <osgUtil/LineSegmentIntersector>

class PointIntersector: public osgUtil::LineSegmentIntersector {
public:
  PointIntersector();
  PointIntersector(const osg::Vec3& start, const osg::Vec3& end);
  PointIntersector(CoordinateFrame cf, double x, double y);

  void setPickBias(float bias) {
    _pick_bias = bias;
  }
  float getPickBias() const {
    return _pick_bias;
  }

  virtual Intersector* clone(osgUtil::IntersectionVisitor& iv);
  virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable);

protected:
  virtual ~PointIntersector() {
  }
  float _pick_bias;
};

#endif //POINT_INTERSECTOR_H_
