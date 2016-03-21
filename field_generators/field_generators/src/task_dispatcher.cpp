#include <QProgressBar>
#include <QFutureWatcher>
#include <QtConcurrentFilter>

#include "singleton.h"
#include "main_window.h"
#include "task_dispatcher.h"

#include "task_dispatcher.h"

TaskDispatcher* TaskDispatcher::getInstance(void) {
  return Singleton<TaskDispatcher>::instance();
}

TaskDispatcher::TaskDispatcher(void) {
  global_thread_num_ = QThreadPool::globalInstance()->maxThreadCount();
}

void TaskDispatcher::processTasks(std::vector<Task>& tasks, const std::string& task_name) {
  processTasks(tasks, task_name, global_thread_num_);
}

void TaskDispatcher::processTasks(std::vector<Task>& tasks, const std::string& task_name, int thread_num) {
  tasks_queue_.push_back(std::vector < Task > (tasks));

  QThreadPool::globalInstance()->setMaxThreadCount(thread_num);

  QProgressBar* progress_bar = new QProgressBar(MainWindow::getInstance());
  progress_bar->setRange(0, tasks.size());
  progress_bar->setValue(0);
  progress_bar->setFormat(QString("%1: %p% completed").arg(task_name.c_str()));
  progress_bar->setTextVisible(true);
  MainWindow::getInstance()->statusBar()->addPermanentWidget(progress_bar);

  MainWindow* main_window = MainWindow::getInstance();
  QFutureWatcher<void>* watcher = new QFutureWatcher<void>(main_window);
  QObject::connect(watcher, SIGNAL(progressValueChanged(int)), progress_bar, SLOT(setValue(int)));
  QObject::connect(watcher, SIGNAL(finished()), progress_bar, SLOT(deleteLater()));
  QObject::connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));
  QObject::connect(watcher, SIGNAL(finished()), main_window, SLOT(slotCleanUpTaskDispatcher()));

  watcher->setFuture(QtConcurrent::filter(tasks_queue_.back(), &Task::operator()));

  return;
}

void TaskDispatcher::cleanUp(void) {
  tasks_queue_.pop_front();
  QThreadPool::globalInstance()->setMaxThreadCount(global_thread_num_);

  return;
}
