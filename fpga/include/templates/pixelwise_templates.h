#pragma once
#include "common.h"

#define PIXELWISE_IO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH) __kernel \
void NAME(uint items) {                                                             \
  TYPE(out_data, OUT_TYPE, UNROLL);                                                 \
  TYPE(in_data, IN_TYPE, UNROLL);                                                   \
  uint counter = 0;                                                                 \
  LOG("Starting", #NAME)                                                            \
  LOGP(#NAME ".items", items)                                                       \
  LOGP(#NAME ".SIMD", UNROLL)                                                       \
  ASSERT (items % 32 == 0)                                                          \
  while (counter < items) {                                                         \
    in_data = read_channel_intel(IN_CH);                                            \
    CAT2(FUNC,UNROLL)(in_data, &out_data);                                          \
    write_channel_intel(OUT_CH, out_data);                                          \
    counter += (UNROLL);                                                            \
  }                                                                                 \
  LOG("Finished", #NAME);                                                           \
}

#define PIXELWISE_IOOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CH, OUT_CH0, OUT_CH1, OUT_CH2) __kernel \
void NAME(uint items) {\
  TYPE(in_data, IN_TYPE, UNROLL) ;\
  TYPE(out_data0, OUT_TYPE, UNROLL);\
  TYPE(out_data1, OUT_TYPE, UNROLL);\
  TYPE(out_data2, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data = read_channel_intel(IN_CH);\
    CAT2(FUNC,UNROLL)(in_data, &out_data0, &out_data1, &out_data2);\
    write_channel_intel(OUT_CH0, out_data0);\
    write_channel_intel(OUT_CH1, out_data1);\
    write_channel_intel(OUT_CH2, out_data2);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CH0, IN_CH1, OUT_CH) __kernel \
void NAME(uint items) {           \
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(out_data, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CH0);\
    in_data1 = read_channel_intel(IN_CH1);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, &out_data);\
    write_channel_intel(OUT_CH, out_data);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIIO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CH0, IN_CH1, IN_CH2, OUT_CH) __kernel \
void NAME(uint items) {\
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(in_data2, IN_TYPE, UNROLL);\
  TYPE(out_data, OUT_TYPE, UNROLL) ;\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CH0);\
    in_data1 = read_channel_intel(IN_CH1);\
    in_data2 = read_channel_intel(IN_CH2);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, in_data2, &out_data);\
    write_channel_intel(OUT_CH, out_data);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIIOOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CH0, IN_CH1, IN_CH2, OUT_CH0, OUT_CH1, OUT_CH2) __kernel \
void NAME(uint items) {\
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(in_data2, IN_TYPE, UNROLL);\
  TYPE(out_data0, OUT_TYPE, UNROLL);\
  TYPE(out_data1, OUT_TYPE, UNROLL);\
  TYPE(out_data2, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CH0);\
    in_data1 = read_channel_intel(IN_CH1);\
    in_data2 = read_channel_intel(IN_CH2);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, in_data2, &out_data0,  &out_data1,  &out_data2);\
    write_channel_intel(OUT_CH0, out_data0);\
    write_channel_intel(OUT_CH1, out_data1);\
    write_channel_intel(OUT_CH2, out_data2);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIIOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CH0, IN_CH1, IN_CH2, OUT_CH0, OUT_CH1) __kernel \
void NAME(uint items) {\
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(in_data2, IN_TYPE, UNROLL);\
  TYPE(out_data0, OUT_TYPE, UNROLL);\
  TYPE(out_data1, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CH0);\
    in_data1 = read_channel_intel(IN_CH1);\
    in_data2 = read_channel_intel(IN_CH2);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, in_data2, &out_data0,  &out_data1);\
    write_channel_intel(OUT_CH0, out_data0);\
    write_channel_intel(OUT_CH1, out_data1);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CH0, IN_CH1, OUT_CH0, OUT_CH1) __kernel \
void NAME(uint items) {\
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(out_data0, OUT_TYPE, UNROLL);\
  TYPE(out_data1, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CH0);\
    in_data1 = read_channel_intel(IN_CH1);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, &out_data0,  &out_data1);\
    write_channel_intel(OUT_CH0, out_data0);\
    write_channel_intel(OUT_CH1, out_data1);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}
