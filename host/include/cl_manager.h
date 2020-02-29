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

class CLManager {
public:
  CLManager(){}
  void init_cl(const std::string&);
  void cleanup();
  void enqueue_cut(GraphCut);
  void get_device_info();
  void wait_finish(int);
  void program_board(int no);
  cl::Buffer create_fpga_buffer(cl_mem_flags, size_t);
  cl::Buffer create_fpga_buffer(cl_mem_flags, size_t, void*);
private:
  cl::Program createProgramFromBinary(const cl::Context&, const std::string&, const std::vector<cl::Device>&);
  char *loadBinaryFile(const char*, size_t*);
  bool fileExists(const char *);
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
