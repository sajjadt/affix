#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl2.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h> 
#include <unistd.h> // readlink, chdir

#include <string>
#include <vector>
#include "cut.h"

class OpenCLManager {
public:
  OpenCLManager(){}
  void Init(const std::string&);
  void Cleanup();
  void EnqueueCut(GraphCut);
  void GetDeviceInfo();
  void WaitUntilFinished(int);
  void ConfigureFPGA(int no);
  cl::Buffer CreateFpgaBuffer(cl_mem_flags, size_t);
  cl::Buffer CreateFpgaBuffer(cl_mem_flags, size_t, void*);
private:
  cl::Program CreateProgramFromBinary(const cl::Context&, const std::string&, const std::vector<cl::Device>&);
  char *LoadBinaryFile(const char*, size_t*);
  bool FileExists(const char *);
  const static int MAX_KERNELS = 100;
  const static int MAX_QUEUES = 100;

public:
  cl::Platform platform;
  cl::Device device;
  cl::Context context;
  std::vector<cl::CommandQueue> queues;
  std::vector<cl::Kernel> kernels;
  cl::Program program;
};
