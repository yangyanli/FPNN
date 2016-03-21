#include <osg/Geode>
#include <osg/Geometry>

#include <boost/filesystem.hpp>

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/io/obj_io.h>
#include <pcl/common/common.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/search/impl/flann_search.hpp>

#include "color_map.h"
#include "cgal_types.h"
#include "osg_utility.h"
#include "dense_field.h"

#include "point_cloud.h"

PointCloud::PointCloud(void) :
    data_(new PclPointCloud), tree_(new pcl::search::FlannSearch<PclPoint>(false)), show_normals_(false), color_mode_(PointCloudColorMode::UNIFORM) {
}

PointCloud::PointCloud(PclPointCloud::Ptr data, PclSearchTree::Ptr tree) :
    data_(data), tree_(tree), show_normals_(false), color_mode_(PointCloudColorMode::UNIFORM) {
}

PointCloud::~PointCloud(void) {
}

void PointCloud::setSensorInformation(const osg::Vec3& eye, const osg::Vec3& direction) {
  Eigen::Vector3f z_negative(0.0f, 0.0f, -1.0f);

  data_->sensor_origin_ = Eigen::Vector4f(eye.x(), eye.y(), eye.z(), 0.0f);
  data_->sensor_orientation_.setFromTwoVectors(z_negative, Eigen::Vector3f(direction.x(), direction.y(), direction.z()));

  return;
}

bool PointCloud::load(const std::string& filename) {
  if (!boost::filesystem::exists(filename))
    return false;

  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  bool success = false;
  std::string extension = boost::filesystem::path(filename).extension().string();
  if (extension == ".pcd")
    success = (pcl::io::loadPCDFile(filename, *data_) == 0);
  else if (extension == ".ply")
    success = (pcl::io::loadPLYFile(filename, *data_) == 0);
  else if (extension == ".obj")
    success = (pcl::io::loadOBJFile(filename, *data_) == 0);

  setMatrix(osg::Matrix::identity());
  buildTree();

  return success;
}

void PointCloud::buildTree(void) {
  if (!data_->empty())
    tree_->setInputCloud(data_);

  return;
}

bool PointCloud::save(const std::string& filename) {
  if (data_->empty())
    return false;

  QReadLocker locker(&read_write_lock_);

  bool success = false;
  std::string extension = boost::filesystem::path(filename).extension().string();

  if (getMatrix().isIdentity()) {
    if (extension == ".pcd") {
      pcl::PCDWriter pcd_writer;
      success = (pcd_writer.writeBinaryCompressed < PclPoint > (filename, *data_) != 0);
    } else if (extension == ".ply") {
      pcl::PLYWriter ply_writer;
      success = (ply_writer.write < PclPoint > (filename, *data_, true) != 0);
    }
  } else {
    Eigen::Matrix4d transformation = PclMatrixCaster<osg::Matrix>(getMatrix());
    PclPointCloud::Ptr data_transformed(new PclPointCloud);
    pcl::transformPointCloudWithNormals(*data_, *data_transformed, transformation);
    if (extension == ".pcd") {
      pcl::PCDWriter pcd_writer;
      success = (pcd_writer.writeBinaryCompressed < PclPoint > (filename, *data_transformed) != 0);
    } else if (extension == ".ply") {
      pcl::PLYWriter ply_writer;
      success = (ply_writer.write < PclPoint > (filename, *data_transformed, true) != 0);
    }
  }

  return success;
}

void PointCloud::updateImpl() {
  size_t partition_size = 10000;
  size_t item_num = data_->size();
  size_t partition_num = (item_num + partition_size - 1) / partition_size;
  osg::Geode* geode = new osg::Geode;
  for (size_t i = 0, i_end = partition_num; i < i_end; ++i) {
    size_t j_end = (i + 1) * partition_size;
    j_end = (j_end > item_num) ? item_num : j_end;

    osg::ref_ptr < osg::Vec3Array > vertices = new osg::Vec3Array;
    osg::ref_ptr < osg::Vec3Array > normals = new osg::Vec3Array;
    osg::ref_ptr < osg::Vec4Array > colors = new osg::Vec4Array;
    size_t vertex_num = j_end - i * partition_size;
    vertices->reserve(vertex_num);
    normals->reserve(vertex_num);
    colors->reserve(vertex_num);
    for (size_t j = i * partition_size; j < j_end; ++j) {
      const PclPoint& point = data_->at(j);
      vertices->push_back(osg::Vec3(point.x, point.y, point.z));
      normals->push_back(osg::Vec3(point.normal_x, point.normal_y, point.normal_z));
      colors->push_back(getColor(j));
    }

    osg::ref_ptr < osg::Geometry > geometry = new osg::Geometry;
    geometry->setUseDisplayList(true);
    geometry->setVertexArray(vertices);
    geometry->setNormalArray(normals);
    normals->setBinding(osg::Array::BIND_PER_VERTEX);
    geometry->setColorArray(colors);
    colors->setBinding(osg::Array::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices->size()));
    geode->addDrawable(geometry);
  }
  content_root_->addChild(geode);

  if (show_normals_)
    renderNormals();

  return;
}

void PointCloud::toggleShowNormals(void) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  show_normals_ = !show_normals_;

  return;
}

