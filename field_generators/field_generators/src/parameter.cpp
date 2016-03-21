#include <fstream>
#include <iomanip>

#include <boost/algorithm/string.hpp>

#include <QCheckBox>
#include <QSpinBox>
#include <QColorDialog>

#include "parameter.h"

//////////////////////////////////////////////////////////////////////////////////////////////
void Parameter::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) {
  getEditorData(editor);
  std::pair<QVariant, int> model_data = toModelData();
  model->setData(index, model_data.first, model_data.second);
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::string IntParameter::valueTip() {
  return QString("value range: [%1, %3]").arg(low_).arg(high_).toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////////////
QWidget *
IntParameter::createEditor(QWidget *parent) {
  QSpinBox *editor = new QSpinBox(parent);
  editor->setMinimum(low_);
  editor->setMaximum(high_);
  editor->setSingleStep(step_);

  return editor;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void IntParameter::setEditorData(QWidget *editor) {
  QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
  spinBox->setAlignment(Qt::AlignHCenter);

  int value = int(*this);
  spinBox->setValue(value);
}

//////////////////////////////////////////////////////////////////////////////////////////////
void IntParameter::getEditorData(QWidget *editor) {
  QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
  int value = spinBox->text().toInt();
  current_value_ = value;
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::pair<QVariant, int> IntParameter::toModelData() {
  std::pair<QVariant, int> model_data;
  model_data.first = int(*this);
  model_data.second = Qt::EditRole;
  return (model_data);
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::string BoolParameter::valueTip() {
  return QString("bool value").toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////////////
QWidget *
BoolParameter::createEditor(QWidget *parent) {
  QCheckBox *editor = new QCheckBox(parent);

  return editor;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void BoolParameter::setEditorData(QWidget *editor) {
  QCheckBox *checkBox = static_cast<QCheckBox*>(editor);

  bool value = bool(*this);
  checkBox->setCheckState(value ? (Qt::Checked) : (Qt::Unchecked));
}

//////////////////////////////////////////////////////////////////////////////////////////////
void BoolParameter::getEditorData(QWidget *editor) {
  QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
  bool value = (checkBox->checkState() == Qt::Checked);
  current_value_ = value;
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::pair<QVariant, int> BoolParameter::toModelData() {
  std::pair<QVariant, int> model_data;
  model_data.first = bool(*this);
  model_data.second = Qt::EditRole;
  return (model_data);
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::string DoubleParameter::valueTip() {
  return QString("value range: [%1, %3]").arg(low_).arg(high_).toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////////////
QWidget *
DoubleParameter::createEditor(QWidget *parent) {
  QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
  editor->setMinimum(low_);
  editor->setMaximum(high_);
  editor->setSingleStep(step_);
  editor->setDecimals(6);

  return editor;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void DoubleParameter::setEditorData(QWidget *editor) {
  QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
  spinBox->setAlignment(Qt::AlignHCenter);

  double value = double(*this);
  spinBox->setValue(value);
}

//////////////////////////////////////////////////////////////////////////////////////////////
void DoubleParameter::getEditorData(QWidget *editor) {
  QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
  double value = spinBox->text().toDouble();
  current_value_ = value;
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::pair<QVariant, int> DoubleParameter::toModelData() {
  std::pair<QVariant, int> model_data;
  model_data.first = double(*this);
  model_data.second = Qt::EditRole;
  return (model_data);
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::string ColorParameter::valueTip() {
  return ("Color");
}

//////////////////////////////////////////////////////////////////////////////////////////////
QWidget *
ColorParameter::createEditor(QWidget *parent) {
  QColorDialog *editor = new QColorDialog(parent);

  return editor;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ColorParameter::setEditorData(QWidget *editor) {
  QColorDialog *color_dialog = static_cast<QColorDialog*>(editor);

  QColor value = QColor(*this);
  color_dialog->setCurrentColor(value);

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ColorParameter::getEditorData(QWidget *editor) {
  QColorDialog *color_dialog = static_cast<QColorDialog*>(editor);

  QColor value = color_dialog->currentColor();
  current_value_ = value;
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::pair<QVariant, int> ColorParameter::toModelData() {
  std::pair<QVariant, int> model_data;
  model_data.first = QBrush(QColor(*this));
  model_data.second = Qt::BackgroundRole;
  return (model_data);
}
