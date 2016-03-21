#pragma once
#ifndef CGAL_UTILITY_H_
#define CGAL_UTILITY_H_

#include "cgal_types.h"

namespace CGALUtility {
void projectPointsToPlane(const std::vector<CgalPoint>& points, const CgalPlane& plane, std::vector<CgalPoint2>& coords_2d);
void computeConvexHull(const std::vector<CgalPoint>& points, std::vector<CgalPoint>& convex_hull);
void computeConvexHull(const std::vector<CgalPoint>& points, const CgalPlane& plane, std::vector<CgalPoint>& convex_hull);
void computeConvexHull(const std::vector<CgalPoint2>& points, std::vector<CgalPoint2>& convex_hull);
bool pointOutsidePolygon(const CgalPoint2& point, const std::vector<CgalPoint2>& points);

double computeSegment(const std::vector<CgalPoint>& points, CgalSegment& segment);
double computeSegment(const std::vector<CgalPoint>& points, const CgalPoint& centroid, CgalSegment& segment);
void computeSegment(const std::vector<CgalPoint>& points, const CgalLine& line, CgalSegment& segment);

bool testSegmentDiskIntersection(const CgalSegment& segment, const CgalPoint& center, const CgalVector& normal, double radius);
double distanceToDisk(const CgalPoint& point, const CgalPoint& center, const CgalVector& normal, double radius);

CgalPoint computeCenter(const std::vector<CgalPoint>& points);

CgalVector normalize(CgalVector vector);
CgalVector randomNormal(void);
}
;

#endif // CGAL_UTILITY_H_
