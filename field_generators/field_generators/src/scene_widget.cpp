#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QMetaObject>
#include <QElapsedTimer>

#include <osg/LightModel>

#include <glog/logging.h>

#include <boost/thread.hpp>

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/common/common.h>
#include <pcl/common/transforms.h>
#include <pcl/search/impl/flann_search.hpp>

#include "common.h"
#include "scaler.h"
#include "dragger.h"
#include "color_map.h"
#include "cgal_types.h"
#include "mesh_model.h"
#include "main_window.h"
#include "point_cloud.h"
#include "dense_field.h"
#include "information.h"
#include "pick_handler.h"
#include "parameter_manager.h"

#include "scene_widget.h"

SceneWidget::SceneWidget(QWidget * parent, const QGLWidget * shareWidget, Qt::WindowFlags f) :
    OSGViewerWidget(parent, shareWidget, f), point_cloud_(new PointCloud), distance_field_(new DenseField) {
  osg::ref_ptr < osg::LightModel > light_model = new osg::LightModel();
  light_model->setTwoSided(true);
  scene_root_->getOrCreateStateSet()->setAttributeAndModes(light_model.get(), osg::StateAttribute::ON);

  addSceneChild(point_cloud_);
  addSceneChild(distance_field_);

  addEventHandler(new PickHandler());
}

SceneWidget::~SceneWidget() {
}

void SceneWidget::clearMeshModels(void) {
  for (size_t i = 0, i_end = mesh_models_.size(); i < i_end; ++i)
    removeSceneChild(mesh_models_[i]);
  mesh_models_.clear();

  return;
}

void SceneWidget::updateInformation(void) {
  std::string information = point_cloud_->getInformation();
  information_->setText(information);

  if (information_->isHidden())
    information_->toggleHidden();

  return;
}

PointCloud* SceneWidget::getPointCloud(void) {
  return point_cloud_;
}

void SceneWidget::setPointCloud(PointCloud* point_cloud) {
  removeSceneChild(point_cloud_);
  point_cloud_ = point_cloud;
  addSceneChild(point_cloud_);

  return;
}

DenseField* SceneWidget::getDistanceField(void) {
  return distance_field_;
}

void SceneWidget::setDistanceField(DenseField* distance_field) {
  removeSceneChild(distance_field_);
  distance_field_ = distance_field;
  addSceneChild(distance_field_);

  return;
}

void SceneWidget::prepareContextMenu(QMenu* menu) {
  menu->addAction("Open Point Cloud", this, SLOT(slotOpenPointCloud()));
  //QMenu* scene_widget_menu = menu->addMenu("Scene Widget");
  //menu->addMenu(scene_widget_menu);

  OSGViewerWidget::prepareContextMenu(menu);

  return;
}

void SceneWidget::slotOpenPointCloud(void) {
  MainWindow* main_window = MainWindow::getInstance();

  QString filename = QFileDialog::getOpenFileName(main_window, "Open Point Cloud", main_window->getWorkspace().c_str(), "Point Cloud (*.pcd *.ply *.obj)");
  if (filename.isEmpty())
    return;

  if (point_cloud_->load(filename.toStdString())) {
    updateInformation();
    centerScene();
  }

  return;
}

void SceneWidget::slotSavePointCloud(void) {
  MainWindow* main_window = MainWindow::getInstance();

  QString filename = QFileDialog::getSaveFileName(main_window, "Save Point Cloud", main_window->getWorkspace().c_str(), "Point Cloud (*.pcd *.ply)");
  if (filename.isEmpty())
    return;

  //point_cloud_->save(filename.toStdString());
  boost::thread thread(&PointCloud::save, point_cloud_, filename.toStdString());
  thread.detach();

  return;
}

void SceneWidget::slotOpenDistanceField(void) {
  MainWindow* main_window = MainWindow::getInstance();

  QString filename = QFileDialog::getOpenFileName(main_window, "Open Distance Field", main_window->getWorkspace().c_str(), "Distance Field (*.h5)");
  if (filename.isEmpty())
    return;

  if (distance_field_->load(filename.toStdString())) {
    updateInformation();
    centerScene();
  }

  return;
}

