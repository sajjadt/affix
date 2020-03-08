#pragma once

#include "common.h"

#define SINK_KERNEL_uchar_(UNROLL, IN_CH) __kernel \
void kernel_sink( __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
                       uint items) {\
  uchar16 in_vector[2];\
  ulong4 out_data;\
  TYPE(in_data, uchar, UNROLL) ;\
  int counter = 0;\
  LOG("Starting", "sink")\
  LOGP("sink.items", items)\
  ASSERT (items % 32 == 0)\
  ASSERT (UNROLL <= 32)\
  int read =0 ;\
  while (counter < items) {\
    in_data = read_channel_intel(IN_CH);\
    if(read < 16) {\
      _Pragma("unroll")\
      for(int j =0 ;j < UNROLL; j++) {\
        in_vector[0][read + j] = in_data[j];\
      }\
    }\
    else {\
      _Pragma("unroll")\
      for(int j =0 ;j < UNROLL; j++) {\
        in_vector[1][read - 16 + j] = in_data[j];\
      }\
    }\
    read += (UNROLL);\
    if(read == 32) {\
      out_data.lo = as_ulong2(in_vector[0]);\
      out_data.hi = as_ulong2(in_vector[1]);\
      write_pipe(device_out, &out_data);\
      counter += 32;\
      read = 0;\
    }\
  }\
  LOG("Finished", "sink");\
}

#define SINK_KERNEL_uchar_1(UNROLL, IN_CH) __kernel \
void kernel_sink( __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
                       uint items) {\
  uchar16 in_vector[2];\
  ulong4 out_data;\
  TYPE(in_data, uchar, 1) ;\
  int counter = 0;\
  LOG("Starting", "sink")\
  LOGP("sink.items", items)\
  ASSERT (items % 32 == 0)\
  ASSERT (UNROLL <= 32)\
  int read =0 ;\
  while (counter < items) {\
    in_data = read_channel_intel(IN_CH);\
    if(read < 16) {\
      in_vector[0][read] = in_data;\
    }\
    else {\
      in_vector[1][read - 16] = in_data;\
    }\
    read += (UNROLL);\
    if(read == 32) {\
      out_data.lo = as_ulong2(in_vector[0]);\
      out_data.hi = as_ulong2(in_vector[1]);\
      write_pipe(device_out, &out_data);\
      counter += 32;\
      read = 0;\
    }\
  }\
  LOG("Finished", "sink");\
}

#define SINK_KERNEL_uchar_16(UNROLL, IN_CH) __kernel \
void kernel_sink( __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
                       uint items) {\
  uchar16 in_vector[2];\
  ulong4 out_data;\
  TYPE(in_data, uchar, 16) ;\
  int counter = 0;\
  LOG("Starting", "sink")\
  LOGP("sink.items", items)\
  ASSERT (items % 32 == 0)\
  ASSERT (UNROLL <= 32)\
  int read =0 ;\
  while (counter < items) {\
    in_data = read_channel_intel(IN_CH);\
    if(read == 0) {\
      out_data.lo = as_ulong2(in_data);\
    }\
    else {\
      out_data.hi = as_ulong2(in_data);\
    }\
    counter += 16;\
    read += (16);\
    if(read == 32) {\
      write_pipe(device_out, &out_data);\
      read = 0;\
    }\
  }\
  LOG("Finished", "sink");\
}


#define SINK_KERNEL_uchar_32(UNROLL, IN_CH) __kernel \
void kernel_sink( __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
                       uint items) {\
  TYPE(in_data, uchar, 32);\
  ulong4 out_data;\
  int counter = 0;\
  LOG("Starting", "sink")\
  LOGP("sink.items", items)\
  ASSERT (items % 32 == 0)\
  ASSERT (UNROLL <= 32)\
  int read =0 ;\
  while (counter < items) {\
    in_data = read_channel_intel(IN_CH);\
    out_data.lo = as_ulong2(in_data.lo);\
    out_data.hi = as_ulong2(in_data.hi);\
    write_pipe(device_out, &out_data);\
    counter += 32;\
  }\
  LOG("Finished", "sink");\
}


