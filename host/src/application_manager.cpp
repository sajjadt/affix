#include "application_manager.h"
#include "chrono"
#include <iostream>
#include <cassert>
#include "common.h"
#include "application.h"
#include <dlfcn.h>

#include <algorithm>

typedef Application* (*PFN_GET_APP) (OpenCLManager*);

extern void dump_error(const char *str, cl_int status);
constexpr unsigned int hash(const char* const str, int h = 0) {
  return !str[h] ? 5381 : (hash(str, h+1)*33) ^ str[h];
}

void ApplicationManager::InitApplication(const std::string& application_path, OpenCLManager* cl_manager) {
  void * handle = dlopen(application_path.c_str(), RTLD_LAZY);
  if (handle == NULL) {
    std::cerr << dlerror() << std::endl;
  }

  PFN_GET_APP GetAppliaction = reinterpret_cast<PFN_GET_APP>(dlsym(handle, "GetAppliaction"));
  if (!GetAppliaction) {
    std::cerr << "Function GetAppliaction not found" << std::endl;
  }
  this->app = GetAppliaction(cl_manager);
}

ApplicationManager::ApplicationManager(
  const std::string& application_path, 
  int _num_workers, 
  int _num_threads,
  OpenCLManager* cl_manager):
    application_path(application_path) {
  ApplicationManager::InitApplication(application_path, cl_manager);
}
