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

class CLIOManager {
public:
  CLIOManager() {}
  void init(const cl::Context&);
  void release();
  int init_io();
  void overlapped_io(unsigned char* inbuffer, unsigned char* outbuffer, cl_ulong insize, cl_ulong outsize);
  void io(unsigned char* inbuffer, unsigned char* outbuffer, cl_ulong insize, cl_ulong outsize);
  cl::Pipe read_pipe, write_pipe;

private:	
  static const int packet_sz = 32;
  static const int  KB = 1024;
  static const int  MB = 1024*1024;
  static const int  GB = 1024*1024*1024;
};
