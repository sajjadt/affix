#pragma once

enum  ConvertPolicy {
  WRAP, SATURATE
};

unsigned char cmp_gt(char a, char b) {
  return a > b ? 255 : 0;
}

#define INVERT(UNROLL)                                                                   \
inline void CAT2(invert_uchar, UNROLL) (TTYPE(uchar,UNROLL) a, TTYPE(uchar,UNROLL) *out) {\
  *out = ~a;                                                                         \
}                                                                                     \
inline void CAT2(invert_short, UNROLL) (TTYPE(short,UNROLL) a,  TTYPE(short,UNROLL) *out) {\
  *out = ~a;                                                                         \
}
inline void invert_uchar_32 (struct uchar32 a, struct uchar32 *out) {
  struct uchar32 _out;
  _out.lo = ~(a.lo);
  _out.hi = ~(a.hi);
  *out = _out;
}
INVERT(2)
INVERT(4)
INVERT(8)
INVERT(16)

unsigned char saturate_add(unsigned char a, unsigned char b) {
  unsigned short val = (unsigned short)a + (unsigned short)b;
  return val >= 255? 255 : (unsigned char)val;
}
unsigned char cmp(unsigned char a, unsigned char b) {
  return a == b ? 255 : 0;
}
unsigned char blend(unsigned char a, unsigned char b, unsigned char mask) {
  if (mask)
    return b;
  else
    return a;
}

unsigned char saturate_sub(short a, short b) {
  short val = a - b;
  return val < 0 ? 0 : (unsigned char)val;
}

#define AND(UNROLL)                                                                   \
inline void CAT2(and_uchar, UNROLL) (TYPE(a, uchar,UNROLL), TYPE(b, uchar,UNROLL), TYPE(*out, uchar,UNROLL)) {\
  *out = a&b;                                                                         \
}                                                                                     \
inline void CAT2(and_short, UNROLL) (TYPE(a, short,UNROLL), TYPE(b, short,UNROLL), TYPE(*out, short,UNROLL)) {\
  *out = a&b;                                                                         \
}
AND(1)
AND(2)
AND(4)
AND(8)
AND(16)

#define OR(UNROLL)                                                                   \
inline void CAT2(or_uchar, UNROLL) (TYPE(a, uchar,UNROLL), TYPE(b, uchar,UNROLL), TYPE(*out, uchar,UNROLL)) {\
  *out = a|b;                                                                         \
}                                                                                     \
inline void CAT2(or_short, UNROLL) (TYPE(a, short,UNROLL), TYPE(b, short,UNROLL), TYPE(*out, short,UNROLL)) {\
  *out = a|b;                                                                         \
}
OR(1)
OR(2)
OR(4)
OR(8)
OR(16)

enum NormType {
  L1, L2
};

#define PHASE_REGION(UNROLL)                                                                          \
void CAT2(phase_region,UNROLL)(CAT(short,UNROLL) af, CAT(short,UNROLL) bf, CAT(short,UNROLL)  *_ret) { \
  CAT(short,UNROLL) ret = 0;                                                                          \
  _Pragma("unroll")                                                                                   \
  for(int i  = 0;i < UNROLL ;i++) {                                                                   \
    float ratio = af[i]/(float)bf[i];                                                                 \
    if (ratio< -0.2415f)                                                                              \
      ret[i] = 2 ;                                                                                    \
    else if (ratio < -0.415f)                                                                         \
      ret[i] = 3;                                                                                     \
    else if (ratio < 0.415f)                                                                          \
      ret[i] = 0;                                                                                     \
    else                                                                                              \
      ret[i] = 1;                                                                                     \
  }                                                                                                   \
  *_ret = ret;                                                                                        \
}

void phase_region_1(short af, short bf, short  *_ret) {
  short ret = 0;
  float ratio = af/(float)bf;
  if (ratio< -0.2415f)
    ret = 2;
  else if (ratio < -0.415f)
    ret = 3;
  else if (ratio < 0.415f)
    ret = 0;
  else
    ret = 1;
  *_ret = ret;
}
PHASE_REGION(2)
PHASE_REGION(4)
PHASE_REGION(8)
PHASE_REGION(16)

