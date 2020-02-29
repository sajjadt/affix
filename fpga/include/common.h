#pragma once

#include "../../common/include/host_dev_shared.h"

#ifdef VERBOSE 
#define LOG(event, name) \
  printf("%s %s.\n",event, name);
#define LOGSTR(value) \
  printf(value);
#define LOGP(property, value) \
  printf("%s equals to = %d.\n", property, value);
#else
#define LOG(event, name)
#define LOGSTR(value)
#define LOGP(property, value)
#endif

#ifdef DEBUG
#define LOGI(value) \
  printf("%d ", value);
#define LOGARR(name, array, len) \
  printf("%s \n", name);\
  for (int __i = 0 ; __i < buffer_size ; __i++)\
    printf("%d ", (int)array[__i]);\
  printf("\n");
#define ASSERT(condition) \
  if (!(condition)) { \
    LOG("Assertion", "failed")\
    return; }
#else
#define LOGI(value)
#define LOGARR(name, array, len)
#define ASSERT(condition)
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#ifdef EMULATION
#ifdef DEFAULT_CHANNEL_DEPTH
#undef DEFAULT_CHANNEL_DEPTH
#endif
#define DEFAULT_CHANNEL_DEPTH 2000000
#pragma message ("Using long buffers for emulation")
#else
#ifndef DEFAULT_CHANNEL_DEPTH
#define DEFAULT_CHANNEL_DEPTH 0
#endif
#endif

typedef struct TILING_PARAMS {
  bool reordered;
  int pad_left, pad_right, pad_top, pad_bottom;
} TILING_PARAMS;

#define CAT_IND(X, Y) X##Y
#define CAT(a, b) CAT_IND(a, b)

#define CAT2_IND(a, b) a##_##b
#define CAT2(a, b) CAT2_IND(a, b)

#define CAT3_IND(a, b, c) a##_##b##_##c
#define CAT3(a, b, c) CAT3_IND(a, b, c)

#define TYPE_1(NAME,T) T NAME
#define TYPE_2(NAME,T) T##2 NAME
#define TYPE_4(NAME,T) T##4 NAME
#define TYPE_8(NAME,T) T##8 NAME
#define TYPE_16(NAME,T) T##16 NAME
#define TYPE_32(NAME,T) struct T##32 NAME
#define IND_TYPE(NAME, T, S) TYPE_##S(NAME,T)
#define TYPE(NAME, T, S) IND_TYPE(NAME,T, S)

#define TTYPE_1(T) T
#define TTYPE_2(T) T##2
#define TTYPE_4(T) T##4
#define TTYPE_8(T) T##8
#define TTYPE_16(T) T##16
#define TTYPE_32(T) struct T##32
#define IND_TTYPE(T, S) TTYPE_##S(T)
#define TTYPE(T, S) IND_TTYPE(T, S)

#define CHANNEL(NAME,_TYPE,UNROLL,DEPTH) channel TTYPE(_TYPE, UNROLL) NAME  __attribute__((depth((DEPTH))));
