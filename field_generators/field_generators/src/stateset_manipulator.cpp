#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/ShadeModel>

#include "stateset_manipulator.h"

StateSetManipulator::StateSetManipulator(osg::StateSet* stateset) :
    osgGA::StateSetManipulator(stateset), _transparency(false), _keyEventToggleTransparency('o'), _smooth_shade(false), _keyEventToggleSmoothShade('s') {
}

StateSetManipulator::~StateSetManipulator(void) {
}

void StateSetManipulator::getUsage(osg::ApplicationUsage &usage) const {
  usage.addKeyboardMouseBinding(reinterpret_cast<const char*>(&_keyEventToggleTransparency), "Toggle Transparency");
  usage.addKeyboardMouseBinding(reinterpret_cast<const char*>(&_keyEventToggleSmoothShade), "Toggle Smooth Shade");

  return;
}

void StateSetManipulator::setTransparencyEnabled(bool transparency) {
  if (_transparency == transparency)
    return;

  clone();

  _transparency = transparency;
  if (_transparency) {
    // enable transparency
    // Enable blending, select transparent bin.
    _stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    _stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    // Enable depth test so that an opaque polygon will occlude a transparent one behind it.
    _stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    // Conversely, disable writing to depth buffer so that
    // a transparent polygon will allow polygons behind it to shine through.
    // OSG renders transparent polygons after opaque ones.
    osg::Depth* depth = new osg::Depth;
    depth->setWriteMask(false);
    _stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);
  } else {
    _stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
    _stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    osg::Depth* depth = new osg::Depth;
    depth->setWriteMask(true);
    _stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);

    _stateset->setMode(GL_BLEND, osg::StateAttribute::OFF);
  }

  return;
}

void StateSetManipulator::setSmoothShadeEnabled(bool smooth_shade) {
  if (_smooth_shade == smooth_shade)
    return;

  _smooth_shade = smooth_shade;

  osg::ref_ptr < osg::ShadeModel > shade_model(new osg::ShadeModel);
  shade_model->setMode(_smooth_shade ? (osg::ShadeModel::SMOOTH) : (osg::ShadeModel::FLAT));

  _stateset->setAttributeAndModes(shade_model, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

  return;
}

bool StateSetManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  if ((ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN && ea.getKey() == _keyEventToggleTransparency)) {
    if (!_initialized)
      _transparency = (_stateset->getMode(GL_BLEND) & osg::StateAttribute::ON);
    setTransparencyEnabled(!getTransparencyEnabled());
    aa.requestRedraw();

    return true;
  }

  if ((ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN && ea.getKey() == _keyEventToggleSmoothShade)) {
    setSmoothShadeEnabled(!getSmoothShadeEnabled());
    aa.requestRedraw();

    return true;
  }

  return osgGA::StateSetManipulator::handle(ea, aa);
}