void phase_region_32(struct short32 af, struct short32 bf, struct short32  *_ret) {
  struct short32 ret;
  _Pragma("unroll")
  for(int i  = 0;i < 16 ;i++) {
    float ratio = af.lo[i]/(float)bf.lo[i];
    if (ratio< -0.2415f)
      ret.lo[i] = 2;
    else if (ratio < -0.415f)
      ret.lo[i] = 3;
    else if (ratio < 0.415f)
      ret.lo[i] = 0;
    else
      ret.lo[i] = 1;
    ratio = af.hi[i]/(float)bf.hi[i];
    if (ratio< -0.2415f)
      ret.hi[i] = 2;
    else if (ratio < -0.415f)
      ret.hi[i] = 3;
    else if (ratio < 0.415f)
      ret.hi[i] = 0;
    else
      ret.hi[i] = 1;
  }
  *_ret = ret;
}


#define ABSDIFF(UNROLL)                                                                          \
void CAT2(absdiff,UNROLL)(CAT(uchar,UNROLL) a, CAT(uchar,UNROLL) b, CAT(uchar,UNROLL)  *_ret) { \
  CAT(uchar,UNROLL) ret = 0;                                                                          \
  _Pragma("unroll")                                                                                   \
  for(int i  = 0;i < UNROLL ;i++) {                                                                   \
    if (a[i] > b[i])                                                                                  \
      ret[i] = a[i] - b[i];                                                                           \
    else                                                                                              \
      ret[i] = b[i] - a[i];                                                                           \
  }                                                                                                   \
  *_ret = ret;                                                                                        \
}

void absdiff_1(uchar a, uchar b, uchar  *_ret) {
  if (a < b)
    *_ret = b - a;
  else
    *_ret = a - b;
}

ABSDIFF(2)
ABSDIFF(4)
ABSDIFF(8)
ABSDIFF(16)

void absdiff_32(struct uchar32 a, struct uchar32 b, struct uchar32  *_ret) {
  uchar16 lo, hi;
  absdiff_16(a.lo, b.lo, &lo);
  absdiff_16(a.hi, b.hi, &hi);

  _ret->lo = lo;
  _ret->hi = hi;
}

#define MAG_L1(UNROLL)                                                                                \
void CAT2(magnitude_l1,UNROLL)(CAT(short,UNROLL) af, CAT(short,UNROLL) bf, CAT(short,UNROLL)  *_ret) { \
  CAT(short,UNROLL) ret = 0;                                                                          \
  _Pragma("unroll")                                                                                   \
  for(int i  = 0;i < UNROLL ;i++) {                                                                   \
    if (af[i]<0) af[i] *= -1;                                                                         \
    if (bf[i]<0) bf[i] *= -1;                                                                         \
    ret[i] =  af[i] + bf[i];                                                                          \
  }                                                                                                   \
    *_ret = ret;                                                                                      \
}

#define MAG_L2(UNROLL)                                                                                \
void CAT2(magnitude_l2,UNROLL)(CAT(short,UNROLL) af, CAT(short,UNROLL) bf, CAT(short,UNROLL)  *_ret) { \
  CAT(short,UNROLL) ret = 0;                                                                          \
  _Pragma("unroll")                                                                                   \
  for(int i  = 0;i < UNROLL ;i++) {                                                                   \
    ret[i] =  (short) sqrt( (float)( ((int)af[i]*(int)af[i] + (int)bf[i]*(int)bf[i]) ) );             \
  }                                                                                                   \
  *_ret = ret;                                                                                        \
}

void magnitude_l1_1 (short af, short bf, short* ret) {
  if (af<0) af *= -1;
  if (bf<0) bf *= -1;
  *ret =  af + bf;
}
MAG_L1(2)
MAG_L1(4)
MAG_L1(8)
MAG_L1(16)