void PointCloud::renderNormals() {
  double normal_len = 0.05;

  size_t partition_size = 10000;
  size_t item_num = data_->size();
  size_t jump = 10;
  if (item_num < 2000)
    jump = 1;
  size_t partition_num = (item_num + partition_size - 1) / partition_size;
  osg::Geode* geode = new osg::Geode;
  for (size_t i = 0, i_end = partition_num; i < i_end; ++i) {
    osg::ref_ptr < osg::Vec3Array > vertices = new osg::Vec3Array;
    osg::ref_ptr < osg::Vec4Array > colors = new osg::Vec4Array;

    size_t j_end = (i + 1) * partition_size;
    j_end = (j_end > item_num) ? item_num : j_end;
    for (size_t j = i * partition_size; j < j_end; j += jump) {
      const PclPoint& point = data_->at(j);

      vertices->push_back(osg::Vec3(point.x, point.y, point.z));
      vertices->push_back(osg::Vec3(point.x, point.y, point.z));
      vertices->back() = vertices->back() + osg::Vec3(point.normal_x, point.normal_y, point.normal_z) * normal_len;

      osg::Vec4 color = getColor(j);
      colors->push_back(color);
      colors->push_back(color);
    }

    osg::ref_ptr < osg::Geometry > geometry = new osg::Geometry;
    geometry->setUseDisplayList(true);
    geometry->setVertexArray(vertices);
    geometry->setColorArray(colors);
    colors->setBinding(osg::Array::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vertices->size()));
    geode->addDrawable(geometry);
  }
  content_root_->addChild(geode);

  return;
}

void PointCloud::setColorMode(PointCloudColorMode color_mode) {
  if (color_mode == color_mode_)
    return;

  QWriteLocker locker(&read_write_lock_);
  expired_ = true;
  color_mode_ = color_mode;

  return;
}

osg::Vec4 PointCloud::getColor(const PclPoint& point, PointCloudColorMode color_mode) const {
  osg::Vec4 gray(0.5f, 0.5f, 0.5f, 1.0f);
  switch (color_mode) {
  case PointCloudColorMode::ORIGINAL:
    return osg::Vec4(point.r / 255.0, point.g / 255.0, point.b / 255.0, 1.0);
  case PointCloudColorMode::UNIFORM:
    return ColorMap::getInstance().getNamedColor(ColorMap::LIGHT_BLUE);
  }

  return osg::Vec4(point.r / 255.0, point.g / 255.0, point.b / 255.0, 1.0);
}

osg::Vec4 PointCloud::getColor(size_t idx) const {
  const PclPoint& point = data_->at(idx);
  return getColor(point, color_mode_);
}

void PointCloud::estimateNormals(double normal_estimation_radius) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  PointCloud::estimateNormals(data_, tree_, normal_estimation_radius);

  return;
}

void PointCloud::estimateCurvatures(double normal_estimation_radius) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  PointCloud::estimateCurvatures(data_, tree_, normal_estimation_radius);

  return;
}

void PointCloud::orientNormals(int normal_orientation_neighbor_num) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  PointCloud::orientNormals(data_, normal_orientation_neighbor_num);

  return;
}

void PointCloud::flipAllNormals(void) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  for (size_t i = 0, i_end = data_->size(); i < i_end; ++i) {
    PclPoint& point = data_->at(i);
    point.normal_x = -point.normal_x;
    point.normal_y = -point.normal_y;
    point.normal_z = -point.normal_z;
  }

  return;
}

void PointCloud::voxelGridFilter(double grid_size, bool use_original_points) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  if (use_original_points) {
    PointCloud::voxelGridFilter(data_, tree_, grid_size);
  } else {
    PointCloud::voxelGridFilter(data_, grid_size);
  }
  tree_->setInputCloud(data_);

  return;
}

std::string PointCloud::getInformation(void) const {
  return Common::int2String(data_->size() / 1000, 5) + "K Points\n";
}

bool PointCloud::buildDistanceField(DenseField* distance_field) {
  QWriteLocker locker(&(distance_field->getReadWriteLock()));

  PclPoint min_pt, max_pt;
  pcl::getMinMax3D(*data_, min_pt, max_pt);

  double x_center = (min_pt.x+max_pt.x)/2;
  double y_center = (min_pt.y+max_pt.y)/2;
  double z_center = (min_pt.z+max_pt.z)/2;

  double padding_scale = 1.25;
  double x_range = max_pt.x-min_pt.x;
  double y_range = max_pt.y-min_pt.y;
  double z_range = max_pt.z-min_pt.z;
  double range = padding_scale*std::max(x_range, std::max(y_range, z_range));

  double x_min = x_center-range/2;
  double y_min = y_center-range/2;
  double z_min = z_center-range/2;
  distance_field->setCorner(x_min, y_min, z_min);

  int resolution = distance_field->getResolution();
  double step = range/resolution;
  distance_field->setStep(step);

  // Build a potentially sparser search tree for computing distance field
  double grid_size = step/2;
  pcl::VoxelGrid<PclPoint> voxel_grid;
  voxel_grid.setDownsampleAllData(true);
  voxel_grid.setInputCloud(data_);
  voxel_grid.setLeafSize(grid_size, grid_size, grid_size);
  PclPointCloud::Ptr data_filtered(new PclPointCloud);
  voxel_grid.filter(*data_filtered);
  PclSearchTree::Ptr search_tree(new pcl::search::FlannSearch<PclPoint>());
  search_tree->setInputCloud(data_filtered); 

  double scale = resolution/range;
  std::vector<int> neighbor_indices(1);
  std::vector<float> neighbor_distances(1);
  PclPoint query;
  for (int i = 0; i < resolution; ++ i) {
    query.x = x_min + i*step + 0.5*step;
    for (int j = 0; j < resolution; ++ j) {
      query.y = y_min + j*step + 0.5*step;
      for (int k = 0; k < resolution; ++ k) {
        query.z = z_min + k*step + 0.5*step;
        search_tree->nearestKSearch(query, 1, neighbor_indices, neighbor_distances);
        distance_field->at(i, j, k) = std::sqrt(neighbor_distances[0])*scale;
      }
    }
  }

  locker.unlock();
  distance_field->expire();

  return true;
}
