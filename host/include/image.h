#pragma once
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <memory>
#include <VX/vx.h>
#include <assert.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include "common.h"

#define AOCL_ALIGNMENT 64

class Dimension {
public:
  int rows;
  int cols;
  int total() const {return rows*cols;}
};

enum Color {
  RGBX, GRAYSCALE
};

// Image supported by OpenVX
// VX_DF_IMAGE_RGB
// VX_DF_IMAGE_RGBX
// VX_DF_IMAGE_U8
// VX_DF_IMAGE_S16
// VX_DF_IMAGE_U32
// VX_DF_IMAGE_NV12

// Represent images and arrays
class Matrix {
public:
  unsigned char* data;
  Dimension dim;
  int elemSize;
  int alloc_size;

  int int_data(int index) {
    assert (elemSize == 4);
    return ((int*)data)[index];
  }

  // Factory
  static std::unique_ptr<Matrix> makeMatrix(Dimension dim, int elemSize) {
    Matrix* img_out = new Matrix;
    img_out->dim = dim;
    img_out->elemSize = elemSize;

    int img_size = img_out->total();

    if (img_size % 32)
      img_out->alloc_size =  img_size + (32 - img_size % 32);
    else
      img_out->alloc_size =  img_size;

     posix_memalign ((void**)&img_out->data, AOCL_ALIGNMENT, img_out->alloc_size);
    // img_out->data = new unsigned char [img_out->alloc_size];
    return std::unique_ptr<Matrix>(img_out);
  }

  static std::unique_ptr<Matrix> randomMatrix(Dimension dim, int elemSize) {
    Matrix* img_out = new Matrix;
    img_out->dim = dim;
    img_out->elemSize = elemSize;

    int img_size = img_out->total();

    if (img_size % 32)
      img_out->alloc_size =  img_size + (32 - img_size % 32);
    else
      img_out->alloc_size =  img_size;

    img_out->data =  new unsigned char [img_out->alloc_size];
    for (int i= 0;i < img_out->total(); i++) {
      img_out->data[i] = rand() % 105;
    }
    return std::unique_ptr<Matrix>(img_out);
  }

static std::unique_ptr<Matrix> niceMatrix(Dimension dim, int elemSize) {
    Matrix* img_out = new Matrix;
    img_out->dim = dim;
    img_out->elemSize = elemSize;

    int img_size = img_out->total();

    if (img_size % 32)
      img_out->alloc_size =  img_size + (32 - img_size % 32);
    else
      img_out->alloc_size =  img_size;

    img_out->data =  new unsigned char [img_out->alloc_size];
    for (int i= 0;i < img_out->total(); i++) {
      img_out->data[i] = i%255;
    }
    return std::unique_ptr<Matrix>(img_out);
  }

  static std::unique_ptr<Matrix> nullMatrix() {
    Matrix* img_out = new Matrix;
    img_out->dim = Dimension{.rows=0, .cols=0};
    img_out->elemSize = 0;
    img_out->data =  NULL;
    img_out->alloc_size = 0;
    return std::unique_ptr<Matrix>(img_out);
  }

  std::unique_ptr<Matrix> clone() {
    Matrix* img_out = new Matrix;
    img_out->dim = dim;
    img_out->alloc_size = alloc_size;
    img_out->elemSize = elemSize;
    img_out->data =  new unsigned char [img_out->total()];
    memcpy(img_out->data, data, total());
    return std::unique_ptr<Matrix>(img_out);
  }

  // Convertors
  vx_image to_vx_image(vx_context context) {
    // TODO
    vx_image m_image = vxCreateImage(context,
                                      dim.cols,
                                      dim.rows,
                                      elemSize == 4 ? VX_DF_IMAGE_RGBX: VX_DF_IMAGE_U8);

    //create a vx rectangle object which corresponds to the full image
    vx_rectangle_t rectFullMatrix = {0, 0, dim.cols, dim.rows};
    vx_map_id map_id;
    vx_uint8 *data_ptr;
    vx_imagepatch_addressing_t imagepatch;
    vxMapImagePatch(m_image, &rectFullMatrix, 0, &map_id, &imagepatch, (void **)&data_ptr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);
    memcpy(data_ptr, data, total());
    vxUnmapImagePatch(m_image, map_id);

    return m_image;
  }

