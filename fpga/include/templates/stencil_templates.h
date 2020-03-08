#pragma once
#include "common.h"

#define STENCIL_KERNEL_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3, (FILTER_DIM-1)+1);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, 1);\
  const int offset =  MAX(pad_size, 1);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(1, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  LOGP(#NAME ".SIMD_SIZE", 1)\
  int tile_items = img_rows*(TILE_DIM);\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+1) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = 1; j < buffer_size; ++j) {\
        buffer[j-1] = buffer[j]; \
      }\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, 1);\
        input  = read_channel_intel(IN_CH); \
        buffer[buffer_size-1] = input;\
      } else {\
        buffer[buffer_size-1] = zero;\
      }\
      LOGARR("after.shift and new data", buffer, buffer_size)\
      TYPE(filter_out, OUT_TYPE, UNROLL);\
      TYPE(real_filter_out, OUT_TYPE, UNROLL);\
      filter_out = FUNC(buffer, TILE_DIM, offset);\
      _Pragma("unroll")\
      if (i >= buffer_ready_size) { \
        int curr_out_y = (i - buffer_ready_size) / (TILE_DIM);\
        int curr_out_x = (i - buffer_ready_size) % (TILE_DIM);\
        if (curr_out_x < pad_size || \
            curr_out_y < pad_size || \
            curr_out_x >= TILE_DIM - pad_size || \
            curr_out_y >= img_rows - pad_size) {  \
          uchar repl_value = buffer[buffer_ready_size];\
          real_filter_out = repl_value;\
        } else {\
          real_filter_out = filter_out;\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}
#define STENCIL_KERNEL_(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  LOGP(#NAME ".SIMD_SIZE", UNROLL)\
  LOGP(#NAME ".buffer_size", buffer_size)\
  LOGP(#NAME ".buffer_ready_size", buffer_ready_size)\
  int tile_items = img_rows*(TILE_DIM);\
  LOGP(#NAME ".tile_items", tile_items)\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {\
      LOGARR("before.shift", buffer, ç)\
      _Pragma("unroll") \
      for(int j = UNROLL; j < buffer_size; ++j) {\
        buffer[j-UNROLL] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, UNROLL);\
        input  = read_channel_intel(IN_CH); \
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++) {\
          buffer[buffer_size-1-j] = input[UNROLL - 1 - j];\
        }\
      } else {\
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++)\
          buffer[buffer_size-1-j] = 0;\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL);\
      TYPE(real_filter_out, OUT_TYPE, UNROLL);\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
          filter_out[j] = FUNC(buffer, TILE_DIM, offset+j);\
      }\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            real_filter_out[k] = repl_value;\
          } else {\
            real_filter_out[k] = filter_out[k];\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}
#define STENCIL_KERNEL_32(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  int tile_items = img_rows*(TILE_DIM);\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = UNROLL; j < buffer_size; ++j) {\
        buffer[j-UNROLL] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, UNROLL);\
        input  = read_channel_intel(IN_CH); \
        _Pragma("unroll") \
        for(int j =0 ;j < 16;j++) {\
          buffer[buffer_size-1-j] = input.hi[16 - 1 - j];\
          buffer[buffer_size-1-j-16] = input.lo[16 - 1 - j];\
        }\
      } else {\
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++)\
          buffer[buffer_size-1-j] = zero;\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL);\
      TYPE(real_filter_out, OUT_TYPE, UNROLL);\
      _Pragma("unroll")\
      for(int j = 0 ;j < 16; j++) {\
          filter_out.lo[j] = FUNC(buffer, TILE_DIM, offset+j);\
          filter_out.hi[j] = FUNC(buffer, TILE_DIM, offset+j+16);\
      }\
      for(int k = 0 ; k < 32; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            if (k < 16)\
              real_filter_out.lo[k] = repl_value;\
            else \
              real_filter_out.hi[k-16] = repl_value;\
          } else {\
            if (k < 16)\
              real_filter_out.lo[k] = filter_out.lo[k];\
            else \
              real_filter_out.hi[k-16] = filter_out.hi[k-16];\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}
#define STENCIL_KERNEL_2(A0, A1, A2, A3, A4, A5, A6, A7, A8) STENCIL_KERNEL_(A0, A1, A2, A3, A4, A5, A6, A7, A8)
#define STENCIL_KERNEL_4(A0, A1, A2, A3, A4, A5, A6, A7, A8) STENCIL_KERNEL_(A0, A1, A2, A3, A4, A5, A6, A7, A8)
#define STENCIL_KERNEL_8(A0, A1, A2, A3, A4, A5, A6, A7, A8) STENCIL_KERNEL_(A0, A1, A2, A3, A4, A5, A6, A7, A8)
#define STENCIL_KERNEL_16(A0, A1, A2, A3, A4, A5, A6, A7, A8) STENCIL_KERNEL_(A0, A1, A2, A3, A4, A5, A6, A7, A8)
#define STENCIL_KERNEL(NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH) \
 CAT2(STENCIL_KERNEL, SIMD_SIZE) (NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH)

