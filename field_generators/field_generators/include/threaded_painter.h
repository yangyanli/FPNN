#pragma once
#ifndef THREADED_PAINTER_H
#define THREADED_PAINTER_H

#include <QMutex>
#include <QThread>
#include <QPainter>

class QResizeEvent;
class OSGViewerWidget;

class ThreadedPainter: public QObject {
  Q_OBJECT

public:
  ThreadedPainter(OSGViewerWidget *widget);
  virtual ~ThreadedPainter(void);
  void stop();

public slots:
  void start();

protected:
  void timerEvent(QTimerEvent *event);

private:
  QMutex mutex_;
  OSGViewerWidget *gl_widget_;
  bool do_rendering_;
};

#endif // THREADED_PAINTER_H