  static std::unique_ptr<Matrix> from_mat(cv::Mat& mat) {

    Dimension dim{.rows=mat.rows, .cols= mat.cols};

    std::unique_ptr<Matrix> image_out =  makeMatrix(dim, mat.elemSize());
    memcpy(image_out->data, mat.data, image_out->total());
    return image_out;
  }

  static std::unique_ptr<Matrix> from_vx_image(vx_image& image) {
      //create a vx rectangle object which corresponds to the full image
    vx_imagepatch_addressing_t imagepatch;
    vx_uint8 *data_ptr = NULL;
    vx_map_id map_id;

    vx_uint32 width = 0;
    vx_uint32 height = 0;
    vx_size num_planes = 0;
    vx_df_image format = 0;

    vxQueryImage(image, VX_IMAGE_WIDTH, &width, sizeof(width));
    vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(height));
    vxQueryImage(image, VX_IMAGE_PLANES, &num_planes, sizeof(num_planes));
    vxQueryImage(image, VX_IMAGE_FORMAT, &format, sizeof(format));
    vx_rectangle_t rectFullMatrix = {0, 0, width, height};



    vxMapImagePatch(image, &rectFullMatrix, 0, &map_id, &imagepatch, (void **)&data_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    int elem_size = 1;
    if (format == VX_DF_IMAGE_RGBX) elem_size = 4;
    if (format == VX_DF_IMAGE_U8) elem_size = 1;

    Dimension dim{.rows=height, .cols= width};
    std::unique_ptr<Matrix> image_out =  makeMatrix(dim, elem_size);
    memcpy(image_out->data, data_ptr, image_out->total());
    vxUnmapImagePatch(image, map_id);
    return image_out;
  }

  friend std::ostream& operator<<(std::ostream& os, const Matrix& dt){
    os<<"["<<dt.dim.rows<<","<<dt.dim.cols<<"]*"<<dt.elemSize<<std::endl;;
    int cnt = 0;
    for(int i =0; i < dt.dim.rows; i++) {
      for(int j=0;j < dt.dim.cols; j++) {
        for(int k=0; k < dt.elemSize; k++)
          os<<(int)dt.data[cnt++]<<" ";
      }
      os<<std::endl;
    }
    return os;
  }

  void reorder(const int tile_width, bool forward) {

    if(tile_width > dim.cols)
      throw std::runtime_error(Formatter() << "Can't reorder the image! "<< tile_width <<" > " << dim.cols);
    if(tile_width == dim.cols)
      return;

    int written_bytes = 0;
    unsigned char* reordered_data =  new unsigned char [this->total()];

    if (forward) { // reorder
      // Iterate over tiles
      for(int j =0 ;j < dim.cols; j+=tile_width) {
        for (int i = 0; i < dim.rows; i+=1) {
          // Copy a sub-row
          int offset = i*(dim.cols)+j;
          memcpy(reordered_data+written_bytes, data+offset*elemSize, tile_width*elemSize);
          written_bytes += tile_width*elemSize;
        }
      }
    } else {
      for (int i = 0; i < dim.rows*tile_width; i+=tile_width) {
        for(int j =0 ;j < dim.cols/tile_width; j+=1) {
         // Copy a sub-row
          int offset = j*(dim.rows)*tile_width+i;
          memcpy(reordered_data+written_bytes, data+offset*elemSize, tile_width*elemSize);
          written_bytes += tile_width*elemSize;
        }
      }
    }

    delete [] this->data;
    this->data = reordered_data;
  }

