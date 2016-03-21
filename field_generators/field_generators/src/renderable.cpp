#include <random>

#include <QMenu>
#include <QImage>
#include <QColor>
#include <QCursor>

#include <osg/Point>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Material>
#include <osgViewer/Viewer>
#include <osgUtil/UpdateVisitor>
#include <osg/ComputeBoundsVisitor>

#include "osg_utility.h"
#include "update_callback.h"
#include "force_update_visitor.h"

#include "renderable.h"

std::mutex Renderable::mutex_graphics_context_;

Renderable::Renderable(void) :
    read_write_lock_(QReadWriteLock::NonRecursive), content_root_(new osg::MatrixTransform), expired_(true), hidden_(false) {
  addChild(content_root_);

  setUpdateCallback(new UpdateCallback);
  setDataVariance(Object::DYNAMIC);

  return;
}

Renderable::~Renderable(void) {
}

void Renderable::expire(void) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  return;
}

void Renderable::update(void) {
  if (!read_write_lock_.tryLockForRead())
    return;

  if (!expired_) {
    read_write_lock_.unlock();
    return;
  }

  expired_ = false;
  content_root_->removeChildren(0, content_root_->getNumChildren());

  if (!hidden_)
    updateImpl();

  read_write_lock_.unlock();
  return;
}

void Renderable::forceUpdate(void) {
  read_write_lock_.lockForRead();

  expired_ = false;
  content_root_->removeChildren(0, content_root_->getNumChildren());

  if (!hidden_)
    updateImpl();

  read_write_lock_.unlock();
  return;
}

void Renderable::toggleHidden(void) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;
  hidden_ = !hidden_;

  return;
}

osg::BoundingBox Renderable::getBoundingBox(void) {
  ForceUpdateVisitor force_update_visitor;
  this->accept(force_update_visitor);

  osg::ComputeBoundsVisitor visitor;
  content_root_->accept(visitor);

  return visitor.getBoundingBox();
}

static void saveColorImage(osg::Image* color_image, osg::Image* depth_image, const std::string& filename) {
  int width = color_image->s();
  int height = color_image->t();
  float* z_buffer = (float*) (depth_image->data());

  QImage q_image(width, height, QImage::Format_ARGB32);
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      float z = z_buffer[y * width + x];
      osg::Vec4 color = color_image->getColor(x, y);
      color = color * 255;
      QRgb rgba = QColor(color.r(), color.g(), color.b(), (z == 1.0) ? (0) : (255)).rgba();
      q_image.setPixel(x, height - 1 - y, rgba);
    }
  }

  q_image.save(filename.c_str());

  return;
}

static void saveDepthImage(osg::Image* depth_image, const std::string& filename) {
  int width = depth_image->s();
  int height = depth_image->t();

  float z_min, z_max;
  z_min = std::numeric_limits<float>::max();
  z_max = std::numeric_limits<float>::lowest();
  float* z_buffer = (float*) (depth_image->data());
  for (size_t x = 0; x < width; ++x) {
    for (size_t y = 0; y < height; ++y) {
      float z = z_buffer[y * width + x];
      if (z == 1.0)
        continue;

      z_min = std::min(z_min, z);
      z_max = std::max(z_max, z);
    }
  }

  QImage q_image(width, height, QImage::Format_ARGB32);
  for (size_t x = 0; x < width; ++x) {
    for (size_t y = 0; y < height; ++y) {
      float z = z_buffer[y * width + x];
      float value = (z == 1.0) ? (1.0) : (z - z_min) * 0.8 / (z_max - z_min);
      value *= 255;
      QRgb rgba = QColor(value, value, value, (z == 1.0) ? (0) : (255)).rgba();
      q_image.setPixel(x, height - 1 - y, rgba);
    }
  }

  q_image.save(filename.c_str());

  return;
}

