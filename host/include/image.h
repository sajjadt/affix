#pragma once
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <memory>
#include <assert.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include "common.h"

#define AOCL_ALIGNMENT 64

class Dimension {
public:
  int rows;
  int cols;
  int Total() const {return rows*cols;}
};

enum Color {
  RGBX, GRAYSCALE
};

static cv::Mat do_reorder(const cv::Mat& src, const int tile_width, bool forward) {

  if(tile_width > src.cols)
    throw std::runtime_error(Formatter() << "Can't reorder the image! "<< tile_width <<" >= " << src.cols);
  
  if(tile_width == src.cols)
    return src;

  int written_bytes = 0;
  cv::Mat dst(src.rows, src.cols, src.type()); 

  unsigned char* src_ptr = src.data;
  unsigned char* dst_ptr = dst.data;
  
  if (forward) { // reorder
    // Iterate over tiles
    for(int j =0 ;j < src.cols; j+=tile_width) {
      for (int i = 0; i < src.rows; i+=1) {
        // Copy a sub-row
        int offset = i*(src.cols)+j;
        memcpy(dst_ptr+written_bytes, src_ptr+offset*src.elemSize(), tile_width*src.elemSize());
        written_bytes += tile_width*src.elemSize();
      }
    }
  } else {
    for (int i = 0; i < src.rows*tile_width; i+=tile_width) {
      for(int j =0 ;j < src.cols/tile_width; j+=1) {
        // Copy a sub-row
        int offset = j*(src.rows)*tile_width+i;
        memcpy(dst_ptr+written_bytes, src_ptr+offset*src.elemSize(), tile_width*src.elemSize());
        written_bytes += tile_width*src.elemSize();
      }
    }
  }
  return dst;
}