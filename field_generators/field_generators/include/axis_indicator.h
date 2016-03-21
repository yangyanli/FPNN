#pragma once
#ifndef AXIS_INDICATOR_H
#define AXIS_INDICATOR_H

#include "renderable.h"

class OSGViewerWidget;

class AxisIndicator: public Renderable {
public:
  AxisIndicator(OSGViewerWidget* osg_viewer_widget);
  virtual ~AxisIndicator(void);

  META_Renderable(AxisIndicator)
  ;

protected:
  virtual void updateImpl();

  OSGViewerWidget* osg_viewer_widget_;
};

#endif // AXIS_INDICATOR_H
