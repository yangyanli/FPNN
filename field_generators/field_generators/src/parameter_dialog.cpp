#include <cassert>
#include <fstream>

#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QAbstractItemView>

#include "parameter_dialog.h"
#include "parameter.h"

//////////////////////////////////////////////////////////////////////////////////////////////
void ParameterDialog::addParameter(Parameter* parameter) {
  if (name_parameter_map_.find(parameter->getName()) == name_parameter_map_.end()) {
    name_parameter_map_.insert(std::make_pair(parameter->getName(), parameter));
  } else {
    assert(false);
  }
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ParameterDialog::removeParameter(Parameter* parameter) {
  std::map<std::string, Parameter*>::iterator name_parameter_map_it = name_parameter_map_.find(parameter->getName());
  if (name_parameter_map_it == name_parameter_map_.end()) {
    assert(false);
  } else {
    name_parameter_map_.erase(name_parameter_map_it);
  }
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
ParameterDialog::ParameterDialog(const std::string& title, QWidget* parent) :
    QDialog(parent), parameter_model_(NULL) {
  setModal(false);
  setWindowTitle(QString(title.c_str()) + " Parameters");
}

//////////////////////////////////////////////////////////////////////////////////////////////
int ParameterDialog::exec() {
  parameter_model_ = new ParameterModel(int(name_parameter_map_.size()), 2, this);

  QStringList headerLabels;
  headerLabels.push_back("Variable Name");
  headerLabels.push_back("Variable Value");
  parameter_model_->setHorizontalHeaderLabels(headerLabels);

  QTableView tableView(this);
  tableView.setModel(parameter_model_);

  size_t currentRow = 0;
  for (std::map<std::string, Parameter*>::iterator it = name_parameter_map_.begin(); it != name_parameter_map_.end(); ++it) {
    QModelIndex name = parameter_model_->index(int(currentRow), 0, QModelIndex());
    parameter_model_->setData(name, QVariant(it->first.c_str()));

    QModelIndex value = parameter_model_->index(int(currentRow), 1, QModelIndex());
    std::pair<QVariant, int> model_data = it->second->toModelData();
    parameter_model_->setData(value, model_data.first, model_data.second);

    currentRow++;
  }

  tableView.setItemDelegate(new ParameterDelegate(name_parameter_map_));

  tableView.horizontalHeader()->setStretchLastSection(true);
  tableView.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  tableView.setShowGrid(true);
  tableView.verticalHeader()->hide();
  tableView.setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView.resizeColumnsToContents();

  int totlen = tableView.columnWidth(0) + tableView.columnWidth(1) + frameSize().width();
  setMinimumWidth(totlen);

  QPushButton* pushButtonReset = new QPushButton("Reset", this);
  QPushButton* pushButtonApply = new QPushButton("Apply", this);
  QPushButton* pushButtonCancel = new QPushButton("Cancel", this);
  pushButtonApply->setDefault(true);

  connect(pushButtonReset, SIGNAL(clicked()), this, SLOT(reset()));
  connect(pushButtonApply, SIGNAL(clicked()), this, SLOT(accept()));
  connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  QGridLayout gridLayout(this);
  gridLayout.addWidget(&tableView, 0, 0, 1, 3);
  gridLayout.addWidget(pushButtonReset, 1, 0);
  gridLayout.addWidget(pushButtonApply, 1, 1);
  gridLayout.addWidget(pushButtonCancel, 1, 2);
  setLayout(&gridLayout);

  int result = QDialog::exec();

  return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ParameterDialog::reset() {
  size_t currentRow = 0;
  for (std::map<std::string, Parameter*>::iterator it = name_parameter_map_.begin(); it != name_parameter_map_.end(); ++it) {
    it->second->reset();

    QModelIndex value = parameter_model_->index(int(currentRow), 1, QModelIndex());
    std::pair<QVariant, int> model_data = it->second->toModelData();
    parameter_model_->setData(value, model_data.first, model_data.second);

    currentRow++;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
Parameter* ParameterDelegate::getCurrentParameter(const QModelIndex &index) const {
  std::map<std::string, Parameter*>::iterator currentParameter = parameter_map_.begin();

  size_t currentRow = 0;
  while (currentRow < index.row() && currentParameter != parameter_map_.end()) {
    ++currentParameter;
    ++currentRow;
  }

  assert(currentParameter != parameter_map_.end());

  return currentParameter->second;
}

//////////////////////////////////////////////////////////////////////////////////////////////
ParameterDelegate::ParameterDelegate(std::map<std::string, Parameter*>& parameterMap, QObject *parent) :
    QStyledItemDelegate(parent), parameter_map_(parameterMap) {
}

//////////////////////////////////////////////////////////////////////////////////////////////
QWidget *
ParameterDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const {
  return getCurrentParameter(index)->createEditor(parent);
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ParameterDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  getCurrentParameter(index)->setEditorData(editor);
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ParameterDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  getCurrentParameter(index)->setModelData(editor, model, index);
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ParameterDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
  editor->setGeometry(option.rect);
}

//////////////////////////////////////////////////////////////////////////////////////////////
void ParameterDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const {
  option->displayAlignment |= Qt::AlignHCenter;
  QStyledItemDelegate::initStyleOption(option, index);
}
