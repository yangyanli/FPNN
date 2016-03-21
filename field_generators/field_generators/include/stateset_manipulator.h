#pragma once
#ifndef STATESET_MANIPULATOR_H
#define STATESET_MANIPULATOR_H

#include <osgGA/StateSetManipulator>

class StateSetManipulator: public osgGA::StateSetManipulator {
public:

  StateSetManipulator(osg::StateSet* stateset = 0);
  virtual ~StateSetManipulator(void);

  virtual void getUsage(osg::ApplicationUsage &usage) const;
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

  void setTransparencyEnabled(bool transparency);
  bool getTransparencyEnabled() const {
    return _transparency;
  }
  ;
  void setKeyEventToggleTransparency(int key) {
    _keyEventToggleTransparency = key;
  }
  int getKeyEventToggleTransparency() const {
    return _keyEventToggleTransparency;
  }

  void setSmoothShadeEnabled(bool smooth_shade);
  bool getSmoothShadeEnabled() const {
    return _smooth_shade;
  }
  ;
  void setKeyEventToggleSmoothShade(int key) {
    _keyEventToggleSmoothShade = key;
  }
  int getKeyEventToggleSmoothShade() const {
    return _keyEventToggleSmoothShade;
  }

protected:
  bool _transparency;
  int _keyEventToggleTransparency;

  bool _smooth_shade;
  int _keyEventToggleSmoothShade;
};

#endif // STATESET_MANIPULATOR_H
