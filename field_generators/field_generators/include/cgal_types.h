#pragma once
#ifndef CGAL_TYPES_H
#define CGAL_TYPES_H

#include <CGAL/Cartesian.h>

typedef CGAL::Cartesian<double> Kernel;
typedef Kernel::Point_2 CgalPoint2;
typedef Kernel::Point_3 CgalPoint;
typedef Kernel::Vector_2 CgalVector2;
typedef Kernel::Vector_3 CgalVector;
typedef Kernel::Line_3 CgalLine;
typedef Kernel::Plane_3 CgalPlane;
typedef Kernel::Segment_3 CgalSegment;
typedef Kernel::Direction_3 CgalDirection;
typedef Kernel::Sphere_3 CgalSphere;
typedef Kernel::Circle_3 CgalCircle;

template<class Vector3_from, class Vector3_to>
class Vec3Caster {
public:
  Vec3Caster(const Vector3_from& v) :
      x_(v.x()), y_(v.y()), z_(v.z()) {
  }
  operator Vector3_to() const {
    return Vector3_to(x_, y_, z_);
  }
private:
  double x_, y_, z_;
};

#endif // CGAL_TYPES_H
