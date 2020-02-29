#pragma OPENCL EXTENSION cl_intel_fpga_host_pipe : enable
#pragma OPENCL EXTENSION cl_intel_channels : enable

#include "templates/templates.h"
#include "common.h"
#include "kernels/pixel_wise.h"
#include "kernels/stencil.h"

// Config
// Max of 32
#ifndef SIMD_SIZE
#define SIMD_SIZE 8
#endif
#ifndef TILE_DIM
#define TILE_DIM 80
#endif

// Channels
CHANNEL(ch_in, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_x, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_y, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_mag, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_census, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_out, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)

// Kernels
SRC_KERNEL(uchar, SIMD_SIZE, ch_in)
STENCIL_KERNEL_IOO(kernel_sobel3x3, TILE_DIM, SIMD_SIZE, 3, uchar, short, ch_in, ch_sobel_x, ch_sobel_y, sobel_x, sobel_y)
PIXELWISE_IIO(kernel_magnitude, SIMD_SIZE, short, short, magnitude_l2, ch_sobel_x, ch_sobel_y, ch_mag)
STENCIL_KERNEL(kernel_census3x3, TILE_DIM, SIMD_SIZE, 3, short, uchar, census, ch_mag, ch_census)
SINK_KERNEL(uchar, SIMD_SIZE, ch_census)
