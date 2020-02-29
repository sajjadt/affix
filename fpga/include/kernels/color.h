#pragma once
#include "pixel_wise.h"

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

// RGB -> YCbCr
#define CRGB2Y(R, G, B) CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16)
#define CRGB2Cb(R, G, B) CLIP((36962 * (B - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)
#define CRGB2Cr(R, G, B) CLIP((46727 * (R - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)

// YCbCr -> RGB
#define CYCbCr2R(Y, Cb, Cr) CLIP( Y + ( 91881 * Cr >> 16 ) - 179 )
#define CYCbCr2G(Y, Cb, Cr) CLIP( Y - (( 22544 * Cb + 46793 * Cr ) >> 16) + 135)
#define CYCbCr2B(Y, Cb, Cr) CLIP( Y + (116129 * Cb >> 16 ) - 226 )

typedef struct YUV{
  uchar y, u,v;  
} YUV;

typedef struct RGB{
  uchar r, g,b;  
} RGB;

typedef struct LAB{
  uchar l, a, b;
} LAB;

typedef struct CMYK{
  uchar c, m, y, k;
} CMYK;

RGB rgb_from_int(int in_data) {
  RGB rgb;
  rgb.r = (in_data >> 16) & 0xff;
  rgb.g = (in_data >> 8) & 0xff;
  rgb.b = in_data & 0xff;
  return rgb;
}

int cmyk_to_int(CMYK cmyk) {
  int cmyk_int = cmyk.c;
  cmyk_int = (cmyk_int << 8) | cmyk.m;
  cmyk_int = (cmyk_int << 8) | cmyk.y;
  cmyk_int = (cmyk_int << 8) | cmyk.k;
  return cmyk_int;
}

RGB int_to_rgb(int pixel) {
  RGB rgb;
  rgb.r = (uchar)((pixel>>16) & 255);
  rgb.g = (uchar)((pixel>>8) & 255);
  rgb.b = (uchar)(pixel & 255);
  return rgb;
}

YUV int_to_yuv(int pixel) {
  YUV yuv;
  yuv.y = (uchar)((pixel>>16) & 255);
  yuv.u = (uchar)((pixel>>8) & 255);
  yuv.v = (uchar)(pixel & 255);
  return yuv;
}

int rgb_to_int(RGB rgb) {
  int pixel;
  pixel = rgb.r;
  pixel = (pixel << 8) | rgb.g;
  pixel = (pixel << 8) | rgb.b;
  return pixel;
}

uchar usat8(int a) {
  if (a > 255)
    a = 255;
  if (a < 0)
    a = 0;
  return a;
}

int rgb_index(int r, int g, int b) {
  return r*17*17 + g*17+ b;
}

RGB yuv2rgb_bt709(YUV in_pixel)
{
  /*
  R'= Y' + 0.0000*U + 1.5748*V
  G'= Y' - 0.1873*U - 0.4681*V
  B'= Y' + 1.8556*U + 0.0000*V
  */
  RGB rgb;

  float f_y = (float)in_pixel.y;
  float f_u = (float)(in_pixel.u - 128);
  float f_v = (float)(in_pixel.v - 128);
  float f_r = f_y + 1.5748f*f_v;
  float f_g = f_y - 0.1873f*f_u - 0.4681f*f_v;
  float f_b = f_y + 1.8556f*f_u;
  int i_r = (int)f_r;
  int i_g = (int)f_g;
  int i_b = (int)f_b;
  rgb.r = usat8(i_r);
  rgb.g = usat8(i_g);
  rgb.b = usat8(i_b);
  return rgb;
}

static YUV rgb2yuv_bt709(RGB in) {
  /*
  Y'= 0.2126*R' + 0.7152*G' + 0.0722*B'
  U'=-0.1146*R' - 0.3854*G' + 0.5000*B'
  V'= 0.5000*R' - 0.4542*G' - 0.0458*B'
  */

  YUV yuv;
  float f_r = (float)in.r;
  float f_g = (float)in.g;
  float f_b = (float)in.b;
  float f_y = 0 + 0.2126f*f_r + 0.7152f*f_g + 0.0722f*f_b;
  float f_u = 0 - 0.1146f*f_r - 0.3854f*f_g + 0.5000f*f_b;
  float f_v = 0 + 0.5000f*f_r - 0.4542f*f_g - 0.0458f*f_b;
  int i_y = (int)f_y;
  int i_u = f_u + 128;
  int i_v = f_v + 128;
  
  yuv.y  = usat8(i_y);
  yuv.u = usat8(i_u);
  yuv.v = usat8(i_v);
  
  return yuv;
}

