#pragma once
#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <boost/bimap.hpp>

#include <QColor>
#include <QModelIndex>

#include "abstract_parameter.h"

class QWidget;
class QAbstractItemModel;

class Parameter: public AbstractParameter {
public:
  Parameter(const std::string& name, const boost::any& value) :
      AbstractParameter(name, name, value) {
  }
  virtual ~Parameter(void) {
  }

  virtual QWidget* createEditor(QWidget *parent) = 0;
  virtual void setEditorData(QWidget *editor) = 0;
  virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index);
  virtual std::pair<QVariant, int> toModelData() = 0;
  virtual void getEditorData(QWidget *editor) = 0;
};

class BoolParameter: public Parameter {
public:
  BoolParameter(const std::string& name, bool value) :
      Parameter(name, value) {
  }
  virtual ~BoolParameter() {
  }
  operator bool() const {
    return boost::any_cast<bool>(current_value_);
  }
  virtual std::string valueTip();
  virtual QWidget* createEditor(QWidget *parent);
  virtual void setEditorData(QWidget *editor);
  virtual std::pair<QVariant, int> toModelData();
  virtual void getEditorData(QWidget *editor);
};

class IntParameter: public Parameter {
public:
  IntParameter(const std::string& name, int value, int low, int high, int step = 1) :
      Parameter(name, value), low_(low), high_(high), step_(step) {
  }
  virtual ~IntParameter() {
  }
  operator int() const {
    return boost::any_cast<int>(current_value_);
  }
  virtual std::string valueTip();
  virtual QWidget* createEditor(QWidget *parent);
  virtual void setEditorData(QWidget *editor);
  virtual std::pair<QVariant, int> toModelData();
  int getDefaultValue(void) const {
    return boost::any_cast<int>(default_value_);
  }
  void setLow(int low) {
    low_ = low;
  }
  int getLow(void) const {
    return low_;
  }
  void setHigh(int high) {
    high_ = high;
  }
  int getHigh(void) const {
    return high_;
  }
  void setStep(int step) {
    step_ = step;
  }
  virtual void getEditorData(QWidget *editor);

protected:
  int low_;
  int high_;
  int step_;
};

template<class T>
class EnumParameter: public Parameter {
public:
  EnumParameter(const std::string& name, T value, const boost::bimap<T, std::string>& candidate_string_map) :
      Parameter(name, value), candidate_string_map_(candidate_string_map) {
  }
  virtual ~EnumParameter() {
  }
  operator T() const {
    return boost::any_cast < T > (current_value_);
  }
  virtual std::string valueTip();
  virtual QWidget* createEditor(QWidget *parent);
  virtual void setEditorData(QWidget *editor);
  virtual std::pair<QVariant, int> toModelData();

  void setCandidateStringMap(const boost::bimap<T, std::string>& candidate_string_map) {
    candidate_string_map_ = candidate_string_map;
  }

  virtual void getEditorData(QWidget *editor);

protected:
  boost::bimap<T, std::string> candidate_string_map_;
};

class DoubleParameter: public Parameter {
public:
  DoubleParameter(const std::string& name, double value, double low, double high, double step = 0.01) :
      Parameter(name, value), low_(low), high_(high), step_(step) {
  }
  virtual ~DoubleParameter() {
  }
  operator double() const {
    return boost::any_cast<double>(current_value_);
  }
  virtual std::string valueTip();
  virtual QWidget* createEditor(QWidget *parent);
  virtual void setEditorData(QWidget *editor);
  virtual std::pair<QVariant, int> toModelData();
  double getDefaultValue(void) const {
    return boost::any_cast<double>(default_value_);
  }
  void setLow(double low) {
    low_ = low;
  }
  double getLow(void) const {
    return low_;
  }
  void setHigh(double high) {
    high_ = high;
  }
  double getHigh(void) const {
    return high_;
  }
  void setStep(double step) {
    step_ = step;
  }
  virtual void getEditorData(QWidget *editor);

protected:
  double low_;
  double high_;
  double step_;
};

class ColorParameter: public Parameter {
public:
  ColorParameter(const std::string& name, QColor value) :
      Parameter(name, value) {
  }
  virtual ~ColorParameter() {
  }
  operator QColor() const {
    return boost::any_cast < QColor > (current_value_);
  }
  virtual std::string valueTip();
  virtual QWidget* createEditor(QWidget *parent);
  virtual void setEditorData(QWidget *editor);
  virtual std::pair<QVariant, int> toModelData();
  virtual void getEditorData(QWidget *editor);
};

#include "impl/parameter.hpp"

#endif // PARAMETER_H_
