#pragma once
#ifndef PICK_HANDLER_H
#define PICK_HANDLER_H

#include <string>

#include <osgViewer/ViewerEventHandlers>

class PickHandler: public osgGA::GUIEventHandler {
public:
  PickHandler(void);
  virtual ~PickHandler(void);

  virtual void getUsage(osg::ApplicationUsage &usage) const;
  bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
};

#endif // PICK_HANDLER_H