  void reorder_tiled(const int tile_dim, bool forward) {

    int written_bytes = 0;
    unsigned char* reordered_data =  new unsigned char [this->total()];

    if (forward) { // reorder
      // Iterate over tiles
      for (int i = 0; i < dim.rows; i+=tile_dim) {
        for(int j =0 ;j < dim.cols; j+=tile_dim) {
          int start_offset = i*(dim.cols)+j;
          // Copy a tile
          for (int k = 0; k < tile_dim; k+=1) {
            memcpy(reordered_data+written_bytes, data+(start_offset+k*dim.cols)*elemSize, tile_dim*elemSize);
            written_bytes += tile_dim*elemSize;
          }
        }
      }
    } else {
      int num_tiles_x = dim.cols / tile_dim;
      int num_tiles_y = dim.rows / tile_dim;


      for (int i = 0; i < num_tiles_y; i+=1) {
        for(int k=0; k < tile_dim; k++) { // Number of lines per tile
          // Do it for each tile
          for(int j =0 ;j < num_tiles_x; j+=1) {
            int start_offset = (i*(num_tiles_x)+j)*(tile_dim*tile_dim);
            // Copy a tile
            memcpy(reordered_data+written_bytes, data+(start_offset+k*tile_dim)*elemSize, tile_dim*elemSize);
            written_bytes += tile_dim*elemSize;
          }
        }
      }
    }

    delete [] this->data;
    this->data = reordered_data;
  }

  // Reorder multiple smaller tiles within a bigger tile
  // A B | C D | E F | G H
  // I J | K L | M N | O P
  // Forwards
  // A B | I J | C D | K L
  // E F | M N | G H | O P
  // Backwards
  // A B | C D | E F | G H
  // I J | K L | M N | O P


  // void forward_reorder_recursive(int tile_size, int min_size, unsigned char* start, unsigned char* dest) {
  //   // Stop condition
  //   if (tile_size < min_size)
  //     return;

  //   // Top-left
  //   forward_reorder_recursive(tile_size/2, min_size, start, dest);
  //   // Top-right
  //   forward_reorder_recursive(tile_size/2, min_size, start + , dest);
  //   // Bottom-left
  //   forward_reorder_recursive(tile_size/2, min_size, start, dest);
  //   // Bottom-right
  //   forward_reorder_recursive(tile_size/2, min_size, start, dest);


  // }

  void reorder_grouptiled(const int unit_tile_dim, int num_unit_tiles, bool forward) {

    int written_bytes = 0;
    int copy_size = unit_tile_dim*elemSize;
    unsigned char* reordered_data =  new unsigned char [this->total()];
    int supertile_rows = unit_tile_dim*num_unit_tiles;
    int supertile_cols = unit_tile_dim*num_unit_tiles;
    int unit_tile_rows = unit_tile_dim;
    int unit_tile_cols = unit_tile_dim;
    int supertile_size = supertile_cols*supertile_rows;
    int unit_tile_size = unit_tile_cols*unit_tile_rows;
    std::cout<<"st.rows:"<<supertile_rows<<", st.cols:"<<supertile_cols<<",elemsize:"<<elemSize<<std::endl;
    if (forward) { // reorder
      // Iterate over grouped tiles
      for (int i = 0; i < dim.rows; i+=supertile_rows) {
        for(int j =0; j < dim.cols; j+=supertile_cols) {
          // Process each unit tile from (i,j) position
          for (int k = 0; k < supertile_rows; k+=unit_tile_rows) {
            for (int l = 0; l < supertile_cols; l+=unit_tile_cols) {
              // Copy all the tile elements
              int start_offset = (i+k)*(dim.cols)+ j + l;
              for (int m = 0; m < unit_tile_dim; m+=1) {
                memcpy(reordered_data+written_bytes, data+(start_offset)*elemSize, unit_tile_dim*elemSize);
                written_bytes += unit_tile_dim*elemSize;
                start_offset += dim.cols;
              }
            }
          }
        }
      }
    } else {
      int num_tiles_x = supertile_cols / unit_tile_cols;
      int num_tiles_y = supertile_rows / unit_tile_rows;
      int num_supertiles_x = dim.cols / supertile_cols;
      int num_supertiles_y = dim.rows / supertile_rows;

      int num_unit_tiles_in_st_row = num_unit_tiles;
      int num_unit_tiles_in_st_col = num_unit_tiles;

      std::cout<<"num_supertiles_x:"<<num_supertiles_x<<",supertile_size:"<<supertile_size<<std::endl;
      // Advance over supertiles each time (iterates dim.rows/st.rows tiemes)
      for(int i = 0 ;i < dim.rows*dim.cols; i+=supertile_rows*dim.cols) {
        // For every line in supertile
        std::cout<<"i:"<<i<<std::endl;
        for (int m = 0; m < supertile_rows; m+=1) {
          // extract a supertile_row from each supertile in the row
          //for(int k=0; k < supertile_size*num_supertiles_x; k+=(dim.cols*unit_tile_rows)) {
          for(int j =0 ;j < (num_supertiles_x*supertile_size); j+=(supertile_size)) {
          // i  + j is the beggining of this supertile

          // Dump mth line from supertile at (i+j)

              // For every tile in a supertile col
              for(int l=0; l < 1; l+=num_unit_tiles_in_st_col) {
                //std::cout<<"i+j:"<<i+j<<std::endl;
                int start_offset = i + j +  m +  l*unit_tile_size;
                memcpy(reordered_data+written_bytes, data+(start_offset)*elemSize, copy_size);
                written_bytes += copy_size;
              }
            }
          }
        }
      }


    delete [] this->data;
    this->data = reordered_data;
    std::cout<<"Written bytes:"<<written_bytes<<std::endl;
  }