void SceneWidget::slotSaveDistanceField(void) {
  MainWindow* main_window = MainWindow::getInstance();

  QString filename = QFileDialog::getSaveFileName(main_window, "Save Distance Field", main_window->getWorkspace().c_str(), "Distance Field (*.h5)");
  if (filename.isEmpty())
    return;

  //distance_field_->save(filename.toStdString());
  boost::thread thread(&DenseField::save, distance_field_, filename.toStdString());
  thread.detach();

  return;
}

void SceneWidget::slotEstimateNormals(void) {
  double normal_estimation_radius = 0;
  if (!ParameterManager::getInstance().getNormalEstimationRadius(normal_estimation_radius))
    return;

  //point_cloud_->estimateNormals(normal_estimation_radius);
  boost::thread thread(static_cast<void (PointCloud::*)(double)>(&PointCloud::estimateNormals), point_cloud_, normal_estimation_radius);
  thread.detach();

  return;
}

void SceneWidget::slotEstimateCurvatures(void) {
  double normal_estimation_radius = 0;
  if (!ParameterManager::getInstance().getNormalEstimationRadius(normal_estimation_radius))
    return;

  //point_cloud_->estimateNormals(normal_estimation_radius);
  boost::thread thread(static_cast<void (PointCloud::*)(double)>(&PointCloud::estimateCurvatures), point_cloud_, normal_estimation_radius);
  thread.detach();

  return;
}

void SceneWidget::slotOrientNormals(void) {
  int normal_orientation_neighbor_num = 0;
  if (!ParameterManager::getInstance().getNormalOrientationNeighborNum(normal_orientation_neighbor_num))
    return;

  //point_cloud_->orientNormals(normal_orientation_neighbor_num);
  boost::thread thread(static_cast<void (PointCloud::*)(int)>(&PointCloud::orientNormals), point_cloud_, normal_orientation_neighbor_num);
  thread.detach();

  return;
}

void SceneWidget::slotFlipAllNormals(void) {
  boost::thread thread(static_cast<void (PointCloud::*)(void)>(&PointCloud::flipAllNormals), point_cloud_);
  thread.detach();

  return;
}

void SceneWidget::slotVoxelGridFilter(void) {
  double voxel_filter_grid_size = 0;
  if (!ParameterManager::getInstance().getVoxelFilterGridSize(voxel_filter_grid_size))
    return;

  //point_cloud_->voxelGridFilter(voxel_filter_grid_size);
  boost::thread thread(static_cast<void (PointCloud::*)(double, bool)>(&PointCloud::voxelGridFilter), point_cloud_, voxel_filter_grid_size, true);
  thread.detach();

  return;
}

void SceneWidget::slotBuildDistanceField(void) {
  MainWindow* main_window = MainWindow::getInstance();

  int resolution;
  if (!ParameterManager::getInstance().getDistanceFieldParameters(resolution))
    return;

  osg::ref_ptr<DenseField> distance_field(new DenseField(resolution));
  point_cloud_->buildDistanceField(distance_field);

  removeSceneChild(distance_field_);
  distance_field_ = distance_field;
  addSceneChild(distance_field_);

  return;
}

void SceneWidget::slotToggleShowNormals(void) {
  point_cloud_->toggleShowNormals();

  return;
}

void SceneWidget::slotToggleMeshModels(void) {
  for (size_t i = 0, i_end = mesh_models_.size(); i < i_end; ++i) {
    if (scene_root_->containsNode(mesh_models_[i].get()))
      removeSceneChild(mesh_models_[i]);
    else
      addSceneChild(mesh_models_[i]);
  }

  return;
}

void SceneWidget::slotColorizeOriginal(void) {
  point_cloud_->setColorMode(PointCloudColorMode::ORIGINAL);

  return;
}

void SceneWidget::slotColorizeUniform(void) {
  point_cloud_->setColorMode(PointCloudColorMode::UNIFORM);

  return;
}

void SceneWidget::slotTogglePointCloud(void) {
  if (scene_root_->containsNode(point_cloud_.get()))
    removeSceneChild(point_cloud_);
  else
    addSceneChild(point_cloud_);

  return;
}

void SceneWidget::slotToggleDistanceField(void) {
  if (scene_root_->containsNode(distance_field_.get()))
    removeSceneChild(distance_field_);
  else
    addSceneChild(distance_field_);

  return;
}

