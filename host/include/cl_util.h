#pragma once

#include "cl_manager.h"
#include "common.h"
#include <vector>

// OpenCL Kernel Arguments
typedef std::pair<int, void*> KernelArg;
typedef std::vector<KernelArg> ArgVector;
typedef std::vector<std::pair<std::string, ArgVector> > KernelInfo;

std::vector<cl::Kernel> setup_kernels(
  const cl::Program& program, 
  const KernelInfo& kernels_info) {

  std::vector<cl::Kernel> kernels;
  cl_int status;

  for (auto& kernel: kernels_info) {
    cl::Kernel new_kernel = cl::Kernel(program, kernel.first.c_str(), &status);
    if(status != CL_SUCCESS) {
      throw std::runtime_error(Formatter() << "failed to create kernel" << kernel.first);
    }

    int i = 0;
    for (auto& arg: kernel.second) {
      status = new_kernel.setArg(i, arg.first, arg.second);
      if(status != CL_SUCCESS) {
        throw std::runtime_error(Formatter() << "failed to set kernel parameter" << kernel.first << i);
      }
      std::cout<<"Set"<<kernel.first << " " << i << std::endl;
      ++i;
    }
    kernels.push_back(new_kernel);
  }
  return kernels;
}



