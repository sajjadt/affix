#ifndef HOST_DEV_SHARED
#define HOST_DEV_SHARED

struct uchar32 {
#if defined(INTELFPGA_CL)
  uchar16 lo, hi;
#else
  cl_uchar16 lo, hi;
#endif
};

struct short32 {
#if defined(INTELFPGA_CL)
  short16 lo, hi;
#else
  cl_short16 lo, hi;
#endif
};

struct Matrix3x3f {
  float m[3][3];
};

#endif
