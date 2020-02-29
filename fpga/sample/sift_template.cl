#pragma OPENCL EXTENSION cl_intel_fpga_host_pipe : enable
#pragma OPENCL EXTENSION cl_intel_channels : enable

#include "common.h"
#include "templates/templates.h"
#include "kernels/stencil.h"
#include "kernels/pixel_wise.h"

// Extension functions
uchar sift_minmax(uchar* buffer0, uchar* buffer1, uchar* buffer2, int dim, int j) {
  if ( buffer1[dim+1+j] >= buffer1[dim+j] &&
    buffer1[dim+1+j] >= buffer1[dim+2+j] &&
    buffer1[dim+1+j] >= buffer1[0+j] &&
    buffer1[dim+1+j] >= buffer1[1+j] &&
    buffer1[dim+1+j] >= buffer1[2+j] &&
    buffer1[dim+1+j] >= buffer1[2*dim+j] &&
    buffer1[dim+1+j] >= buffer1[2*dim+1+j] &&
    buffer1[dim+1+j] >= buffer1[2*dim+2+j] &&

    buffer1[dim+1+j] >= buffer0[dim+j] &&
    buffer1[dim+1+j] >= buffer0[dim+2+j] &&
    buffer1[dim+1+j] >= buffer0[0+j] &&
    buffer1[dim+1+j] >= buffer0[1+j] &&
    buffer1[dim+1+j] >= buffer0[2+j] &&
    buffer1[dim+1+j] >= buffer0[2*dim+j] &&
    buffer1[dim+1+j] >= buffer0[2*dim+1+j] &&
    buffer1[dim+1+j] >= buffer0[2*dim+2+j] &&

    buffer1[dim+1+j] >= buffer2[dim+j] &&
    buffer1[dim+1+j] >= buffer2[dim+2+j] &&
    buffer1[dim+1+j] >= buffer2[0+j] &&
    buffer1[dim+1+j] >= buffer2[1+j] &&
    buffer1[dim+1+j] >= buffer2[2+j] &&
    buffer1[dim+1+j] >= buffer2[2*dim+j] &&
    buffer1[dim+1+j] >= buffer2[2*dim+1+j] &&
    buffer1[dim+1+j] >= buffer2[2*dim+2+j] ) {
    return buffer1[dim+1+j];
  } else {
    return 0;
  }
}


// Config
// Max of 16
#ifndef SIMD_SIZE
#define SIMD_SIZE 16
#endif

#ifndef BASE_DIM
#define BASE_DIM 256
#endif 

