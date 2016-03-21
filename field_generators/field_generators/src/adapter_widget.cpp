#include <QKeyEvent>

#include "adapter_widget.h"

class QtKeyboardMap {

public:
  QtKeyboardMap() {
    mKeyMap[Qt::Key_Escape] = osgGA::GUIEventAdapter::KEY_Escape;
    mKeyMap[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
    mKeyMap[Qt::Key_Home] = osgGA::GUIEventAdapter::KEY_Home;
    mKeyMap[Qt::Key_Enter] = osgGA::GUIEventAdapter::KEY_KP_Enter;
    mKeyMap[Qt::Key_End] = osgGA::GUIEventAdapter::KEY_End;
    mKeyMap[Qt::Key_Return] = osgGA::GUIEventAdapter::KEY_Return;
    mKeyMap[Qt::Key_PageUp] = osgGA::GUIEventAdapter::KEY_Page_Up;
    mKeyMap[Qt::Key_PageDown] = osgGA::GUIEventAdapter::KEY_Page_Down;
    mKeyMap[Qt::Key_Left] = osgGA::GUIEventAdapter::KEY_Left;
    mKeyMap[Qt::Key_Right] = osgGA::GUIEventAdapter::KEY_Right;
    mKeyMap[Qt::Key_Up] = osgGA::GUIEventAdapter::KEY_Up;
    mKeyMap[Qt::Key_Down] = osgGA::GUIEventAdapter::KEY_Down;
    mKeyMap[Qt::Key_Backspace] = osgGA::GUIEventAdapter::KEY_BackSpace;
    mKeyMap[Qt::Key_Tab] = osgGA::GUIEventAdapter::KEY_Tab;
    mKeyMap[Qt::Key_Space] = osgGA::GUIEventAdapter::KEY_Space;
    mKeyMap[Qt::Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
    mKeyMap[Qt::Key_Alt] = osgGA::GUIEventAdapter::KEY_Alt_L;
    mKeyMap[Qt::Key_Shift] = osgGA::GUIEventAdapter::KEY_Shift_L;
    mKeyMap[Qt::Key_Control] = osgGA::GUIEventAdapter::KEY_Control_L;
    mKeyMap[Qt::Key_Meta] = osgGA::GUIEventAdapter::KEY_Meta_L;

    mKeyMap[Qt::Key_F1] = osgGA::GUIEventAdapter::KEY_F1;
    mKeyMap[Qt::Key_F2] = osgGA::GUIEventAdapter::KEY_F2;
    mKeyMap[Qt::Key_F3] = osgGA::GUIEventAdapter::KEY_F3;
    mKeyMap[Qt::Key_F4] = osgGA::GUIEventAdapter::KEY_F4;
    mKeyMap[Qt::Key_F5] = osgGA::GUIEventAdapter::KEY_F5;
    mKeyMap[Qt::Key_F6] = osgGA::GUIEventAdapter::KEY_F6;
    mKeyMap[Qt::Key_F7] = osgGA::GUIEventAdapter::KEY_F7;
    mKeyMap[Qt::Key_F8] = osgGA::GUIEventAdapter::KEY_F8;
    mKeyMap[Qt::Key_F9] = osgGA::GUIEventAdapter::KEY_F9;
    mKeyMap[Qt::Key_F10] = osgGA::GUIEventAdapter::KEY_F10;
    mKeyMap[Qt::Key_F11] = osgGA::GUIEventAdapter::KEY_F11;
    mKeyMap[Qt::Key_F12] = osgGA::GUIEventAdapter::KEY_F12;
    mKeyMap[Qt::Key_F13] = osgGA::GUIEventAdapter::KEY_F13;
    mKeyMap[Qt::Key_F14] = osgGA::GUIEventAdapter::KEY_F14;
    mKeyMap[Qt::Key_F15] = osgGA::GUIEventAdapter::KEY_F15;
    mKeyMap[Qt::Key_F16] = osgGA::GUIEventAdapter::KEY_F16;
    mKeyMap[Qt::Key_F17] = osgGA::GUIEventAdapter::KEY_F17;
    mKeyMap[Qt::Key_F18] = osgGA::GUIEventAdapter::KEY_F18;
    mKeyMap[Qt::Key_F19] = osgGA::GUIEventAdapter::KEY_F19;
    mKeyMap[Qt::Key_F20] = osgGA::GUIEventAdapter::KEY_F20;

    mKeyMap[Qt::Key_hyphen] = '-';
    mKeyMap[Qt::Key_Equal] = '=';

    mKeyMap[Qt::Key_division] = osgGA::GUIEventAdapter::KEY_KP_Divide;
    mKeyMap[Qt::Key_multiply] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
    mKeyMap[Qt::Key_Minus] = '-';
    mKeyMap[Qt::Key_Plus] = '+';
    //mKeyMap[Qt::Key_H        ] = osgGA::GUIEventAdapter::KEY_KP_Home;
    //mKeyMap[Qt::Key_         ] = osgGA::GUIEventAdapter::KEY_KP_Up;
    //mKeyMap[92               ] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
    //mKeyMap[86               ] = osgGA::GUIEventAdapter::KEY_KP_Left;
    //mKeyMap[87               ] = osgGA::GUIEventAdapter::KEY_KP_Begin;
    //mKeyMap[88               ] = osgGA::GUIEventAdapter::KEY_KP_Right;
    //mKeyMap[83               ] = osgGA::GUIEventAdapter::KEY_KP_End;
    //mKeyMap[84               ] = osgGA::GUIEventAdapter::KEY_KP_Down;
    //mKeyMap[85               ] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
    mKeyMap[Qt::Key_Insert] = osgGA::GUIEventAdapter::KEY_KP_Insert;
    //mKeyMap[Qt::Key_Delete   ] = osgGA::GUIEventAdapter::KEY_KP_Delete;
  }

  ~QtKeyboardMap() {
  }

  int remapKey(QKeyEvent* event) {
    KeyMap::iterator itr = mKeyMap.find(event->key());
    if (itr == mKeyMap.end()) {
      return int(event->text().at(0).toLatin1());
    } else
      return itr->second;
  }

private:
  typedef std::map<unsigned int, int> KeyMap;
  KeyMap mKeyMap;
};

static QtKeyboardMap s_QtKeyboardMap;

AdapterWidget::AdapterWidget(QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f, bool forwardKeyEvents) :
    QGLWidget(parent, shareWidget, f), _gw(new osgViewer::GraphicsWindowEmbedded(0, 0, width(), height())), _forwardKeyEvents(forwardKeyEvents) {
  setFocusPolicy(Qt::ClickFocus);
}

AdapterWidget::AdapterWidget(QGLContext* context, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f, bool forwardKeyEvents) :
    QGLWidget(context, parent, shareWidget, f), _gw(new osgViewer::GraphicsWindowEmbedded(0, 0, width(), height())), _forwardKeyEvents(forwardKeyEvents) {
  setFocusPolicy(Qt::ClickFocus);
}

AdapterWidget::AdapterWidget(const QGLFormat& format, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f, bool forwardKeyEvents) :
    QGLWidget(format, parent, shareWidget, f), _gw(new osgViewer::GraphicsWindowEmbedded(0, 0, width(), height())), _forwardKeyEvents(forwardKeyEvents) {
  setFocusPolicy(Qt::ClickFocus);
}

AdapterWidget::~AdapterWidget() {
  _gw->close();
}

void AdapterWidget::setKeyboardModifiers(QInputEvent* event) {
  int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
  unsigned int mask = 0;
  if (modkey & Qt::ShiftModifier)
    mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
  if (modkey & Qt::ControlModifier)
    mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
  if (modkey & Qt::AltModifier)
    mask |= osgGA::GUIEventAdapter::MODKEY_ALT;
  _gw->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
}

void AdapterWidget::resizeEvent(QResizeEvent* event) {
  const QSize& size = event->size();
  _gw->resized(x(), y(), size.width(), size.height());
  _gw->getEventQueue()->windowResize(x(), y(), size.width(), size.height());
  _gw->requestRedraw();
}

void AdapterWidget::moveEvent(QMoveEvent* event) {
  const QPoint& pos = event->pos();
  _gw->resized(pos.x(), pos.y(), width(), height());
  _gw->getEventQueue()->windowResize(pos.x(), pos.y(), width(), height());
}

void AdapterWidget::glDraw() {
  QGLWidget::glDraw();
  _gw->requestRedraw();
}

void AdapterWidget::keyPressEvent(QKeyEvent* event) {
  setKeyboardModifiers(event);
  int value = s_QtKeyboardMap.remapKey(event);
  _gw->getEventQueue()->keyPress(value);

  // this passes the event to the regular Qt key event processing,
  // among others, it closes popup windows on ESC and forwards the event to the parent widgets
  if (_forwardKeyEvents)
    inherited::keyPressEvent(event);
}

void AdapterWidget::keyReleaseEvent(QKeyEvent* event) {
  if (event->isAutoRepeat()) {
    event->ignore();
  } else {
    setKeyboardModifiers(event);
    int value = s_QtKeyboardMap.remapKey(event);
    _gw->getEventQueue()->keyRelease(value);
  }

  // this passes the event to the regular Qt key event processing,
  // among others, it closes popup windows on ESC and forwards the event to the parent widgets
  if (_forwardKeyEvents)
    inherited::keyReleaseEvent(event);
}

void AdapterWidget::mousePressEvent(QMouseEvent* event) {
  int button = 0;
  switch (event->button()) {
  case Qt::LeftButton:
    button = 1;
    break;
  case Qt::MidButton:
    button = 2;
    break;
  case Qt::RightButton:
    button = 3;
    break;
  case Qt::NoButton:
    button = 0;
    break;
  default:
    button = 0;
    break;
  }
  setKeyboardModifiers(event);
  _gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void AdapterWidget::mouseReleaseEvent(QMouseEvent* event) {
  int button = 0;
  switch (event->button()) {
  case Qt::LeftButton:
    button = 1;
    break;
  case Qt::MidButton:
    button = 2;
    break;
  case Qt::RightButton:
    button = 3;
    break;
  case Qt::NoButton:
    button = 0;
    break;
  default:
    button = 0;
    break;
  }
  setKeyboardModifiers(event);
  _gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}

void AdapterWidget::mouseDoubleClickEvent(QMouseEvent* event) {
  int button = 0;
  switch (event->button()) {
  case Qt::LeftButton:
    button = 1;
    break;
  case Qt::MidButton:
    button = 2;
    break;
  case Qt::RightButton:
    button = 3;
    break;
  case Qt::NoButton:
    button = 0;
    break;
  default:
    button = 0;
    break;
  }
  setKeyboardModifiers(event);
  _gw->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);
}

void AdapterWidget::mouseMoveEvent(QMouseEvent* event) {
  setKeyboardModifiers(event);
  _gw->getEventQueue()->mouseMotion(event->x(), event->y());
}

void AdapterWidget::wheelEvent(QWheelEvent* event) {
  setKeyboardModifiers(event);
  _gw->getEventQueue()->mouseScroll(
      event->orientation() == Qt::Vertical ?
          (event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN) :
          (event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_LEFT : osgGA::GUIEventAdapter::SCROLL_RIGHT));
}
