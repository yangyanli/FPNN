#include <osg/LineSegment>

#include "osg_utility.h"
#include "osg_viewer_widget.h"

#include "axis_indicator.h"

AxisIndicator::AxisIndicator(OSGViewerWidget* osg_viewer_widget) :
    osg_viewer_widget_(osg_viewer_widget) {
  toggleHidden();
}

AxisIndicator::~AxisIndicator(void) {
}

void AxisIndicator::updateImpl() {
  osg::BoundingBox bounding_box = osg_viewer_widget_->getBoundingBox();
  if (bounding_box.xMax() == -std::numeric_limits<float>::max()) {
    bounding_box._min = osg::Vec3(0.0, 0.0, 0.0);
    bounding_box._max = osg::Vec3(1.0, 1.0, 1.0);
  }

  double radius = bounding_box.radius();
  double cylinder_thickness = radius / 100;
  double cone_thickness = radius / 50;

  double up_len = radius / 4;
  osg::Vec3 up_vector = osg_viewer_widget_->getUpVector();
  content_root_->addChild(
      OSGUtility::drawArrow(bounding_box._min, bounding_box._min + up_vector * up_len, cylinder_thickness, 0.2 * up_len, osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f)));

  double x_len = bounding_box.xMax() - bounding_box.xMin();
  content_root_->addChild(
      OSGUtility::drawArrow(bounding_box._min, bounding_box._min + osg::Vec3(0.8 * x_len, 0, 0), cylinder_thickness, 0.2 * x_len,
          osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)));

  double y_len = bounding_box.yMax() - bounding_box.yMin();
  content_root_->addChild(
      OSGUtility::drawArrow(bounding_box._min, bounding_box._min + osg::Vec3(0, 0.8 * y_len, 0), cylinder_thickness, 0.2 * y_len,
          osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)));

  double z_len = bounding_box.zMax() - bounding_box.zMin();
  content_root_->addChild(
      OSGUtility::drawArrow(bounding_box._min, bounding_box._min + osg::Vec3(0, 0, 0.8 * z_len), cylinder_thickness, 0.2 * z_len,
          osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)));

  osg::ref_ptr < osgText::Text > corner = new osgText::Text;
  corner->setDataVariance(osg::Object::DYNAMIC);
  corner->setFont("fonts/times.ttf");
  corner->setCharacterSize(radius / 16);
  corner->setColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
  corner->setAutoRotateToScreen(true);
  corner->setPosition(bounding_box._min);
  std::string corner_string = "(" + std::to_string(bounding_box.xMin()) + "," + std::to_string(bounding_box.yMin()) + "," + std::to_string(bounding_box.zMin())
      + ")";
  corner->setText(corner_string);

  osg::ref_ptr < osgText::Text > x_axis = new osgText::Text;
  x_axis->setDataVariance(osg::Object::DYNAMIC);
  x_axis->setFont("fonts/times.ttf");
  x_axis->setCharacterSize(radius / 16);
  x_axis->setColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
  x_axis->setAutoRotateToScreen(true);
  x_axis->setPosition(osg::Vec3(x_len, 0, 0) + bounding_box._min);
  if (x_len != 0) {
    std::string x_axis_string = std::to_string(x_len);
    x_axis->setText(x_axis_string);
  }

  osg::ref_ptr < osgText::Text > y_axis = new osgText::Text;
  y_axis->setDataVariance(osg::Object::DYNAMIC);
  y_axis->setFont("fonts/times.ttf");
  y_axis->setCharacterSize(radius / 16);
  y_axis->setColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
  y_axis->setAutoRotateToScreen(true);
  y_axis->setPosition(osg::Vec3(0, y_len, 0) + bounding_box._min);
  if (y_len != 0) {
    std::string y_axis_string = std::to_string(y_len);
    y_axis->setText(y_axis_string);
  }

  osg::ref_ptr < osgText::Text > z_axis = new osgText::Text;
  z_axis->setDataVariance(osg::Object::DYNAMIC);
  z_axis->setFont("fonts/times.ttf");
  z_axis->setCharacterSize(radius / 16);
  z_axis->setColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
  z_axis->setAutoRotateToScreen(true);
  z_axis->setPosition(osg::Vec3(0, 0, z_len) + bounding_box._min);
  if (z_len != 0) {
    std::string z_axis_string = std::to_string(z_len);
    z_axis->setText(z_axis_string);
  }

  osg::Geode* geode = new osg::Geode();
  osg::StateSet* state_set = geode->getOrCreateStateSet();
  state_set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  state_set->setMode(GL_BLEND, osg::StateAttribute::ON);
  state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  geode->addDrawable(corner);
  geode->addDrawable(x_axis);
  geode->addDrawable(y_axis);
  geode->addDrawable(z_axis);
  content_root_->addChild(geode);

  return;
}
