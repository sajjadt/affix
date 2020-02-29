#pragma once

enum BORDER_TYPE {
  CONSTANT, REFLECT
};

static void sortNet9(unsigned char * const data) {
  int swap;
  //if (data[0] > data[5]) { swap = data[0]; data[0] = data[5]; data[5] = swap; }
  if (data[1] > data[6]) { swap = data[1]; data[1] = data[6]; data[6] = swap; }
  if (data[2] > data[7]) { swap = data[2]; data[2] = data[7]; data[7] = swap; }
  if (data[3] > data[8]) { swap = data[3]; data[3] = data[8]; data[8] = swap; }
  if (data[4] > data[9]) { swap = data[4]; data[4] = data[9]; data[9] = swap; }
  //if (data[0] > data[3]) { swap = data[0]; data[0] = data[3]; data[3] = swap; }
  if (data[5] > data[8]) { swap = data[5]; data[5] = data[8]; data[8] = swap; }
  if (data[1] > data[4]) { swap = data[1]; data[1] = data[4]; data[4] = swap; }
  if (data[6] > data[9]) { swap = data[6]; data[6] = data[9]; data[9] = swap; }
  //if (data[0] > data[2]) { swap = data[0]; data[0] = data[2]; data[2] = swap; }
  if (data[3] > data[6]) { swap = data[3]; data[3] = data[6]; data[6] = swap; }
  if (data[7] > data[9]) { swap = data[7]; data[7] = data[9]; data[9] = swap; }
  //if (data[0] > data[1]) { swap = data[0]; data[0] = data[1]; data[1] = swap; }
  if (data[2] > data[4]) { swap = data[2]; data[2] = data[4]; data[4] = swap; }
  if (data[5] > data[7]) { swap = data[5]; data[5] = data[7]; data[7] = swap; }
  if (data[8] > data[9]) { swap = data[8]; data[8] = data[9]; data[9] = swap; }
  if (data[1] > data[2]) { swap = data[1]; data[1] = data[2]; data[2] = swap; }
  if (data[3] > data[5]) { swap = data[3]; data[3] = data[5]; data[5] = swap; }
  if (data[4] > data[6]) { swap = data[4]; data[4] = data[6]; data[6] = swap; }
  if (data[7] > data[8]) { swap = data[7]; data[7] = data[8]; data[8] = swap; }
  if (data[1] > data[3]) { swap = data[1]; data[1] = data[3]; data[3] = swap; }
  if (data[4] > data[7]) { swap = data[4]; data[4] = data[7]; data[7] = swap; }
  if (data[2] > data[5]) { swap = data[2]; data[2] = data[5]; data[5] = swap; }
  if (data[6] > data[8]) { swap = data[6]; data[6] = data[8]; data[8] = swap; }
  if (data[2] > data[3]) { swap = data[2]; data[2] = data[3]; data[3] = swap; }
  if (data[4] > data[5]) { swap = data[4]; data[4] = data[5]; data[5] = swap; }
  if (data[6] > data[7]) { swap = data[6]; data[6] = data[7]; data[7] = swap; }
  if (data[3] > data[4]) { swap = data[3]; data[3] = data[4]; data[4] = swap; }
  if (data[5] > data[6]) { swap = data[5]; data[5] = data[6]; data[6] = swap; }
}

inline short box(uchar* buffer, int dim, int j) {
  int k, l;
  ushort coeffs[3][3] = {
  {1, 1, 1},
  {1, 1, 1},
  {1, 1, 1}};
  short acc = 0;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      acc += (coeffs[k][l] * (ushort) buffer[k*dim+l+j]);
    }
  }
  return acc/9;
}


inline short oriented_suppression(short* phase_buffer, short* mag_buffer, int dim, int j) {
  int k, l;
  
  short center_phase = phase_buffer[dim+j+1];
  short center_mag = mag_buffer[dim+j+1];

  if (center_phase == 0) { //0
    if (center_mag > mag_buffer[j+1] && center_mag> mag_buffer[2*dim+j+1])
      return center_mag;
    else
      return 0;
  } else if (center_phase == 1) { // 45
    if (center_mag > mag_buffer[j] && center_mag> mag_buffer[2*dim+j+2])
      return center_mag;
    else
      return 0;
  } else if (center_phase == 2) { //90
    if (center_mag > mag_buffer[dim+j] && center_mag> mag_buffer[dim+j+2])
      return center_mag;
    else
      return 0;
  } else { //135
    if (center_mag > mag_buffer[j+2] && center_mag> mag_buffer[2*dim+j])
      return center_mag;
    else
      return 0;
  }
}