double Renderable::virtualScan(const osg::Vec3Array* eyes, const osg::Vec3Array* centers, const osg::Vec3Array* ups, int resolution, double noise,
    std::vector<pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr>& point_clouds, float fovy) {
  bool with_noise = (noise != 0.0);
  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, with_noise ? (noise / 2) : (0.00000001));

  osg::ref_ptr < osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits;
  traits->x = 0;
  traits->y = 0;
  traits->width = resolution;
  traits->height = resolution;
  traits->windowDecoration = false;
  traits->doubleBuffer = false;
  traits->sharedContext = 0;
  traits->pbuffer = true;

  mutex_graphics_context_.lock();
  osg::ref_ptr < osg::GraphicsContext > graphics_context = osg::GraphicsContext::createGraphicsContext(traits.get());
  mutex_graphics_context_.unlock();

  osgViewer::Viewer* viewer = new osgViewer::Viewer;
  osg::ref_ptr < osg::Camera > camera = viewer->getCamera();
  camera->setGraphicsContext(graphics_context);
  camera->setViewport(new osg::Viewport(0, 0, resolution, resolution));
  camera->setProjectionMatrixAsPerspective(fovy, 1.0f, 1.0f, 10000.0f);
  camera->setClearColor(osg::Vec4(1, 1, 1, 1.0));

  osg::ref_ptr < osg::Group > node = new osg::Group;
  OSGUtility::applyShaderNormal (node);
  node->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  node->addChild(this);
  viewer->setSceneData(node);
  viewer->setDataVariance(osg::Object::DYNAMIC);
  viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
  viewer->realize();

  osg::ref_ptr < osg::Image > color_image = new osg::Image;
  osg::ref_ptr < osg::Image > depth_image = new osg::Image;
  color_image->allocateImage(traits->width, traits->height, 1, GL_RGBA, GL_FLOAT);
  depth_image->allocateImage(traits->width, traits->height, 1, GL_DEPTH_COMPONENT, GL_FLOAT);
  camera->attach(osg::Camera::COLOR_BUFFER, color_image.get());
  camera->attach(osg::Camera::DEPTH_BUFFER, depth_image.get());

  double avg_distance = 0.0;
  for (size_t i = 0, i_end = eyes->size(); i < i_end; ++i) {
    const osg::Vec3& eye = eyes->at(i);
    const osg::Vec3& center = centers->at(i);
    const osg::Vec3& up = ups->at(i);
    osg::Vec3 eye_direction(center - eye);
    avg_distance += eye_direction.length();
    eye_direction.normalize();
    camera->setViewMatrixAsLookAt(eye, center, up);
    viewer->frame();

    //saveDepthImage(depth_image.get(), "depth.png");
    //saveColorImage(color_image.get(), depth_image.get(), "color.png");

    osg::Matrix matrix_vpw(camera->getViewMatrix() * camera->getProjectionMatrix());
    matrix_vpw.postMult(camera->getViewport()->computeWindowMatrix());
    osg::Matrix matrix_vpw_inverse;
    matrix_vpw_inverse.invert(matrix_vpw);

    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr point_cloud = point_clouds[i];
    point_cloud->sensor_origin_ = Eigen::Vector4f(eye.x(), eye.y(), eye.z(), 0.0f);
    Eigen::Vector3f z_negative(0.0f, 0.0f, -1.0f);
    point_cloud->sensor_orientation_.setFromTwoVectors(z_negative, Eigen::Vector3f(eye_direction.x(), eye_direction.y(), eye_direction.z()));
    pcl::PointXYZRGBNormal point;
    float* z_buffer = (float*) (depth_image->data());
    for (size_t window_x = 0; window_x < resolution; ++window_x) {
      for (size_t window_y = 0; window_y < resolution; ++window_y) {
        double window_z = z_buffer[window_y * resolution + window_x];
        if (window_z == 1.0)
          continue;
        osg::Vec3 world = osg::Vec3(window_x, window_y, window_z) * matrix_vpw_inverse;

        point.x = world.x() + (with_noise ? (distribution(generator)) : (0.0));
        point.y = world.y() + (with_noise ? (distribution(generator)) : (0.0));
        point.z = world.z() + (with_noise ? (distribution(generator)) : (0.0));

        osg::Vec4 normal_4 = color_image->getColor(window_x, window_y);
        osg::Vec3 normal(normal_4.x(), normal_4.y(), normal_4.z());
        normal = normal * 2 - osg::Vec3(1.0f, 1.0f, 1.0f);
        if (normal * eye_direction > 0)
          normal = -normal;
        normal.normalize();
        point.normal_x = normal.x();
        point.normal_y = normal.y();
        point.normal_z = normal.z();
        point_cloud->push_back(point);
      }
    }
  }

  mutex_graphics_context_.lock();
  delete viewer;
  mutex_graphics_context_.unlock();

  avg_distance /= eyes->size();
  double grid_size = avg_distance * tan(fovy / 360.0 * M_PI) * 2 / resolution;
  return grid_size;
}

