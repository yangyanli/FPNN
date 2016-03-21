#pragma once
#ifndef MainWindow_H
#define MainWindow_H

#include <string>
#include <cassert>

#include <QMutex>
#include <QMainWindow>

#include "ui_main_window.h"

class SceneWidget;

class MainWindow: public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
  virtual ~MainWindow();

  static MainWindow* getInstance();

  const std::string& getWorkspace(void) const {
    return workspace_;
  }

  SceneWidget* getSceneWidget(void);

  void showInformation(const std::string& information);
  void showStatus(const std::string& status, int timeout = 0);
  void showMessageBox(const std::string& text, const std::string& informative_text);
  void showYesNoMessageBox(const std::string& text, const std::string& informative_text);

public slots:
  void slotShowMessageBox(const QString& text, const QString& informative_text);
  bool slotShowYesNoMessageBox(const QString& text, const QString& informative_text);
  void slotCleanUpTaskDispatcher(void);

  signals:
  void keyDownPressed(void);
  void showInformationRequested(const QString& information);
  void showStatusRequested(const QString& status, int timeout);
  void showMessageBoxRequested(const QString& text, const QString& informative_text);
  void showYesNoMessageBoxRequested(const QString& text, const QString& informative_text);
  
protected:
  virtual void closeEvent(QCloseEvent *event);
  virtual void keyPressEvent(QKeyEvent* event);

private slots:
  bool slotSetWorkspace(void);
  void slotShowInformation(const QString& information);
  void slotShowStatus(const QString& status, int timeout);

private:
  void loadSettings();
  void saveSettings();
  void saveStatusLog();

  Ui::MainWindowClass ui_;
  std::string workspace_;
};

#endif // MainWindow_H
