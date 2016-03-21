#pragma once

#ifndef TASK_DISPATCHER_H_
#define TASK_DISPATCHER_H_

#include <list>
#include <vector>
#include <string>

typedef std::function<bool(void)> Task;

class TaskDispatcher {
public:
  TaskDispatcher(void);
  virtual ~TaskDispatcher(void) {
  }

  static TaskDispatcher* getInstance(void);

  void processTasks(std::vector<Task>& tasks, const std::string& task_name);

  void processTasks(std::vector<Task>& tasks, const std::string& task_name, int thread_num);

  void cleanUp(void);

private:
  int global_thread_num_;
  std::list<std::vector<Task>> tasks_queue_;
};

#endif /*TASK_DISPATCHER_H_*/