double Renderable::virtualScan(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up, int width, int height, double noise,
    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr point_cloud, float fovy) {
  bool with_noise = (noise != 0.0);
  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, with_noise ? (noise / 2) : (0.00000001));

  osg::ref_ptr < osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits;
  traits->x = 0;
  traits->y = 0;
  traits->width = width;
  traits->height = height;
  traits->windowDecoration = false;
  traits->doubleBuffer = false;
  traits->sharedContext = 0;
  traits->pbuffer = true;

  mutex_graphics_context_.lock();
  osg::ref_ptr < osg::GraphicsContext > graphics_context = osg::GraphicsContext::createGraphicsContext(traits.get());
  mutex_graphics_context_.unlock();

  osgViewer::Viewer* viewer = new osgViewer::Viewer;
  osg::ref_ptr < osg::Camera > camera = viewer->getCamera();
  camera->setGraphicsContext(graphics_context);
  camera->setViewport(new osg::Viewport(0, 0, width, height));
  camera->setProjectionMatrixAsPerspective(fovy, 1.0f * width / height, 1.0f, 10000.0f);
  camera->setClearColor(osg::Vec4(1, 1, 1, 1.0));

  osg::ref_ptr < osg::Group > node = new osg::Group;
  OSGUtility::applyShaderNormal (node);
  node->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  node->addChild(this);
  viewer->setSceneData(node);
  viewer->setDataVariance(osg::Object::DYNAMIC);
  viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
  viewer->realize();

  osg::ref_ptr < osg::Image > color_image = new osg::Image;
  osg::ref_ptr < osg::Image > depth_image = new osg::Image;
  color_image->allocateImage(traits->width, traits->height, 1, GL_RGBA, GL_FLOAT);
  depth_image->allocateImage(traits->width, traits->height, 1, GL_DEPTH_COMPONENT, GL_FLOAT);
  camera->attach(osg::Camera::COLOR_BUFFER, color_image.get());
  camera->attach(osg::Camera::DEPTH_BUFFER, depth_image.get());

  osg::Vec3 eye_direction(center - eye);
  double avg_distance = eye_direction.length();
  eye_direction.normalize();
  camera->setViewMatrixAsLookAt(eye, center, up);
  viewer->frame();

  //saveDepthImage(depth_image.get(), "depth.png");
  //saveColorImage(color_image.get(), depth_image.get(), "color.png");

  osg::Matrix matrix_vpw(camera->getViewMatrix() * camera->getProjectionMatrix());
  matrix_vpw.postMult(camera->getViewport()->computeWindowMatrix());
  osg::Matrix matrix_vpw_inverse;
  matrix_vpw_inverse.invert(matrix_vpw);

  point_cloud->sensor_origin_ = Eigen::Vector4f(eye.x(), eye.y(), eye.z(), 0.0f);
  Eigen::Vector3f z_negative(0.0f, 0.0f, -1.0f);
  point_cloud->sensor_orientation_.setFromTwoVectors(z_negative, Eigen::Vector3f(eye_direction.x(), eye_direction.y(), eye_direction.z()));
  pcl::PointXYZRGBNormal point;
  float* z_buffer = (float*) (depth_image->data());
  for (size_t window_x = 0; window_x < width; ++window_x) {
    for (size_t window_y = 0; window_y < height; ++window_y) {
      double window_z = z_buffer[window_y * width + window_x];
      if (window_z == 1.0)
        continue;
      osg::Vec3 world = osg::Vec3(window_x, window_y, window_z) * matrix_vpw_inverse;

      point.x = world.x() + (with_noise ? (distribution(generator)) : (0.0));
      point.y = world.y() + (with_noise ? (distribution(generator)) : (0.0));
      point.z = world.z() + (with_noise ? (distribution(generator)) : (0.0));

      osg::Vec4 normal_4 = color_image->getColor(window_x, window_y);
      osg::Vec3 normal(normal_4.x(), normal_4.y(), normal_4.z());
      normal = normal * 2 - osg::Vec3(1.0f, 1.0f, 1.0f);
      if (normal * eye_direction > 0)
        normal = -normal;
      normal.normalize();
      point.normal_x = normal.x();
      point.normal_y = normal.y();
      point.normal_z = normal.z();
      point_cloud->push_back(point);
    }
  }

  mutex_graphics_context_.lock();
  delete viewer;
  mutex_graphics_context_.unlock();

  double grid_size = avg_distance * tan(fovy / 360.0 * M_PI) * 2 / (std::max(width, height));
  return grid_size;
}

