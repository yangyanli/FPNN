#pragma once
#ifndef UpdateCallback_H
#define UpdateCallback_H

#include <osg/NodeCallback>

class UpdateCallback: public osg::NodeCallback {
public:
  UpdateCallback(void);
  virtual ~UpdateCallback(void);

  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#endif // UpdateCallback_H
