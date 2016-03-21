#pragma once
#ifndef SCALER_H
#define SCALER_H

#include <osgManipulator/Command>
#include <osgManipulator/Scale1DDragger>

#include "renderable.h"

namespace osgManipulator {
class ScaleAxisDragger;
}

class ScaleCommand: public osgManipulator::Scale1DCommand {
public:

  ScaleCommand();
  virtual ~ScaleCommand();

  virtual osg::Matrix getMotionMatrix() const {
    double scale_center = getScaleCenter();
    double scale = getScale();
    return (osg::Matrix::translate(-scale_center, 0.0, 0.0) * osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(scale_center, 0.0, 0.0));
  }
};

class ScaleUniformDragger: public osgManipulator::Scale1DDragger {
public:
  ScaleUniformDragger(void);
  virtual ~ScaleUniformDragger(void);

  virtual bool handle(const osgManipulator::PointerInfo& pi, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
};

class Scaler: public Renderable {
public:
  Scaler(Renderable* drag_object);
  virtual ~Scaler(void);

  META_Renderable(Scaler)
  ;

protected:
  virtual void updateImpl();

private:
  osg::ref_ptr<Renderable> scale_object_;
  osg::ref_ptr<osgManipulator::ScaleAxisDragger> axis_scale_dragger_;
  osg::ref_ptr<ScaleUniformDragger> uniform_scale_dragger_;
};

#endif // SCALER_H
