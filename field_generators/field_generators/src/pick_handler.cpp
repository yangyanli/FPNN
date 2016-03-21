#include "common.h"
#include "renderable.h"
#include "osg_utility.h"

#include "pick_handler.h"

PickHandler::PickHandler(void) {
}

PickHandler::~PickHandler(void) {
}

void PickHandler::getUsage(osg::ApplicationUsage &usage) const {
  usage.addKeyboardMouseBinding("Click+Modifier Key", "Pick Renderable Object");
  return;
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  switch (ea.getEventType()) {
  case (osgGA::GUIEventAdapter::PUSH): {
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view == nullptr)
      return false;

    int mod_key_mask = ea.getModKeyMask();
    PickMode pick_mode = PickMode::UNKNOWN;
    if (mod_key_mask & osgGA::GUIEventAdapter::MODKEY_CTRL)
      pick_mode = PickMode::CTRL;
    else if (mod_key_mask & osgGA::GUIEventAdapter::MODKEY_SHIFT)
      pick_mode = PickMode::SHIFT;
    else if (mod_key_mask & osgGA::GUIEventAdapter::MODKEY_ALT)
      pick_mode = PickMode::ALT;
    else
      return false;

    osgUtil::LineSegmentIntersector::Intersection intersection;
    osg::NodePath node_path;
    Renderable* renderable = OSGUtility::computeIntersection<Renderable>(view, ea, intersection, node_path);
    //Renderable* renderable = OSGUtility::computePointIntersection<Renderable>(view, ea, intersection, node_path);
    if (renderable == nullptr)
      return false;

    renderable->pickEvent(pick_mode);
    return true;
  }
    break;
  default:
    return false;
  }

  return false;
}