static uchar rgb2y_bt709(RGB in) {
  /*
  Y'= 0.2126*R' + 0.7152*G' + 0.0722*B'
  U'=-0.1146*R' - 0.3854*G' + 0.5000*B'
  V'= 0.5000*R' - 0.4542*G' - 0.0458*B'
  */

  float f_r = (float)in.r;
  float f_g = (float)in.g;
  float f_b = (float)in.b;
  float f_y = 0 + 0.2126f*f_r + 0.7152f*f_g + 0.0722f*f_b;
  int i_y = (int)f_y;
  
  return usat8(i_y);
}

RGB yuv2rgb_approx(YUV yuv) {
  RGB rgb;
  rgb.r = YUV2R(yuv.y, yuv.u, yuv.v);
  rgb.g = YUV2G(yuv.y, yuv.u, yuv.v);
  rgb.b = YUV2B(yuv.y, yuv.u, yuv.v);
  return rgb;
}

#define YUV2RGB(UNROLL)                                                                                                   \
void CAT2(yuv2rgb_iiio, UNROLL)(CAT(uchar,UNROLL) y, CAT(uchar,UNROLL) u, CAT(uchar,UNROLL) v, CAT(uint,UNROLL) *_pixel) { \
  CAT(uint,UNROLL) pixel;                                                                                                 \
  _Pragma ("unroll")                                                                                                      \
  for(int i=0; i < UNROLL; i++){                                                                                          \
    RGB rgb;                                                                                                              \
    rgb.r = YUV2R(y[i], u[i], v[i]);                                                                                      \
    rgb.g = YUV2G(y[i], u[i], v[i]);                                                                                      \
    rgb.b = YUV2B(y[i], u[i], v[i]);                                                                                      \
    pixel[i] = rgb_to_int(rgb);                                                                                           \
  }                                                                                                                       \
  *_pixel = pixel;                                                                                                        \
}
void yuv2rgb_iiio_1(uchar y, uchar u, uchar v, uint *_pixel) {
  uint pixel;
  RGB rgb;
  rgb.r = YUV2R(y, u, v); 
  rgb.g = YUV2G(y, u, v);
  rgb.b = YUV2B(y, u, v); 
  *_pixel = rgb_to_int(rgb);
}
YUV2RGB(2)
YUV2RGB(4)
YUV2RGB(8)

#define RGB2YUV(UNROLL)                                                                                                      \
void CAT2(rgb2yuv_iooo, UNROLL)(CAT(uint,UNROLL) pixel, CAT(uchar,UNROLL) *_y, CAT(uchar,UNROLL) *_u, CAT(uchar,UNROLL) *_v) {\
  CAT(uchar,UNROLL) y, u, v;                                                                                                 \
  _Pragma ("unroll")                                                                                                         \
  for(int i=0; i < UNROLL; i++){                                                                                             \
    RGB rgb = int_to_rgb(pixel[i]);                                                                                          \
    y[i] = RGB2Y(rgb.r, rgb.g, rgb.b);                                                                                       \
    u[i] = RGB2U(rgb.r, rgb.g, rgb.b);                                                                                       \
    v[i] = RGB2V(rgb.r, rgb.g, rgb.b);                                                                                       \
  }                                                                                                                          \
  *_y = y;                                                                                                                   \
  *_u = u;                                                                                                                   \
  *_v = v;                                                                                                                   \
}
void rgb2yuv_iooo_1(uint pixel, uchar *_y, uchar *_u, uchar *_v) {
  uchar y, u, v;
  RGB rgb = int_to_rgb(pixel);   
  *_y = RGB2Y(rgb.r, rgb.g, rgb.b);
  *_u = RGB2U(rgb.r, rgb.g, rgb.b);
  *_v = RGB2V(rgb.r, rgb.g, rgb.b);
}
RGB2YUV(2)
RGB2YUV(4)
RGB2YUV(8)

