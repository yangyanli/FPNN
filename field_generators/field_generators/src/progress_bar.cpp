#include "progress_bar.h"

ProgressBar::ProgressBar(QWidget * parent) :
    QProgressBar(parent) {

}

ProgressBar::~ProgressBar() {

}

void ProgressBar::setValue(int value) {
  if (value > this->value()) {
    setStyleSheet("QProgressBar::chunk {background-color: #05B8CC;}");
  } else {
    setStyleSheet("QProgressBar::chunk {background-color: #FF0000;}");
  }

  QProgressBar::setValue(value);

  return;
}
