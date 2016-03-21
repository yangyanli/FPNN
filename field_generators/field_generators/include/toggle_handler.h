#pragma once
#ifndef TOGGLE_HANDLER_H
#define TOGGLE_HANDLER_H

#include <string>

#include <osgViewer/ViewerEventHandlers>

class Renderable;

class ToggleHandler: public osgGA::GUIEventHandler {
public:
  ToggleHandler(Renderable* renderable, int key, const std::string& description);
  virtual ~ToggleHandler(void);

  virtual void getUsage(osg::ApplicationUsage &usage) const;
  bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
protected:
  osg::ref_ptr<Renderable> renderable_;
  int key_;
  std::string description_;
};

#endif // TOGGLE_HANDLER_H
