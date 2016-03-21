#pragma once
#ifndef PARAMETER_MANAGER_H_
#define PARAMETER_MANAGER_H_

#include <boost/shared_ptr.hpp>

class IntParameter;
class DoubleParameter;
class BoolParameter;
class ParameterDialog;

class ParameterManager {
public:
  static ParameterManager& getInstance() {
    static ParameterManager theSingleton;
    return theSingleton;
  }

  double getDummy(void) const;
  bool getDummy(double& dummy);

  int getVirtualScanResolution(void) const;
  double getVirtualScanNoise(void) const;
  bool getVirtualScanParameters(int& virtual_scan_resolution, double& virtual_scan_noise);

  int getSampleScanResolution(void) const;
  bool getSampleScanParameters(int& sample_scan_resolution);

  int getDistanceFieldResolution(void) const;
  bool getDistanceFieldParameters(int& distance_field_resolution);

  double getNormalEstimationRadius(void) const;
  bool getNormalEstimationRadius(double& normal_estimation_radius);

  int getNormalOrientationNeighborNum(void) const;
  bool getNormalOrientationNeighborNum(int& normal_orientation_neighbor_num);

  double getVoxelFilterGridSize(void) const;
  bool getVoxelFilterGridSize(double& voxel_filter_grid_size);

protected:

private:
  ParameterManager(void);
  ParameterManager(const ParameterManager &) {
  }            // copy ctor hidden
  ParameterManager& operator=(const ParameterManager &) {
    return (*this);
  }   // assign op. hidden
  virtual ~ParameterManager();

  boost::shared_ptr<DoubleParameter> dummy_;

  boost::shared_ptr<IntParameter> mesh_sample_num_;
  boost::shared_ptr<IntParameter> sample_scan_resolution_;

  boost::shared_ptr<IntParameter> distance_field_resolution_;

  boost::shared_ptr<IntParameter> virtual_scan_resolution_;
  boost::shared_ptr<DoubleParameter> virtual_scan_noise_;

  boost::shared_ptr<DoubleParameter> normal_estimation_radius_;
  boost::shared_ptr<IntParameter> normal_orientation_neighbor_num_;
  boost::shared_ptr<DoubleParameter> voxel_filter_grid_size_;
};

#endif // PARAMETER_MANAGER_H_
