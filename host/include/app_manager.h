#pragma once
#include <vector>
#include <string>
#include <map>
#include "cut.h"
#include "app_info.h"
#include "application.h"
#include "cl_manager.h"

class ApplicationManager {
public:
  ApplicationManager(const std::string&, int, int, int, CLManager*);
  void init_app(const std::string&, CLManager*, int);
  std::vector<std::future<void>> tile_process(int, Tile);

  // Appliaction graph related
  //std::vector<std::string> get_kernels() {return app_kernels;}

  Application* get_app() {return app;}
private:
  std::vector<std::string> apps;
  std::map<std::string, ApplicationInfo> app_infos;
  int graph_cuts;
  std::string app_path;

  // Parallel worker stuff
  int num_threads;
  int num_workers; // for now they are equal
  //std::unique_ptr<Task::Scheduler> scheduler;

  Application* app;
};
