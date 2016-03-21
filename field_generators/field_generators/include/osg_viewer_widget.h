#pragma once
#ifndef OSG_VIEWER_WIDGET_H
#define OSG_VIEWER_WIDGET_H

#include <QMutex>
#include <QThread>
#include <osgViewer/Viewer>
#include <osg/BoundingSphere>

#include "threaded_painter.h"
#include "adapter_widget.h"

class QMenu;
class Information;
class AxisIndicator;

namespace osg {
class Node;
}

class OSGViewerWidget: public AdapterWidget, public osgViewer::Viewer {
  Q_OBJECT
public:
  OSGViewerWidget(QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
  virtual ~OSGViewerWidget();

  void startRendering();
  void stopRendering();

  void addSceneChild(osg::Node *child);
  void addOtherChild(osg::Node *child);
  void removeSceneChild(osg::Node *child);
  void removeOtherChild(osg::Node *child);
  void removeSceneChildren(void);
  void replaceSceneChild(osg::Node* old_child, osg::Node* new_child);
  bool hasSceneChild(osg::Node* child);
  bool hasOtherChild(osg::Node* child);

  osg::BoundingSphere getBoundingSphere(void) const;
  osg::BoundingBox getBoundingBox(void) const;
  Information* getInformation(void) {
    return information_;
  }

  void setUpVector(osg::Vec3 up_vector) {
    up_vector_ = up_vector;
  }
  const osg::Vec3& getUpVector(void) const {
    return up_vector_;
  }

  virtual void eventTraversal();public slots:
  void increasePointSize(void);
  void decreasePointSize(void);
  void increaseLineWidth(void);
  void decreaseLineWidth(void);
  void slotSaveCamera(void);
  void slotLoadCamera(void);
  void centerScene(void);

protected:
  virtual void paintGL(void);
  virtual void paintEvent(QPaintEvent *event);
  virtual void closeEvent(QCloseEvent *event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void prepareContextMenu(QMenu* menu);

  osg::ref_ptr<osg::Group> scene_root_;
  osg::ref_ptr<osg::Group> other_root_;
  QThread gl_thread_;
  ThreadedPainter threaded_painter_;
  QMutex mutex_;

  static QMutex mutex_first_frame_;
  bool first_frame_;

  osg::ref_ptr<Information> information_;
  osg::ref_ptr<AxisIndicator> axis_indicator_;

  osg::Vec3 up_vector_;
};

#endif // OSG_VIEWER_WIDGET_H
