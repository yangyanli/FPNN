#pragma once
#ifndef DENSE_FIELD_H
#define DENSE_FIELD_H

#include "renderable.h"

class OSGViewerWidget;

class DenseField: public Renderable {

public:
  DenseField(void);
  DenseField(int resolution);
  virtual ~DenseField(void);

  META_Renderable(DenseField);

  int getResolution(void) const {
    return resolution_;
  }

  double getStep(void) const {
    return step_;
  }
  void setStep(double step) {
    step_ = step;
  }

  void getCorner(double& x_min, double& y_min, double& z_min) const {
    x_min = x_min_;
    y_min = y_min_;
    z_min = z_min_;
  }

  void setCorner(double x_min, double y_min, double z_min) {
    x_min_ = x_min;
    y_min_ = y_min;
    z_min_ = z_min;
  }

  const float& at(int x, int y, int z) const {
    return data_[index(x, y, z)];
  }
  float& at(int x, int y, int z) {
    return data_[index(x, y, z)];
  }

  bool load(const std::string& filename, OSGViewerWidget* osg_viewer_widget = nullptr);
  bool save(const std::string& filename);

protected:
  virtual void updateImpl(void);
  int index(int x, int y, int z) const {
    return (x*resolution_+y)*resolution_+z;
  }

protected:
  int resolution_;
  float *data_;

  double step_;
  double x_min_;
  double y_min_;
  double z_min_;

private:
  bool readHdf5File(const std::string& filename);
  bool saveHdf5File(const std::string& filename);
};

#endif // DENSE_FIELD_H
