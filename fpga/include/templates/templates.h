#ifndef TEMPLATES_H_
#define TEMPLATES_H_

#include "src_templates.h"
#include "sink_templates.h"
#include "pixelwise_templates.h"
#include "stencil_templates.h"
#include "mem_templates.h"
#include "stats_templates.h"
#include "geo_templates.h"

#define REPLICATE_IOO(NAME, DATA_TYPE, UNROLL, IN_CH, OUT_CH0, OUT_CH1)__kernel\
 void NAME(uint items) {                                                       \
  int counter = 0;                                                             \
  LOG("starting", #NAME)                                                       \
  LOGP(#NAME ".items", items)                                                  \
  while (counter < items) {                                                    \
    TYPE(data, DATA_TYPE, UNROLL);                                             \
    data  = read_channel_intel(IN_CH);                                         \
    write_channel_intel(OUT_CH0, data);                                        \
    write_channel_intel(OUT_CH1, data);                                        \
    counter += (UNROLL);                                                       \
  }                                                                            \
  LOG("finished", #NAME)                                                       \
}

#define REPLICATE(NAME, DATA_TYPE, UNROLL, NUM_OUT, IN_CH, OUT_CHS)__kernel\
 void NAME(uint items) {                                                       \
  int counter = 0;                                                             \
  LOG("starting", #NAME)                                                       \
  LOGP(#NAME ".items", items)                                                  \
  while (counter < items) {                                                    \
    TYPE(data, DATA_TYPE, UNROLL);                                             \
    data  = read_channel_intel(IN_CH);                                         \
    _Pragma("unroll")\
    for(int i =0;i < NUM_OUT; i++){\
      write_channel_intel(OUT_CHS[i], data);                                        \
    }\
    counter += (UNROLL);                                                       \
  }                                                                            \
  LOG("finished", #NAME)                                                       \
}

#define REPLICATE_AUTO(NAME, DATA_TYPE, UNROLL, NUM_OUT, IN_CH, OUT_CHS)       \
 __attribute__((max_global_work_dim(0)))                                       \
 __attribute__((autorun)) __kernel                                             \
 void NAME() {                                                                 \
  int counter = 0;                                                             \
  LOG("starting", #NAME)                                                       \
  while (1) {                                                                  \
    TYPE(data, DATA_TYPE, UNROLL);                                             \
    data  = read_channel_intel(IN_CH);                                         \
    _Pragma("unroll")                                                          \
    for(int i =0;i < NUM_OUT; i++){                                            \
      write_channel_intel(OUT_CHS[i], data);                                   \
    }                                                                          \
  }                                                                            \
}

#define REPLICATE_IOOO(NAME, DATA_TYPE, UNROLL, IN_CH, OUT_CH0, OUT_CH1, OUT_CH2)  \
__kernel void NAME( uint items) {                                                  \
  int counter = 0;                                                                 \
  LOG("starting", #NAME)                                                           \
  LOGP(#NAME ".items", items)                                                      \
  while (counter < items) {                                                        \
    TYPE(data, DATA_TYPE, UNROLL);                                                 \
    data  = read_channel_intel(IN_CH);                                             \
    write_channel_intel(OUT_CH0, data);                                            \
    write_channel_intel(OUT_CH1, data);                                            \
    write_channel_intel(OUT_CH2, data);                                            \
    counter += (UNROLL);                                                           \
  }                                                                                \
  LOG("finished", #NAME)                                                           \
}

#define UPSCALE(NAME, DATA_TYPE, UNROLL, IN_TILE_DIM, IN_CH, OUT_CH)       \
__kernel void NAME(uint in_items) {                                        \
  int counter = 0;                                                         \
  ASSERT(UNROLL <= 8)                                                      \
  TYPE(in_data, DATA_TYPE, UNROLL);                                        \
  TYPE(out_data, DATA_TYPE, 16);                                          \
  TYPE(zero, DATA_TYPE, UNROLL);                                           \
  zero  = 0;                                                               \
  LOG("starting", #NAME)                                                   \
  LOGP(#NAME ".in_items", in_items)                                        \
  LOGP(#NAME ".IN_TILE_DIM", IN_TILE_DIM)                                  \
  while (counter < in_items) {                                             \
    for(int i =0 ;i < (IN_TILE_DIM); i+=UNROLL) {                          \
      in_data = read_channel_intel(IN_CH);                                 \
      out_data.even = in_data;                                             \
      write_channel_intel(OUT_CH, out_data.lo);                            \
      write_channel_intel(OUT_CH, out_data.hi);                            \
    }                                                                      \
    for(int i =0 ;i < 2*(IN_TILE_DIM); i+=UNROLL) {                        \
      write_channel_intel(OUT_CH, zero);                                   \
    }                                                                      \
    counter += (IN_TILE_DIM);                                              \
  }                                                                        \
  LOG("finished", #NAME)                                                   \
}
#endif