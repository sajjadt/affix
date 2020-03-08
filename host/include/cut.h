#pragma once
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl2.hpp>

// #include "image.h"
#include <opencv2/opencv.hpp>
#include <vector>

struct CutDataSize {
  CutDataSize(){}
  CutDataSize(int no_elements, int pixel_size): 
    no_elements(no_elements), pixel_size(pixel_size) {

    }
  int no_elements;
  int pixel_size;

  int Total() {
    return no_elements*pixel_size;
  }
};

// Collection of cut's input/output mats 
class CutData {
 public:
  explicit CutData(){}
  CutData(const CutData& c): mat(c.mat) {};
  CutData& operator=(const CutData& c) = delete;
  int Total() {
    return mat.total() ; // TODO more than one mat
  }
  int total_bytes() {
    return mat.total() * mat.elemSize() ; // TODO more than one mat
  }
  cv::Mat mat; //one mat for now
  CutDataSize size;
};

class InputCutData: public CutData {};

class OutputCutData: public CutData {};

class GraphCut {
 public:
  // I/O transfer size in bytes
  CutDataSize input_size;
  CutDataSize output_size;

  // These are needed by OpenCL runtime to gets enqueued
  std::vector<cl::Kernel> kernels;
};

class ExecutionGraph {
 public:
  // List of graph cuts
  std::vector<GraphCut> cuts;
};
