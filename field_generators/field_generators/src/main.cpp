#include <boost/filesystem.hpp>
#include <QtWidgets/QApplication>
#include <glog/logging.h>
#include <gflags/gflags.h>

#include "command_line.h"
#include "main_window.h"

int main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (!FLAGS_log_dir.empty()) {
    if (!boost::filesystem::exists(FLAGS_log_dir)) {
      boost::filesystem::create_directory(FLAGS_log_dir);
    }
  }
  ::google::InitGoogleLogging("FieldGenerators");
#ifndef NDEBUG
  ::google::SetStderrLogging(google::INFO);
#endif

  if(CommandLine::generateDistanceFields()) {
    return 0;
  }

  // Make Xlib and GLX thread safe under X11
  QApplication::setAttribute(Qt::AA_X11InitThreads);
  QApplication application(argc, argv);

  MainWindow::getInstance()->showMaximized();

  return application.exec();
}
