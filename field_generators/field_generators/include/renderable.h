#pragma once
#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <mutex>
#include <QReadWriteLock>
#include <osg/BoundingBox>
#include <osg/MatrixTransform>

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

#include "common.h"

typedef pcl::PointXYZRGBNormal PclPoint;
typedef pcl::PointCloud<PclPoint> PclPointCloud;

template<class Matrix>
class PclMatrixCaster {
public:
  PclMatrixCaster(const Eigen::Matrix4d& m) :
      m_(m) {
  }

  PclMatrixCaster(const Matrix& m) {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        m_(i, j) = m(j, i);
  }

  operator Eigen::Matrix4d() const {
    return m_;
  }

  operator Matrix() const {
    Matrix m;
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        m(i, j) = m_(j, i);
    return m;
  }

private:
  Eigen::Matrix4d m_;
};

#define META_Renderable(name) \
  virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const name *>(obj)!=nullptr; } \
  virtual const char* className() const { return #name; } \
  virtual void accept(osg::NodeVisitor& nv) { if (nv.validNodeMask(*this)) { nv.pushOntoNodePath(this); nv.apply(*this); nv.popFromNodePath(); } } \
 
class UpdateCallback;

class Renderable: public osg::MatrixTransform {
public:
  Renderable(void);
  virtual ~Renderable(void);

  META_Renderable(Renderable);

  void expire(void);

  inline bool isHidden(void) const {
    return hidden_;
  }
  virtual void toggleHidden(void);

  QReadWriteLock& getReadWriteLock(void) {
    return read_write_lock_;
  }

  osg::BoundingBox getBoundingBox(void);

  virtual void pickEvent(PickMode pick_mode) {
  }

  double virtualScan(const osg::Vec3Array* eyes, const osg::Vec3Array* centers, const osg::Vec3Array* ups, int resolution, double noise,
      std::vector<pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr>& point_clouds, float fovy = 43.0f);
  double virtualScan(const osg::Vec3Array* eyes, const osg::Vec3Array* centers, const osg::Vec3Array* ups, int resolution, double noise,
      pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr point_cloud, float fovy = 43.0f);
  double virtualScan(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up, int width, int height, double noise,
      pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr point_cloud, float fovy = 43.0f);
  double virtualScan(const osg::Vec3Array* eye_directions, int resolution, double noise,
      std::vector<pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr>& point_clouds, float fovy = 43.0f);
  double virtualScan(const osg::Vec3Array* eye_directions, int resolution, double noise, pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr point_cloud, float fovy = 43.0f);
protected:
  friend class UpdateCallback;
  void update(void);
  friend class ForceUpdateVisitor;
  void forceUpdate(void);
  virtual void updateImpl(void) = 0;

protected:
  mutable QReadWriteLock read_write_lock_;
  osg::ref_ptr<osg::MatrixTransform> content_root_;
  bool expired_;
  bool hidden_;

  static std::mutex mutex_graphics_context_;
};

#endif // RENDERABLE_H
