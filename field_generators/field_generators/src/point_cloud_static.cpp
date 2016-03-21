#include <unordered_set>

#include <pcl/io/pcd_io.h>
#include <pcl/common/common.h>
#include <pcl/common/centroid.h>
#include <pcl/features/feature.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/search/impl/flann_search.hpp>

#include <CGAL/property_map.h>
#include <CGAL/mst_orient_normals.h>

#include "color_map.h"
#include "cgal_types.h"

#include "point_cloud.h"

void PointCloud::estimateNormals(PclPointCloud::Ptr point_cloud, double normal_estimation_radius) {
  PclSearchTree::Ptr search_tree(new pcl::search::FlannSearch<PclPoint>());
  search_tree->setInputCloud(point_cloud);

  estimateNormals(point_cloud, search_tree, normal_estimation_radius);

  return;
}

void PointCloud::estimateNormals(PclPointCloud::Ptr point_cloud, PclSearchTree::Ptr search_tree, double normal_estimation_radius) {
  int point_num = (int) (point_cloud->size());
  unsigned int threads = 0;
#ifdef _OPENMP
#pragma omp parallel for shared (point_cloud) num_threads(threads)
#endif
  for (int i = 0; i < point_num; ++i) {
    PclPoint& point = point_cloud->at(i);

    std::vector<int> neighbor_indices;
    std::vector<float> neighbor_distances;
    if (search_tree->radiusSearch(point, normal_estimation_radius, neighbor_indices, neighbor_distances) >= 3) {
      EIGEN_ALIGN16 Eigen::Matrix3f
      covariance_matrix;
      Eigen::Vector4f xyz_centroid;
      pcl::computeMeanAndCovarianceMatrix(*point_cloud, neighbor_indices, covariance_matrix, xyz_centroid);
      pcl::solvePlaneParameters(covariance_matrix, point.normal_x, point.normal_y, point.normal_z, point.curvature);
    } else {
      point.normal_x = point.normal_y = point.normal_z = point.curvature = 0.0f;
    }
  }

  return;
}

void PointCloud::estimateCurvatures(PclPointCloud::Ptr point_cloud, double normal_estimation_radius) {
  PclSearchTree::Ptr search_tree(new pcl::search::FlannSearch<PclPoint>());
  search_tree->setInputCloud(point_cloud);

  estimateCurvatures(point_cloud, search_tree, normal_estimation_radius);

  return;
}

void PointCloud::estimateCurvatures(PclPointCloud::Ptr point_cloud, PclSearchTree::Ptr search_tree, double normal_estimation_radius) {
  int point_num = (int) (point_cloud->size());
  unsigned int threads = 0;
#ifdef _OPENMP
#pragma omp parallel for shared (point_cloud) num_threads(threads)
#endif
  for (int i = 0; i < point_num; ++i) {
    PclPoint& point = point_cloud->at(i);

    std::vector<int> neighbor_indices;
    std::vector<float> neighbor_distances;
    if (search_tree->radiusSearch(point, normal_estimation_radius, neighbor_indices, neighbor_distances) >= 3) {
      EIGEN_ALIGN16 Eigen::Matrix3f
      covariance_matrix;
      Eigen::Vector4f xyz_centroid;
      pcl::computeMeanAndCovarianceMatrix(*point_cloud, neighbor_indices, covariance_matrix, xyz_centroid);
      float nx, ny, nz;
      pcl::solvePlaneParameters(covariance_matrix, nx, ny, nz, point.curvature);
    } else {
      point.curvature = 0.0f;
    }
  }

  return;
}

void PointCloud::orientNormals(PclPointCloud::Ptr point_cloud, int normal_orientation_neighbor_num) {
  // TODO: implement this part with property map directly on PclPoint
  typedef boost::tuple<CgalPoint, CgalVector, size_t> PointVectorIdTuple;
  std::vector<PointVectorIdTuple> points;
  points.reserve(point_cloud->size());
  for (size_t i = 0, i_end = point_cloud->size(); i < i_end; ++i) {
    const PclPoint& point = point_cloud->at(i);
    CgalPoint cgal_point(point.x, point.y, point.z);
    CgalVector cgal_normal(point.normal_x, point.normal_y, point.normal_z);
    points.push_back(PointVectorIdTuple(cgal_point, cgal_normal, i));
  }

  std::vector<PointVectorIdTuple>::iterator unoriented_points_begin = CGAL::mst_orient_normals(points.begin(), points.end(),
      CGAL::Nth_of_tuple_property_map<0, PointVectorIdTuple>(), CGAL::Nth_of_tuple_property_map<1, PointVectorIdTuple>(), normal_orientation_neighbor_num);

  for (std::vector<PointVectorIdTuple>::iterator it = points.begin(); it != unoriented_points_begin; ++it) {
    const PointVectorIdTuple& point = *it;
    const CgalVector& normal = point.get<1>();
    PclPoint& pcl_point = point_cloud->at(point.get<2>());
    pcl_point.normal_x = normal.x();
    pcl_point.normal_y = normal.y();
    pcl_point.normal_z = normal.z();
  }

  return;
}

void PointCloud::voxelGridFilter(PclPointCloud::Ptr point_cloud, double grid_size) {
  pcl::VoxelGrid<PclPoint> voxel_grid;
  voxel_grid.setDownsampleAllData(true);
  voxel_grid.setInputCloud(point_cloud);
  voxel_grid.setLeafSize(grid_size, grid_size, grid_size);

  PclPointCloud::Ptr point_cloud_filtered(new PclPointCloud);
  voxel_grid.filter(*point_cloud_filtered);

  *point_cloud = *point_cloud_filtered;

  return;
}

void PointCloud::voxelGridFilter(PclPointCloud::Ptr point_cloud, PclSearchTree::Ptr search_tree, double grid_size) {
  pcl::VoxelGrid<PclPoint> voxel_grid;
  voxel_grid.setDownsampleAllData(false);
  voxel_grid.setInputCloud(point_cloud);
  voxel_grid.setLeafSize(grid_size, grid_size, grid_size);

  PclPointCloud::Ptr point_cloud_filtered(new PclPointCloud);
  voxel_grid.filter(*point_cloud_filtered);

  std::vector<int> neighbor_indices(1);
  std::vector<float> neighbor_distances(1);
  for (size_t i = 0, i_end = point_cloud_filtered->size(); i < i_end; ++i) {
    PclPoint& point = point_cloud_filtered->at(i);
    search_tree->nearestKSearch(point_cloud_filtered->at(i), 1, neighbor_indices, neighbor_distances);
    point = point_cloud->at(neighbor_indices[0]);
  }

  *point_cloud = *point_cloud_filtered;

  return;
}

void PointCloud::downSampleFilter(PclPointCloud::Ptr point_cloud, int sample_rate) {
  PclPointCloud::Ptr point_cloud_filtered(new PclPointCloud);

  std::vector < size_t > random_indices(point_cloud->size());
  for (size_t i = 0, i_end = point_cloud->size(); i < i_end; ++i)
    random_indices[i] = i;
  std::random_shuffle(random_indices.begin(), random_indices.end());

  size_t new_size = point_cloud->size() / sample_rate;
  for (size_t i = 0; i < new_size; ++i)
    point_cloud_filtered->push_back(point_cloud->at(random_indices[i]));

  *point_cloud = *point_cloud_filtered;

  return;
}
