#pragma once
#ifndef MESH_MODEL_H
#define MESH_MODEL_H

#include "renderable.h"

class OSGViewerWidget;

class MeshModel: public Renderable {
public:
  MeshModel(void);
  virtual ~MeshModel(void);

  META_Renderable(MeshModel)
  ;

  bool load(const std::string& filename, OSGViewerWidget* osg_viewer_widget = nullptr);
  bool save(const std::string& filename);
  bool empty(void) const {
    return vertices_->empty();
  }

  double virtualScan(PclPointCloud::Ptr point_cloud, int resolution, double noise);

  double sampleScan(PclPointCloud::Ptr point_cloud, int resolution, double noise);

  void merge(const MeshModel& mesh_model, osg::Matrix transformation = osg::Matrix::identity());

  void scale(double expected_height);

  void savePOVRay(const std::string& filename, osg::Matrix transformation = osg::Matrix::identity());

  void savePOVRay(const std::string& filename, const std::string& colorname, osg::Matrix transformation = osg::Matrix::identity());

protected:
  virtual void updateImpl(void);

protected:
  osg::ref_ptr<osg::Vec3Array> vertices_;
  osg::ref_ptr<osg::Vec4Array> colors_;
  std::vector<std::vector<int> > faces_;
  osg::ref_ptr<osg::Vec3Array> face_normals_;

private:
  bool readObjFile(const std::string& filename);
  bool readOffFile(const std::string& filename);
  bool readPlyFile(const std::string& filename);
  bool savePlyFile(const std::string& filename);
};

#endif // MESH_MODEL_H
