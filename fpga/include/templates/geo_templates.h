#pragma once
#include "common.h"

#define WARP_LOAD(NAME, FUNC, UNROLL, OUT_CH)__kernel \
void NAME(                                              \
  uint rows,                                            \
  uint cols,                                            \
  global const struct Matrix3x3f* restrict M,           \
  global const uchar* restrict g_mem) {                 \
  TYPE(data_in, uchar, UNROLL);                       \
  TYPE(data_out, uchar, UNROLL);                      \
  TYPE(zero, uchar, UNROLL);                          \
  LOG("starting", "warp");                              \
  uint x_ind, y_ind;                                    \
  for(uint y = 0; y < rows; y++) {                      \
    for(uint x = 0; x < cols ; x += UNROLL) {         \
      _Pragma("unroll")                                 \
      for(int i =0 ;i < UNROLL; i++) {                \
        struct Point p_in = {.x = x, .y = y};           \
        struct Point p_out = FUNC(p_in, *M);            \
        int new_x = p_out.x;                            \
        int new_y = p_out.y;                            \
        if (new_x >= cols || new_y >= rows              \
            || new_x < 0 || new_y < 0)                  \
          data_out[i] = 0;                              \
        else {                                          \
          int index = new_y * cols + new_x;             \
          data_out[i] = g_mem[index];                   \
        }                                               \
      }                                                 \
      write_channel_intel(OUT_CH, data_out);            \
    }                                                   \
  }                                                     \
  LOG("Finished", #NAME);                               \
}
