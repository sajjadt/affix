#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl2.hpp>

// AFFIX Internals
#include "common.h"
#include "cut.h"
#include "params.h"
#include "image.h"
#include "host_dev_shared.h"

// OpenCL Interop
#include "opencl_manager.h"

// Utility
#include <map>
#include <vector>
#include <cassert>
#include <string>
#include <memory>

class Application {
 public:
  Application(std::string name, int num_cuts, int seq_size, Color color) : 
    name(name), num_cuts(num_cuts), seq_size(seq_size), color(color) {}

  virtual void SetCutDataInfo(const cv::Mat&) = 0;
  virtual void PreProcessCut(int, int) = 0;
  virtual void PostProcessCut(int, int) = 0;
  virtual int GetCutInputTotal(int index) {
    return cut_inputs[index].Total();
  }
  virtual int GetCutOutputTotal(int index) {
    return cut_outputs[index].Total();
  }

  virtual CutData GetInputData(int) = 0;
  virtual CutData GetOutputData(int) = 0;

  const int GetCutsNumber() { return num_cuts;}
  const int GetSeqSize() { return seq_size;}
  virtual bool NeedReordering(int) {
    return false;
  };

  virtual GraphCut GetExecutionCut(const cl::Program&, const cl::Pipe&, const cl::Pipe& , int, int) = 0;

  Color GetColor() {return color;}

 protected:
  std::string name;
  int num_cuts;
  int seq_size;

  // Vector of data per cut per frame
  std::vector<CutData> cut_inputs;
  std::vector<CutData> cut_outputs;

  // Vector of kernels per cut
  std::vector<std::vector<std::string>> application_kernels;
  Color color;
  OpenCLManager * cl_manager; 
};

extern "C" {
  Application* GetAppliaction(OpenCLManager * cl_manager);
}
