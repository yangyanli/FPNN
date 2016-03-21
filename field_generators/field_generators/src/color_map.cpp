#include "color_map.h"

static const double continuous[64][3] = { 0, 0, 0.5625, 0, 0, 0.6250, 0, 0, 0.6875, 0, 0, 0.7500, 0, 0, 0.8125, 0, 0, 0.8750, 0, 0, 0.9375, 0, 0, 1.0000, 0,
    0.0625, 1.0000, 0, 0.1250, 1.0000, 0, 0.1875, 1.0000, 0, 0.2500, 1.0000, 0, 0.3125, 1.0000, 0, 0.3750, 1.0000, 0, 0.4375, 1.0000, 0, 0.5000, 1.0000, 0,
    0.5625, 1.0000, 0, 0.6250, 1.0000, 0, 0.6875, 1.0000, 0, 0.7500, 1.0000, 0, 0.8125, 1.0000, 0, 0.8750, 1.0000, 0, 0.9375, 1.0000, 0, 1.0000, 1.0000, 0.0625,
    1.0000, 0.9375, 0.1250, 1.0000, 0.8750, 0.1875, 1.0000, 0.8125, 0.2500, 1.0000, 0.7500, 0.3125, 1.0000, 0.6875, 0.3750, 1.0000, 0.6250, 0.4375, 1.0000,
    0.5625, 0.5000, 1.0000, 0.5000, 0.5625, 1.0000, 0.4375, 0.6250, 1.0000, 0.3750, 0.6875, 1.0000, 0.3125, 0.7500, 1.0000, 0.2500, 0.8125, 1.0000, 0.1875,
    0.8750, 1.0000, 0.1250, 0.9375, 1.0000, 0.0625, 1.0000, 1.0000, 0, 1.0000, 0.9375, 0, 1.0000, 0.8750, 0, 1.0000, 0.8125, 0, 1.0000, 0.7500, 0, 1.0000,
    0.6875, 0, 1.0000, 0.6250, 0, 1.0000, 0.5625, 0, 1.0000, 0.5000, 0, 1.0000, 0.4375, 0, 1.0000, 0.3750, 0, 1.0000, 0.3125, 0, 1.0000, 0.2500, 0, 1.0000,
    0.1875, 0, 1.0000, 0.1250, 0, 1.0000, 0.0625, 0, 1.0000, 0, 0, 0.9375, 0, 0, 0.8750, 0, 0, 0.8125, 0, 0, 0.7500, 0, 0, 0.6875, 0, 0, 0.6250, 0, 0, 0.5625,
    0, 0, 0.5000, 0, 0 };

static const double discrete[20][3] = { 0.7500, 0.9400, 0.6000, 0.5804, 0.0000, 0.8275, 0.2000, 0.2000, 0.8000, 0.1000, 0.0000, 0.5020, 0.0000, 0.3569, 0.5020,
    0.3750, 0.5625, 0.0000, 0.0000, 0.6100, 0.5800, 0.8600, 0.0000, 0.3500, 0.6900, 0.3725, 0.2353, 0.0000, 1.0000, 0.4980, 0.0000, 0.3922, 0.0000, 0.8627,
    0.0784, 0.2353, 0.7216, 0.5255, 0.0431, 1.0000, 0.2706, 0.0000, 0.9255, 0.3255, 0.6588, 0.1600, 0.4000, 0.7215, 0.5852, 0.4117, 0.9960, 0.0230, 0.8627,
    0.9843, 1.0000, 0.5700, 0.0000, 0.8000, 0.0000, 0.2000 };

ColorMap::ColorMap() {
  for (int i = 0, iEnd = 64; i < iEnd; ++i) {
    continuous_.push_back(osg::Vec4(continuous[i][0], continuous[i][1], continuous[i][2], 1.0f));
  }

  for (int i = 0, iEnd = 20; i < iEnd; ++i) {
    discrete_.push_back(osg::Vec4(discrete[i][0], discrete[i][1], discrete[i][2], 1.0f));
  }

  light_blue_ = osg::Vec4(0.75f, 0.94f, 1.0f, 1.0f);
  return;
}

const osg::Vec4& ColorMap::getNamedColor(NamedColor named_color) {
  switch (named_color) {
  case (LIGHT_BLUE):
    return light_blue_;
    break;
  default:
    return light_blue_;
    break;
  }

  return light_blue_;
}

osg::Vec4 ColorMap::getContinusColor(float value, float low, float high, bool interpolate) {
  int color_count = continuous_.size();

  if (low > high) {
    std::swap(low, high);
  }

  if (interpolate) {
    float index = std::abs((value - low) / (high - low) * (color_count - 1));
    int index_low = std::floor(index);
    int index_high = std::ceil(index);

    if (index_low < 0)
      return continuous_[0];
    if (index_high >= color_count)
      return continuous_[color_count - 1];
    if (index_low == index_high)
      return continuous_[index_low];

    osg::Vec4 color = continuous_[index_low] * (index_high - index) + continuous_[index_high] * (index - index_low);
    return color;
  } else {
    int index = std::abs((value - low) / (high - low) * (color_count - 1));
    if (index >= color_count)
      index = color_count - 1;

    return continuous_[index];
  }
}

const osg::Vec4& ColorMap::getDiscreteColor(int idx) {
  idx = idx % discrete_.size();
  if (idx < 0)
    idx += discrete_.size();

  return discrete_[idx];
}
