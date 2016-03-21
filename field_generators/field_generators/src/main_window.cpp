#include <QToolTip>
#include <QKeyEvent>
#include <QSettings>
#include <QGridLayout>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include "common.h"
#include "singleton.h"
#include "mesh_model.h"
#include "scene_widget.h"
#include "task_dispatcher.h"
#include "parameter_dialog.h"
#include "parameter_manager.h"

#include "main_window.h"

MainWindow* MainWindow::getInstance(void) {
  // The MainWindow can not be deleted by the normal Singleton in a crash free way.
  // Instead, setAttribute(Qt::WA_DeleteOnClose) for it, and make it deleted there.
  return SingletonWithNullDeleter<MainWindow>::instance();
}

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags) :
    workspace_(".") {
  ui_.setupUi(this);

  ParameterManager::getInstance();

  SceneWidget* scene_widget = new SceneWidget(this);
  setCentralWidget(scene_widget);
  scene_widget->startRendering();

  connect(this, SIGNAL(showInformationRequested(const QString&)), this, SLOT(slotShowInformation(const QString&)));
  connect(this, SIGNAL(showStatusRequested(const QString&, int)), this, SLOT(slotShowStatus(const QString&, int)));
  connect(this, SIGNAL(showMessageBoxRequested(const QString&, const QString&)), this, SLOT(slotShowMessageBox(const QString&, const QString&)));
  connect(this, SIGNAL(showYesNoMessageBoxRequested(const QString&, const QString&)), this, SLOT(slotShowYesNoMessageBox(const QString&, const QString&)));

  loadSettings();

  // File
  connect(ui_.actionSetWorkspace, SIGNAL(triggered()), this, SLOT(slotSetWorkspace()));
  connect(ui_.actionOpenPointCloud, SIGNAL(triggered()), scene_widget, SLOT(slotOpenPointCloud()));
  connect(ui_.actionSavePointCloud, SIGNAL(triggered()), scene_widget, SLOT(slotSavePointCloud()));
  connect(ui_.actionOpenDistanceField, SIGNAL(triggered()), scene_widget, SLOT(slotOpenDistanceField()));
  connect(ui_.actionSaveDistanceField, SIGNAL(triggered()), scene_widget, SLOT(slotSaveDistanceField()));
  connect(ui_.actionImportMeshModels, SIGNAL(triggered()), scene_widget, SLOT(slotImportMeshModels()));
  connect(ui_.actionClearMeshModels, SIGNAL(triggered()), scene_widget, SLOT(slotClearMeshModels()));

  // Algorithms
  connect(ui_.actionNormalEstimation, SIGNAL(triggered()), scene_widget, SLOT(slotEstimateNormals()));
  connect(ui_.actionCurvatureEstimation, SIGNAL(triggered()), scene_widget, SLOT(slotEstimateCurvatures()));
  connect(ui_.actionNormalOrientation, SIGNAL(triggered()), scene_widget, SLOT(slotOrientNormals()));
  connect(ui_.actionFlipAllNormals, SIGNAL(triggered()), scene_widget, SLOT(slotFlipAllNormals()));
  connect(ui_.actionVoxelGridFilter, SIGNAL(triggered()), scene_widget, SLOT(slotVoxelGridFilter()));
  connect(ui_.actionVirtualScan, SIGNAL(triggered()), scene_widget, SLOT(slotVirtualScan()));
  connect(ui_.actionSampleScan, SIGNAL(triggered()), scene_widget, SLOT(slotSampleScan()));
  connect(ui_.actionBuildDistanceField, SIGNAL(triggered()), scene_widget, SLOT(slotBuildDistanceField()));

  // Visualization
  connect(ui_.actionTogglePointCloud, SIGNAL(triggered()), scene_widget, SLOT(slotTogglePointCloud()));
  connect(ui_.actionToggleDistanceField, SIGNAL(triggered()), scene_widget, SLOT(slotToggleDistanceField()));
  connect(ui_.actionToggleCloudNormals, SIGNAL(triggered()), scene_widget, SLOT(slotToggleShowNormals()));
  connect(ui_.actionToggleCloudDraggers, SIGNAL(triggered()), scene_widget, SLOT(slotToggleCloudDraggers()));
  connect(ui_.actionToggleCloudScalers, SIGNAL(triggered()), scene_widget, SLOT(slotToggleCloudScalers()));
  connect(ui_.actionToggleMeshModels, SIGNAL(triggered()), scene_widget, SLOT(slotToggleMeshModels()));
  connect(ui_.actionColorizeOriginal, SIGNAL(triggered()), scene_widget, SLOT(slotColorizeOriginal()));
  connect(ui_.actionColorizeUniform, SIGNAL(triggered()), scene_widget, SLOT(slotColorizeUniform()));
}

MainWindow::~MainWindow() {
  saveSettings();

  return;
}

void MainWindow::slotShowInformation(const QString& information) {
  QToolTip::showText(QCursor::pos(), information);
}

void MainWindow::showInformation(const std::string& information) {
  emit showInformationRequested(information.c_str());
}

void MainWindow::slotShowStatus(const QString& status, int timeout) {
  ui_.statusBar->showMessage(status, timeout);
}

void MainWindow::showStatus(const std::string& status, int timeout) {
  emit showStatusRequested(status.c_str(), timeout);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  QMainWindow::closeEvent(event);

  return;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Down) {
    emit keyDownPressed();
  }

  QMainWindow::keyPressEvent(event);

  return;
}

SceneWidget* MainWindow::getSceneWidget(void) {
  return dynamic_cast<SceneWidget*>(centralWidget());
}

bool MainWindow::slotSetWorkspace(void) {
  QString directory = QFileDialog::getExistingDirectory(this, tr("Set Workspace"), workspace_.c_str(), QFileDialog::ShowDirsOnly);

  if (directory.isEmpty())
    return false;

  workspace_ = directory.toStdString();
  saveSettings();

  return true;
}

void MainWindow::loadSettings() {
  QSettings settings("ReDevelop", "ReDevelop");

  workspace_ = settings.value("workspace").toString().toStdString();

  return;
}

void MainWindow::saveSettings() {
  QSettings settings("ReDevelop", "ReDevelop");

  QString workspace(workspace_.c_str());
  settings.setValue("workspace", workspace);

  return;
}

void MainWindow::showMessageBox(const std::string& text, const std::string& informative_text) {
  emit showMessageBoxRequested(text.c_str(), informative_text.c_str());
}

void MainWindow::showYesNoMessageBox(const std::string& text, const std::string& informative_text) {
  emit showYesNoMessageBoxRequested(text.c_str(), informative_text.c_str());
}

void MainWindow::slotShowMessageBox(const QString& text, const QString& informative_text) {
  QMessageBox msg_box;
  msg_box.setText(text);
  msg_box.setInformativeText(informative_text);
  msg_box.setStandardButtons(QMessageBox::Ok);
  msg_box.setDefaultButton(QMessageBox::Ok);
  msg_box.exec();

  return;
}

bool MainWindow::slotShowYesNoMessageBox(const QString& text, const QString& informative_text) {
  QMessageBox msg_box;
  msg_box.setText(text);
  msg_box.setInformativeText(informative_text);
  msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msg_box.setDefaultButton(QMessageBox::Yes);
  int ret = msg_box.exec();

  return (ret == QMessageBox::Yes);
}

void MainWindow::slotCleanUpTaskDispatcher(void) {
  TaskDispatcher::getInstance()->cleanUp();
  return;
}
