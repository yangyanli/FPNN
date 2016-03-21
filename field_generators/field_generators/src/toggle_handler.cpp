#include "renderable.h"
#include "toggle_handler.h"

ToggleHandler::ToggleHandler(Renderable* renderable, int key, const std::string& description) :
    renderable_(renderable), key_(key), description_(description) {
}

ToggleHandler::~ToggleHandler(void) {
}

void ToggleHandler::getUsage(osg::ApplicationUsage &usage) const {
  usage.addKeyboardMouseBinding(reinterpret_cast<const char*>(&key_), description_);
  return;
}

bool ToggleHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  switch (ea.getEventType()) {
  case (osgGA::GUIEventAdapter::KEYUP): {
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view && ea.getKey() == key_) {
      renderable_->toggleHidden();
      return true;
    }
    return false;
  }
  default:
    return false;
  }

  return false;
}