double Renderable::virtualScan(const osg::Vec3Array* eyes, const osg::Vec3Array* centers, const osg::Vec3Array* ups, int resolution, double noise,
    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr point_cloud, float fovy) {
  std::vector < pcl::PointCloud < pcl::PointXYZRGBNormal > ::Ptr > point_clouds;
  for (size_t i = 0, i_end = eyes->size(); i < i_end; ++i)
    point_clouds.push_back(pcl::PointCloud < pcl::PointXYZRGBNormal > ::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>()));

  double grid_size = virtualScan(eyes, centers, ups, resolution, noise, point_clouds, fovy);

  point_cloud->clear();
  for (size_t i = 0, i_end = point_clouds.size(); i < i_end; ++i)
    *point_cloud += *(point_clouds[i]);

  return grid_size;
}

double Renderable::virtualScan(const osg::Vec3Array* eye_directions, int resolution, double noise,
    std::vector<pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr>& point_clouds, float fovy) {
  osg::BoundingBox bbox = getBoundingBox();
  osg::Vec3 center = bbox.center();
  osg::Vec3 up(0.0, 1.0, 0.0);
  double distance = bbox.radius() * 2.5;

  osg::ref_ptr < osg::Vec3Array > eyes(new osg::Vec3Array);
  osg::ref_ptr < osg::Vec3Array > centers(new osg::Vec3Array);
  osg::ref_ptr < osg::Vec3Array > ups(new osg::Vec3Array);
  eyes->reserve(eye_directions->size());
  centers->assign(eye_directions->size(), center);
  ups->assign(eye_directions->size(), up);
  for (size_t i = 0, i_end = eye_directions->size(); i < i_end; ++i)
    eyes->push_back(center - eye_directions->at(i) * distance);

  return virtualScan(eyes, centers, ups, resolution, noise, point_clouds, fovy);
}

double Renderable::virtualScan(const osg::Vec3Array* eye_directions, int resolution, double noise, pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr point_cloud,
    float fovy) {
  std::vector < pcl::PointCloud < pcl::PointXYZRGBNormal > ::Ptr > point_clouds;
  for (size_t i = 0, i_end = eye_directions->size(); i < i_end; ++i)
    point_clouds.push_back(pcl::PointCloud < pcl::PointXYZRGBNormal > ::Ptr(new pcl::PointCloud<pcl::PointXYZRGBNormal>()));

  double grid_size = virtualScan(eye_directions, resolution, noise, point_clouds, fovy);

  point_cloud->clear();
  for (size_t i = 0, i_end = point_clouds.size(); i < i_end; ++i)
    *point_cloud += *(point_clouds[i]);

  return grid_size;
}

