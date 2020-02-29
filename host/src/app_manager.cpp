#include "app_manager.h"
#include "chrono"
#include <iostream>
#include <cassert>
#include "common.h"
#include "application.h"
#include <dlfcn.h>

#include <algorithm>
//using namespace Task;

typedef Application* (*PFN_GET_APP) (CLManager*);

extern void dump_error(const char *str, cl_int status);
constexpr unsigned int hash(const char* const str, int h = 0) {
  return !str[h] ? 5381 : (hash(str, h+1)*33) ^ str[h];
}

//Application* ApplicationManager::app;
void ApplicationManager::init_app(const std::string& app_path, CLManager* cl_manager, int seq_size) {

  void * handle = dlopen(app_path.c_str(), RTLD_LAZY);
  if (handle == NULL) {
    std::cerr << dlerror() << std::endl;
  }

  PFN_GET_APP get_app = reinterpret_cast<PFN_GET_APP>(dlsym(handle, "get_app"));
  if (! get_app) {
    std::cerr << "Function get_app not found" << std::endl;
  }
  this->app = get_app(cl_manager);
}

ApplicationManager::ApplicationManager(
  const std::string& app_path, 
  int _num_workers, 
  int _num_threads, 
  int seq_size,
  CLManager* cl_manager): app_path(app_path), num_workers(_num_workers), num_threads(_num_threads) {
  //scheduler = Task::Module::makeScheduler(num_workers);

  ApplicationManager::init_app(app_path, cl_manager, seq_size);
}


// Tile is streamed consecutively
// Tile: size, dimension, location
// Cut information
// Output ? (multiple output)
//std::vector<std::future<void>> ApplicationManager::tile_process(int cut_no, Tile tile){
//  int num_workers = 1; // TODO
//  return app->tile_process(num_workers, cut_no, tile, scheduler);
//}
