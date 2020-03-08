#pragma once
#include "common.h"


#define LUT_KERNEL_(NAME, SIMD_SIZE, SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL) __kernel \
void NAME(uint items) {                                                 \
  TYPE(out_data, OUT_TYPE, SIMD_SIZE);                                  \
  TYPE(in_data, IN_TYPE, SIMD_SIZE);                                    \
  OUT_TYPE __attribute__((memory,                                       \
  singlepump,                                                           \
  numreadports(SIMD_SIZE),                                              \
  numwriteports(1))) table[SIZE];                                       \
  for (int i = 0 ;i < SIZE; i++) {                                      \
    table[i] = i;                                                       \
  }                                                                     \
  int counter = 0;                                                      \
  LOG("Starting", #NAME)                                                \
  LOGP(#NAME ".items", items)                                           \
  ASSERT (items % 32 == 0)                                              \
  while (counter < items) {                                             \
    in_data = read_channel_intel(IN_CHANNEL);                           \
    _Pragma("unroll")                                                   \
    for(int j =0 ;j < SIMD_SIZE;j++)                                    \
      out_data[j] = table[in_data[j]];                                  \
    write_channel_intel(OUT_CHANNEL, out_data);                         \
    counter += (SIMD_SIZE);                                             \
  }                                                                     \
  LOG("Finished", #NAME);                                               \
}

#define LUT_KERNEL_1(NAME, SIMD_SIZE, SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL) __kernel \
void NAME(uint items) {                                                 \
  OUT_TYPE out_data;                                                    \
  IN_TYPE in_data;                                                      \
  OUT_TYPE __attribute__((memory,                                       \
  singlepump,                                                           \
  numreadports(1),                                                      \
  numwriteports(1))) table[SIZE];                                       \
  for (int i = 0 ;i < SIZE; i++) {                                      \
    table[i] = i;                                                       \
  }                                                                     \
  int counter = 0;                                                      \
  LOG("Starting", #NAME)                                                \
  LOGP(#NAME ".items", items)                                           \
  ASSERT (items % 32 == 0)                                              \
  while (counter < items) {                                             \
    in_data = read_channel_intel(IN_CHANNEL);                           \
    out_data = table[in_data];                                          \
    write_channel_intel(OUT_CHANNEL, out_data);                         \
    counter += (1);                                                     \
  }                                                                     \
  LOG("Finished", #NAME);                                               \
}

#define LUT_KERNEL_2(A0, A1, A2, A3, A4, A5, A6) LUT_KERNEL_(A0, A1, A2, A3, A4, A5, A6)
#define LUT_KERNEL_4(A0, A1, A2, A3, A4, A5, A6) LUT_KERNEL_(A0, A1, A2, A3, A4, A5, A6)
#define LUT_KERNEL_8(A0, A1, A2, A3, A4, A5, A6) LUT_KERNEL_(A0, A1, A2, A3, A4, A5, A6)
#define LUT_KERNEL_16(A0, A1, A2, A3, A4, A5, A6) LUT_KERNEL_(A0, A1, A2, A3, A4, A5, A6)
#define LUT_KERNEL(NAME, SIMD_SIZE, TABLE_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL) \
CAT2(LUT_KERNEL, SIMD_SIZE) (NAME, SIMD_SIZE, TABLE_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL)

#define HIST_1_(NAME, HIST_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL)                        \
  __kernel void NAME(uint items) {                                                                  \
  TYPE(out_data, OUT_TYPE, 1) ;                                                                     \
  TYPE(in_data, IN_TYPE, 1) ;                                                                       \
  int counter = 0;                                                                                  \
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table0[(HIST_SIZE)];\
  for(int j=0; j < HIST_SIZE; j++) {                                                                \
    hist_table0[j] = 0;                                                                             \
  }                                                                                                 \
  LOG("Starting", #NAME)                                                                            \
  LOGP(#NAME ".items", items)                                                                       \
  while (counter < items) {                                                                         \
    in_data = read_channel_intel(IN_CHANNEL);                                                       \
    hist_table0[in_data]+=1;                                                                        \
    counter += (1);                                                                                 \
  }                                                                                                 \
  for(int i=0; i < HIST_SIZE; i++) {                                                                \
    write_channel_intel(OUT_CHANNEL, hist_table0[i]);                                               \
  }                                                                                                 \
  LOG("Finished", #NAME);                                                                           \
}

#define HIST_2_(NAME, HIST_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL)                        \
  __kernel void NAME(uint items) {                                                                  \
  TYPE(out_data, OUT_TYPE, 2) ;                                                                     \
  TYPE(in_data, IN_TYPE, 2) ;                                                                       \
  int counter = 0;                                                                                  \
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table0[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table1[(HIST_SIZE)];\
  for(int j=0 ; j < HIST_SIZE  ;j++) {                                                              \
    hist_table0[j] = 0;                                                                             \
    hist_table1[j] = 0;                                                                             \
  }                                                                                                 \
  LOG("Starting", #NAME)                                                                            \
  LOGP(#NAME ".items", items)                                                                       \
  while (counter < items) {                                                                         \
    in_data = read_channel_intel(IN_CHANNEL);                                                       \
    hist_table0[in_data[0]]+=1;                                                                     \
    hist_table1[in_data[1]]+=1;                                                                     \
    counter += (2);                                                                                 \
  }                                                                                                 \
  int done = 0;                                                                                     \
  for(int i=0; i < HIST_SIZE; i++) {                                                                \
    OUT_TYPE par_sum = 0 ;                                                                          \
    par_sum = hist_table0[i]+hist_table1[i];                                                        \
    out_data[done] = par_sum;                                                                       \
    done++;                                                                                         \
    if (done == 2) {                                                                                \
      write_channel_intel(OUT_CHANNEL, out_data);                                                   \
      done = 0;                                                                                     \
    }                                                                                               \
  }                                                                                                 \
  LOG("Finished", #NAME);                                                                           \
}


#define HIST_4_(NAME, HIST_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL)                        \
  __kernel void NAME(uint items) {                                                                  \
  TYPE(out_data, OUT_TYPE, 4);                                                                      \
  TYPE(in_data, IN_TYPE, 4);                                                                        \
  int counter = 0;                                                                                  \
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table0[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table1[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table2[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table3[(HIST_SIZE)];\
  for(int j=0 ; j < HIST_SIZE  ;j++) {                                                              \
    hist_table0[j] = 0;                                                                             \
    hist_table1[j] = 0;                                                                             \
    hist_table2[j] = 0;                                                                             \
    hist_table3[j] = 0;                                                                             \
  }                                                                                                 \
  LOG("Starting", #NAME)                                                                            \
  LOGP(#NAME ".items", items)                                                                       \
  while (counter < items) {                                                                         \
    in_data = read_channel_intel(IN_CHANNEL);                                                       \
    hist_table0[in_data[0]]+=1;                                                                     \
    hist_table1[in_data[1]]+=1;                                                                     \
    hist_table2[in_data[2]]+=1;                                                                     \
    hist_table3[in_data[3]]+=1;                                                                     \
    counter += (4);                                                                                 \
  }                                                                                                 \
  int done = 0;                                                                                     \
  for(int i=0; i < HIST_SIZE; i++) {                                                                \
    OUT_TYPE par_sum = 0 ;                                                                          \
    par_sum = __fpga_reg(hist_table0[i]+hist_table1[i])+__fpga_reg(hist_table2[i]+hist_table3[i]);  \
    out_data[done] = par_sum;                                                                       \
    done++;                                                                                         \
    if (done == 4) {                                                                                \
      write_channel_intel(OUT_CHANNEL, out_data);                                                   \
      done = 0;                                                                                     \
    }                                                                                               \
  }                                                                                                 \
  LOG("Finished", #NAME);                                                                           \
}

#define HIST_8_(NAME, HIST_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL)                        \
 __kernel void NAME(uint items) {                                                                   \
  TYPE(out_data, OUT_TYPE, 8);                                                                      \
  TYPE(in_data, IN_TYPE, 8);                                                                        \
  int counter = 0;                                                                                  \
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table0[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table1[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table2[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table3[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table4[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table5[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table6[(HIST_SIZE)];\
  __attribute__((memory, numbanks(1), bankwidth(sizeof(OUT_TYPE)))) OUT_TYPE hist_table7[(HIST_SIZE)];\
  for(int j=0 ; j < HIST_SIZE  ;j++) {                                                              \
    hist_table0[j] = 0;                                                                             \
    hist_table1[j] = 0;                                                                             \
    hist_table2[j] = 0;                                                                             \
    hist_table3[j] = 0;                                                                             \
    hist_table4[j] = 0;                                                                             \
    hist_table5[j] = 0;                                                                             \
    hist_table6[j] = 0;                                                                             \
    hist_table7[j] = 0;                                                                             \
  }                                                                                                 \
  LOG("Starting", #NAME)                                                                            \
  LOGP(#NAME ".items", items)                                                                       \
  while (counter < items) {                                                                         \
    in_data = read_channel_intel(IN_CHANNEL);                                                       \
    hist_table0[in_data[0]]+=1;                                                                     \
    hist_table1[in_data[1]]+=1;                                                                     \
    hist_table2[in_data[2]]+=1;                                                                     \
    hist_table3[in_data[3]]+=1;                                                                     \
    hist_table4[in_data[4]]+=1;                                                                     \
    hist_table5[in_data[5]]+=1;                                                                     \
    hist_table6[in_data[6]]+=1;                                                                     \
    hist_table7[in_data[7]]+=1;                                                                     \
    counter += (8);                                                                                 \
  }                                                                                                 \
  int done = 0;                                                                                     \
  for(int i=0; i < HIST_SIZE; i++) {                                                                \
    OUT_TYPE par_sum = 0 ;                                                                          \
    par_sum = __fpga_reg(                                                                           \
              __fpga_reg(hist_table0[i]+hist_table1[i])+                                            \
              __fpga_reg(hist_table2[i]+hist_table3[i]))+                                           \
              __fpga_reg(__fpga_reg(hist_table4[i]+hist_table5[i])+                                 \
              __fpga_reg(hist_table6[i]+hist_table7[i]));                                           \
    out_data[done] = par_sum;                                                                       \
    done++;                                                                                         \
    if (done == 8) {                                                                                \
      write_channel_intel(OUT_CHANNEL, out_data);                                                   \
      done = 0;                                                                                     \
    }                                                                                               \
  }                                                                                                 \
  LOG("Finished", #NAME)                                                                            \
}

#define HIST_1(A0, A1, A2, A3, A4, A5) HIST_1_(A0, A1, A2, A3, A4, A5)
#define HIST_2(A0, A1, A2, A3, A4, A5) HIST_2_(A0, A1, A2, A3, A4, A5)
#define HIST_4(A0, A1, A2, A3, A4, A5) HIST_4_(A0, A1, A2, A3, A4, A5)
#define HIST_8(A0, A1, A2, A3, A4, A5) HIST_8_(A0, A1, A2, A3, A4, A5)
#define HIST(NAME, HIST_SIZE, SIMD_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL) \
 CAT2(HIST, SIMD_SIZE) (NAME, HIST_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL)

#define REG_HIST(NAME, HIST_SIZE, SIMD_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL) __kernel \
void NAME(uint items) {\
  TYPE(out_data, OUT_TYPE, SIMD_SIZE) ;\
  TYPE(in_data, IN_TYPE, SIMD_SIZE) ;\
  int counter = 0;\
  local __attribute__((register)) OUT_TYPE hist_table[SIMD_SIZE][HIST_SIZE];\
  for(int j=0 ; j < HIST_SIZE  ;j++) {\
    _Pragma("unroll SIMD_SIZE")\
    for(int i=0 ; i < SIMD_SIZE  ;i++) {\
      hist_table[i][j] = 0;\
    }\
  }\
  LOG("Starting", #NAME)\
  LOGP(#NAME ".items", items)\
  while (counter < items) {\
    in_data = read_channel_intel(IN_CHANNEL);\
    _Pragma("unroll SIMD_SIZE")\
    for(int i=0 ; i < SIMD_SIZE ;i++) {\
      int index = in_data[i];\
      hist_table[i][index]+=1;\
    }\
    counter += (SIMD_SIZE);\
  }\
  int done = 0;\
  for(int i=0; i < HIST_SIZE; i++) {\
    OUT_TYPE par_sum = 0 ;\
    _Pragma("unroll SIMD_SIZE")\
    for(int j=0; j < SIMD_SIZE; j++) {\
      par_sum += hist_table[j][i];\
    }\
    out_data[done] = par_sum;\
    done++;\
    if (done == SIMD_SIZE) {\
      write_channel_intel(OUT_CHANNEL, out_data);\
      done = 0; \
    }\
  }\
  LOG("Finished", #NAME);\
}
// Tables are not inferred as registers by ACOL compiler since access pattern is data driven
// IT will have minimum ii of 2 since array content will be increamented
// perhaps we can double pump stuff here...
#define ALT_HIST4(NAME, HIST_SIZE, SIMD_SIZE, IN_TYPE, OUT_TYPE, IN_CHANNEL, OUT_CHANNEL) __kernel \
void NAME(uint items) {                                               \
  TYPE(out_data, OUT_TYPE, SIMD_SIZE) ;                               \
  TYPE(in_data, IN_TYPE, SIMD_SIZE) ;                                 \
  int counter = 0;                                                    \
  private OUT_TYPE hist_table0[HIST_SIZE];                            \
  private OUT_TYPE hist_table1[HIST_SIZE];                            \
  private OUT_TYPE hist_table2[HIST_SIZE];                            \
  private OUT_TYPE hist_table3[HIST_SIZE];                            \
  for(int j=0 ; j < HIST_SIZE  ;j++) {                                \
    hist_table0[j] = 0;                                               \
    hist_table1[j] = 0;                                               \
    hist_table2[j] = 0;                                               \
    hist_table3[j] = 0;                                               \
  }                                                                   \
  LOG("Starting", #NAME)                                              \
  LOGP(#NAME ".items", items)                                         \
  while (counter < items) {                                           \
    in_data = read_channel_intel(IN_CHANNEL);                         \
    hist_table0[in_data[0]]+=1;                                       \
    hist_table1[in_data[1]]+=1;                                       \
    hist_table2[in_data[2]]+=1;                                       \
    hist_table3[in_data[3]]+=1;                                       \
    counter += (SIMD_SIZE);                                           \
  }                                                                   \
  int done = 0;                                                       \
  for(int i=0; i < HIST_SIZE; i++) {                                  \
    OUT_TYPE par_sum = 0 ;                                            \
    par_sum = hist_table0[i] + hist_table1[i] + hist_table2[i] + hist_table3[i];\
    out_data[done] = par_sum;                                         \
    done++;                                                           \
    if (done == SIMD_SIZE) {                                          \
      write_channel_intel(OUT_CHANNEL, out_data);                     \
      done = 0;                                                       \
    }                                                                 \
  }                                                                   \
  LOG("Finished", #NAME);                                             \
}