#pragma once
#ifndef FORCE_UPDATE_VISITOR_H
#define FORCE_UPDATE_VISITOR_H

#include <osg/NodeVisitor>

class ForceUpdateVisitor: public osg::NodeVisitor {
public:
  ForceUpdateVisitor(void);
  virtual ~ForceUpdateVisitor(void);

  virtual void apply(osg::Node &node);
};

#endif // FORCE_UPDATE_VISITOR_H
