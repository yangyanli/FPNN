#include <osg/Geode>
#include <osg/Camera>
#include <osgText/Text>

#include "information.h"

std::mutex Information::mutex_default_font_;

Information::Information(float font_size) :
    Renderable(), width_(1024), height_(768), x_(32), y_(32), font_size_(font_size) {
  toggleHidden();
}

Information::~Information(void) {
}

void Information::setFontSize(float font_size) {
  if (font_size == font_size_)
    return;

  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  font_size_ = font_size;

  return;
}

void Information::setText(const std::string& text, float x, float y) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  text_ = text;
  x_ = x;
  y_ = y;

  return;
}

void Information::updateImpl() {
  osg::Camera* hud_camera = new osg::Camera;
  content_root_->addChild(hud_camera);

  hud_camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  hud_camera->setViewMatrix(osg::Matrix::identity());
  hud_camera->setRenderOrder(osg::Camera::POST_RENDER);
  hud_camera->setClearMask(GL_DEPTH_BUFFER_BIT);
  hud_camera->setAllowEventFocus(false);
  hud_camera->setProjectionMatrix(osg::Matrix::ortho2D(0, width_, 0, height_));

  osg::Geode* geode = new osg::Geode();
  hud_camera->addChild(geode);
  osg::StateSet* stateset = geode->getOrCreateStateSet();
  stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
  stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  mutex_default_font_.lock();
  osgText::Text* text = new osgText::Text;
  text->setFont("fonts/times.ttf");
  mutex_default_font_.unlock();

  geode->addDrawable(text);

  text->setCharacterSize(font_size_);
  text->setColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
  text->setDataVariance(osg::Object::DYNAMIC);
  text->setText(text_);

  double height = 2.0 / hud_camera->getProjectionMatrix()(1, 1);
  text->setPosition(osg::Vec3(x_, height - y_, 0.0f));

  return;
}
