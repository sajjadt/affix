#pragma OPENCL EXTENSION cl_intel_fpga_host_pipe : enable
#pragma OPENCL EXTENSION cl_intel_channels : enable

#include "templates/templates.h"
#include "common.h"
#include "kernels/pixel_wise.h"
#include "kernels/stencil.h"

// Max of 32
#ifndef SIMD_SIZE
#define SIMD_SIZE 8
#endif

#ifndef TILE_DIM
#define TILE_DIM 80
#endif

#define HISTOGRAM 256
// Channels
CHANNEL(ch_in, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_blurred, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_x, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_y, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_x_rep[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_y_rep[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_mag, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_phase, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_supp, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_out, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)

// Kernels
SRC_KERNEL(uchar, SIMD_SIZE, ch_in)
STENCIL_KERNEL(kernel_gblur5x5, TILE_DIM, SIMD_SIZE, 5, uchar, uchar, gaussian5x5, ch_in, ch_blurred)
STENCIL_KERNEL_IOO(kernel_sobel3x3, TILE_DIM, SIMD_SIZE, 3, uchar, short, ch_blurred, ch_sobel_x, ch_sobel_y, sobel_x, sobel_y)
REPLICATE_AUTO(kernel_replicate_x, short, SIMD_SIZE, 2, ch_sobel_x, ch_sobel_x_rep)
REPLICATE_AUTO(kernel_replicate_y, short, SIMD_SIZE, 2, ch_sobel_y, ch_sobel_y_rep)
PIXELWISE_IIO(kernel_magnitude, SIMD_SIZE, short, short, magnitude_l2, ch_sobel_x_rep[1], ch_sobel_y_rep[1], ch_mag)
PIXELWISE_IIO(kernel_phase, SIMD_SIZE, short, short, phase_region, ch_sobel_x_rep[0], ch_sobel_y_rep[0], ch_phase)
STENCIL_KERNEL_IIO(kernel_nonmax_suppression, TILE_DIM, SIMD_SIZE, 3, short, short, ch_phase, ch_mag, ch_supp, oriented_suppression)
//TODO: pass parameters through a struct
PIXELWISE_IO(kernel_threshold, SIMD_SIZE, short, uchar, threshold, ch_supp, ch_out)
SINK_KERNEL(uchar, SIMD_SIZE, ch_out)
