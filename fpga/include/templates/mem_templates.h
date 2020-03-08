#pragma once
#include "common.h"

#define SAVE_KERNEL(NAME, UNROLL, DATA_TYPE, IN_CHANNEL)          \
 __kernel void NAME(                                              \
  uint items,                                                     \
  global TTYPE(DATA_TYPE, UNROLL) * restrict mem) {               \
  int counter = 0;                                                \
  int counter_x = 0;                                              \
  LOG("starting", #NAME)                                          \
  LOGP(#NAME ".items", items)                                     \
  while (counter < items) {                                       \
    mem[counter/UNROLL] = read_channel_intel(IN_CHANNEL);         \
    counter += (UNROLL);                                          \
  }                                                               \
  LOG("finished", #NAME)                                          \
}

#define LOAD_KERNEL(NAME, UNROLL, DATA_TYPE, OUT_CHANNEL) __kernel \
void NAME(                                                         \
  uint items,                                                      \
  global const TTYPE(DATA_TYPE, UNROLL) * restrict mem) {          \
  int counter = 0;                                                 \
  int counter_x = 0;                                               \
  LOG("starting", #NAME)                                           \
  LOGP(#NAME ".items", items)                                      \
  while (counter < items) {                                        \
    write_channel_intel(OUT_CHANNEL, mem[counter/UNROLL]);         \
    counter += (UNROLL);                                           \
  }                                                                \
  LOG("finished", #NAME)                                           \
}

// Input is an image with Column-Tiled ordering
#define SAVE_KERNEL_COL_TILED(NAME, UNROLL, DATA_TYPE, COL_WIDTH, IN_CHANNEL)          \
 __kernel void NAME(                                              \
  uint rows, uint cols,                                           \
  global TTYPE(DATA_TYPE, UNROLL) * restrict mem) {               \
  int counter = 0;                                                \
  int x = 0;                                                      \
  int y = 0;                                                      \
  uint items = cols*rows;                                         \
  LOG("starting", #NAME)                                          \
  LOGP(#NAME ".items", items)                                     \
  TTYPE(DATA_TYPE, UNROLL) input;                                 \
  uint col_size = COL_WIDTH*rows;                                 \
  while (counter < items) {                                       \
    input = read_channel_intel(IN_CHANNEL);                       \
    mem[(y*cols+x)/UNROLL] = data;                                \
    counter += (UNROLL);                                          \
    x += (UNROLL);                                                \
    if (x == COL_WIDTH)   {                                       \
      y++;                                                        \
      x = 0;                                                      \
    }                                                             \
    y+=1;                                                         \
    if(y == rows) {                                               \
      y = 0;                                                      \
    }                                                             \
  }                                                               \
  LOG("finished", #NAME)                                          \
}

#define LOAD_KERNEL_COL_TILED(NAME, UNROLL, DATA_TYPE, COL_WIDTH, OUT_CHANNEL) __kernel \
void NAME(                                                         \
  uint rows, uint cols,                                            \
  global const TTYPE(DATA_TYPE, UNROLL) * restrict mem) {          \
  int counter = 0;                                                 \
  int x = 0;                                                       \
  int y = 0;                                                       \
  int col = 0;                                                     \
  uint items = cols*rows;                                          \
  LOG("starting", #NAME)                                           \
  LOGP(#NAME ".items", items)                                      \
  while (counter < items) {                                        \
    write_channel_intel(OUT_CHANNEL, mem[(y*cols+col*COL_WIDTH+x)/UNROLL]);      \
    counter += (UNROLL);                                           \
    x += (UNROLL);                                                 \
    if (x == COL_WIDTH)   {                                        \
      y++;                                                         \
      x = 0;                                                       \
    }                                                              \
    y+=1;                                                          \
    if(y == rows) {                                                \
      y = 0;                                                       \
    }                                                              \
  }                                                                \
  LOG("finished", #NAME)                                           \
}

#define SAVE_DELAY_OBJ_KERNEL(NAME, DATA_TYPE, UNROLL, INDEX, SIZE, IN_CHANNEL)          \
 __kernel void NAME(                                              \
  uint items,                                                     \
  global TTYPE(DATA_TYPE, UNROLL) * restrict mem,                 \
  int index) {                                                    \
  int counter = 0;                                                \
  int counter_x = 0;                                              \
  LOG("starting", #NAME)                                          \
  LOGP(#NAME ".items", items)                                     \
  LOGP(#NAME ".index", index)                                     \
  int base = (items * (index % INDEX))/UNROLL;                    \
  uchar control;                                                  \
  LOGP(#NAME ".Start saving into", base)                          \
  while (counter < items) {                                       \
    mem[base + counter/UNROLL] = read_channel_intel(IN_CHANNEL);  \
    counter += (UNROLL);                                          \
  }                                                               \
  LOG("finished", #NAME)                                          \
}

#define LOAD_DELAY_OBJ_KERNEL(NAME, DATA_TYPE, UNROLL, INDEX, SIZE, OUT_CHANNEL) __kernel \
void NAME(                                                         \
  uint items,                                                      \
  global const TTYPE(DATA_TYPE, UNROLL) * restrict mem,            \
  int index){                                                      \
  int counter = 0;                                                 \
  int counter_x = 0;                                               \
  LOG("starting", #NAME)                                           \
  LOGP(#NAME ".items", items)                                      \
  LOGP(#NAME ".index", index)                                      \
  int base = (items * (index % INDEX));                            \
  TTYPE(DATA_TYPE, UNROLL) zero;                                   \
  zero = 0;                                                        \
  LOGP(#NAME ".Start loading from", base)                          \
  while (counter < items) {                                        \
    if (base<0)                                                    \
      write_channel_intel(OUT_CHANNEL, zero);                      \
    else                                                           \
      write_channel_intel(OUT_CHANNEL, mem[base/UNROLL + (counter)/UNROLL]);  \
    counter += (UNROLL);                                           \
  }                                                                \
  LOG("finished", #NAME)                                           \
}

#define BRAM_KERNEL(NAME, DATA_TYPE, SIMD_SIZE, BUFFER_SIZE, IN_CHANNEL, OUT_CHANNEL) __kernel \
void NAME() {                                                                                  \
  local TTYPE(IN_TYPE,SIMD_SIZE) buffer[BUFFER_SIZE];                                          \
  TYPE(input, IN_TYPE, SIMD_SIZE);                                                             \
  int counter_read = 0, counter_write=0;                                                       \
  LOG("starting", #NAME)                                                                       \
  LOGP(#NAME ".items", items)                                                                  \
  while (counter_read < items) {                                                               \
    input = read_channel_intel(IN_CHANNEL);                                                    \
    buffer[counter_read] = input;                                                              \
    counter_read += 1;                                                                         \
  }                                                                                            \
  while (counter_write < items) {                                                              \
    write_channel_intel(OUT_CHANNEL, buffer[counter_write]);                                   \
    counter_write += 1;                                                                        \
  }                                                                                            \
  LOG("finished", #NAME)                                                                       \
}