#define STENCIL_KERNEL_G_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3, (FILTER_DIM-1)+1);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, 1);\
  const int offset =  MAX(pad_size, 1);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(1, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  int tile_items = img_rows*(TILE_DIM);\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+1) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = 1; j < buffer_size; ++j) {\
        buffer[j-1] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, 1);\
        input  = read_channel_intel(IN_CH); \
        buffer[buffer_size-1] = input;\
      } else {\
        buffer[buffer_size-1] = zero;\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL);\
      TYPE(real_filter_out, OUT_TYPE, UNROLL);\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
          filter_out = FUNC(buffer, COEFFS, TILE_DIM, offset+j, SHIFT);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            real_filter_out = repl_value;\
          } else {\
            real_filter_out = filter_out;\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}

#define STENCIL_KERNEL_G_(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  int tile_items = img_rows*(TILE_DIM);\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = UNROLL; j < buffer_size; ++j) {\
        buffer[j-UNROLL] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, UNROLL);\
        input  = read_channel_intel(IN_CH); \
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++) {\
          buffer[buffer_size-1-j] = input[UNROLL - 1 - j];\
        }\
      } else {\
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++)\
          buffer[buffer_size-1-j] = zero;\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL);\
      TYPE(real_filter_out, OUT_TYPE, UNROLL);\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
          filter_out[j] = FUNC(buffer, COEFFS, TILE_DIM, offset+j, SHIFT);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            real_filter_out[k] = repl_value;\
          } else {\
            real_filter_out[k] = filter_out[k];\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}

#define STENCIL_KERNEL_G_2(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_G_4(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_G_8(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_G_16(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_GEN(NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 CAT2(STENCIL_KERNEL_G, SIMD_SIZE) (NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT)

#define STENCIL_KERNEL_COL_G_(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  int tile_items = img_rows*(TILE_DIM);\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = UNROLL; j < buffer_size; ++j) {\
        buffer[j-UNROLL] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, UNROLL);\
        input  = read_channel_intel(IN_CH); \
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++) {\
          buffer[buffer_size-1-j] = input[UNROLL - 1 - j];\
        }\
      } else {\
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++)\
          buffer[buffer_size-1-j] = zero;\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL);\
      TYPE(real_filter_out, OUT_TYPE, UNROLL);\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
          filter_out[j] = FUNC(buffer, COEFFS, TILE_DIM, offset+j, SHIFT);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            real_filter_out[k] = repl_value;\
          } else {\
            real_filter_out[k] = filter_out[k];\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}

#define STENCIL_KERNEL_COL_G_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, 1);\
  const int offset =  MAX(pad_size, 1);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(1, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  int tile_items = img_rows*(TILE_DIM);\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+1) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = 1; j < buffer_size; ++j) {\
        buffer[j-1] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        IN_TYPE input;\
        input  = read_channel_intel(IN_CH); \
        buffer[buffer_size-1] = input;\
      } else {\
        buffer[buffer_size-1] = zero;\
      }\
      OUT_TYPE filter_out;\
      OUT_TYPE real_filter_out;\
      filter_out = FUNC(buffer, COEFFS, TILE_DIM, offset, SHIFT);\
        if (i >= buffer_ready_size) { \
          int curr_out_y = (i - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            real_filter_out = repl_value;\
          } else {\
            real_filter_out = filter_out;\
          }\
        }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}
#define STENCIL_KERNEL_COL_G_2(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_COL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_COL_G_4(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_COL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_COL_G_8(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_COL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_COL_G_16(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_COL_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_COL_GEN(NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 CAT2(STENCIL_KERNEL_COL_G, SIMD_SIZE) (NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT)


