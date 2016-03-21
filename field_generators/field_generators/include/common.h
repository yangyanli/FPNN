#pragma once
#ifndef COMMON_H_
#define COMMON_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <osg/Vec3>
#include <cgal_types.h>

enum class PointCloudColorMode {
  ORIGINAL, UNIFORM
};

enum class PickMode {
  CTRL, SHIFT, ALT, UNKNOWN
};

namespace Common {
std::string int2String(int i, int width);
void randomK(std::vector<int>& random_k, int k, int N);
}

#endif // COMMON_H_
