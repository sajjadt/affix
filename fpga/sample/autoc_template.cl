#pragma OPENCL EXTENSION cl_intel_fpga_host_pipe : enable
#pragma OPENCL EXTENSION cl_intel_channels : enable

// Max of 8 
#ifndef SIMD_SIZE
#define SIMD_SIZE 8
#endif
#define HISTOGRAM 256

#include "common.h"
#include "templates/templates.h"
#include "kernels/pixel_wise.h"
#include "kernels/color.h"

// Channels
CHANNEL(ch_in, uint, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_y_temp, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_y_in[2], uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_u_in, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_v_in, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)

CHANNEL(ch_hist, uint, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_y_out, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_u_out, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_v_out, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_y_final, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_out, uint, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)

// Kernels
// Partition 0
SRC_KERNEL(uint, SIMD_SIZE, ch_in)
//PIXELWISE_IOOO(kernel_rgbx_to_yuv4, SIMD_SIZE, uint, uchar, rgb2yuv_iooo, ch_in, ch_y_temp, ch_u_in, ch_v_in)
PIXELWISE_IOOO(kernel_rgbx_to_yuv4, SIMD_SIZE, uint, uchar, rgb2yuv_float, ch_in, ch_y_temp, ch_u_in, ch_v_in)

REPLICATE(kernel_replicate_y, uchar, SIMD_SIZE, 2, ch_y_temp, ch_y_in)

HIST(kernel_hist, HISTOGRAM, SIMD_SIZE, uchar, uint, ch_y_in[0], ch_hist)
// Parameters will be  passed at runtime
SAVE_KERNEL(kernel_save_y, SIMD_SIZE, uchar, ch_y_in[1])
SAVE_KERNEL(kernel_save_u, SIMD_SIZE, uchar, ch_u_in)
SAVE_KERNEL(kernel_save_v, SIMD_SIZE, uchar, ch_v_in)

// Partition 1
LOAD_KERNEL(kernel_load_y, SIMD_SIZE, uchar, ch_y_out)
LOAD_KERNEL(kernel_load_u, SIMD_SIZE, uchar, ch_u_out)
LOAD_KERNEL(kernel_load_v, SIMD_SIZE, uchar, ch_v_out)

// Extension

#if SIMD_SIZE > 1 
__kernel void kernel_equalize(uint items) {
  int counter;
  TYPE(data, uchar, SIMD_SIZE);
  TYPE(hist_data, uint, SIMD_SIZE);
  TYPE(data_out, uchar, SIMD_SIZE);
  
  uchar __attribute__((memory,
    numreadports(SIMD_SIZE),
    numwriteports(1))) cdf[HISTOGRAM];
  uint __attribute__((memory,
    numreadports(1),
    numwriteports(1))) cdf_sum[HISTOGRAM];
  uint __attribute__((memory,
    numreadports(1),
    numwriteports(1))) cdf_temp[HISTOGRAM];

  float norm_factor = 255.0/items;
  LOG("starting", "equalize")
  LOGP("equalize.items", items)
  
  for(int i = 0 ; i < HISTOGRAM ; i+=SIMD_SIZE){
    hist_data = read_channel_intel(ch_hist);
    for(int j=0 ;j < SIMD_SIZE; j++) {
      cdf_temp[i+j] = hist_data[j];
    }
  }

  cdf_sum[0] = 0;
  for(int j=1 ;j < HISTOGRAM; j++) {
      cdf_sum[j] = cdf_sum[j-1] + cdf_temp[j-1];
  }

  for(int j=0 ; j < HISTOGRAM  ;j++) {
    uchar value = (uchar) ((float)cdf_sum[j] * norm_factor);
    cdf[j] = value;
  }

  counter = 0;
  while (counter < (int)items) {
    data = read_channel_intel(ch_y_out);
    #pragma unroll
    for(int i =0;i < SIMD_SIZE; i++) {
      data_out[i] = __fpga_reg(cdf[__fpga_reg(data[i])]);  
    }
    //write_channel_intel(ch_y_final, data);
    write_channel_intel(ch_y_final, data_out);
    counter += SIMD_SIZE;
  }  
  LOG("finished", "equalize")
}
#else
__kernel void kernel_equalize(uint items) {
  int counter;
  TYPE(data, uchar, 1);
  TYPE(hist_data, uint, 1);
  TYPE(data_out, uchar, 1);
  
  uchar __attribute__((memory,
    numreadports(SIMD_SIZE),
    numwriteports(1))) cdf[HISTOGRAM];
  uint __attribute__((memory,
    numreadports(1),
    numwriteports(1))) cdf_sum[HISTOGRAM];
  uint __attribute__((memory,
    numreadports(1),
    numwriteports(1))) cdf_temp[HISTOGRAM];

  float norm_factor = 255.0/items;
  LOG("starting", "equalize")
  LOGP("equalize.items", items)
  
  for(int i = 0 ; i < HISTOGRAM ; i+=1){
    hist_data = read_channel_intel(ch_hist);
    cdf_temp[i] = hist_data;
  }
  cdf_sum[0] = cdf_temp[0];
  for(int j=1 ;j < HISTOGRAM; j++) {
      cdf_sum[j] = cdf_sum[j-1] + cdf_temp[j];
  }
  for(int j=0 ; j < HISTOGRAM  ;j++) {
    uchar value = (uchar) ((float)cdf_sum[j] * norm_factor);
    cdf[j] = value;
  }
  counter = 0;
  while (counter < (int)items) {
    data = read_channel_intel(ch_y_out);
    data_out = cdf[data];
    write_channel_intel(ch_y_final, data_out);
    counter += 1;
  }  
  LOG("finished", "equalize")
}
#endif

PIXELWISE_IIIO(kernel_yuv4_to_rgbx, SIMD_SIZE, uchar, uint, yuv2rgb_float, ch_y_final, ch_u_out, ch_v_out, ch_out)
SINK_KERNEL(uint, SIMD_SIZE, ch_out)
