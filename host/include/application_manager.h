#pragma once
#include <vector>
#include <string>
#include <map>
#include "cut.h"
#include "application_info.h"
#include "application.h"
#include "opencl_manager.h"

class ApplicationManager {
public:
  ApplicationManager(const std::string&, int, int, OpenCLManager*);
  void InitApplication(const std::string&, OpenCLManager*);
  Application* GetAppliaction() {return app;}

private:
  std::vector<std::string> apps;
  std::map<std::string, ApplicationInfo> application_info;
  int graph_cuts;
  std::string application_path;
  Application* app;
};