inline short unit(uchar* buffer, int dim, int j) {
  int k, l;
  ushort coeffs[3][3] = {
  {0, 0, 0},
  {0, 1, 0},
  {0, 0, 0}};
  short acc = 0;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      acc += (coeffs[k][l] * (ushort) buffer[k*dim+l+j]);
    }
  }
  return acc;
}

inline short sobel_x(uchar* buffer, int dim, int j) {
  int k, l;
  short kernel_x[3][3] = {
  {-1, 0, 1},
  {-2, 0, 2},
  {-1, 0, 1}};

  short res_x = 0;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      res_x += kernel_x[k][l] * (short) buffer[k*dim+l+j];
    }
  }

  return res_x;
}

inline short sobel_y(uchar* buffer, int dim, int j) {
  int k, l;
  short kernel_y[3][3] = {
  {-1, -2, -1},
  {0, 0, 0},
  {1, 2, 1}};

  short res_y = 0;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      res_y += kernel_y[k][l] * (short) buffer[k*dim+l+j];
    }
  }

  return res_y;
}


inline short sobel_x_short(short* buffer, int dim, int j) {
  int k, l;
  short kernel_x[3][3] = {
  {-1, 0, 1},
  {-2, 0, 2},
  {-1, 0, 1}};

  short res_x = 0;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      res_x += kernel_x[k][l] * (short) buffer[k*dim+l+j];
    }
  }

  return res_x;
}

inline short sobel_y_short(short* buffer, int dim, int j) {
  int k, l;
  short kernel_y[3][3] = {
  {-1, -2, -1},
  {0, 0, 0},
  {1, 2, 1}};

  short res_y = 0;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      res_y += kernel_y[k][l] * (short) buffer[k*dim+l+j];
    }
  }

  return res_y;
}

static short gaussian5x5_bad(uchar* buffer, int dim, int j) {
  int k, l;
  unsigned short coeffs[5][5] = {
    {1, 4, 6, 4, 1},
    {4, 16, 24, 16, 4},
    {6, 24, 36, 24, 6},
    {4, 16, 24, 16, 4},
    {1, 4, 6, 4, 1}
  };
  int acc = 0;
  // 0,0
  #pragma unroll
  for (int k=0; k <5; ++k) {
    #pragma unroll
    for (int l=k+1; l < 5; ++l) {
      acc += coeffs[k][l] * (unsigned short) buffer[k*dim+l+j];
    }
  }
  return (acc>>8);
}

#define GEN_FILTER(DIM, IN_TYPE, OUT_TYPE)                                                              \
OUT_TYPE CAT2(filter,DIM) (IN_TYPE* buffer, constant short coeffs[DIM][DIM], int dim, int j, int shift){ \
  int k, l;                                                                                             \
  int acc = 0;                                                                                         \
  _Pragma("unroll")                                                                                     \
  for (int k=0; k < DIM; ++k) {                                                                         \
     _Pragma("unroll")                                                                                  \
    for (int l=0; l < DIM; ++l) {                                                                       \
      acc += (short)coeffs[k][l] * (short) buffer[k*dim+l+j];                                           \
    }                                                                                                   \
  }                                                                                                     \
  return (acc>>shift);                                                                                  \
}

GEN_FILTER(3, uchar, uchar)
GEN_FILTER(5, uchar, uchar)
GEN_FILTER(7, uchar, uchar)
GEN_FILTER(9, uchar, uchar)
GEN_FILTER(11, uchar, uchar)
GEN_FILTER(13, uchar, uchar)
GEN_FILTER(15, uchar, uchar)


inline short filter_zero(uchar* buffer, constant short coeffs[7][7], int dim, int j, int shift) {
  return 0;
}