CHANNEL(ch_in, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
SRC_KERNEL(uchar, SIMD_SIZE, ch_in)

CHANNEL(ch_in_rep[5], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
REPLICATE_AUTO(kernel_replicate_1, uchar, SIMD_SIZE, 5, ch_in, ch_in_rep)

// Gaussians
CHANNEL(ch_temp[5], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_blur_1[5], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
// Sigma = 1.23
constant short coeffs_0[7] = {1, 5, 14, 20, 14, 5, 1};
STENCIL_KERNEL_COL_GEN(kernel_gblur_1_1_col, BASE_DIM, SIMD_SIZE, 7, uchar, uchar, col_filter_symm_7, ch_in_rep[0], ch_temp[0], coeffs_0, 6)
STENCIL_KERNEL_ROW_GEN(kernel_gblur_1_1_row, BASE_DIM, SIMD_SIZE, 7, uchar, uchar, row_filter_symm_7, ch_temp[0], ch_blur_1[0], coeffs_0, 6)
// Sigma = 1.55
constant short coeffs_1[7] = {2, 7, 13, 16, 13, 7, 2};
STENCIL_KERNEL_COL_GEN(kernel_gblur_1_2_col, BASE_DIM, SIMD_SIZE, 7, uchar, uchar, col_filter_symm_7, ch_in_rep[1], ch_temp[1], coeffs_1, 6)
STENCIL_KERNEL_ROW_GEN(kernel_gblur_1_2_row, BASE_DIM, SIMD_SIZE, 7, uchar, uchar, row_filter_symm_7, ch_temp[1], ch_blur_1[1], coeffs_1, 6)
// Sigma = 1.95
constant short coeffs_2[9] = {1, 4, 7, 11, 13, 11, 7, 4, 1};
STENCIL_KERNEL_COL_GEN(kernel_gblur_1_3_col, BASE_DIM, SIMD_SIZE, 9, uchar, uchar, col_filter_symm_9, ch_in_rep[2], ch_temp[2], coeffs_2, 6)
STENCIL_KERNEL_ROW_GEN(kernel_gblur_1_3_row, BASE_DIM, SIMD_SIZE, 9, uchar, uchar, row_filter_symm_9, ch_temp[2], ch_blur_1[2], coeffs_2, 6)
// Sigma = 2.45
constant short coeffs_3[11] = {1, 2, 5, 7, 9, 10, 9, 7, 5, 2, 1};
STENCIL_KERNEL_COL_GEN(kernel_gblur_1_4_col, BASE_DIM, SIMD_SIZE, 11, uchar, uchar, col_filter_symm_11, ch_in_rep[3], ch_temp[3], coeffs_3, 6)
STENCIL_KERNEL_ROW_GEN(kernel_gblur_1_4_row, BASE_DIM, SIMD_SIZE, 11, uchar, uchar, row_filter_symm_11, ch_temp[3], ch_blur_1[3], coeffs_3, 6)
// Sigma = 3.1
constant short coeffs_4[13] = {1, 2, 3, 5, 6, 8, 8, 8, 8, 6, 5, 3, 2, 1};
STENCIL_KERNEL_COL_GEN(kernel_gblur_1_5_col, BASE_DIM, SIMD_SIZE, 13, uchar, uchar, col_filter_symm_13, ch_in_rep[4], ch_temp[4], coeffs_4, 6)
STENCIL_KERNEL_ROW_GEN(kernel_gblur_1_5_row, BASE_DIM, SIMD_SIZE, 13, uchar, uchar, row_filter_symm_13, ch_temp[4], ch_blur_1[4], coeffs_4, 6)

CHANNEL(ch_blur_1_rep_1[2], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_blur_1_rep_2[2], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_blur_1_rep_3[2], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
REPLICATE_AUTO(kernel_replicate_2, uchar, SIMD_SIZE, 2, ch_blur_1[1], ch_blur_1_rep_1)
REPLICATE_AUTO(kernel_replicate_3, uchar, SIMD_SIZE, 2, ch_blur_1[2], ch_blur_1_rep_2)
REPLICATE_AUTO(kernel_replicate_4, uchar, SIMD_SIZE, 2, ch_blur_1[3], ch_blur_1_rep_3)

// DAG
CHANNEL(ch_dag_1[4], uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
PIXELWISE_IIO(kernel_absdiff_1, SIMD_SIZE, uchar, uchar, absdiff, ch_blur_1[0], ch_blur_1_rep_1[0], ch_dag_1[0])
PIXELWISE_IIO(kernel_absdiff_2, SIMD_SIZE, uchar, uchar, absdiff, ch_blur_1_rep_1[1], ch_blur_1_rep_2[0], ch_dag_1[1])
PIXELWISE_IIO(kernel_absdiff_3, SIMD_SIZE, uchar, uchar, absdiff, ch_blur_1_rep_2[1], ch_blur_1_rep_3[0], ch_dag_1[2])
PIXELWISE_IIO(kernel_absdiff_4, SIMD_SIZE, uchar, uchar, absdiff, ch_blur_1_rep_3[1], ch_blur_1[4], ch_dag_1[3])

CHANNEL(ch_dag_1_1[2], uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_dag_1_2[2], uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
REPLICATE_AUTO(kernel_replicate_5, uchar, SIMD_SIZE, 2, ch_dag_1[1], ch_dag_1_1)
REPLICATE_AUTO(kernel_replicate_6, uchar, SIMD_SIZE, 2, ch_dag_1[2], ch_dag_1_2)


// Minmax
CHANNEL(ch_minmax[2], uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
STENCIL_KERNEL_IIIO(kernel_minmax_1, BASE_DIM, SIMD_SIZE, 3, uchar, uchar, ch_dag_1[0], ch_dag_1_1[0], ch_dag_1_2[0], ch_minmax[0], sift_minmax)
STENCIL_KERNEL_IIIO(kernel_minmax_2, BASE_DIM, SIMD_SIZE, 3, uchar, uchar, ch_dag_1_1[1], ch_dag_1_2[1], ch_dag_1[3], ch_minmax[1], sift_minmax)

// Reject low contrast pixels
CHANNEL(ch_thresh_0, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_thresh_1, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
PIXELWISE_IO(kernel_thresh_1, SIMD_SIZE, uchar, uchar, simple_threshold_uchar, ch_minmax[0], ch_thresh_0)
PIXELWISE_IO(kernel_thresh_2, SIMD_SIZE, uchar, uchar, simple_threshold_uchar, ch_minmax[1], ch_thresh_1)
//HOLE(kernel_hole, uchar, SIMD_SIZE, ch_thresh[1])

// Reject edge response 0
CHANNEL(ch_sobel_x_, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_y_, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_xx_, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_xy_, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_yx_, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_yy_, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_xx_rep[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_yy_rep[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_trace, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_det, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_add_t_[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
// Gx and Gy
CHANNEL(ch_thresh_rep[2], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
REPLICATE_AUTO(kernel_replicate_7, uchar, SIMD_SIZE, 2, ch_thresh_0, ch_thresh_rep)
STENCIL_KERNEL_IOO(kernel_sobel3x3_1, BASE_DIM, SIMD_SIZE, 3, uchar, short, ch_thresh_rep[0], ch_sobel_x_, ch_sobel_y_, sobel_x, sobel_y)
STENCIL_KERNEL_IOO(kernel_sobel3x3_2, BASE_DIM, SIMD_SIZE, 3, short, short, ch_sobel_x_, ch_sobel_xx_, ch_sobel_xy_, sobel_x_short, sobel_y_short)
STENCIL_KERNEL_IOO(kernel_sobel3x3_3, BASE_DIM, SIMD_SIZE, 3, short, short, ch_sobel_y_, ch_sobel_yx_, ch_sobel_yy_, sobel_x_short, sobel_y_short)
// Trace and Det
REPLICATE_AUTO(kernel_replicate_9, short, SIMD_SIZE, 2, ch_sobel_xx_, ch_sobel_xx_rep)
REPLICATE_AUTO(kernel_replicate_10, short, SIMD_SIZE, 2, ch_sobel_yy_, ch_sobel_yy_rep)
PIXELWISE_IIO(kernel_trace_1, SIMD_SIZE, short, short, add_short, ch_sobel_xx_rep[0], ch_sobel_yy_rep[0], ch_trace)
PIXELWISE_IIO(kernel_mr_1, SIMD_SIZE, short, short, pixelwise_mul_short, ch_sobel_xx_rep[1], ch_sobel_yy_rep[1], ch_add_t_[0])
PIXELWISE_IIO(kernel_mr_2, SIMD_SIZE, short, short, pixelwise_mul_short, ch_sobel_yx_, ch_sobel_xy_, ch_add_t_[1])
PIXELWISE_IIO(kernel_sr_1, SIMD_SIZE, short, short, sub_short, ch_add_t_[0], ch_add_t_[1], ch_det)
CHANNEL(ch_out_0, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
__kernel void kernel_fixme(uint items) {
  TYPE(in_data, uchar, SIMD_SIZE);
  TYPE(trace, short, SIMD_SIZE);
  TYPE(det, short, SIMD_SIZE);
  TYPE(out_data, uchar, SIMD_SIZE) ;
  int counter = 0;
  LOG("Starting", "kernel_fixme")
  LOGP("kernel_fixme.items", items)
  while (counter < items) {
    in_data = read_channel_intel(ch_thresh_rep[1]);
    trace = read_channel_intel(ch_trace);
    det = read_channel_intel(ch_det);
    #pragma unroll 
    for(int i =0; i < SIMD_SIZE; i++) {
      if( det[i] != 0 && (trace[i] * trace[i] / det[i])  <= (12) ) {
        out_data[i] = in_data[i];
      } else {
        out_data[i] = 0;
      }
    }
    write_channel_intel(ch_out_0, out_data);
    counter += (SIMD_SIZE);
  }
  LOG("Finished kernel_fixme",);
}

// Reject edge response 1
CHANNEL(ch_sobel_x_a, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_y_a, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_xx_a, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_xy_a, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_yx_a, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_yy_a, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_xx_repa[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_sobel_yy_repa[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_tracea, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_deta, short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
CHANNEL(ch_add_t_a[2], short , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
// Gx and Gy
CHANNEL(ch_thresh_repa[2], uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
REPLICATE_AUTO(kernel_replicate_7a, uchar, SIMD_SIZE, 2, ch_thresh_1, ch_thresh_repa)
STENCIL_KERNEL_IOO(kernel_sobel3x3_1a, BASE_DIM, SIMD_SIZE, 3, uchar, short, ch_thresh_repa[0], ch_sobel_x_a, ch_sobel_y_a, sobel_x, sobel_y)
STENCIL_KERNEL_IOO(kernel_sobel3x3_2a, BASE_DIM, SIMD_SIZE, 3, short, short, ch_sobel_x_a, ch_sobel_xx_a, ch_sobel_xy_a, sobel_x_short, sobel_y_short)
STENCIL_KERNEL_IOO(kernel_sobel3x3_3a, BASE_DIM, SIMD_SIZE, 3, short, short, ch_sobel_y_a, ch_sobel_yx_a, ch_sobel_yy_a, sobel_x_short, sobel_y_short)
// Trace and Det
REPLICATE_AUTO(kernel_replicate_9a, short, SIMD_SIZE, 2, ch_sobel_xx_a, ch_sobel_xx_repa)
REPLICATE_AUTO(kernel_replicate_10a, short, SIMD_SIZE, 2, ch_sobel_yy_a, ch_sobel_yy_repa)
PIXELWISE_IIO(kernel_trace_1a, SIMD_SIZE, short, short, add_short, ch_sobel_xx_repa[0], ch_sobel_yy_repa[0], ch_tracea)
PIXELWISE_IIO(kernel_mr_1a, SIMD_SIZE, short, short, pixelwise_mul_short, ch_sobel_xx_repa[1], ch_sobel_yy_repa[1], ch_add_t_a[0])
PIXELWISE_IIO(kernel_mr_2a, SIMD_SIZE, short, short, pixelwise_mul_short, ch_sobel_yx_a, ch_sobel_xy_a, ch_add_t_a[1])
PIXELWISE_IIO(kernel_sr_1a, SIMD_SIZE, short, short, sub_short, ch_add_t_a[0], ch_add_t_a[1], ch_deta)
CHANNEL(ch_out_1, uchar , SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
__kernel void kernel_fixmea(uint items) {
  TYPE(in_data, uchar, SIMD_SIZE);
  TYPE(trace, short, SIMD_SIZE);
  TYPE(det, short, SIMD_SIZE);
  TYPE(out_data, uchar, SIMD_SIZE) ;
  int counter = 0;
  LOG("Starting", "kernel_fixme")
  LOGP("kernel_fixme.items", items)
  while (counter < items) {
    in_data = read_channel_intel(ch_thresh_repa[1]);
    trace = read_channel_intel(ch_tracea);
    det = read_channel_intel(ch_deta);
    #pragma unroll 
    for(int i =0; i < SIMD_SIZE; i++) {
      if( det[i] != 0 && (trace[i] * trace[i] / det[i]) <= (12) ) {
        out_data[i] = in_data[i];
      } else {
        out_data[i] = 0;
      }
    }
    write_channel_intel(ch_out_1, out_data);
    counter += (SIMD_SIZE);
  }
  LOG("Finished kernel_fixme",);
}

// Merge results
CHANNEL(ch_level_out, uchar, SIMD_SIZE, DEFAULT_CHANNEL_DEPTH)
PIXELWISE_IIO(kernel_or_end, SIMD_SIZE, uchar, uchar, or_uchar, ch_out_0, ch_out_1, ch_level_out)
SINK_KERNEL(uchar, SIMD_SIZE, ch_level_out)

