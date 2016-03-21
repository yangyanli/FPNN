#include <osg/Geode>
#include <osg/Camera>

#include <osgManipulator/ScaleAxisDragger>

#include "scaler.h"

namespace {

double computeScale(const osg::Vec3d& startProjectedPoint, const osg::Vec3d& projectedPoint, double scaleCenter) {
  double denom = startProjectedPoint[0] - scaleCenter;
  double scale = denom ? (projectedPoint[0] - scaleCenter) / denom : 1.0;
  return scale;
}

}

ScaleCommand::ScaleCommand() {

}

ScaleCommand::~ScaleCommand() {

}

ScaleUniformDragger::ScaleUniformDragger(void) {

}

ScaleUniformDragger::~ScaleUniformDragger(void) {

}

bool ScaleUniformDragger::handle(const osgManipulator::PointerInfo& pointer, const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  // Check if the dragger node is in the nodepath.
  if (!pointer.contains(this))
    return false;

  switch (ea.getEventType()) {
  // Pick start.
  case (osgGA::GUIEventAdapter::PUSH): {
    // Get the LocalToWorld matrix for this node and set it for the projector.
    osg::NodePath nodePathToRoot;
    computeNodePathToRoot(*this, nodePathToRoot);
    osg::Matrix localToWorld = osg::computeLocalToWorld(nodePathToRoot);
    _projector->setLocalToWorld(localToWorld);

    if (_projector->project(pointer, _startProjectedPoint)) {
      _scaleCenter = 0.0;
      if (_scaleMode == SCALE_WITH_OPPOSITE_HANDLE_AS_PIVOT) {
        if (pointer.contains(_leftHandleNode.get()))
          _scaleCenter = _projector->getLineEnd()[0];
        else if (pointer.contains(_rightHandleNode.get()))
          _scaleCenter = _projector->getLineStart()[0];
      }

      // Generate the motion command.
      osg::ref_ptr<ScaleCommand> cmd = new ScaleCommand();
      cmd->setStage(osgManipulator::MotionCommand::START);
      cmd->setLocalToWorldAndWorldToLocal(_projector->getLocalToWorld(), _projector->getWorldToLocal());

      // Dispatch command.
      dispatch(*cmd);

      // Set color to pick color.
      setMaterialColor(_pickColor, *this);

      aa.requestRedraw();
    }
    return true;
  }

    // Pick move.
  case (osgGA::GUIEventAdapter::DRAG): {
    osg::Vec3d projectedPoint;
    if (_projector->project(pointer, projectedPoint)) {
      // Generate the motion command.
      osg::ref_ptr<ScaleCommand> cmd = new ScaleCommand();

      // Compute scale.
      double scale = computeScale(_startProjectedPoint, projectedPoint, _scaleCenter);
      if (scale < getMinScale())
        scale = getMinScale();

      // Snap the referencePoint to the line start or line end depending on which is closer.
      double referencePoint = _startProjectedPoint[0];
      if (fabs(_projector->getLineStart()[0] - referencePoint) < fabs(_projector->getLineEnd()[0] - referencePoint))
        referencePoint = _projector->getLineStart()[0];
      else
        referencePoint = _projector->getLineEnd()[0];

      cmd->setStage(osgManipulator::MotionCommand::MOVE);
      cmd->setLocalToWorldAndWorldToLocal(_projector->getLocalToWorld(), _projector->getWorldToLocal());
      cmd->setScale(scale);
      cmd->setScaleCenter(_scaleCenter);
      cmd->setReferencePoint(referencePoint);
      cmd->setMinScale(getMinScale());

      // Dispatch command.
      dispatch(*cmd);

      aa.requestRedraw();
    }
    return true;
  }

    // Pick finish.
  case (osgGA::GUIEventAdapter::RELEASE): {
    osg::ref_ptr<ScaleCommand> cmd = new ScaleCommand();

    cmd->setStage(osgManipulator::MotionCommand::FINISH);
    cmd->setLocalToWorldAndWorldToLocal(_projector->getLocalToWorld(), _projector->getWorldToLocal());

    // Dispatch command.
    dispatch(*cmd);

    // Reset color.
    setMaterialColor(_color, *this);

    aa.requestRedraw();

    return true;
  }
  default:
    return false;
  }
}

Scaler::Scaler(Renderable* drag_object) :
    Renderable(), scale_object_(drag_object), axis_scale_dragger_(new osgManipulator::ScaleAxisDragger), uniform_scale_dragger_(new ScaleUniformDragger) {
  axis_scale_dragger_->setupDefaultGeometry();
  axis_scale_dragger_->setHandleEvents(true);

  uniform_scale_dragger_->setupDefaultGeometry();
  uniform_scale_dragger_->setHandleEvents(true);

  axis_scale_dragger_->addTransformUpdating(uniform_scale_dragger_);
  uniform_scale_dragger_->addTransformUpdating(axis_scale_dragger_);

  axis_scale_dragger_->addTransformUpdating(scale_object_);
  uniform_scale_dragger_->addTransformUpdating(scale_object_);

  return;
}

Scaler::~Scaler(void) {
}

void Scaler::updateImpl() {
  osg::BoundingSphere boundingSphere = scale_object_->getBound();
  double radius = boundingSphere.radius();

  if (radius == 0)
    return;

  osg::Matrix matrix = osg::Matrix::translate(boundingSphere.center());

  float axis_scale = 0.5 * radius;
  axis_scale_dragger_->setMatrix(osg::Matrix::scale(axis_scale, axis_scale, axis_scale) * matrix);
  addChild(axis_scale_dragger_);

  float uniform_scale = radius * 1.2;
  uniform_scale_dragger_->setMatrix(osg::Matrix::scale(uniform_scale, uniform_scale, uniform_scale) * matrix);
  addChild(uniform_scale_dragger_);

  return;
}