  // This function changes the actual number of elements
  void reorder_with_padding(const int tile_width, const int pad_size, bool forward) {

    int written_bytes = 0;
    assert(this->dim.cols % tile_width == 0); // TODO

    int num_tiles = this->dim.cols / tile_width;
    printf("dim_cols=%d, dim_rows = %d, num_tiles = %d\n", this->dim.cols, dim.rows, num_tiles);
    if (forward)
      alloc_size = this->dim.rows * this->elemSize * (tile_width + (num_tiles-1)*(tile_width+pad_size));
    else
      alloc_size = this->dim.total() * this->elemSize;

    unsigned char* reordered_data =  new unsigned char [alloc_size];

    int copy_size;

    if (forward) { // reorder
      // Iterate over tiles
      for(int j =0 ;j < dim.cols; j+=tile_width) {

        if (j + tile_width == dim.cols) // last tile
          copy_size = tile_width;
        else
          copy_size = tile_width + pad_size;

        for (int i = 0; i < dim.rows; i+=1) {
          // Copy a sub-row
          memcpy(reordered_data+written_bytes, data+i*(dim.cols)+j, copy_size);
          written_bytes += copy_size;
        }
      }
    } else {
      for (int i = 0; i < dim.rows; i+=1) {
        for(int j =0 ;j < num_tiles; j+=1) {
          //printf("i,j = %d,%d ", i, j);
          copy_size = (j < num_tiles - 1) ? (tile_width + pad_size): tile_width;
          // Copy a sub-row
          memcpy(reordered_data+written_bytes, data+j*(dim.rows)*(tile_width+pad_size)+i*copy_size, tile_width);
          written_bytes += tile_width;
        }
      }
    }

    delete [] this->data;
    this->data = reordered_data;
  }

  int total() const {return dim.total()*elemSize;}

};


static cv::Mat do_reorder(const cv::Mat& src, const int tile_width, bool forward) {

  if(tile_width > src.cols)
    throw std::runtime_error(Formatter() << "Can't reorder the image! "<< tile_width <<" >= " << src.cols);
  
  if(tile_width == src.cols)
    return src;

  int written_bytes = 0;
  cv::Mat dst(src.rows, src.cols, src.type()); 

  unsigned char* src_ptr = src.data;
  unsigned char* dst_ptr = dst.data;
  
  if (forward) { // reorder
    // Iterate over tiles
    for(int j =0 ;j < src.cols; j+=tile_width) {
      for (int i = 0; i < src.rows; i+=1) {
        // Copy a sub-row
        int offset = i*(src.cols)+j;
        memcpy(dst_ptr+written_bytes, src_ptr+offset*src.elemSize(), tile_width*src.elemSize());
        written_bytes += tile_width*src.elemSize();
      }
    }
  } else {
    for (int i = 0; i < src.rows*tile_width; i+=tile_width) {
      for(int j =0 ;j < src.cols/tile_width; j+=1) {
        // Copy a sub-row
        int offset = j*(src.rows)*tile_width+i;
        memcpy(dst_ptr+written_bytes, src_ptr+offset*src.elemSize(), tile_width*src.elemSize());
        written_bytes += tile_width*src.elemSize();
      }
    }
  }
  return dst;
}