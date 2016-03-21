#pragma once
#ifndef POINT_CLOUD_H
#define POINT_CLOUD_H

#include <pcl/search/search.h>

#include "common.h"
#include "renderable.h"

typedef pcl::search::Search<PclPoint> PclSearchTree;

class DenseField;

class PointCloud: public Renderable {
public:
  PointCloud(void);
  PointCloud(PclPointCloud::Ptr data, PclSearchTree::Ptr tree);
  virtual ~PointCloud(void);

  META_Renderable(PointCloud)
  ;

  PclPointCloud::Ptr& data(void) {
    return data_;
  }
  PclSearchTree::Ptr& tree(void) {
    return tree_;
  }
  void buildTree(void);

  bool load(const std::string& filename);
  bool save(const std::string& filename);

  void toggleShowNormals(void);
  void setColorMode(PointCloudColorMode color_mode);

  static void downSampleFilter(PclPointCloud::Ptr point_cloud, int sample_rate);

  void voxelGridFilter(double grid_size, bool use_original_points = false);
  static void voxelGridFilter(PclPointCloud::Ptr point_cloud, double grid_size);
  static void voxelGridFilter(PclPointCloud::Ptr point_cloud, PclSearchTree::Ptr search_tree, double grid_size);

  void estimateCurvatures(double normal_estimation_radius);
  static void estimateCurvatures(PclPointCloud::Ptr point_cloud, double normal_estimation_radius);
  static void estimateCurvatures(PclPointCloud::Ptr point_cloud, PclSearchTree::Ptr search_tree, double normal_estimation_radius);

  void estimateNormals(double normal_estimation_radius);
  static void estimateNormals(PclPointCloud::Ptr point_cloud, double normal_estimation_radius);
  static void estimateNormals(PclPointCloud::Ptr point_cloud, PclSearchTree::Ptr search_tree, double normal_estimation_radius);

  void orientNormals(int normal_orientation_neighbor_num);
  static void orientNormals(PclPointCloud::Ptr point_cloud, int normal_orientation_neighbor_num);

  void flipAllNormals(void);

  void setSensorInformation(const osg::Vec3& eye, const osg::Vec3& direction);

  std::string getInformation(void) const;
  PointCloudColorMode getColorMode(void) const {
    return color_mode_;
  }
  osg::Vec4 getColor(const PclPoint& point, PointCloudColorMode color_mode) const;

  bool buildDistanceField(DenseField* distance_field);

protected:
  virtual void updateImpl(void);

  osg::Vec4 getColor(size_t idx) const;

  void renderNormals(void);

private:
  PclPointCloud::Ptr data_;
  PclSearchTree::Ptr tree_;

  bool show_normals_;
  PointCloudColorMode color_mode_;
};

#endif // POINT_CLOUD_H