// #define SINK_KERNEL_UCHAR_32(IN_CH0, IN_CH1) __kernel                         \
// void kernel_sink(                                                                       \
//    __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
//                        uint items) {   \
//   ulong4 out_data;\
//   TYPE(in_data0, uchar, 16);\
//   TYPE(in_data1, uchar, 16);\
//   int counter = 0;\
//   LOG("Starting", "sink")\
//   LOGP("sink.items", items)\
//   ASSERT (items % 32 == 0)\
//   ASSERT (UNROLL <= 32)\
//   int read =0 ;\
//   while (counter < items) {\
//     in_data0 = read_channel_intel(IN_CH0);\
//     in_data1 = read_channel_intel(IN_CH1);\
//     out_data.lo = as_ulong2(in_data0);\
//     out_data.hi = as_ulong2(in_data1);\
//     write_pipe(device_out, &out_data);\
//     counter += 32;\
//   }\
//   LOG("Finished", "sink");\
// }


#define SINK_KERNEL_short_(UNROLL, IN_CH) __kernel \
void kernel_sink( __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
                       uint items) {\
  uint8 in_vector;\
  ulong4 out_data;\
  TYPE(in_data, short, UNROLL) ;\
  int counter = 0;\
  LOG("Starting", "sink")\
  LOGP("sink.items", items)\
  ASSERT (items % 32 == 0)\
  ASSERT (UNROLL <= 16)\
  while (counter < items) {\
    for(int i=0; i< 16; i+=UNROLL) {\
      in_data = read_channel_intel(IN_CH);\
      _Pragma("unroll")\
      for(int j =0 ;j < UNROLL; j++)\
        in_vector[i+j] = in_data[j];\
    }\
    out_data = as_ulong4(in_vector);\
    write_pipe(device_out, &out_data);\
    counter += 8;\
  }\
  LOG("Finished", "sink");\
}

#define SINK_KERNEL_uint_(UNROLL, IN_CH) __kernel \
void kernel_sink( __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
                       uint items) {\
  uint8 in_vector;\
  ulong4 out_data;\
  TYPE(in_data, uint, UNROLL) ;\
  int counter = 0;\
  LOG("Starting", "sink")\
  LOGP("sink.items", items)\
  ASSERT (items % 32 == 0)\
  ASSERT (UNROLL <= 8)\
  while (counter < items) {\
    for(int i=0; i< 8; i+=UNROLL) {\
      in_data = read_channel_intel(IN_CH);\
      _Pragma("unroll")\
      for(int j =0 ;j < UNROLL; j++)\
        in_vector[i+j] = in_data[j];\
    }\
    out_data = as_ulong4(in_vector);\
    write_pipe(device_out, &out_data);\
    counter += 8;\
  }\
  LOG("Finished", "sink");\
}

#define SINK_KERNEL_uint_1(UNROLL, IN_CH) __kernel \
void kernel_sink( __attribute__((intel_host_accessible, blocking)) __write_only pipe ulong4 device_out,\
                       uint items) {\
  uint8 in_vector;\
  ulong4 out_data;\
  TYPE(in_data, uint, 1) ;\
  int counter = 0;\
  LOG("Starting", "sink")\
  LOGP("sink.items", items)\
  ASSERT (items % 32 == 0)\
  while (counter < items) {\
    for(int i=0; i< 8; i+=1) {\
      in_data = read_channel_intel(IN_CH);\
      in_vector[i] = in_data;\
    }\
    out_data = as_ulong4(in_vector);\
    write_pipe(device_out, &out_data);\
    counter += 8;\
  }\
  LOG("Finished", "sink");\
}

// Variadic macors are not standard in OpenCL
#define SINK_KERNEL_uchar_2(A0, A1) SINK_KERNEL_uchar_(A0, A1)
#define SINK_KERNEL_uchar_4(A0, A1) SINK_KERNEL_uchar_(A0, A1)
#define SINK_KERNEL_uchar_8(A0, A1) SINK_KERNEL_uchar_(A0, A1)

#define SINK_KERNEL_uint_2(A0, A1) SINK_KERNEL_uint_(A0, A1)
#define SINK_KERNEL_uint_4(A0, A1) SINK_KERNEL_uint_(A0, A1)
#define SINK_KERNEL_uint_8(A0, A1) SINK_KERNEL_uint_(A0, A1)


#define SINK_KERNEL(TYPE, SIMD_SIZE, SRC_CHANNEL) CAT3(SINK_KERNEL, TYPE, SIMD_SIZE) (SIMD_SIZE, SRC_CHANNEL)


#define HOLE(NAME, TYPE, SIMD_SIZE, IN_CH)\
 __attribute__((max_global_work_dim(0)))                                       \
 __attribute__((autorun)) __kernel                                             \
void NAME() {\
  while (1) {\
    read_channel_intel(IN_CH);\
  }\
}