void magnitude_l2_1 (short af, short bf, short* ret) {
  *ret =  (short) sqrt( (float)( ((int)af*(int)af + (int)bf*(int)bf) ) );
}
MAG_L2(2)
MAG_L2(4)
MAG_L2(8)
MAG_L2(16)

void magnitude_l2_32 (struct short32 af, struct short32 bf, struct short32* _ret) {
  struct short32 ret;
  _Pragma("unroll")
  for(int i  = 0;i < 16 ;i++) {
    ret.lo[i] =  (short) sqrt( (float)( ((int)af.lo[i]*(int)af.lo[i] + (int)bf.lo[i]*(int)bf.lo[i])));
    ret.hi[i] =  (short) sqrt( (float)( ((int)af.hi[i]*(int)af.hi[i] + (int)bf.hi[i]*(int)bf.hi[i])));
  }
  *_ret = ret; 
}

uchar down_bit_depth(short in_pixel, enum ConvertPolicy policy, int shift) {
  if (policy == WRAP) {
    return (uchar) (in_pixel >> shift);
  } else{ //(policy == SATURATE)
    short temp = in_pixel >> shift;
    temp = temp < 0 ? 0 : temp;
    temp = temp > 255 ? 255: temp;
    return (uchar) temp;
  }
}

#define THRESHOLD(UNROLL)                                                      \
void CAT2(threshold,UNROLL)(CAT(short,UNROLL) input, CAT(uchar,UNROLL) *_ret) { \
  CAT(uchar,UNROLL) ret = 0;                                                   \
  short low = 50;                                                              \
  short high = 170;                                                            \
  _Pragma("unroll")                                                            \
  for(int i =0; i < UNROLL; i++) {                                             \
    if (input[i] > high)                                                       \
      ret[i] = 255;                                                            \
    else if(input[i] < low)                                                    \
      ret[i] = 0;                                                              \
    else                                                                       \
      ret[i] = (uchar)input[i];                                                \
  }                                                                            \
  *_ret = ret;                                                                 \
}
void threshold_1(short input, uchar *_ret) {
  uchar ret = 0;
  short low = 50;
  short high = 170;
  if (input > high)
    ret = 255;
  else if(input < low)
    ret = 0;
  else
    ret = (uchar)input;

  *_ret = ret;
}
THRESHOLD(2)
THRESHOLD(4)
THRESHOLD(8)
THRESHOLD(16)

void threshold_32 (struct short32 input, struct uchar32 *_ret) {
  struct uchar32 ret;
  short low = 50;
  short high = 170;
  _Pragma("unroll")
  for(int i =0; i < 16; i++) {
    if (input.lo[i] > high)
      ret.lo[i] = 255;
    else if(input.lo[i] < low)
      ret.lo[i] = 0;
    else
      ret.lo[i] = (uchar)input.lo[i];
    if (input.hi[i] > high)
      ret.hi[i] = 255;
    else if(input.hi[i] < low)
      ret.hi[i] = 0;
    else
      ret.hi[i] = (uchar)input.hi[i];
  }
  *_ret = ret;
}

#define THRESHOLD_UCHAR(UNROLL)                                                      \
void CAT2(threshold_uchar,UNROLL)(CAT(uchar,UNROLL) input, CAT(uchar,UNROLL) *_ret) { \
  CAT(uchar,UNROLL) ret = 0;                                                   \
  short low = 50;                                                              \
  short high = 170;                                                            \
  _Pragma("unroll")                                                            \
  for(int i =0; i < UNROLL; i++) {                                             \
    if (input[i] > high)                                                       \
      ret[i] = 255;                                                            \
    else if(input[i] < low)                                                    \
      ret[i] = 0;                                                              \
    else                                                                       \
      ret[i] = (uchar)input[i];                                                \
  }                                                                            \
  *_ret = ret;                                                                 \
}
void threshold_uchar_1(uchar input, uchar *_ret) {
  uchar ret = 0;
  uchar low = 50;
  uchar high = 170;
  if (input > high)
    ret = 255;
  else if(input < low)
    ret = 0;
  else
    ret = (uchar)input;

  *_ret = ret;
}
THRESHOLD_UCHAR(2)
THRESHOLD_UCHAR(4)
THRESHOLD_UCHAR(8)
THRESHOLD_UCHAR(16)

