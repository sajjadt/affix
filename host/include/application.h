#pragma once
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl2.hpp>

// Affix Internals
#include "common.h"
#include "cut.h"
#include "params.h"
#include "image.h"
#include "host_dev_shared.h"

// OpenCL Interop
#include "cl_manager.h"

// Execution scheduling
#include <future>
#include <chrono>

// Utility
#include <map>
#include <vector>
#include <cassert>
#include <string>
#include <memory>

class Application {
  public:
    Application(std::string name, 
    int num_cuts, 
    int seq_size,
    Color color): name(name), num_cuts(num_cuts), seq_size(seq_size), color(color) {
      std::cout<< "Allocated app " << name << std::endl;
    }

    // Virtual functions

    // Set cut in/out proprties
    virtual void set_cut_data_info(const cv::Mat&) = 0;

    // Process cuts' input and ouptut data
    virtual void pre_process_cut(int, int) = 0;
    virtual void post_process_cut(int, int) = 0;

    // Returs series of events
    //virtual std::vector<std::future<void>> tile_process(int, int, Tile, std::unique_ptr<Task::Scheduler>&) {
    //  std::vector<std::future<void>> futures;
    //  return futures;
    //}

    virtual int get_cut_input_total(int index) {
      return cut_inputs[index].total();
    }
    virtual int get_cut_output_total(int index) {
      return cut_outputs[index].total();
    }

    virtual CutData get_input_data(int) = 0;
    virtual CutData get_output_data(int) = 0;

    const int get_cuts_number() { return num_cuts;}
    const int get_seq_size() { return seq_size;}
    virtual bool need_reordering(int) {
      return false;
    };

    virtual GraphCut get_execution_cut(const cl::Program&, const cl::Pipe&, const cl::Pipe& , int, int) = 0;
    //TODO virtual ~Application();

    Color get_color() {return color;}

  protected:
    std::string name;
    int num_cuts;
    int seq_size;
    // Vector of data per cut per frame
    std::vector<CutData> cut_inputs;
    std::vector<CutData> cut_outputs;
    // Vector of kernels per cut
    std::vector< std::vector<std::string>> app_kernels;
    Color color;
    CLManager * cl_manager; 
};

extern "C" {
  Application* get_app(CLManager * cl_manager);
}
