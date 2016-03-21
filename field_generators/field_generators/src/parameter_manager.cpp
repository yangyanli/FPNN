#include <iostream>
#include <QTextStream>

#include "parameter.h"
#include "main_window.h"
#include "parameter_dialog.h"
#include "parameter_manager.h"

ParameterManager::ParameterManager(void) :
    dummy_(new DoubleParameter("Dummy Parameter", 0.5, 0.0, 1.0, 0.01)),
    sample_scan_resolution_(new IntParameter("Sample Scan Resolution", 100, 64, 1024, 32)),
    distance_field_resolution_(new IntParameter("Distance Field Resolution", 100, 64, 1024, 32)),
    virtual_scan_resolution_(new IntParameter("Virtual Scan Resolution", 200, 128, 2048, 64)),
    virtual_scan_noise_(new DoubleParameter("Virtual Scan Noise", 0.0, 0.0, 0.04, 0.001)),
    normal_estimation_radius_(new DoubleParameter("Normal Estimation Radius", 0.10, 0.01, 0.5, 0.01)),
    normal_orientation_neighbor_num_(new IntParameter("Normal Orient. Neighbor Num", 8, 4, 128, 1)),
    voxel_filter_grid_size_(new DoubleParameter("Voxel Filter Grid Size", 0.02, 0.001, 0.1, 0.001)) {
}

ParameterManager::~ParameterManager(void) {
}

double ParameterManager::getDummy(void) const {
  return *dummy_;
}

bool ParameterManager::getDummy(double& dummy) {
  ParameterDialog parameter_dialog("Dummy Parameter", MainWindow::getInstance());
  parameter_dialog.addParameter(dummy_.get());
  if (!parameter_dialog.exec() == QDialog::Accepted)
    return false;

  dummy = *dummy_;

  return true;
}

int ParameterManager::getSampleScanResolution(void) const {
  return *sample_scan_resolution_;
}

bool ParameterManager::getSampleScanParameters(int& sample_scan_resolution) {
  ParameterDialog parameter_dialog("Sample Scan Resolution", MainWindow::getInstance());
  parameter_dialog.addParameter(sample_scan_resolution_.get());
  if (!parameter_dialog.exec() == QDialog::Accepted)
    return false;

  sample_scan_resolution = *sample_scan_resolution_;

  return true;
}


int ParameterManager::getDistanceFieldResolution(void) const {
  return *distance_field_resolution_;
}

bool ParameterManager::getDistanceFieldParameters(int& distance_field_resolution) {
  ParameterDialog parameter_dialog("Distance Field Resolution", MainWindow::getInstance());
  parameter_dialog.addParameter(distance_field_resolution_.get());
  if (!parameter_dialog.exec() == QDialog::Accepted)
    return false;

  distance_field_resolution = *distance_field_resolution_;

  return true;
}

int ParameterManager::getVirtualScanResolution(void) const {
  return *virtual_scan_resolution_;
}

double ParameterManager::getVirtualScanNoise(void) const {
  return *virtual_scan_noise_;
}

bool ParameterManager::getVirtualScanParameters(int& virtual_scan_resolution, double& virtual_scan_noise) {
  ParameterDialog parameter_dialog("Virtual Scan Resolution", MainWindow::getInstance());
  parameter_dialog.addParameter(virtual_scan_resolution_.get());
  parameter_dialog.addParameter(virtual_scan_noise_.get());
  if (!parameter_dialog.exec() == QDialog::Accepted)
    return false;

  virtual_scan_resolution = *virtual_scan_resolution_;
  virtual_scan_noise = *virtual_scan_noise_;

  return true;
}

double ParameterManager::getNormalEstimationRadius(void) const {
  return *normal_estimation_radius_;
}

bool ParameterManager::getNormalEstimationRadius(double& normal_estimation_radius) {
  ParameterDialog parameter_dialog("Normal Estimation Radius", MainWindow::getInstance());
  parameter_dialog.addParameter(normal_estimation_radius_.get());
  if (!parameter_dialog.exec() == QDialog::Accepted)
    return false;

  normal_estimation_radius = *normal_estimation_radius_;

  return true;
}

int ParameterManager::getNormalOrientationNeighborNum(void) const {
  return *normal_orientation_neighbor_num_;
}

bool ParameterManager::getNormalOrientationNeighborNum(int& normal_orientation_neighbor_num) {
  ParameterDialog parameter_dialog("Normal Orientation Neighbor Num", MainWindow::getInstance());
  parameter_dialog.addParameter(normal_orientation_neighbor_num_.get());
  if (!parameter_dialog.exec() == QDialog::Accepted)
    return false;

  normal_orientation_neighbor_num = *normal_orientation_neighbor_num_;

  return true;
}

double ParameterManager::getVoxelFilterGridSize(void) const {
  return *normal_estimation_radius_;
}

bool ParameterManager::getVoxelFilterGridSize(double& voxel_filter_grid_size) {
  ParameterDialog parameter_dialog("Voxel Filter Grid Size", MainWindow::getInstance());
  parameter_dialog.addParameter(voxel_filter_grid_size_.get());
  if (!parameter_dialog.exec() == QDialog::Accepted)
    return false;

  voxel_filter_grid_size = *voxel_filter_grid_size_;

  return true;
}
