#pragma once
#ifndef SCENE_WIDGET_H_
#define SCENE_WIDGET_H_

#include "osg_viewer_widget.h"

class QMenu;
class Scaler;
class Dragger;
class MeshModel;
class PointCloud;
class DenseField;

class SceneWidget: public OSGViewerWidget {
  Q_OBJECT

public:
  SceneWidget(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
  virtual ~SceneWidget();

  virtual QSize sizeHint() const {
    return QSize(256, 256);
  }

  PointCloud* getPointCloud(void);
  void setPointCloud(PointCloud* point_cloud);

  DenseField* getDistanceField(void);
  void setDistanceField(DenseField* distance_field);

protected:
  virtual void prepareContextMenu(QMenu* menu);

public slots:
  void slotOpenPointCloud(void);
  void slotSavePointCloud(void);

  void slotImportMeshModels(void);
  void slotClearMeshModels(void);

  void slotOpenDistanceField(void);
  void slotSaveDistanceField(void);

  void slotEstimateNormals(void);
  void slotEstimateCurvatures(void);
  void slotOrientNormals(void);
  void slotFlipAllNormals(void);
  void slotVoxelGridFilter(void);

  void slotSampleScan(void);
  void slotBuildDistanceField(void);
  void slotVirtualScan(void);

  void slotColorizeOriginal(void);
  void slotColorizeUniform(void);

  void slotTogglePointCloud(void);
  void slotToggleDistanceField(void);
  void slotToggleMeshModels(void);

  void slotToggleShowNormals(void);
  void slotToggleCloudDraggers(void);
  void slotToggleCloudScalers(void);

private:
  osg::ref_ptr<PointCloud> point_cloud_;
  std::vector<osg::ref_ptr<MeshModel>> mesh_models_;
  osg::ref_ptr<DenseField> distance_field_;

  std::vector<osg::ref_ptr<Dragger>> draggers_;
  std::vector<osg::ref_ptr<Scaler>> scalers_;

  void clearMeshModels(void);
  void updateInformation(void);
};

#endif // SCENE_WIDGET_H_