#define RGB2YUV_FLOAT(UNROLL)                                                                                                 \
void CAT2(rgb2yuv_float, UNROLL)(CAT(uint,UNROLL) pixel, CAT(uchar,UNROLL) *_y, CAT(uchar,UNROLL) *_u, CAT(uchar,UNROLL) *_v) {\
  CAT(uchar,UNROLL) y, u, v;                                                                                                  \
  _Pragma ("unroll")                                                                                                          \
  for(int i=0; i < UNROLL; i++){                                                                                              \
    RGB rgb = int_to_rgb(pixel[i]);                                                                                           \
    YUV yuv = rgb2yuv_bt709(rgb);                                                                                             \
    y[i] = yuv.y;                                                                                                             \
    u[i] = yuv.u;                                                                                                             \
    v[i] = yuv.v;                                                                                                             \
  }                                                                                                                           \
  *_y = y;                                                                                                                    \
  *_u = u;                                                                                                                    \
  *_v = v;                                                                                                                    \
}

void rgb2yuv_float_1(uint pixel, uchar *_y, uchar *_u, uchar *_v) {
  uchar y, u, v;
  RGB rgb = int_to_rgb(pixel);   
  YUV yuv = rgb2yuv_bt709(rgb);
  *_y = yuv.y;
  *_u = yuv.u;
  *_v = yuv.v;
}
RGB2YUV_FLOAT(2)
RGB2YUV_FLOAT(4)
RGB2YUV_FLOAT(8)

#define RGB2Y_FLOAT(UNROLL)                                                                                                 \
void CAT2(rgb2y_float, UNROLL)(CAT(uint,UNROLL) pixel, CAT(uchar,UNROLL) *_y) {\
  CAT(uchar,UNROLL) y;                                                                                                  \
  _Pragma ("unroll")                                                                                                          \
  for(int i=0; i < UNROLL; i++){                                                                                              \
    RGB rgb = int_to_rgb(pixel[i]);                                                                                           \
    y[i]  = rgb2y_bt709(rgb);                                                                                         \
  }                                                                                                                           \
  *_y = y;                                                                                                                  \
}
void rgb2y_float_1(uint pixel, uchar *_y) {
  uchar y;
  RGB rgb = int_to_rgb(pixel);   
  *_y = rgb2y_bt709(rgb);
}
RGB2Y_FLOAT(2)
RGB2Y_FLOAT(4)
RGB2Y_FLOAT(8)

#define YUV2RGB_FLOAT(UNROLL)                                                                                              \
void CAT2(yuv2rgb_float, UNROLL)(CAT(uchar,UNROLL) y, CAT(uchar,UNROLL) u, CAT(uchar,UNROLL) v, CAT(uint,UNROLL) *_pixel) { \
  CAT(uint,UNROLL) pixel;                                                                                                  \
  _Pragma ("unroll")                                                                                                       \
  for(int i=0; i < UNROLL; i++){                                                                                           \
    YUV yuv;                                                                                                               \
    yuv.y = y[i];                                                                                                          \
    yuv.u = u[i];                                                                                                          \
    yuv.v = v[i];                                                                                                          \
    RGB rgb = yuv2rgb_bt709(yuv);                                                                                          \
    pixel[i] = rgb_to_int(rgb);                                                                                            \
  }                                                                                                                        \
  *_pixel = pixel;                                                                                                         \
}

void yuv2rgb_float_1(uchar y, uchar u, uchar v, uint *_pixel) {
  uint pixel;
  YUV yuv;
  yuv.y = y;
  yuv.u = u;
  yuv.v = v;
  RGB rgb = yuv2rgb_bt709(yuv); 
  *_pixel = rgb_to_int(rgb);
}

YUV2RGB_FLOAT(2)
YUV2RGB_FLOAT(4)
YUV2RGB_FLOAT(8)