#define ADD_UCHAR(UNROLL)                                                      \
void CAT2(add_uchar,UNROLL)(CAT(uchar,UNROLL) input0, CAT(uchar,UNROLL) input1, CAT(uchar,UNROLL) *_ret) { \
  CAT(uchar,UNROLL) ret = 0;                                                   \
  _Pragma("unroll")                                                            \
  for(int i =0; i < UNROLL; i++) {                                             \
      ret[i] = input0[i] + input1[i] ;                                         \
  }                                                                            \
  *_ret = ret;                                                                 \
}
void add_uchar_1(uchar input0, uchar input1, uchar *_ret) {
  *_ret = input0 + input1;
}

ADD_UCHAR(2)
ADD_UCHAR(4)
ADD_UCHAR(8)
ADD_UCHAR(16)


#define ADD_SHORT(UNROLL)                                                      \
void CAT2(add_short,UNROLL)(CAT(short,UNROLL) input0, CAT(short,UNROLL) input1, CAT(short,UNROLL) *_ret) { \
  CAT(short,UNROLL) ret = 0;                                                   \
  _Pragma("unroll")                                                            \
  for(int i =0; i < UNROLL; i++) {                                             \
      ret[i] = input0[i] + input1[i] ;                                         \
  }                                                                            \
  *_ret = ret;                                                                 \
}
void add_short_1(uchar input0, uchar input1, uchar *_ret) {
  *_ret = input0 + input1;
}

ADD_SHORT(2)
ADD_SHORT(4)
ADD_SHORT(8)
ADD_SHORT(16)

#define SUB_SHORT(UNROLL)                                                      \
void CAT2(sub_short,UNROLL)(CAT(short,UNROLL) input0, CAT(short,UNROLL) input1, CAT(short,UNROLL) *_ret) { \
  CAT(short,UNROLL) ret = 0;                                                   \
  _Pragma("unroll")                                                            \
  for(int i =0; i < UNROLL; i++) {                                             \
      ret[i] = input0[i] - input1[i] ;                                         \
  }                                                                            \
  *_ret = ret;                                                                 \
}
void sub_short_1(uchar input0, uchar input1, uchar *_ret) {
  *_ret = input0 - input1;
}
SUB_SHORT(2)
SUB_SHORT(4)
SUB_SHORT(8)
SUB_SHORT(16)

#define PW_MUL_SHORT(UNROLL)                                                      \
void CAT2(pixelwise_mul_short,UNROLL)(CAT(short,UNROLL) input0, CAT(short,UNROLL) input1, CAT(short,UNROLL) *_ret) { \
  CAT(short,UNROLL) ret = 0;                                                   \
  _Pragma("unroll")                                                            \
  for(int i =0; i < UNROLL; i++) {                                             \
      ret[i] = input0[i] * input1[i] ;                                         \
  }                                                                            \
  *_ret = ret;                                                                 \
}
void pixelwise_mul_short_1(uchar input0, uchar input1, uchar *_ret) {
  *_ret = input0 * input1;
}
PW_MUL_SHORT(2)
PW_MUL_SHORT(4)
PW_MUL_SHORT(8)
PW_MUL_SHORT(16)

