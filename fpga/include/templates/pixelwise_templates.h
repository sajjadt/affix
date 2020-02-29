#pragma once
#include "common.h"

#define PIXELWISE_IO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CHANNEL, OUT_CHANNEL) __kernel \
void NAME(uint items) {                                                                       \
  TYPE(out_data, OUT_TYPE, UNROLL);                                                           \
  TYPE(in_data, IN_TYPE, UNROLL);                                                             \
  uint counter = 0;                                                                            \
  LOG("Starting", #NAME)                                                                      \
  LOGP(#NAME ".items", items)  \
  LOGP(#NAME ".SIMD", UNROLL)                                                                 \
  ASSERT (items % 32 == 0)                                                                    \
  while (counter < items) {                                                                   \
    in_data = read_channel_intel(IN_CHANNEL);                                                 \
    CAT2(FUNC,UNROLL)(in_data, &out_data);                                                     \
    write_channel_intel(OUT_CHANNEL, out_data);                                               \
    counter += (UNROLL);                                                                      \
    LOGP("CTR", counter);\
  }                                                                                           \
  LOG("Finished", #NAME);                                                                     \
}


#define PIXELWISE_IOOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CHANNEL, OUT_CHANNEL0, OUT_CHANNEL1, OUT_CHANNEL2) __kernel \
void NAME(uint items) {\
  TYPE(in_data, IN_TYPE, UNROLL) ;\
  TYPE(out_data0, OUT_TYPE, UNROLL);\
  TYPE(out_data1, OUT_TYPE, UNROLL);\
  TYPE(out_data2, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data = read_channel_intel(IN_CHANNEL);\
    CAT2(FUNC,UNROLL)(in_data, &out_data0, &out_data1, &out_data2);\
    write_channel_intel(OUT_CHANNEL0, out_data0);\
    write_channel_intel(OUT_CHANNEL1, out_data1);\
    write_channel_intel(OUT_CHANNEL2, out_data2);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CHANNEL0, IN_CHANNEL1, OUT_CHANNEL) __kernel \
void NAME(uint items) {\
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(out_data, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CHANNEL0);\
    in_data1 = read_channel_intel(IN_CHANNEL1);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, &out_data);\
    write_channel_intel(OUT_CHANNEL, out_data);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIIO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CHANNEL0, IN_CHANNEL1, IN_CHANNEL2, OUT_CHANNEL) __kernel \
void NAME(uint items) {\
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(in_data2, IN_TYPE, UNROLL);\
  TYPE(out_data, OUT_TYPE, UNROLL) ;\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CHANNEL0);\
    in_data1 = read_channel_intel(IN_CHANNEL1);\
    in_data2 = read_channel_intel(IN_CHANNEL2);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, in_data2, &out_data);\
    write_channel_intel(OUT_CHANNEL, out_data);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIIOOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CHANNEL0, IN_CHANNEL1, IN_CHANNEL2, OUT_CHANNEL0, OUT_CHANNEL1, OUT_CHANNEL2) __kernel \
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
    in_data0 = read_channel_intel(IN_CHANNEL0);\
    in_data1 = read_channel_intel(IN_CHANNEL1);\
    in_data2 = read_channel_intel(IN_CHANNEL2);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, in_data2, &out_data0,  &out_data1,  &out_data2);\
    write_channel_intel(OUT_CHANNEL0, out_data0);\
    write_channel_intel(OUT_CHANNEL1, out_data1);\
    write_channel_intel(OUT_CHANNEL2, out_data2);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIIOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CHANNEL0, IN_CHANNEL1, IN_CHANNEL2, OUT_CHANNEL0, OUT_CHANNEL1) __kernel \
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
    in_data0 = read_channel_intel(IN_CHANNEL0);\
    in_data1 = read_channel_intel(IN_CHANNEL1);\
    in_data2 = read_channel_intel(IN_CHANNEL2);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, in_data2, &out_data0,  &out_data1);\
    write_channel_intel(OUT_CHANNEL0, out_data0);\
    write_channel_intel(OUT_CHANNEL1, out_data1);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}

#define PIXELWISE_IIOO(NAME, UNROLL, IN_TYPE, OUT_TYPE, FUNC, IN_CHANNEL0, IN_CHANNEL1, OUT_CHANNEL0, OUT_CHANNEL1) __kernel \
void NAME(uint items) {\
  TYPE(in_data0, IN_TYPE, UNROLL);\
  TYPE(in_data1, IN_TYPE, UNROLL);\
  TYPE(out_data0, OUT_TYPE, UNROLL);\
  TYPE(out_data1, OUT_TYPE, UNROLL);\
  uint counter = 0;\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data0 = read_channel_intel(IN_CHANNEL0);\
    in_data1 = read_channel_intel(IN_CHANNEL1);\
    CAT2(FUNC,UNROLL)(in_data0, in_data1, &out_data0,  &out_data1);\
    write_channel_intel(OUT_CHANNEL0, out_data0);\
    write_channel_intel(OUT_CHANNEL1, out_data1);\
    counter += (UNROLL);\
  }\
  LOG("Finished", #NAME);\
}
