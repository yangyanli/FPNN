#include <boost/filesystem.hpp>

#include <osg/Version>

#include "color_map.h"
#include "osg_utility.h"
#include "osg_viewer_widget.h"

#include "H5Cpp.h"

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif

#include "dense_field.h"

DenseField::DenseField(void) :
    resolution_(0), data_(NULL) {
}

DenseField::DenseField(int resolution) :
    resolution_(resolution), data_(NULL) {
  int voxel_num = resolution_ * resolution_ * resolution_;
  data_ = new float[voxel_num];
  memset(data_, 0, voxel_num * sizeof(float));
}

DenseField::~DenseField(void) {
  delete data_;
}

void DenseField::updateImpl(void) {
  if (resolution_ == 0)
    return;

  float threshold = resolution_/8;
  float scale = 1/(2*threshold);
  osg::ref_ptr < osg::Vec3Array > centers = new osg::Vec3Array;
  osg::ref_ptr < osg::Vec4Array > colors = new osg::Vec4Array;
  std::vector<double> radii;

  osg::Vec3 point;
  for (int i = 0; i < resolution_; ++ i) {
    point.x() = x_min_ + i*step_ + 0.5*step_;
    for (int j = 0; j < resolution_; ++ j) {
      point.y() = y_min_ + j*step_ + 0.5*step_;
      for (int k = 0; k < resolution_; ++ k) {
        point.z() = z_min_ + k*step_ + 0.5*step_;
        float d = at(i, j, k);
        if (d > threshold)
          continue;
        centers->push_back(point);
        float d_gaussian = std::exp(-d*d*scale);
        radii.push_back(d_gaussian*step_/2);
        colors->push_back(ColorMap::getInstance().getContinusColor(d_gaussian));
      }
    }
  }

  content_root_->addChild(OSGUtility::drawSpheres(centers, radii, colors));

  return;
}

bool DenseField::readHdf5File(const std::string& filename) {
  try {
    H5File file(filename, H5F_ACC_RDONLY);

    FloatType data_type(PredType::NATIVE_FLOAT);
    data_type.setOrder(H5T_ORDER_LE);

    DataSet data_set = file.openDataSet("DenseField");
    DataSpace data_space = data_set.getSpace();
    const int dim = 3;
    hsize_t dims[dim];
    data_space.getSimpleExtentDims(dims, NULL);
    resolution_ = dims[0];
    delete data_;
    int voxel_num = resolution_ * resolution_ * resolution_;
    data_ = new float[voxel_num];
    data_set.read(data_, data_type);

    DataSet data_set_meta = file.openDataSet("Meta");
    const int dim_meta = 256;
    float data_meta[dim_meta];
    data_set_meta.read(data_meta, data_type);
    step_ = data_meta[0];
    x_min_ = data_meta[1];
    y_min_ = data_meta[2];
    z_min_ = data_meta[3];
  } catch (FileIException error) {
    error.printError();
    return false;
  } catch (DataSetIException error) {
    error.printError();
    return false;
  } catch (DataSpaceIException error) {
    error.printError();
    return false;
  } catch (DataTypeIException error) {
    error.printError();
    return false;
  }

  return true;
}

bool DenseField::load(const std::string& filename, OSGViewerWidget* osg_viewer_widget) {
  QWriteLocker locker(&read_write_lock_);
  expired_ = true;

  if (!boost::filesystem::exists(filename))
    return false;

  bool flag = false;
  std::string extension = boost::filesystem::path(filename).extension().string();
  if (extension == ".h5") {
    flag = readHdf5File(filename);
  }

  locker.unlock();
  if (flag && osg_viewer_widget != nullptr) {
    osg_viewer_widget->centerScene();
  }

  return flag;
}

bool DenseField::saveHdf5File(const std::string& filename) {
  try {
    H5File file(filename, H5F_ACC_TRUNC);

    FloatType data_type(PredType::NATIVE_FLOAT);
    data_type.setOrder(H5T_ORDER_LE);
 
    const int dim = 3;
    hsize_t dims[dim];
    dims[0] = resolution_;
    dims[1] = resolution_;
    dims[2] = resolution_;
    DataSpace data_space(dim, dims);
    DataSet data_set = file.createDataSet("DenseField", data_type, data_space);
    data_set.write(data_, data_type);

    hsize_t dims_meta[1];
    const int dim_meta = 256;
    dims_meta[0] = dim_meta;
    DataSpace data_space_meta(1, dims_meta);
    DataSet data_set_meta = file.createDataSet("Meta", data_type, data_space_meta);
    float data_meta[dim_meta];
    data_meta[0] = step_;
    data_meta[1] = x_min_;
    data_meta[2] = y_min_;
    data_meta[3] = z_min_;
    data_set_meta.write(data_meta, data_type);
  } catch (FileIException error) {
    error.printError();
    return false;
  } catch (DataSetIException error) {
    error.printError();
    return false;
  } catch (DataSpaceIException error) {
    error.printError();
    return false;
  } catch (DataTypeIException error) {
    error.printError();
    return false;
  }

  return true;
}

bool DenseField::save(const std::string& filename) {
  QReadLocker locker(&read_write_lock_);
  expired_ = true;

  bool flag = false;
  std::string extension = boost::filesystem::path(filename).extension().string();
  if (extension == ".h5") {
    flag = saveHdf5File(filename);
  }

  return flag;
}