#define SIMP_THRESHOLD_UCHAR(UNROLL)                                                      \
void CAT2(simple_threshold_uchar,UNROLL)(CAT(uchar,UNROLL) input, CAT(uchar,UNROLL) *_ret) { \
  CAT(uchar,UNROLL) ret = 0;                                                   \
  short low = 15;                                                              \
  _Pragma("unroll")                                                            \
  for(int i =0; i < UNROLL; i++) {                                             \
    if(input[i] < low)                                                         \
      ret[i] = 0;                                                              \
    else                                                                       \
      ret[i] = (uchar)input[i];                                                \
  }                                                                            \
  *_ret = ret;                                                                 \
}
void simple_threshold_uchar_1(uchar input, uchar *_ret) {
  uchar ret = 0;
  uchar low = 15;
  if(input < low)
    ret = 0;
  else
    ret = (uchar)input;
  *_ret = ret;
}
SIMP_THRESHOLD_UCHAR(2)
SIMP_THRESHOLD_UCHAR(4)
SIMP_THRESHOLD_UCHAR(8)
SIMP_THRESHOLD_UCHAR(16)

#define GRADIENT(UNROLL)                                                      \
void CAT2(gradient,UNROLL)(CAT(short,UNROLL) x_in, CAT(short,UNROLL) y_in,    \
  CAT(uchar,UNROLL) *_out0, CAT(uchar,UNROLL) *_out1) {                       \
  const short thresh_low = 400;                                               \
  const short thresh_high = 400;                                              \
  uchar vmask = 0xff;                                                         \
  TYPE(out0, uchar, UNROLL);                                                  \
  TYPE(out1, uchar, UNROLL);                                                  \
  _Pragma("unroll")                                                           \
  for(int i =0; i < UNROLL; i++) {                                            \
    short inputH = abs(x_in[i]);                                              \
    short inputV = abs(y_in[i]);                                              \
    short gradient = (inputH + inputV);                                       \
    uchar low_val = cmp_gt(gradient, thresh_low);           \
    uchar high_val = cmp_gt(gradient, thresh_high);         \
    out0[i] = low_val;                                      \
    out1[i] = high_val;                                     \
  }                                                         \
  *_out0 = out0;                                            \
  *_out1 = out1;                                            \
}

void gradient_1(short x_in, short y_in,    
  uchar *_out0, uchar *_out1) {                       
  const short thresh_low = 400;                                               
  const short thresh_high = 400;                                              
  uchar vmask = 0xff;                                                         
  uchar out0, out1;                                                           
  short inputH = abs(x_in);                                              
  short inputV = abs(y_in);                                              
  short gradient = (inputH + inputV);                                       
  uchar low_val = cmp_gt(gradient, thresh_low);           
  uchar high_val = cmp_gt(gradient, thresh_high);        
  *_out0 = low_val;                                            
  *_out1 = high_val;                                            
}

GRADIENT(2)
GRADIENT(4)
GRADIENT(8)
GRADIENT(16)

