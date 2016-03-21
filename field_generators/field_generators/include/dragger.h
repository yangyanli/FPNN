#pragma once
#ifndef DRAGGER_H
#define DRAGGER_H

#include "renderable.h"

namespace osgManipulator {
class TranslateAxisDragger;
class TrackballDragger;
}

class Dragger: public Renderable {
public:
  Dragger(Renderable* drag_object);
  virtual ~Dragger(void);

  META_Renderable(Dragger)
  ;

protected:
  virtual void updateImpl();

private:
  osg::ref_ptr<Renderable> drag_object_;
  osg::ref_ptr<osgManipulator::TranslateAxisDragger> translate_dragger_;
  osg::ref_ptr<osgManipulator::TrackballDragger> trackball_dragger_;
};

#endif // DRAGGER_H