void SceneWidget::slotImportMeshModels(void) {
  MainWindow* main_window = MainWindow::getInstance();

  QStringList filenames = QFileDialog::getOpenFileNames(main_window, "Import Mesh Models", main_window->getWorkspace().c_str(),
      "Mesh Models (*.off *.ply *.obj)");
  if (filenames.isEmpty())
    return;

  bool flag = true;
  std::string failed_list;
  for (QStringList::iterator it = filenames.begin(); it != filenames.end(); ++it) {
    osg::ref_ptr<MeshModel> mesh_model(new MeshModel());
    if (mesh_model->load(it->toStdString())) {
      mesh_models_.push_back(mesh_model);
      addSceneChild(mesh_model);
    } else {
      flag = false;
      LOG(ERROR) << "Importing " << it->toStdString() << " failed!" << std::endl;
      failed_list += it->toStdString() + " ";
    }
  }

  if (!flag) {
    main_window->showMessageBox("Some mesh importing failed!", failed_list);
  }

  centerScene();

  return;
}

void SceneWidget::slotSampleScan(void) {
  MainWindow* main_window = MainWindow::getInstance();

  int resolution;
  if (!ParameterManager::getInstance().getSampleScanParameters(resolution))
    return;

  osg::ref_ptr < PointCloud > point_cloud(new PointCloud);
  for (size_t i = 0, i_end = mesh_models_.size(); i < i_end; ++i) {
    if (i == 0) {
      mesh_models_[i]->sampleScan(point_cloud->data(), resolution, 0.0);
    } else {
      PclPointCloud::Ptr samples(new PclPointCloud);
      mesh_models_[i]->sampleScan(samples, resolution, 0.0);
      point_cloud->data()->insert(point_cloud->data()->end(), samples->begin(), samples->end());
    }
  }

  removeSceneChild(point_cloud_);
  point_cloud_ = point_cloud;
  addSceneChild(point_cloud_);

  return;
}

void SceneWidget::slotVirtualScan(void) {
  MainWindow* main_window = MainWindow::getInstance();

  osg::Vec3 eye, center, up;
  osg::Matrix matrix = getCamera()->getViewMatrix();
  matrix.getLookAt(eye, center, up);

  osg::ref_ptr<PointCloud> virtual_scan(new PointCloud);
  for (size_t i = 0, i_end = mesh_models_.size(); i < i_end; ++i) {
    if (i == 0) {
      mesh_models_[i]->Renderable::virtualScan(eye, center, up, width(), height(), 0.0, virtual_scan->data());
    } else {
      PclPointCloud::Ptr virtual_scan_mesh_model(new PclPointCloud);
      mesh_models_[i]->Renderable::virtualScan(eye, center, up, width(), height(), 0.0, virtual_scan_mesh_model);
      virtual_scan->data()->insert(virtual_scan->data()->end(), virtual_scan_mesh_model->begin(), virtual_scan_mesh_model->end());
    }
  }

  removeSceneChild(point_cloud_);
  point_cloud_ = new PointCloud(virtual_scan->data(), virtual_scan->tree());
  addSceneChild(point_cloud_);

  return;
}

void SceneWidget::slotClearMeshModels(void) {
  clearMeshModels();

  return;
}

void SceneWidget::slotToggleCloudDraggers(void) {
  if (draggers_.empty()) {
    osg::ref_ptr<Dragger> dragger(new Dragger(point_cloud_));
    draggers_.push_back(dragger);
    addSceneChild(dragger);
  } else {
    for (size_t i = 0, i_end = draggers_.size(); i < i_end; ++i) {
      removeSceneChild(draggers_[i]);
    }
    draggers_.clear();
  }

  return;
}

void SceneWidget::slotToggleCloudScalers(void) {
  if (scalers_.empty()) {
    osg::ref_ptr<Scaler> scaler(new Scaler(point_cloud_));
    scalers_.push_back(scaler);
    addSceneChild(scaler);
  } else {
    for (size_t i = 0, i_end = scalers_.size(); i < i_end; ++i) {
      removeSceneChild(scalers_[i]);
    }
    scalers_.clear();
  }
}
