#pragma once
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <QProgressBar>

class ProgressBar: public QProgressBar {
  Q_OBJECT
public:
  ProgressBar(QWidget * parent = 0);
  virtual ~ProgressBar();

public slots:
  virtual void setValue(int value);

protected:

private:
};

#endif // PROGRESS_BAR_H