// TODO gen automatically
inline short filter_symm7(uchar* buffer, constant short coeffs[7][7], int dim, int j, int shift) {
  int k, l;
  int acc = 0;

  //0,0
  short psum = buffer[0*dim+0+j] + buffer[0*dim+6+j]+
               buffer[6*dim+0+j] + buffer[6*dim+6+j];
  acc += (short)coeffs[0][0]*psum;

  // 0,1
  psum = buffer[0*dim+1+j] + buffer[0*dim+5+j] +
         buffer[1*dim+0+j] + buffer[1*dim+6+j] +
         buffer[5*dim+0+j] + buffer[5*dim+6+j] +
         buffer[6*dim+1+j] + buffer[6*dim+5+j];
  acc += (short)coeffs[0][1]*psum;

  // 0,2
  psum = buffer[0*dim+2+j] + buffer[0*dim+4+j] + 
         buffer[2*dim+0+j] + buffer[2*dim+6+j] +
         buffer[4*dim+0+j] + buffer[4*dim+6+j] +
         buffer[6*dim+2+j] + buffer[6*dim+4+j];
  acc += (short)coeffs[0][2]*psum; 

  //0, 3
  psum = buffer[0*dim+3+j] + buffer[3*dim+0+j] + 
         buffer[3*dim+6+j] + buffer[6*dim+3+j];
  acc += (short)coeffs[0][3]*psum; 

  // 1,1
  psum = buffer[1*dim+1+j] + buffer[1*dim+5+j] +
         buffer[5*dim+1+j] + buffer[5*dim+5+j];
  acc += (short)coeffs[1][1]*psum; 

  // 1,2
  psum = buffer[1*dim+2+j] + buffer[1*dim+4+j] +
         buffer[2*dim+3+j] + buffer[2*dim+5+j] + 
         buffer[4*dim+1+j] + buffer[4*dim+5+j] +
         buffer[5*dim+2+j] + buffer[5*dim+4+j];
  acc += (short)coeffs[1][2]*psum; 
  
  // 1,3
  psum = buffer[1*dim+3+j] + buffer[3*dim+1+j] +
         buffer[5*dim+3+j] + buffer[3*dim+5+j];
  acc += (short)coeffs[1][3]*psum; 

  // 2,2
  psum = buffer[2*dim+2+j] + buffer[2*dim+4+j] +
         buffer[4*dim+2+j] + buffer[4*dim+4+j];
  acc += (short)coeffs[2][2]*psum; 

  // 2,3
  psum = buffer[2*dim+3+j] + 
         buffer[3*dim+2+j] + buffer[3*dim+4+j] +
         buffer[4*dim+3+j];
  acc += (short)coeffs[2][3]*psum; 


  // 3,3
  psum = buffer[3*dim+3+j]; 
  acc += (short)coeffs[3][3]*psum; 

  return (acc>>shift);
}


#define GEN_SYMM_ROW_FILTER(DIM, IN_TYPE, OUT_TYPE)                                                         \
OUT_TYPE CAT2(row_filter_symm,DIM) (IN_TYPE* buffer, constant short coeffs[DIM], int dim, int j, int shift){ \
  int k, l;                                                                                                 \
  int acc = 0;                                                                                              \
  _Pragma("unroll")                                                                                         \
  for (int l=0; l < (DIM-1)/2; ++l) {                                                                       \
    acc += (short)coeffs[l] * ( (short)buffer[l+j] + (short)buffer[DIM-1-l+j]);                             \
  }                                                                                                         \
  acc += (short)coeffs[(DIM-1)/2] *  (short) buffer[(DIM-1)/2+j];                                          \
  return (acc>>shift);                                                                                      \
}

GEN_SYMM_ROW_FILTER(3, uchar, uchar)
GEN_SYMM_ROW_FILTER(5, uchar, uchar)
GEN_SYMM_ROW_FILTER(7, uchar, uchar)
GEN_SYMM_ROW_FILTER(9, uchar, uchar)
GEN_SYMM_ROW_FILTER(11, uchar, uchar)
GEN_SYMM_ROW_FILTER(13, uchar, uchar)


#define GEN_SYMM_COL_FILTER(DIM, IN_TYPE, OUT_TYPE)                                                          \
OUT_TYPE CAT2(col_filter_symm,DIM) (IN_TYPE* buffer, constant short coeffs[DIM], int dim, int j, int shift){ \
  int k, l;                                                                                                  \
  int acc = 0;                                                                                               \
  _Pragma("unroll")                                                                                          \
  for (int l=0; l < (DIM-1)/2; ++l) {                                                                        \
    acc += (short)coeffs[l] * ( (short) buffer[l*dim+j] +(short) buffer[(DIM-1-l)*dim+j]);                   \
  }                                                                                                          \
  acc += (short)coeffs[(DIM-1)/2] *  (short) buffer[((DIM-1)/2)*dim+j];                                      \
  return (acc>>shift);                                                                                       \
}

