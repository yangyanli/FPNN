#define _USE_MATH_DEFINES
#include <cmath>
#include <random>

#include <CGAL/intersections.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/linear_least_squares_fitting_3.h>

#include "cgal_utility.h"

namespace CGALUtility {
void projectPointsToPlane(const std::vector<CgalPoint>& points, const CgalPlane& plane, std::vector<CgalPoint2>& coords_2d) {
  CgalPoint origin = plane.projection(points[0]);
  CgalVector base1 = plane.base1();
  CgalVector base2 = plane.base2();
  base1 = base1 / std::sqrt(base1.squared_length());
  base2 = base2 / std::sqrt(base2.squared_length());

  CgalLine base_line1(origin, base1);
  CgalLine base_line2(origin, base2);

  for (size_t i = 0, iEnd = points.size(); i < iEnd; ++i) {
    CgalPoint point = plane.projection(points[i]);
    CgalVector x_vector(origin, base_line1.projection(point));
    CgalVector y_vector(origin, base_line2.projection(point));
    double x = std::sqrt(x_vector.squared_length());
    double y = std::sqrt(y_vector.squared_length());
    x = x_vector * base1 < 0 ? -x : x;
    y = y_vector * base2 < 0 ? -y : y;
    coords_2d.push_back(CgalPoint2(x, y));
  }

  return;
}

void computeConvexHull(const std::vector<CgalPoint>& points, std::vector<CgalPoint>& convex_hull) {
  CgalPlane plane;
  CGAL::linear_least_squares_fitting_3(points.begin(), points.end(), plane, CGAL::Dimension_tag<0>());

  computeConvexHull(points, plane, convex_hull);
}

void computeConvexHull(const std::vector<CgalPoint2>& points, std::vector<CgalPoint2>& convex_hull) {
  CGAL::convex_hull_2(points.begin(), points.end(), std::back_inserter(convex_hull));

  return;
}

bool pointOutsidePolygon(const CgalPoint2& point, const std::vector<CgalPoint2>& points) {
  return (CGAL::bounded_side_2(points.begin(), points.end(), point) == CGAL::ON_UNBOUNDED_SIDE);
}

void computeConvexHull(const std::vector<CgalPoint>& points, const CgalPlane& plane, std::vector<CgalPoint>& convex_hull) {
  std::vector<CgalPoint2> coords_2d;
  projectPointsToPlane(points, plane, coords_2d);

  std::vector<CgalPoint2> convex_hull_2d;
  CGAL::convex_hull_2(coords_2d.begin(), coords_2d.end(), std::back_inserter(convex_hull_2d));

  convex_hull.clear();
  CgalPoint origin = plane.projection(points[0]);
  CgalVector base1 = plane.base1();
  CgalVector base2 = plane.base2();
  base1 = base1 / std::sqrt(base1.squared_length());
  base2 = base2 / std::sqrt(base2.squared_length());
  for (size_t i = 0, iEnd = convex_hull_2d.size(); i < iEnd; ++i) {
    double x = convex_hull_2d[i].x();
    double y = convex_hull_2d[i].y();
    CgalPoint point = origin + x * base1 + y * base2;

    convex_hull.push_back(point);
  }

  return;
}

double computeSegment(const std::vector<CgalPoint>& points, CgalSegment& segment) {
  CgalLine line;
  double fitting_score = CGAL::linear_least_squares_fitting_3(points.begin(), points.end(), line, CGAL::Dimension_tag<0>());

  computeSegment(points, line, segment);

  return fitting_score;
}

double computeSegment(const std::vector<CgalPoint>& points, const CgalPoint& centroid, CgalSegment& segment) {
  CgalLine line;

  // assemble covariance matrix
  double covariance[6];
  covariance[0] = covariance[1] = covariance[2] = covariance[3] = covariance[4] = covariance[5] = 0.0;
  for (std::vector<CgalPoint>::const_iterator it = points.begin(); it != points.end(); it++) {
    CgalVector d = *it - centroid;
    covariance[0] += d.x() * d.x();
    covariance[1] += d.x() * d.y();
    covariance[2] += d.y() * d.y();
    covariance[3] += d.x() * d.z();
    covariance[4] += d.y() * d.z();
    covariance[5] += d.z() * d.z();
  }

  // compute fitting line
  double eigen_values[3];
  double eigen_vectors[9];
  double fitting_score = 0.0;

  CGAL::internal::eigen_symmetric<double>(covariance, 3, eigen_vectors, eigen_values);

  if (eigen_values[0] == eigen_values[1] && eigen_values[0] == eigen_values[2]) {
    // assemble a default line along x axis which goes
    // through the centroid.
    line = CgalLine(centroid, CgalVector(1.0, 0.0, 0.0));
  } else {
    // regular case
    CgalVector direction(eigen_vectors[0], eigen_vectors[1], eigen_vectors[2]);
    line = CgalLine(centroid, direction);
    fitting_score = 1.0 - eigen_values[1] / eigen_values[0];
  }

  computeSegment(points, line, segment);

  return fitting_score;
}

void computeSegment(const std::vector<CgalPoint>& points, const CgalLine& line, CgalSegment& segment) {
  std::pair<CgalPoint, double> min_point;
  min_point.second = std::numeric_limits<double>::max();

  std::pair<CgalPoint, double> max_point;
  max_point.second = std::numeric_limits<double>::min();

  CgalPoint anchor = line.projection(points[0]);
  min_point.first = max_point.first = anchor;

  CgalVector normal = line.to_vector();
  for (size_t i = 0, i_end = points.size(); i < i_end; ++i) {
    CgalPoint projection = line.projection(points[i]);
    double distance = normal * (projection - anchor);
    if (distance < min_point.second) {
      min_point.first = projection;
      min_point.second = distance;
    }

    if (distance > max_point.second) {
      max_point.first = projection;
      max_point.second = distance;
    }
  }

  segment = CgalSegment(min_point.first, max_point.first);

  return;
}

CgalVector normalize(CgalVector vector) {
  double length = vector.squared_length();
  if (length == 0)
    return vector;

  vector = vector / std::sqrt(length);
  return vector;
}

bool testSegmentDiskIntersection(const CgalSegment& segment, const CgalPoint& center, const CgalVector& normal, double radius) {
  CgalPlane plane(center, normal);

  if (plane.oriented_side(segment.source()) == plane.oriented_side(segment.target()))
    return false;

  double distance_threshold = radius * radius;
  CGAL::Object intersection = CGAL::intersection(segment, plane);
  if (const CgalPoint* intersection_point = CGAL::object_cast < CgalPoint > (&intersection)) {
    double distance = CGAL::squared_distance(center, *intersection_point);
    if (distance < distance_threshold)
      return true;
    return false;
  } else if (const CgalSegment* intersection_segment = CGAL::object_cast < CgalSegment > (&intersection)) {
    double source_distance = CGAL::squared_distance(center, intersection_segment->source());
    double target_distance = CGAL::squared_distance(center, intersection_segment->target());
    if (source_distance < distance_threshold || target_distance < distance_threshold)
      return true;
    return false;
  }

  return false;
}

double distanceToDisk(const CgalPoint& point, const CgalPoint& center, const CgalVector& normal, double radius) {
  CgalPlane plane(center, normal);
  CgalPoint projection = plane.projection(point);

  double distance = 0;
  double distance_to_plane = CGAL::squared_distance(point, projection);
  double projection_distance_to_center = CGAL::squared_distance(projection, center);
  if (projection_distance_to_center <= radius * radius)
    distance = std::sqrt(distance_to_plane);
  else {
    double distance_to_boundary = std::sqrt(projection_distance_to_center) - radius;
    distance = std::sqrt(distance_to_plane + distance_to_boundary * distance_to_boundary);
  }

  return distance;
}

CgalPoint computeCenter(const std::vector<CgalPoint>& points) {
  CgalVector center(0.0, 0.0, 0.0);
  for (size_t i = 0, i_end = points.size(); i < i_end; ++i)
    center = center + Vec3Caster<CgalPoint, CgalVector>(points[i]);
  center = center / (double) (points.size());

  return Vec3Caster<CgalVector, CgalPoint>(center);
}

CgalVector randomNormal(void) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-1.0, 0.999999);
  //http://www.jasondavies.com/maps/random-points/
  double theta = M_PI * dis(gen);
  double x = (dis(gen) + 1.0) / 2;
  double varphi = acos(2 * x - 1);

  return CgalVector(sin(varphi) * cos(theta), sin(varphi) * sin(theta), cos(varphi));
}
}
;