#define BGSUPP(UNROLL)                                                                        \
inline void CAT2(bg_suppress,UNROLL) (CAT(uchar,UNROLL) l, CAT(uchar,UNROLL) a, CAT(uchar,UNROLL) b,\
           CAT(uchar,UNROLL) *_out_l,CAT(uchar,UNROLL) *_out_a, CAT(uchar,UNROLL) *_out_b) {     \
  const uchar L_thresh = 230;   \
  const uchar a_low = 116;        \
  const uchar a_high = 140;       \
  const uchar b_low = 116;      \
  const uchar b_high = 140;     \
  const uchar whitepointL = 255;\
  const uchar whitepointa = 128;\
  const uchar whitepointb = 128;\
  const uchar L_low_thresh = 70;\
  const uchar neutral = 128;    \
  const uchar plusminus = 10;   \
  uchar neutral_plus_plusminus = saturate_add(neutral, plusminus);\
  uchar neutral_minus_plusminus = saturate_sub(neutral, plusminus);\
  CAT(uchar,UNROLL) out_l , out_a, out_b;\
  _Pragma("unroll")\
  for(int i=0 ;i < UNROLL; i++){    \
    uchar tmp = max(l[i], L_thresh);\
    uchar block0 = cmp(l[i], tmp);   \
    tmp = min(l[i], L_low_thresh);    \
    uchar block1 = cmp(l[i], tmp);    \
    tmp = min(a[i], a_high);           \
    block0 = block0 &  cmp(a[i], tmp);    \
    tmp = max(a[i], a_low);              \
    block0 =  block0 & cmp(a[i], tmp);     \
    tmp = min(a[i], neutral_plus_plusminus);    \
    block1 = block1 & cmp(a[i], tmp);     \
    tmp = max(a[i], neutral_minus_plusminus);    \
    block1 = block1 & cmp(a[i], tmp);     \
    tmp = min(b[i], b_high);               \
    block0 = block0 & cmp(b[i], tmp);     \
    tmp = max(b[i], b_low);                  \
    block0 = block0 & cmp(b[i], tmp);         \
    tmp = min(b[i], neutral_plus_plusminus);    \
    block1 = block1 & cmp(b[i], tmp);            \
    tmp = max(b[i], neutral_minus_plusminus);    \
    block1 = block1 & cmp(b[i], tmp);                 \
    uchar outputL = blend(l[i], whitepointL, block0);    \
    uchar outputA = ((whitepointa&block0)| (neutral& block1));    \
    uchar  outputB = ((whitepointb&block0)|(neutral& block1));    \
    uchar  block0_or_block1 = (block0|block1);                   \
    outputA = (outputA| ((~block0_or_block1) & a[i]));    \
    outputB = (outputB| ((~block0_or_block1) & b[i]));    \
    out_l[i] = outputL;    \
    out_a[i] = outputA;    \
    out_b[i] = outputB;    \
  }    \
  *_out_l = out_l;    \
  *_out_a = out_a;    \
  *_out_b = out_b;    \
}
inline void bg_suppress_1 (uchar l, uchar a, uchar b,
           uchar *_out_l,uchar *_out_a, uchar *_out_b) {     
  const uchar L_thresh = 230;   
  const uchar a_low = 116;        
  const uchar a_high = 140;       
  const uchar b_low = 116;      
  const uchar b_high = 140;     
  const uchar whitepointL = 255;
  const uchar whitepointa = 128;
  const uchar whitepointb = 128;
  const uchar L_low_thresh = 70;
  const uchar neutral = 128;    
  const uchar plusminus = 10;   
  uchar neutral_plus_plusminus = saturate_add(neutral, plusminus);
  uchar neutral_minus_plusminus = saturate_sub(neutral, plusminus);
  uchar out_l , out_a, out_b;

  uchar tmp = max(l, L_thresh);
  uchar block0 = cmp(l, tmp);   
  tmp = min(l, L_low_thresh);    
  uchar block1 = cmp(l, tmp);    
  tmp = min(a, a_high);           
  block0 = block0 &  cmp(a, tmp);    
  tmp = max(a, a_low);              
  block0 =  block0 & cmp(a, tmp);     
  tmp = min(a, neutral_plus_plusminus);    
  block1 = block1 & cmp(a, tmp);     
  tmp = max(a, neutral_minus_plusminus);    
  block1 = block1 & cmp(a, tmp);     
  tmp = min(b, b_high);               
  block0 = block0 & cmp(b, tmp);     
  tmp = max(b, b_low);                  
  block0 = block0 & cmp(b, tmp);         
  tmp = min(b, neutral_plus_plusminus);    
  block1 = block1 & cmp(b, tmp);            
  tmp = max(b, neutral_minus_plusminus);    
  block1 = block1 & cmp(b, tmp);                 
  uchar outputL = blend(l, whitepointL, block0);    
  uchar outputA = ((whitepointa&block0)| (neutral& block1));    
  uchar  outputB = ((whitepointb&block0)|(neutral& block1));    
  uchar  block0_or_block1 = (block0|block1);                   
  outputA = (outputA| ((~block0_or_block1) & a));    
  outputB = (outputB| ((~block0_or_block1) & b));    
  *_out_l = outputL;    
  *_out_a = outputA;    
  *_out_b = outputB;    
}

BGSUPP(2)
BGSUPP(4)
BGSUPP(8)
BGSUPP(16)