#define STENCIL_KERNEL_ROW_G_(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = MAX( (FILTER_DIM)+(UNROLL)-1, 3*UNROLL);\
  const int offset =  pad_size;\
  const int filter_start = buffer_size;\
  const int buffer_ready_size = filter_start;\
  const uchar zero = 0;  \
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  int tile_items = img_rows*(TILE_DIM);\
  int num_tiles = img_cols/(TILE_DIM);\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOG( #NAME ".starting", "tile")\
  for (int t =0; t < num_tiles; t++)\
  for (int i=0; i < tile_items + buffer_ready_size ; i=i+(UNROLL)) {\
    LOGARR("before.shift", buffer, buffer_size)\
    _Pragma("unroll") \
    for(int j = UNROLL; j < buffer_size; ++j) {\
      buffer[j-UNROLL] = buffer[j]; \
    }\
    LOGARR("after.shift", buffer, buffer_size)\
    if (i < tile_items) {\
      TYPE(input, IN_TYPE, UNROLL);\
      input  = read_channel_intel(IN_CH); \
      _Pragma("unroll") \
      for(int j =0 ;j < UNROLL;j++) {\
        buffer[buffer_size-1-j] = input[UNROLL - 1 - j];\
      }\
    } else {\
      _Pragma("unroll") \
      for(int j =0 ;j < UNROLL;j++)\
        buffer[buffer_size-1-j] = zero;\
    }\
    TYPE(filter_out, OUT_TYPE, UNROLL);\
    TYPE(real_filter_out, OUT_TYPE, UNROLL);\
    _Pragma("unroll")\
    for(int j = 0 ;j < UNROLL; j++) {\
      filter_out[j] = FUNC(buffer, COEFFS, TILE_DIM, offset+j, SHIFT);\
    }\
    _Pragma("unroll")\
    for(int k = 0 ; k < UNROLL; k++) {\
      if (i+k >= buffer_ready_size) { \
        int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
        int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
        if (curr_out_x < pad_size || \
            curr_out_y < pad_size || \
            curr_out_x >= TILE_DIM - pad_size || \
            curr_out_y >= img_rows - pad_size) {  \
          uchar repl_value = buffer[buffer_ready_size];\
          real_filter_out[k] = repl_value;\
        } else {\
          real_filter_out[k] = filter_out[k];\
        }\
      }\
    }\
    if (i >= buffer_ready_size) {\
      write_channel_intel(OUT_CH, real_filter_out);\
    }\
  }\
  LOG("finished", #NAME);\
}

#define STENCIL_KERNEL_ROW_G_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = MAX( (FILTER_DIM), 3);\
  const int offset =  pad_size;\
  const int filter_start = buffer_size;\
  const int buffer_ready_size = filter_start;\
  const uchar zero = 0;  \
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  int tile_items = img_rows*(TILE_DIM);\
  int num_tiles = img_cols/(TILE_DIM);\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOG( #NAME ".starting", "tile")\
  for (int t =0; t < num_tiles; t++)\
  for (int i=0; i < tile_items + buffer_ready_size ; i=i+(1)) {\
    LOGARR("before.shift", buffer, buffer_size)\
    _Pragma("unroll") \
    for(int j = 1; j < buffer_size; ++j) {\
      buffer[j-1] = buffer[j]; \
    }\
    LOGARR("after.shift", buffer, buffer_size)\
    if (i < tile_items) {\
      IN_TYPE input;\
      input  = read_channel_intel(IN_CH); \
      buffer[buffer_size-1] = input;\
    } else {\
      buffer[buffer_size-1] = zero;\
    }\
    OUT_TYPE filter_out;\
    OUT_TYPE real_filter_out;\
    filter_out = FUNC(buffer, COEFFS, TILE_DIM, offset, SHIFT);\
    if (i >= buffer_ready_size) { \
      int curr_out_y = (i - buffer_ready_size) / (TILE_DIM);\
      int curr_out_x = (i - buffer_ready_size) % (TILE_DIM);\
      if (curr_out_x < pad_size || \
          curr_out_y < pad_size || \
          curr_out_x >= TILE_DIM - pad_size || \
          curr_out_y >= img_rows - pad_size) {  \
        uchar repl_value = buffer[buffer_ready_size];\
        real_filter_out = repl_value;\
      } else {\
        real_filter_out = filter_out;\
      }\
    }\
    if (i >= buffer_ready_size) {\
      write_channel_intel(OUT_CH, real_filter_out);\
    }\
  }\
  LOG("finished", #NAME);\
}
#define STENCIL_KERNEL_ROW_G_2(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_ROW_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_ROW_G_4(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_ROW_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_ROW_G_8(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_ROW_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_ROW_G_16(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_ROW_G_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_ROW_GEN(NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 CAT2(STENCIL_KERNEL_ROW_G, SIMD_SIZE) (NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT)

#define ERR_DIFF_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH, OUT_CH) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3, (FILTER_DIM-1)+1);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, 1);\
  const int offset =  MAX(pad_size, 1);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(1, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer[buffer_size];\
  int counter = 0;\
  int items = img_rows*img_cols;\
  int width;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".rows", img_rows)\
  LOGP(#NAME ".cols", img_cols)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  int tile_items = img_rows*(TILE_DIM);\
  for (int t =0; t < num_tiles; t++) {\
    LOG( #NAME ".starting", "tile")\
    LOGP(#NAME".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+1) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = 1; j < buffer_size; ++j) {\
        buffer[j-1] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, 1);\
        input  = read_channel_intel(IN_CH); \
        buffer[buffer_size-1] = input;\
      } else {\
        buffer[buffer_size-1] = zero;\
      }\
      uchar real_filter_out;\
      uchar oldpixel = buffer[TILE_DIM+1];\
      uchar newpixel = oldpixel > 127 ? 255 :  0;\
      buffer[TILE_DIM+1] = newpixel;\
      short quant_error = oldpixel - newpixel;\
      buffer[TILE_DIM+2]  = buffer[TILE_DIM+2] + 7/16.0 * quant_error;\
      buffer[2*TILE_DIM] = buffer[2*TILE_DIM] + 3/16.0 * quant_error;\
      buffer[2*TILE_DIM+1] = buffer[2*TILE_DIM+1] + 5/16.0 * quant_error;\
      buffer[2*TILE_DIM+2] = buffer[2*TILE_DIM+2] + 1/16.0 * quant_error;\
      uchar filter_out =  buffer[TILE_DIM+1];\
      _Pragma("unroll")\
      if (i >= buffer_ready_size) { \
        int curr_out_y = (i - buffer_ready_size) / (TILE_DIM);\
        int curr_out_x = (i - buffer_ready_size) % (TILE_DIM);\
        if (curr_out_x < pad_size || \
            curr_out_y < pad_size || \
            curr_out_x >= TILE_DIM - pad_size || \
            curr_out_y >= img_rows - pad_size) {  \
          uchar repl_value = buffer[buffer_ready_size];\
          real_filter_out = repl_value;\
        } else {\
          real_filter_out = filter_out;\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}

// One input 
// Multiple outputs
#define STENCIL_KERNEL_IOO_(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH, OUT_CH0, OUT_CH1, FUNC0, FUNC1) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;                                                                   \
  int num_tiles = img_cols/(TILE_DIM);                                                    \
  IN_TYPE buffer[buffer_size];                                                            \
  int counter = 0;                                                                        \
  int width;                                                                              \
  LOG("Starting", #NAME)                                                                  \
  int items = img_rows*img_cols;                                                          \
  LOGP(#NAME ".TILE_DIM", TILE_DIM)                                                       \
  LOGP(#NAME ".items", items)                                                             \
  LOGP(#NAME ".num_tiles", num_tiles)                                                     \
  for (int t =0; t < num_tiles; t++) {                                                    \
    int tile_items = img_rows*(TILE_DIM);                                                 \
    LOG(#NAME ".starting", "tile")                                                        \
    LOGP(#NAME ".tile_items", tile_items)                                                 \
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {                      \
      LOGARR("before.shift", buffer, buffer_size)                                         \
      _Pragma("unroll")                                                                   \
      for(int j = UNROLL; j < buffer_size; ++j) {                                         \
        buffer[j-UNROLL] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, UNROLL);\
        input  = read_channel_intel(IN_CH); \
        _Pragma("unroll")                                             \
        for(int j =0 ;j < UNROLL;j++) {                               \
          buffer[buffer_size-1-j] = input[UNROLL - 1 - j];            \
        }                                                             \
      } else {                                                        \
        _Pragma("unroll")                                             \
        for(int j =0 ;j < UNROLL;j++)                                 \
          buffer[j] = zero;                                           \
      }                                                               \
      TYPE(filter_out[2], OUT_TYPE, UNROLL);\
      TYPE(real_filter_out[2], OUT_TYPE, UNROLL);\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
        filter_out[0][j] = FUNC0(buffer, TILE_DIM, offset+j);\
        filter_out[1][j] = FUNC1(buffer, TILE_DIM, offset+j);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            real_filter_out[0][k] = repl_value;\
            real_filter_out[0][k] = repl_value;\
          } else {\
            real_filter_out[0][k] = filter_out[0][k];\
            real_filter_out[1][k] = filter_out[1][k];\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH0, real_filter_out[0]);\
        write_channel_intel(OUT_CH1, real_filter_out[1]);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
} 

#define STENCIL_KERNEL_IOO_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH, OUT_CH0, OUT_CH1, FUNC0, FUNC1) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*1, (FILTER_DIM-1)+1);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, 1);\
  const int offset =  MAX(pad_size, 1);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(1, (FILTER_DIM-1));\
  const uchar zero = 0;                                                                   \
  int num_tiles = img_cols/(TILE_DIM);                                                    \
  IN_TYPE buffer[buffer_size];                                                            \
  int counter = 0;                                                                        \
  int width;                                                                              \
  LOG("Starting", #NAME)                                                                  \
  int items = img_rows*img_cols;                                                          \
  LOGP(#NAME ".TILE_DIM", TILE_DIM)                                                       \
  LOGP(#NAME ".items", items)                                                             \
  LOGP(#NAME ".num_tiles", num_tiles)                                                     \
  for (int t =0; t < num_tiles; t++) {                                                    \
    int tile_items = img_rows*(TILE_DIM);                                                 \
    LOG(#NAME ".starting", "tile")                                                        \
    LOGP(#NAME ".tile_items", tile_items)                                                 \
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {                      \
      LOGARR("before.shift", buffer, buffer_size)                                         \
      _Pragma("unroll")                                                                   \
      for(int j = 1; j < buffer_size; ++j) {                                         \
        buffer[j-1] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, 1);\
        input  = read_channel_intel(IN_CH); \
        buffer[buffer_size-1] = input;            \
      } else {                                                        \
        buffer[buffer_size-1] = zero;                               \
      }                                                               \
      TYPE(filter_out[2], OUT_TYPE, 1);\
      TYPE(real_filter_out[2], OUT_TYPE, 1);\
      filter_out[0] = FUNC0(buffer, TILE_DIM, offset);\
      filter_out[1] = FUNC1(buffer, TILE_DIM, offset);\
      if (i >= buffer_ready_size) { \
        int curr_out_y = (i - buffer_ready_size) / (TILE_DIM);\
        int curr_out_x = (i - buffer_ready_size) % (TILE_DIM);\
        if (curr_out_x < pad_size || \
            curr_out_y < pad_size || \
            curr_out_x >= TILE_DIM - pad_size || \
            curr_out_y >= img_rows - pad_size) {  \
          uchar repl_value = buffer[buffer_ready_size];\
          real_filter_out[0] = repl_value;\
          real_filter_out[0] = repl_value;\
        } else {\
          real_filter_out[0] = filter_out[0];\
          real_filter_out[1] = filter_out[1];\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH0, real_filter_out[0]);\
        write_channel_intel(OUT_CH1, real_filter_out[1]);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
}
#define STENCIL_KERNEL_IOO_32(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH, OUT_CH0, OUT_CH1, FUNC0, FUNC1) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;                                                                   \
  int num_tiles = img_cols/(TILE_DIM);                                                    \
  IN_TYPE buffer[buffer_size];                                                            \
  int counter = 0;                                                                        \
  int width;                                                                              \
  LOG("Starting", #NAME)                                                                  \
  int items = img_rows*img_cols;                                                          \
  LOGP(#NAME ".TILE_DIM", TILE_DIM)                                                       \
  LOGP(#NAME ".items", items)                                                             \
  LOGP(#NAME ".num_tiles", num_tiles)                                                     \
  for (int t =0; t < num_tiles; t++) {                                                    \
    int tile_items = img_rows*(TILE_DIM);                                                 \
    LOG(#NAME ".starting", "tile")                                                        \
    LOGP(#NAME ".tile_items", tile_items)                                                 \
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {                      \
      LOGARR("before.shift", buffer, buffer_size)                                         \
      _Pragma("unroll")                                                                   \
      for(int j = UNROLL; j < buffer_size; ++j) {                                         \
        buffer[j-UNROLL] = buffer[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input, IN_TYPE, UNROLL);\
        input  = read_channel_intel(IN_CH); \
        _Pragma("unroll")                                             \
        for(int j =0 ;j < 16;j++) {                                   \
          buffer[buffer_size-1-j] = input.hi[16 - 1 - j];             \
          buffer[buffer_size-1-j-16] = input.lo[16 - 1 - j];          \
        }                                                             \
      } else {                                                        \
        _Pragma("unroll")                                             \
        for(int j =0 ;j < UNROLL;j++)                                 \
          buffer[j] = zero;                                           \
      }                                                               \
      TYPE(filter_out[2], OUT_TYPE, UNROLL);\
      TYPE(real_filter_out[2], OUT_TYPE, UNROLL);\
      _Pragma("unroll")\
      for(int j = 0 ;j < 16; j++) {\
        filter_out[0].lo[j] = FUNC0(buffer, TILE_DIM, offset+j);\
        filter_out[0].hi[j] = FUNC0(buffer, TILE_DIM, offset+j+16);\
        filter_out[1].lo[j] = FUNC1(buffer, TILE_DIM, offset+j);\
        filter_out[1].hi[j] = FUNC1(buffer, TILE_DIM, offset+j+16);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer[buffer_ready_size];\
            if (k < 16){\
              real_filter_out[0].lo[k] = repl_value;\
              real_filter_out[1].lo[k] = repl_value;\
            } else { \
              real_filter_out[0].hi[k-16] = repl_value;\
              real_filter_out[1].hi[k-16] = repl_value;\
            }\
          } else {\
          if (k < 16){\
              real_filter_out[0].lo[k] = filter_out[0].lo[k];\
              real_filter_out[1].lo[k] = filter_out[1].lo[k];\
            } else { \
              real_filter_out[0].hi[k-16] = filter_out[1].hi[k-16];\
              real_filter_out[1].hi[k-16] = filter_out[1].hi[k-16];\
            }\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH0, real_filter_out[0]);\
        write_channel_intel(OUT_CH1, real_filter_out[1]);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
} 
#define STENCIL_KERNEL_IOO_2(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IOO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IOO_4(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IOO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IOO_8(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IOO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IOO_16(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IOO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IOO(NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT) \
 CAT2(STENCIL_KERNEL_IOO, SIMD_SIZE) (NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH, COEFFS, SHIFT)

#define STENCIL_KERNEL_IIO_(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, OUT_CH, FUNC0) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer0[buffer_size];\
  IN_TYPE buffer1[buffer_size];\
  int counter = 0;\
  int width;\
  LOG("Starting", #NAME)\
  int items = img_rows*img_cols;\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".items", items)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  for (int t =0; t < num_tiles; t++) {\
    int tile_items = img_rows*(TILE_DIM);\
    LOG(#NAME ".starting", "tile")\
    LOGP(#NAME ".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = UNROLL; j < buffer_size; ++j) {\
        buffer0[j-UNROLL] = buffer0[j]; \
        buffer1[j-UNROLL] = buffer1[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input0, IN_TYPE, UNROLL);\
        input0  = read_channel_intel(IN_CH0); \
        TYPE(input1, IN_TYPE, UNROLL);\
        input1  = read_channel_intel(IN_CH1); \
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++) {\
          buffer0[buffer_size-1-j] = input0[UNROLL - 1 - j];\
          buffer1[buffer_size-1-j] = input1[UNROLL - 1 - j];\
        }\
      } else {\
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++){\
          buffer0[j] = zero;\
          buffer1[j] = zero;\
        }\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL) ;\
      TYPE(real_filter_out, OUT_TYPE, UNROLL) ;\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
        filter_out[j] = FUNC0(buffer0, buffer1, TILE_DIM, offset+j);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer0[buffer_ready_size];\
            real_filter_out[k] = repl_value;\
          } else {\
            real_filter_out[k] = filter_out[k];\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
} 
#define STENCIL_KERNEL_IIO_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, OUT_CH, FUNC0) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer0[buffer_size];\
  IN_TYPE buffer1[buffer_size];\
  int counter = 0;\
  int width;\
  LOG("Starting", #NAME)\
  int items = img_rows*img_cols;\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".items", items)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  for (int t =0; t < num_tiles; t++) {\
    int tile_items = img_rows*(TILE_DIM);\
    LOG(#NAME ".starting", "tile")\
    LOGP(#NAME ".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+1) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = 1; j < buffer_size; ++j) {\
        buffer0[j-1] = buffer0[j]; \
        buffer1[j-1] = buffer1[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input0, IN_TYPE, 1);\
        input0  = read_channel_intel(IN_CH0); \
        TYPE(input1, IN_TYPE, 1);\
        input1  = read_channel_intel(IN_CH1); \
        buffer0[buffer_size-1] = input0;\
        buffer1[buffer_size-1] = input1;\
      } else {\
        buffer0[buffer_size-1] = zero;\
        buffer1[buffer_size-1] = zero;\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL) ;\
      TYPE(real_filter_out, OUT_TYPE, UNROLL) ;\
      filter_out = FUNC0(buffer0, buffer1, TILE_DIM, offset);\
      if (i >= buffer_ready_size) { \
        int curr_out_y = (i - buffer_ready_size) / (TILE_DIM);\
        int curr_out_x = (i - buffer_ready_size) % (TILE_DIM);\
        if (curr_out_x < pad_size || \
            curr_out_y < pad_size || \
            curr_out_x >= TILE_DIM - pad_size || \
            curr_out_y >= img_rows - pad_size) {  \
          uchar repl_value = buffer0[buffer_ready_size];\
          real_filter_out = repl_value;\
        } else {\
          real_filter_out = filter_out;\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
} 
#define STENCIL_KERNEL_IIO_2(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) STENCIL_KERNEL_IIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)
#define STENCIL_KERNEL_IIO_4(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) STENCIL_KERNEL_IIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)
#define STENCIL_KERNEL_IIO_8(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) STENCIL_KERNEL_IIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)
#define STENCIL_KERNEL_IIO_16(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) STENCIL_KERNEL_IIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)
#define STENCIL_KERNEL_IIO_32(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, OUT_CH, FUNC0) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer0[buffer_size];\
  IN_TYPE buffer1[buffer_size];\
  int counter = 0;\
  int width;\
  LOG("Starting", #NAME)\
  int items = img_rows*img_cols;\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".items", items)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  for (int t =0; t < num_tiles; t++) {\
    int tile_items = img_rows*(TILE_DIM);\
    LOG(#NAME ".starting", "tile")\
    LOGP(#NAME ".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = UNROLL; j < buffer_size; ++j) {\
        buffer0[j-UNROLL] = buffer0[j]; \
        buffer1[j-UNROLL] = buffer1[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input0, IN_TYPE, UNROLL);\
        input0  = read_channel_intel(IN_CH0); \
        TYPE(input1, IN_TYPE, UNROLL);\
        input1  = read_channel_intel(IN_CH1); \
        _Pragma("unroll") \
        for(int j =0 ;j < 16;j++) {\
          buffer0[buffer_size-1-j] = input0.hi[16 - 1 - j];\
          buffer0[buffer_size-1-j-16] = input0.lo[16 - 1 - j];\
          buffer1[buffer_size-1-j] = input1.hi[16 - 1 - j];\
          buffer1[buffer_size-1-j-16] = input1.lo[16 - 1 - j];\
        }\
      } else {\
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++){\
          buffer0[j] = zero;\
          buffer1[j] = zero;\
        }\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL) ;\
      TYPE(real_filter_out, OUT_TYPE, UNROLL) ;\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
        filter_out.lo[j] = FUNC0(buffer0, buffer1, TILE_DIM, offset+j);\
        filter_out.hi[j] = FUNC0(buffer0, buffer1, TILE_DIM, offset+j+16);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer0[buffer_ready_size];\
            if(k < 16)\
              real_filter_out.lo[k] = repl_value;\
            else \
              real_filter_out.hi[k-16] = repl_value;\
          } else {\
            if (k < 16)\
              real_filter_out.lo[k] = filter_out.lo[k];\
            else  \
              real_filter_out.hi[k-16] = filter_out.hi[k-16];\   
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
} 
#define STENCIL_KERNEL_IIO(NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, OUT_CH, FUNC0) \
 CAT2(STENCIL_KERNEL_IIO, SIMD_SIZE) (NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, OUT_CH, FUNC0)

#define STENCIL_KERNEL_IIIO_(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, IN_CH2, OUT_CH, FUNC0) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer0[buffer_size];\
  IN_TYPE buffer1[buffer_size];\
  IN_TYPE buffer2[buffer_size];\
  int counter = 0;\
  int width;\
  LOG("Starting", #NAME)\
  int items = img_rows*img_cols;\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".items", items)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  for (int t =0; t < num_tiles; t++) {\
    int tile_items = img_rows*(TILE_DIM);\
    LOG(#NAME ".starting", "tile")\
    LOGP(#NAME ".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+UNROLL) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = UNROLL; j < buffer_size; ++j) {\
        buffer0[j-UNROLL] = buffer0[j]; \
        buffer1[j-UNROLL] = buffer1[j]; \
        buffer2[j-UNROLL] = buffer2[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input0, IN_TYPE, UNROLL);\
        input0  = read_channel_intel(IN_CH0); \
        TYPE(input1, IN_TYPE, UNROLL);\
        input1  = read_channel_intel(IN_CH1); \
        TYPE(input2, IN_TYPE, UNROLL);\
        input2  = read_channel_intel(IN_CH2); \
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++) {\
          buffer0[buffer_size-1-j] = input0[UNROLL - 1 - j];\
          buffer1[buffer_size-1-j] = input1[UNROLL - 1 - j];\
          buffer2[buffer_size-1-j] = input2[UNROLL - 1 - j];\
        }\
      } else {\
        _Pragma("unroll") \
        for(int j =0 ;j < UNROLL;j++){\
          buffer0[j] = zero;\
          buffer1[j] = zero;\
          buffer2[j] = zero;\
        }\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL) ;\
      TYPE(real_filter_out, OUT_TYPE, UNROLL) ;\
      _Pragma("unroll")\
      for(int j = 0 ;j < UNROLL; j++) {\
        filter_out[j] = FUNC0(buffer0, buffer1, buffer2, TILE_DIM, offset+j);\
      }\
      _Pragma("unroll")\
      for(int k = 0 ; k < UNROLL; k++) {\
        if (i+k >= buffer_ready_size) { \
          int curr_out_y = (i+k - buffer_ready_size) / (TILE_DIM);\
          int curr_out_x = (i+k - buffer_ready_size) % (TILE_DIM);\
          if (curr_out_x < pad_size || \
              curr_out_y < pad_size || \
              curr_out_x >= TILE_DIM - pad_size || \
              curr_out_y >= img_rows - pad_size) {  \
            uchar repl_value = buffer0[buffer_ready_size];\
            real_filter_out[k] = repl_value;\
          } else {\
            real_filter_out[k] = filter_out[k];\
          }\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
} 
#define STENCIL_KERNEL_IIIO_1(NAME, TILE_DIM, UNROLL, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, IN_CH2, OUT_CH, FUNC0) \
 __kernel void NAME(uint img_rows, uint img_cols) {\
  const int pad_size = (FILTER_DIM-1)/2;\
  const int buffer_size = (FILTER_DIM-1)*(TILE_DIM) + MAX(3*UNROLL, (FILTER_DIM-1)+UNROLL);\
  const int filter_start = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(pad_size, UNROLL);\
  const int offset =  MAX(pad_size, UNROLL);\
  const int buffer_ready_size = (FILTER_DIM-1)/2*(TILE_DIM) + MAX(UNROLL, (FILTER_DIM-1));\
  const uchar zero = 0;  \
  int num_tiles = img_cols/(TILE_DIM);\
  IN_TYPE buffer0[buffer_size];\
  IN_TYPE buffer1[buffer_size];\
  IN_TYPE buffer2[buffer_size];\
  int counter = 0;\
  int width;\
  LOG("Starting", #NAME)\
  int items = img_rows*img_cols;\
  LOGP(#NAME ".TILE_DIM", TILE_DIM)\
  LOGP(#NAME ".items", items)\
  LOGP(#NAME ".num_tiles", num_tiles)\
  for (int t =0; t < num_tiles; t++) {\
    int tile_items = img_rows*(TILE_DIM);\
    LOG(#NAME ".starting", "tile")\
    LOGP(#NAME ".tile_items", tile_items)\
    for (int i=0; i < tile_items + buffer_ready_size ; i=i+1) {\
      LOGARR("before.shift", buffer, buffer_size)\
      _Pragma("unroll") \
      for(int j = 1; j < buffer_size; ++j) {\
        buffer0[j-1] = buffer0[j]; \
        buffer1[j-1] = buffer1[j]; \
        buffer2[j-1] = buffer2[j]; \
      }\
      LOGARR("after.shift", buffer, buffer_size)\
      if (i < tile_items) {\
        TYPE(input0, IN_TYPE, 1);\
        input0  = read_channel_intel(IN_CH0); \
        TYPE(input1, IN_TYPE, 1);\
        input1  = read_channel_intel(IN_CH1); \
        TYPE(input2, IN_TYPE, 1);\
        input2  = read_channel_intel(IN_CH2); \
        buffer0[buffer_size-1] = input0;\
        buffer1[buffer_size-1] = input1;\
        buffer2[buffer_size-1] = input2;\
      } else {\
        buffer0[buffer_size-1] = zero;\
        buffer1[buffer_size-1] = zero;\
        buffer2[buffer_size-1] = zero;\
      }\
      TYPE(filter_out, OUT_TYPE, UNROLL) ;\
      TYPE(real_filter_out, OUT_TYPE, UNROLL) ;\
      filter_out = FUNC0(buffer0, buffer1, buffer2, TILE_DIM, offset);\
      if (i >= buffer_ready_size) { \
        int curr_out_y = (i - buffer_ready_size) / (TILE_DIM);\
        int curr_out_x = (i - buffer_ready_size) % (TILE_DIM);\
        if (curr_out_x < pad_size || \
            curr_out_y < pad_size || \
            curr_out_x >= TILE_DIM - pad_size || \
            curr_out_y >= img_rows - pad_size) {  \
          uchar repl_value = buffer0[buffer_ready_size];\
          real_filter_out = repl_value;\
        } else {\
          real_filter_out = filter_out;\
        }\
      }\
      if (i >= buffer_ready_size) {\
        write_channel_intel(OUT_CH, real_filter_out);\
      }\
    }\
  }\
  LOG("finished", #NAME);\
} 
#define STENCIL_KERNEL_IIIO_2(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IIIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IIIO_4(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IIIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IIIO_8(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IIIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IIIO_16(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IIIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IIIO_32(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) STENCIL_KERNEL_IIIO_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)
#define STENCIL_KERNEL_IIIO(NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, IN_CH2,OUT_CH, FUNC0) \
 CAT2(STENCIL_KERNEL_IIIO, SIMD_SIZE) (NAME, TILE_DIM, SIMD_SIZE, FILTER_DIM, IN_TYPE, OUT_TYPE, IN_CH0, IN_CH1, IN_CH2, OUT_CH, FUNC0)

