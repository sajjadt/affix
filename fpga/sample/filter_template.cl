#pragma OPENCL EXTENSION cl_intel_fpga_host_pipe : enable
#pragma OPENCL EXTENSION cl_intel_channels : enable

#include "templates/templates.h"
#include "common.h"
#include "kernels/stencil.h"
#include "kernels/pixel_wise.h"

#ifndef SIMD_SIZE
#define SIMD_SIZE 8
#endif
#ifndef TILE_DIM
#define TILE_DIM 128
#endif

// Channels
CHANNEL(ch_source, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_filter, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)

// Kernels
SRC_KERNEL(uchar, SIMD_SIZE, ch_source)

#if defined F7X7
constant short coeffs[7][7] = {
    {-7, -87, -5, -128 ,-5, -87, -7},
    {-87, 64, -236, 290, -236, 64, -87},
    {-5, -236, 526, -118, 526, -236, -5},
    {-128, 290, -118, 1140, -118,290,-128},
    {-5, -236, 526, -118, 526, -236, -5},
    {-87, 64, -236, 290, -236, 64, -87},
    {-7, -87, -5, -128 ,-5, -87, -7}
  };
// General form
STENCIL_KERNEL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 7, uchar, uchar, filter7, ch_source, ch_filter, coeffs, 10)
#elif defined F7X7SYMM
constant short coeffs[7][7] = {
    {-7, -87, -5, -128 ,-5, -87, -7},
    {-87, 64, -236, 290, -236, 64, -87},
    {-5, -236, 526, -118, 526, -236, -5},
    {-128, 290, -118, 1140, -118,290,-128},
    {-5, -236, 526, -118, 526, -236, -5},
    {-87, 64, -236, 290, -236, 64, -87},
    {-7, -87, -5, -128 ,-5, -87, -7}
  };
// General form
STENCIL_KERNEL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 7, uchar, uchar, filter_symm7, ch_source, ch_filter, coeffs, 10)
#elif defined G5X5
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 5, uchar, uchar, gaussian5x5, ch_source, ch_filter)
#elif defined G5X5ROW
constant short coeffs[5] = {1, 4, 6, 4, 1};
STENCIL_KERNEL_ROW_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 5, uchar, uchar, row_filter5, ch_source, ch_filter, coeffs, 4)
#elif defined G5X5COL
constant short coeffs[5] = {1, 4, 6, 4, 1};
STENCIL_KERNEL_COL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 5, uchar, uchar, col_filter5, ch_source, ch_filter, coeffs, 4)
#elif defined G5X5SEP
constant short coeffs[5] = {1, 4, 6, 4, 1};
CHANNEL(ch_filter0, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
STENCIL_KERNEL_COL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 5, uchar, uchar, col_filter_5, ch_source, ch_filter0, coeffs, 4)
STENCIL_KERNEL_ROW_GEN(kernel_filter1, TILE_DIM, SIMD_SIZE, 5, uchar, uchar, row_filter_5, ch_filter0, ch_filter, coeffs, 4)
#elif defined G7X7SEP
constant short coeffs[7] = {1, 6, 15, 20, 15, 6, 1};
CHANNEL(ch_filter0, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
STENCIL_KERNEL_COL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 7, uchar, uchar, col_filter_7, ch_source, ch_filter0, coeffs, 6)
STENCIL_KERNEL_ROW_GEN(kernel_filter1, TILE_DIM, SIMD_SIZE, 7, uchar, uchar, row_filter_7, ch_filter0, ch_filter, coeffs, 6)
#elif defined G9X9SEP
constant short coeffs[9] = {1, 8, 28, 56, 70, 56, 28, 8, 1};
CHANNEL(ch_filter0, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
STENCIL_KERNEL_COL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 9, uchar, uchar, col_filter_9, ch_source, ch_filter0, coeffs, 8)
STENCIL_KERNEL_ROW_GEN(kernel_filter1, TILE_DIM, SIMD_SIZE, 9, uchar, uchar, row_filter_9, ch_filter0, ch_filter, coeffs, 8)
#elif defined G9X9SEPSYMM
constant short coeffs[9] = {1, 8, 28, 56, 70, 56, 28, 8, 1};
CHANNEL(ch_filter0, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
STENCIL_KERNEL_COL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 9, uchar, uchar, col_filter_symm_9, ch_source, ch_filter0, coeffs, 8)
STENCIL_KERNEL_ROW_GEN(kernel_filter1, TILE_DIM, SIMD_SIZE, 9, uchar, uchar, row_filter_symm_9, ch_filter0, ch_filter, coeffs, 8)
// #elif defined SEPFILTER
// constant short col_coeffs[11] = {-5, -5, -5, 6, 6, 6, 6, 6, -5, -5, -5};
// constant short row_coeffs[3] = {5, 5, 5};
// CHANNEL(ch_filter0, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
// STENCIL_KERNEL_COL_GEN(kernel_filter, TILE_DIM, SIMD_SIZE, 11, uchar, uchar, col_filter_11, ch_source, ch_filter0, col_coeffs, 0)
// STENCIL_KERNEL_ROW_GEN(kernel_filter1, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, row_filter_3, ch_filter0, ch_filter, row_coeffs, 4)
#elif defined G9X9
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 9, uchar, uchar, gaussian9x9, ch_source, ch_filter)
#elif defined G9X9SYMM
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 9, uchar, uchar, gaussian9x9, ch_source, ch_filter)
#elif defined BOX3X3
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, box, ch_source, ch_filter)
#elif defined DILATE
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, dilate, ch_source, ch_filter)
#elif defined ERODE
//TODO bug: when simd size = 1
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, erode, ch_source, ch_filter)
#elif defined MEDIAN
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, median, ch_source, ch_filter)
#elif defined UNIT
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, unit, ch_source, ch_filter)
#elif defined CENSUS
STENCIL_KERNEL(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, census, ch_source, ch_filter)
#elif defined SOBEL
STENCIL_KERNEL_IOO(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, ch_source, ch_filter[0], ch_filter[1], sobel_x, sobel_y)
#elif defined ERRDIFF
ERR_DIFF_1(kernel_filter, TILE_DIM, SIMD_SIZE, 3, uchar, uchar, ch_source, ch_filter)
#else
#error NO_KERNEL_SELECTED_ERROR
#endif

SINK_KERNEL(uchar, SIMD_SIZE, ch_filter)

