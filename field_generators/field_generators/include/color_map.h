#pragma once
#ifndef ColorMap_H
#define ColorMap_H

#include <osg/Vec4>
#include <vector>

class ColorMap {
public:
  static ColorMap& getInstance() {
    static ColorMap singleton_;
    return singleton_;
  }

  typedef enum {
    LIGHT_BLUE
  } NamedColor;

  /* more (non-static) functions here */
  const osg::Vec4& getNamedColor(NamedColor named_color);
  const osg::Vec4& getDiscreteColor(int idx);
  osg::Vec4 getContinusColor(float value, float low = 0.0f, float high = 1.0f, bool interpolate = false);

private:
  ColorMap();                            // ctor hidden
  ColorMap(ColorMap const&) {
  }            // copy ctor hidden
  ColorMap& operator=(ColorMap const&) {
  } // assign op. hidden
  virtual ~ColorMap() {
  }                          // dtor hidden

  std::vector<osg::Vec4> continuous_;
  osg::Vec4 light_blue_;
  std::vector<osg::Vec4> discrete_;
};

#endif // ColorMap_H
