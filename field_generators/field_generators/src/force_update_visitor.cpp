#include <cassert>
#include "renderable.h"
#include "force_update_visitor.h"

ForceUpdateVisitor::ForceUpdateVisitor(void) :
    osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {
}

ForceUpdateVisitor::~ForceUpdateVisitor(void) {
}

void ForceUpdateVisitor::apply(osg::Node &node) {
  Renderable* renderable = dynamic_cast<Renderable*>(&node);
  if (renderable != nullptr) {
    renderable->forceUpdate();
  }

  traverse(node);

  return;
}
