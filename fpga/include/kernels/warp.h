#pragma once
#include "common.h"

struct Point {
  uint x;
  uint y;
};

struct Point warp_affine(const struct Point p_in, const struct Matrix3x3f M) {
  float x0 = M.m[0][0]*(p_in.x) + M.m[0][1]*(p_in.y) + M.m[0][2];
  float y0 = M.m[1][0]*(p_in.x) + M.m[1][1]*(p_in.y) + M.m[1][2];
  struct Point p_out;
  p_out.x = (uint)x0;
  p_out.y = (uint)y0;
  return p_out;
}

struct Point warp_perspective(const struct Point p_in, const struct Matrix3x3f M) {
  float x0 = M.m[0][0]*(p_in.x) + M.m[0][1]*(p_in.y) + M.m[0][2];
  float y0 = M.m[1][0]*(p_in.x) + M.m[1][1]*(p_in.y) + M.m[1][2];
  float z0 = M.m[2][0]*(p_in.x) + M.m[2][1]*(p_in.y) + M.m[2][2];
  struct Point p_out;
  p_out.x = (uint)(x0/z0);
  p_out.y = (uint)(y0/z0);
  return p_out;
}