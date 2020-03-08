#include  "common.h"


#define SRC_KERNEL_uchar(UNROLL, OUT_CH) __kernel                              \
void kernel_source(                                                                 \
 __attribute__((intel_host_accessible, blocking)) __read_only pipe ulong4 host_in,  \
 uint items) {                                                                      \
  int counter = 0;                                                                  \
  ulong4 data_in;                                                                   \
  TYPE(data, uchar, UNROLL) ;                                                       \
  LOG("starting", "source")                                                         \
  LOGP("src.actual_items", items)                                                   \
  int read = 0;                                                                     \
  while (counter < items) {                                                         \
    if (read == 0) {                                                                \
       read_pipe(host_in, &data_in);                                                \
    }                                                                               \
    if(read < 16){                                                                  \
      _Pragma("unroll")                                                             \
      for(int i=0; i < UNROLL; i+=1) {                                              \
        data[i] = as_uchar16(data_in.lo)[read + i];                                 \
      }                                                                             \
    } else {                                                                        \
      _Pragma("unroll")                                                             \
      for(int i=0; i < UNROLL; i+=1) {                                              \
        data[i] = as_uchar16(data_in.hi)[read - 16 + i];                            \
      }                                                                             \
    }                                                                               \
    write_channel_intel(OUT_CH, data);                                         \
    read += (UNROLL);                                                               \
    if (read == 32) read = 0;                                                       \
    counter += (UNROLL);                                                            \
  }                                                                                 \
  LOG("Finished", "source")                                                         \
}

#define SRC_KERNEL_uchar_1(UNROLL, OUT_CH) __kernel                            \
void kernel_source(                                                                 \
 __attribute__((intel_host_accessible, blocking)) __read_only pipe ulong4 host_in,  \
 uint items) {                                                                      \
  int counter = 0;                                                                  \
  ulong4 data_in;                                                                   \
  TYPE(data, uchar, 1);                                                             \
  LOG("starting", "source")                                                         \
  LOGP("src.actual_items", items)                                                   \
  int read = 0;                                                                     \
  while (counter < items) {                                                         \
    if (read == 0) {                                                                \
       read_pipe(host_in, &data_in);                                                \
    }                                                                               \
    if(read < 16){                                                                  \
      data = as_uchar16(data_in.lo)[read ];                                         \
    } else {                                                                        \
      data = as_uchar16(data_in.hi)[read - 16 ];                                    \
    }                                                                               \
    write_channel_intel(OUT_CH, data);                                         \
    read += (1);                                                               \
    if (read == 32) read = 0;                                                       \
    counter += (1);                                                            \
  }                                                                                 \
  LOG("Finished", "source")                                                         \
}

#define SRC_KERNEL_uchar_16(UNROLL, OUT_CH) __kernel                           \
void kernel_source(                                                                 \
 __attribute__((intel_host_accessible, blocking)) __read_only pipe ulong4 host_in,  \
 uint items) {                                                                      \
  int counter = 0;                                                                  \
  ulong4 data_in;                                                                   \
  TYPE(data, uchar, 16);                                                         \
  int read =0;                                                      \
  LOG("starting", "source")                                                         \
  LOGP("src.actual_items", items)                                                   \
  while (counter < items) {                                                         \
    if(read == 0)\
      read_pipe(host_in, &data_in);                                                   \
    if(read==0)\
      data = as_uchar16(data_in.lo);                                               \
    else \
      data = as_uchar16(data_in.hi);                                               \
    write_channel_intel(OUT_CH, data);                                        \
    read = read + 16;        \
    if(read == 32) read = 0; \
    counter += (16);                                                                \
  }                                                                                 \
  LOG("Finished", "source")                                                         \
}

#define SRC_KERNEL_uchar_32(UNROLL, OUT_CH) __kernel                           \
void kernel_source(                                                                 \
 __attribute__((intel_host_accessible, blocking)) __read_only pipe ulong4 host_in,  \
 uint items) {                                                                      \
  int counter = 0;                                                                  \
  ulong4 data_in;                                                                   \
  TYPE(data, uchar, 32);                                                            \
  int read =0;                                                                      \
  LOG("starting", "source")                                                         \
  LOGP("src.actual_items", items)                                                   \
  while (counter < items) {                                                         \
    read_pipe(host_in, &data_in);                                                   \
    data.lo = as_uchar16(data_in.lo);                                               \
    data.hi = as_uchar16(data_in.hi);                                               \
    write_channel_intel(OUT_CH, data);                                         \
    counter += (32);                                                                \
  }                                                                                 \
  LOG("Finished", "source")                                                         \
}

#define SRC_KERNEL_uchar_2(A0, A1) SRC_KERNEL_uchar(A0, A1)
#define SRC_KERNEL_uchar_4(A0, A1) SRC_KERNEL_uchar(A0, A1)
#define SRC_KERNEL_uchar_8(A0, A1) SRC_KERNEL_uchar(A0, A1)

#define SRC_KERNEL_uint_(UNROLL, OUT_CH) __kernel \
void kernel_source( __attribute__((intel_host_accessible, blocking)) __read_only pipe ulong4 host_in, uint items) {\
  int counter = 0;\
  ulong4 data_in;\
  TYPE(data, uint, UNROLL);\
  uint8 casted_data_in;\
  LOG("starting", "source")\
  LOGP("src.actual_items", items)\
  while (counter < items) { \
    read_pipe(host_in, &data_in);\
    casted_data_in = as_uint8(data_in);\
    for(int i=0; i < 8; i+=UNROLL) {\
      _Pragma("unroll")\
      for(int j =0 ;j < UNROLL; j++)\
        data[j] = casted_data_in[i+j];\
      write_channel_intel(OUT_CH, data);\
    }\
    counter += 8;\
  }\
  LOG("Finished", "source")\
}

#define SRC_KERNEL_uint_1(UNROLL, OUT_CH) __kernel \
void kernel_source( __attribute__((intel_host_accessible, blocking)) __read_only pipe ulong4 host_in, uint items) {\
  int counter = 0;\
  ulong4 data_in;\
  TYPE(data, uint, UNROLL);\
  uint8 casted_data_in;\
  LOG("starting", "source")\
  LOGP("src.actual_items", items)\
  while (counter < items) { \
    read_pipe(host_in, &data_in);\
    casted_data_in = as_uint8(data_in);\
    for(int i=0; i < 8; i+=UNROLL) {\
      data = casted_data_in[i];\
      write_channel_intel(OUT_CH, data);\
    }\
    counter += 8;\
  }\
  LOG("Finished", "source")\
}

#define SRC_KERNEL_uint_2(A, B) SRC_KERNEL_uint_(A, B)
#define SRC_KERNEL_uint_4(A, B) SRC_KERNEL_uint_(A, B)
#define SRC_KERNEL_uint_8(A, B) SRC_KERNEL_uint_(A, B)





#define SRC_KERNEL(TYPE, SIMD_SIZE, DEST_CHANNEL) CAT3(SRC_KERNEL, TYPE, SIMD_SIZE) ( SIMD_SIZE, DEST_CHANNEL)
