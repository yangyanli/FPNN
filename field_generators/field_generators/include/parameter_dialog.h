#pragma once
#ifndef PARAMETER_DIALOG_H_
#define PARAMETER_DIALOG_H_

#include <QDialog>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

class Parameter;

class ParameterModel: public QStandardItemModel {
public:
  ParameterModel(QObject * parent = 0) :
      QStandardItemModel(parent) {
  }
  ParameterModel(int rows, int columns, QObject * parent = 0) :
      QStandardItemModel(rows, columns, parent) {
  }
  virtual ~ParameterModel() {
  }

  Qt::ItemFlags flags(const QModelIndex & index) const {
    return (index.column() == 0) ? (Qt::ItemIsEnabled | Qt::ItemIsSelectable) : QStandardItemModel::flags(index);
  }
};

class ParameterDialog: public QDialog {
  Q_OBJECT
public:
  ParameterDialog(const std::string& title, QWidget* parent = 0);
  virtual ~ParameterDialog(void) {
  }

  void
  addParameter(Parameter* parameter);
  void
  removeParameter(Parameter* parameter);

  virtual int
  exec();

protected:
  std::map<std::string, Parameter*> name_parameter_map_;
  ParameterModel* parameter_model_;

protected slots:
  void
  reset();
};

class ParameterDelegate: public QStyledItemDelegate {
  Q_OBJECT
public:
  ParameterDelegate(std::map<std::string, Parameter*>& parameterMap, QObject *parent = 0);

  QWidget *
  createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

  void
  setEditorData(QWidget *editor, const QModelIndex &index) const;

  void
  setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

  void
  updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
  void
  initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

private:
  Parameter*
  getCurrentParameter(const QModelIndex &index) const;

  std::map<std::string, Parameter*>& parameter_map_;
};

#endif // PARAMETER_DIALOG_H_