GEN_SYMM_COL_FILTER(3, uchar, uchar)
GEN_SYMM_COL_FILTER(5, uchar, uchar)
GEN_SYMM_COL_FILTER(7, uchar, uchar)
GEN_SYMM_COL_FILTER(9, uchar, uchar)
GEN_SYMM_COL_FILTER(11, uchar, uchar)
GEN_SYMM_COL_FILTER(13, uchar, uchar)


#define GEN_ROW_FILTER(DIM, IN_TYPE, OUT_TYPE)                                                         \
OUT_TYPE CAT2(row_filter,DIM) (IN_TYPE* buffer, constant short coeffs[DIM], int dim, int j, int shift){ \
  int k, l;                                                                                                 \
  int acc = 0;                                                                                              \
  _Pragma("unroll")                                                                                         \
  for (int l=0; l < DIM; ++l) {                                                                             \
    acc += (short)coeffs[l] * (short) buffer[l+j];                                                          \
  }                                                                                                         \
  return (acc>>shift);                                                                                      \
}
GEN_ROW_FILTER(3, uchar, uchar)
GEN_ROW_FILTER(5, uchar, uchar)
GEN_ROW_FILTER(7, uchar, uchar)
GEN_ROW_FILTER(9, uchar, uchar)
GEN_ROW_FILTER(11, uchar, uchar)
GEN_ROW_FILTER(13, uchar, uchar)


#define GEN_COL_FILTER(DIM, IN_TYPE, OUT_TYPE)                                                         \
OUT_TYPE CAT2(col_filter,DIM) (IN_TYPE* buffer, constant short coeffs[DIM], int dim, int j, int shift){ \
  int k, l;                                                                                                 \
  int acc = 0;                                                                                              \
  _Pragma("unroll")                                                                                         \
  for (int l=0; l < DIM; ++l) {                                                                             \
    acc += (short)coeffs[l] * (short) buffer[l*dim+j];                                                      \
  }                                                                                                         \
  return (acc>>shift);                                                                                      \
}
GEN_COL_FILTER(3, uchar, uchar)
GEN_COL_FILTER(5, uchar, uchar)
GEN_COL_FILTER(7, uchar, uchar)
GEN_COL_FILTER(9, uchar, uchar)
GEN_COL_FILTER(11, uchar, uchar)
GEN_COL_FILTER(13, uchar, uchar)

inline short filter_symm5(uchar* buffer, constant short coeffs[5][5], int dim, int j, int shift) {
  int k, l;
  int acc = 0;

  //0,0
  short psum = buffer[0*dim+0+j] + buffer[0*dim+4+j]+
               buffer[4*dim+0+j] + buffer[4*dim+4+j];
  acc += (short)coeffs[0][0]*psum;

  // 0,1
  psum = buffer[0*dim+1+j] + buffer[0*dim+3+j] +
         buffer[1*dim+0+j] + buffer[1*dim+4+j] +
         buffer[3*dim+0+j] + buffer[3*dim+4+j] +
         buffer[4*dim+1+j] + buffer[4*dim+3+j];
  acc += (short)coeffs[0][1]*psum;

  // 0,2
  psum = buffer[0*dim+2+j] + 
         buffer[2*dim+0+j] + buffer[2*dim+4+j] +
         buffer[4*dim+2+j] ;
  acc += (short)coeffs[0][2]*psum; 

  // 1,1
  psum = buffer[1*dim+1+j] + buffer[1*dim+3+j] +
         buffer[3*dim+1+j] + buffer[3*dim+3+j];
  acc += (short)coeffs[1][1]*psum; 

  // 1,2
  psum = buffer[1*dim+2+j] + 
         buffer[2*dim+1+j] + buffer[2*dim+3+j] + 
         buffer[3*dim+2+j] ;
  acc += (short)coeffs[1][2]*psum; 
  
  // 2,2
  psum = buffer[2*dim+2+j]; 
  acc += (short)coeffs[2][2]*psum; 

  return (acc>>shift);
}

inline short gaussian5x5x4(uchar* buffer, int dim, int j) {
  unsigned short coeffs[5][5] = {
  {4, 16, 24, 16, 4},
  {16, 64, 96, 64, 16},
  {24, 96, 144, 96, 24},
  {16, 64, 96, 64, 16},
  {4, 16, 24, 16, 4}};
  return filter_symm5(buffer, coeffs, dim, j, 6);
}

