#include <osg/Geode>
#include <osg/Camera>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/TranslateAxisDragger>

#include "dragger.h"

Dragger::Dragger(Renderable* drag_object) :
    Renderable(), drag_object_(drag_object), translate_dragger_(new osgManipulator::TranslateAxisDragger), trackball_dragger_(
        new osgManipulator::TrackballDragger) {
  translate_dragger_->setupDefaultGeometry();
  translate_dragger_->setHandleEvents(true);

  trackball_dragger_->setupDefaultGeometry();
  trackball_dragger_->setHandleEvents(true);

  translate_dragger_->addTransformUpdating(trackball_dragger_);
  trackball_dragger_->addTransformUpdating(translate_dragger_);

  translate_dragger_->addTransformUpdating(drag_object_);
  trackball_dragger_->addTransformUpdating(drag_object_);

  return;
}

Dragger::~Dragger(void) {
}

void Dragger::updateImpl() {
  osg::BoundingSphere boundingSphere = drag_object_->getBound();
  double radius = boundingSphere.radius();

  if (radius == 0)
    return;

  osg::Matrix matrix = osg::Matrix::translate(boundingSphere.center());

  float t_scale = radius * 0.8;
  float r_scale = radius * 0.5;
  float s_scale = radius * 1.2;
  translate_dragger_->setMatrix(osg::Matrix::scale(t_scale, t_scale, t_scale) * matrix);
  trackball_dragger_->setMatrix(osg::Matrix::scale(r_scale, r_scale, r_scale) * matrix);
  content_root_->addChild(translate_dragger_);
  content_root_->addChild(trackball_dragger_);

  return;
}
