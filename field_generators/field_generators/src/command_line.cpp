#include <tuple>
#include <thread>
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "mesh_model.h"
#include "point_cloud.h"
#include "dense_field.h"

#include "command_line.h"

DEFINE_string(df_list, "", "Path to distance field list");
DEFINE_bool(skip_converting, false, "Skip converting mesh to point cloud");
DEFINE_bool(skip_generation, false, "Skip distance field generation");

namespace CommandLine {

  typedef std::tuple<std::string, int, std::string> DFItem;

  void generateDistanceField(std::vector<DFItem> df_list, int thread_num, int thread_idx) {
    int step = 100;
    int count = 0;
    for (int i = thread_idx, i_end = df_list.size(); i < i_end; i += thread_num) {
      const std::string filename_df = std::get<2>(df_list[i]);
      LOG(INFO) << "Thread " << thread_idx << ": Processing " << filename_df << "..." << std::endl;

      osg::ref_ptr <PointCloud> point_cloud(new PointCloud);
      boost::filesystem::path path(filename_df);
      std::string filename_pcd = path.parent_path().string()+"/"+path.stem().string()+".pcd";
      if(!point_cloud->load(filename_pcd)) {
        LOG(ERROR) << "Thread " << thread_idx << ": Reading " << filename_pcd << " failed! Skipping it..." << std::endl;
        continue;
      }

      int resolution = std::get<1>(df_list[i]);
      osg::ref_ptr <DenseField> distance_field(new DenseField(resolution));
      point_cloud->buildDistanceField(distance_field);

      distance_field->save(filename_df);

      count ++;
      if (count%step == 0) {
        LOG(INFO) << "Thread " << thread_idx << ":  Processed " << count << " items! (total item number: " << i_end << ")" << std::endl;
      }
    }

    return;
  }

  bool generateDistanceFields(void) {
    if(FLAGS_df_list.empty()) {
      return false;
    }
    if (!boost::filesystem::exists(FLAGS_df_list)) {
      return false;
    }

    std::vector<DFItem> df_list;
    std::ifstream fin(FLAGS_df_list);
    std::string source, target;
    int resolution;
    std::set<std::string> folder_set;
    while (fin >> source >> resolution >> target) {
      df_list.push_back(std::make_tuple(source, resolution, target));
      std::string folder = boost::filesystem::path(target).parent_path().string();
      if(folder_set.find(folder) == folder_set.end()) {
        if (!boost::filesystem::exists(folder)) {
          boost::filesystem::create_directories(folder);
          folder_set.insert(folder);
        }
      }
    }
    LOG(INFO) <<  df_list.size() << " items to be processed!" << std::endl;

    if(!FLAGS_skip_converting) {
      int step = 100;
      for (int i = 0, i_end = df_list.size(); i < i_end; i ++) {
        const std::string filename_df = std::get<2>(df_list[i]);
        boost::filesystem::path path(filename_df);
        std::string filename_pcd = path.parent_path().string()+"/"+path.stem().string()+".pcd";

        osg::ref_ptr <PointCloud> point_cloud(new PointCloud);
        if(point_cloud->load(filename_pcd)) {
          LOG(INFO) << "Skipping generating " << filename_pcd << " as it exists and reads well..." << std::endl;
          continue;
        }

        const std::string filename_mesh = std::get<0>(df_list[i]);
        LOG(INFO) << "Converting " << filename_mesh << " into point cloud..." << std::endl;
        osg::ref_ptr <MeshModel> mesh_model(new MeshModel);
        if(!mesh_model->load(filename_mesh)) {
          LOG(ERROR) << "Reading " << filename_mesh << " failed! Skipping it..." << std::endl;
          continue;
        }

        point_cloud->data()->clear();
        double grid_size = mesh_model->sampleScan(point_cloud->data(), 100, 0.0);
        point_cloud->buildTree();
        point_cloud->voxelGridFilter(grid_size/2, true);
        point_cloud->save(filename_pcd);

        if ((i+1)%step == 0) {
          LOG(INFO) << "Converted " << (i+1) << " items! (total item number: " << i_end << ")" << std::endl;
        }
      }
    }

    if (!FLAGS_skip_generation) {
      unsigned int n = std::thread::hardware_concurrency()-4;
      LOG(INFO) << n << " threads will be used!" << std::endl;

      std::vector<std::thread> threads;
      for (unsigned int i = 0; i < n; ++ i) {
        std::thread thread_obj(generateDistanceField, df_list, n, i);
        threads.push_back(std::move(thread_obj));
      }

      for (unsigned int i = 0; i < n; ++ i) {
        threads[i].join();
      }
      LOG(INFO) << "Distance field generation done!" << std::endl;
    }

    return true;
  }

}