static short gaussian5x5(uchar* buffer, int dim, int j) {
  int k, l;
  unsigned short coeffs[5][5] = {
    {1, 4, 6, 4, 1},
    {4, 16, 24, 16, 4},
    {6, 24, 36, 24, 6},
    {4, 16, 24, 16, 4},
    {1, 4, 6, 4, 1}
  };
  int acc = 0;
  return filter_5(buffer, coeffs, dim, j, 8);
}

static short gaussian9x9(uchar* buffer, int dim, int j) {
  int k, l;
  unsigned short coeffs[9][9] = {
    {1, 8, 28, 56, 70, 56, 28, 8, 1},
    {8, 64, 224, 448, 560, 448, 224, 64, 8},
    {28, 224, 784, 1568, 1960, 1568, 784, 224, 28},
    {56, 448, 1568, 3136, 3920, 3136, 1568, 448, 56},
    {70, 560, 1960, 3920, 4900, 3920, 1960, 560, 70},
    {56, 448, 1568, 3136, 3920, 3136, 1568, 448, 56},
    {28, 224, 784, 1568, 1960, 1568, 784, 224, 28},
    {8, 64, 224, 448, 560, 448, 224, 64, 8},
    {1, 8, 28, 56, 70, 56, 28, 8, 1},
    
  };
  int acc = 0;
  return filter_9(buffer, coeffs, dim, j, 16);
}

inline uchar erode(uchar* buffer, int dim, int j) {
  int k, l;
  uchar min = 255;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      if (min >  buffer[k*dim+l+j])
        min = buffer[k*dim+l+j];
    }
  }
  //printf("%d ", min);
  return min;
}

inline uchar dilate(uchar* buffer, int dim, int j) {
  int k, l;
  uchar max = 0;

  #pragma unroll
  for (int k=0; k <3; ++k) {
    #pragma unroll
    for (int l=0; l < 3; ++l) {
      if (max <  buffer[k*dim+l+j])
        max = buffer[k*dim+l+j];
    }
  }
  return max;
}


inline uchar error_diffuse(uchar* buffer, int dim, int j) {
  short quant_error;


  // uchar oldpixel = buffer(dim+1+j);
  // in.at<uchar>(y, x) = oldpixel > 127 ? 255 :  0;
  // quant_error = oldpixel - in.at<uchar>(y, x);
  // in.at<uchar>(y, x+1)  = in.at<uchar>(y, x+1) + 7/16.0 * quant_error;
  // in.at<uchar>(y+1, x-1) = in.at<uchar>(y+1, x-1) + 3/16.0 * quant_error;
  // in.at<uchar>(y+1, x) = in.at<uchar>(y+1, x) + 5/16.0 * quant_error;
  // in.at<uchar>(y+1, x+1) = in.at<uchar>(y+1, x+1) + 1/16.0 * quant_error;
  return 0;
}


inline uchar census(short* buffer, int dim, int j) {

  uchar current_val = 0;
  int center_add = dim + 1 + j;
  short center_val = buffer[center_add];

  if (center_val < buffer[center_add - dim - 1])
    current_val |= 1;
  current_val <<= 1;

  if (center_val < buffer[center_add - dim])
    current_val |= 1;
  current_val <<= 1;

  if (center_val < buffer[center_add - dim + 1])
    current_val |= 1;
  current_val <<= 1;

  if (center_val < buffer[center_add  - 1])
    current_val |= 1;
  current_val <<= 1;

  if (center_val < buffer[center_add + 1])
    current_val |= 1;
  current_val <<= 1;

  if (center_val < buffer[center_add + dim - 1])
    current_val |= 1;
  current_val <<= 1;

  if (center_val < buffer[center_add + dim])
    current_val |= 1;
  current_val <<= 1;

  if (center_val < buffer[center_add + dim + 1])
    current_val |= 1;
  current_val <<= 1;

  return current_val;
}


inline uchar median(uchar* buffer, int dim, int j) {
  uchar ch_buffer[9];

  #pragma unroll
  for (int k=0; k <3; ++k)
    #pragma unroll
    for (int l=0; l < 3; ++l)
      ch_buffer[k*3+l] = buffer[k*dim+l+j];

  sortNet9(ch_buffer);
  return ch_buffer[5];
}
