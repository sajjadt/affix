#include "image.h"
#include <gtest/gtest.h>
#include <cstdlib>

// OpenVX
#include <VX/vx.h>
#include <VX/vxu.h>

// OpenCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

TEST(ImageToVXImage, Properties) {

  const int width = 600;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::makeMatrix(Dimension{.rows = height, .cols = width}, 1);
  vx_context context = vxCreateContext();
  vx_image image = char_img->to_vx_image(context);

  vx_uint32 output_width = 0;
  vx_uint32 output_height = 0;
  vx_size num_planes = 0;
  vx_size size = 0;
  vxQueryImage(image, VX_IMAGE_WIDTH, &output_width, sizeof(output_width));
  vxQueryImage(image, VX_IMAGE_HEIGHT, &output_height, sizeof(output_height));
  vxQueryImage(image, VX_IMAGE_PLANES, &num_planes, sizeof(num_planes));
  vxQueryImage(image, VX_IMAGE_SIZE, &size, sizeof(size));
  
  EXPECT_EQ (width, output_width);
  EXPECT_EQ (height, output_height);
  EXPECT_EQ (num_planes, 1);
}

TEST(ImageToVXImage, NoDataCopy) {

  const int width = 600;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::makeMatrix(Dimension{.rows = height, .cols = width}, 3);
  vx_context context = vxCreateContext();

  vx_imagepatch_addressing_t addr;
  addr.dim_x = width;
  addr.dim_y = height;
  addr.stride_x = 3*sizeof( vx_uint8 );
  addr.stride_y = 3*width; // cols*elemSize
  void *ptrs [] = { 
    (void*) char_img->data
  };
  vx_image vx_rgb = vxCreateImageFromHandle( context, VX_DF_IMAGE_RGB, &addr, ptrs, VX_MEMORY_TYPE_HOST );
  
  void *base = NULL;// NULL means ‘map’
  vx_map_id map_id;
  vx_rectangle_t rect = { 0u, 0u, width, height};
  vxMapImagePatch(vx_rgb, &rect, 0, &map_id, &addr, &base, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0 );
  for( int i =0; i < char_img->total(); i++) {
    ASSERT_EQ( char_img->data[i],  ((unsigned char*)base)[i] );
  }
  vxUnmapImagePatch(vx_rgb, map_id);

  for( int i =0; i < char_img->total(); i++) {
    char_img->data[i] = char_img->data[i] + 1;
  }

  vxMapImagePatch(vx_rgb, &rect, 0, &map_id, &addr, &base, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0 );
  for( int i =0; i < char_img->total(); i++) {
    ASSERT_EQ( char_img->data[i],  ((unsigned char*)base)[i] );
  }
  vxUnmapImagePatch(vx_rgb, map_id);

}


TEST(VXU, Not) {

  const int width = 32;
  const int height= 32;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 1);
  vx_context context = vxCreateContext();
  vx_image image = char_img->to_vx_image(context);

  vx_image out_image = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
  vx_status status = vxuNot(context, image, out_image);
  EXPECT_EQ (status, VX_SUCCESS);

  // TODO: why does this fail?
  // vx_rectangle_t rectFullImage = {0, 0, width, height};
  // vx_imagepatch_addressing_t imagepatch;
  // vx_uint8 *data_ptr = NULL;
  // vx_map_id map_id;
  // vxMapImagePatch(out_image, &rectFullImage, 0, &map_id, &imagepatch, (void **)&data_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
  // for(int i =0 ;i < char_img->dim.total(); i++) {
  //   EXPECT_EQ (0,  (int)(data_ptr[i] & char_img->data[i]));
  // }
}

TEST(VXU, Not_ImageFromHandle) {

  const int width = 32;
  const int height= 32;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 1);
  vx_context context = vxCreateContext();
  
  vx_imagepatch_addressing_t addr;
  addr.dim_x = width;
  addr.dim_y = height;
  addr.stride_x = 1*sizeof( vx_uint8 );
  addr.stride_y = 1*width; // cols*elemSize
  void *ptrs [] = { 
    (void*) char_img->data
  };
  vx_image in_image = vxCreateImageFromHandle( context, VX_DF_IMAGE_U8, &addr, ptrs, VX_MEMORY_TYPE_HOST );
  vx_image out_image = vxCreateImageFromHandle( context, VX_DF_IMAGE_U8, &addr, ptrs, VX_MEMORY_TYPE_HOST );
  
  vx_status status = vxuNot(context, in_image, out_image);
  EXPECT_EQ (status, VX_SUCCESS);

}

TEST(OpenCV, Mat) {
  int cols = 640;
  int rows = 480;
  cv::Mat img = cv::Mat::zeros(rows, cols, CV_8UC1);

  EXPECT_EQ (img.cols, cols);
  EXPECT_EQ (img.rows, rows);
}


TEST(ImageToVXImage, DataU8) {
  const int width = 600;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 1);

  vx_context context = vxCreateContext();
  vx_image image = char_img->to_vx_image(context);
  
  vx_rectangle_t rectFullImage = {0, 0, width, height};
  vx_imagepatch_addressing_t imagepatch;

  vx_uint8 *data_ptr = NULL;
  vx_map_id map_id;

  vxMapImagePatch(image, &rectFullImage, 0, &map_id, &imagepatch, (void **)&data_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
  for(int i =0 ;i < char_img->dim.total(); i++) {
    EXPECT_EQ (data_ptr[i], char_img->data[i]);
  } 
  vxReleaseContext(&context);
  delete[] char_img->data;
}

TEST(ImageToVXImage, DataRGBX) {
  const int width = 600;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 4);
  
  vx_context context = vxCreateContext();
  vx_image image = char_img->to_vx_image(context);
  
  vx_rectangle_t rectFullImage = {0, 0, width, height};
  vx_imagepatch_addressing_t imagepatch;

  vx_uint32 *data_ptr = NULL;
  vx_map_id map_id;

  vxMapImagePatch(image, &rectFullImage, 0, &map_id, &imagepatch, (void **)&data_ptr, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
  for(int i =0 ;i < char_img->dim.total(); i++) {
    EXPECT_EQ (data_ptr[i], ((int*)char_img->data)[i]);
  } 
  vxReleaseContext(&context);
}

TEST(ImageToVXImageToImage, Meta) {
  const int width = 600;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::makeMatrix(Dimension{.rows = height, .cols = width}, 4);
  
  vx_context context = vxCreateContext();
  vx_image image = char_img->to_vx_image(context);
  std::unique_ptr<Matrix>  char_img_2 = Matrix::from_vx_image(image);
  
  EXPECT_EQ (char_img->dim.cols, char_img_2->dim.cols);
  EXPECT_EQ (char_img->dim.rows, char_img_2->dim.rows);
  EXPECT_EQ (char_img->total(), char_img_2->total());
  vxReleaseContext(&context);
  delete[] char_img->data;
  delete[] char_img_2->data;
}

TEST(ImageToVXImageToImage, DataU32) {
  const int width = 600;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 4);
  
  vx_context context = vxCreateContext();
  vx_image image = char_img->to_vx_image(context);
  std::unique_ptr<Matrix>  char_img_2 = Matrix::from_vx_image(image);
  
  for(int i =0 ;i < char_img->dim.total(); i++) {
    EXPECT_EQ (((int*)char_img->data)[i], ((int*)char_img_2->data)[i]);
  } 
  vxReleaseContext(&context);
  delete[] char_img->data;
  delete[] char_img_2->data;
}

TEST(ImageToVXImageToImage, DataU8) {
  const int width = 512;
  const int height= 256;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width},1);
  
  vx_context context = vxCreateContext();
  vx_image image = char_img->to_vx_image(context);
  std::unique_ptr<Matrix>  char_img_2 = Matrix::from_vx_image(image);
  
  for(int i =0 ;i < char_img->dim.total(); i++) {
    EXPECT_EQ (char_img->data[i], char_img_2->data[i]);
  } 
  vxReleaseContext(&context);
  delete[] char_img->data;
  delete[] char_img_2->data;
}

TEST(Reorder, grayscale) {

  const int tile_width = 128;
  const int width = 256;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 1);
  std::unique_ptr<Matrix> clone = char_img->clone();

  clone->reorder(128, true);
  clone->reorder(128, false);

  for(int i =0 ;i < char_img->dim.total(); i++) {
    EXPECT_EQ (char_img->data[i], clone->data[i]);
  }
}

TEST(Reorder_tiled, grayscale) {

  const int tile_width = 4;
  const int width = 16;
  const int height= 32;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 4);
  std::unique_ptr<Matrix> clone = char_img->clone();

  clone->reorder_tiled(tile_width, true);  
  clone->reorder_tiled(tile_width, false);

  for(int i =0 ;i < char_img->dim.total(); i++) {
    EXPECT_EQ (char_img->data[i], clone->data[i]);
  }
}

TEST(Reorder_grouptiled, grayscale) {

  const int tile_width = 2;
  const int tile_height = 2;
  const int tiles_in_supertile = 2;
  const int num_supertiles_per_dim = 2;
  const int width = tile_width*tiles_in_supertile*num_supertiles_per_dim;
  const int height= tile_height*tiles_in_supertile*num_supertiles_per_dim;
  std::unique_ptr<Matrix> char_img = Matrix::niceMatrix(Dimension{.rows = height, .cols = width}, 1);
  std::unique_ptr<Matrix> clone = char_img->clone();
  std::cout<<*char_img<<std::endl;
  // 4 Tiles per big tile
  clone->reorder_grouptiled(tile_width, tiles_in_supertile, true);
  std::cout<<*clone<<std::endl;
  clone->reorder_grouptiled(tile_width, tiles_in_supertile, false);
  std::cout<<*clone<<std::endl;
  
  for(int i =0 ;i < char_img->dim.total(); i++) {
  //  EXPECT_EQ (char_img->data[i], clone->data[i]);
  }
}

TEST(Reorder, rgbx) {

  const int tile_width = 128;
  const int width = 256;
  const int height= 512;
  Dimension dim{.rows = height, .cols = width};
  std::unique_ptr<Matrix> color_img = Matrix::randomMatrix(dim, 4);
  
  std::unique_ptr<Matrix> clone = color_img->clone();

  clone->reorder(128, true);
  clone->reorder(128, false);

  for(int i =0 ;i < color_img->dim.total(); i++) {
    EXPECT_EQ (color_img->int_data(i), clone->int_data(i));
  }
}

TEST(Reorder, With_padding_multi_tiles_tiny) {
  const int tile_width = 8;
  const int width = 16;
  const int height= 4;
  const int pad_size = 3;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 1);
  
  std::unique_ptr<Matrix> clone = char_img->clone();
  clone->reorder_with_padding(tile_width, pad_size, true);
  
  int num_tiles = width/tile_width;
  EXPECT_EQ (clone->alloc_size, height*( (num_tiles-1)*(tile_width+pad_size) + tile_width));
  clone->reorder_with_padding(tile_width, pad_size, false);
  
  for(int i =0 ;i < char_img->dim.total(); i+=(char_img->dim.total()/20)) {
    EXPECT_EQ (char_img->data[i], clone->data[i]);
 }
}

TEST(Reorder, With_padding_one_tile) {
  const int tile_width = 128;
  const int width = 128;
  const int height= 512;

  std::unique_ptr<Matrix> char_img = Matrix::makeMatrix(Dimension{.rows = height, .cols = width}, 1);
  std::unique_ptr<Matrix> clone = char_img->clone();
  int num_tiles = width/tile_width;

  // 3x3 kernel to 13x13
  for(int pad_size = 1; pad_size < 7; pad_size++) {
    clone->reorder_with_padding(tile_width, pad_size, true);
    EXPECT_EQ (clone->alloc_size, height*( (num_tiles-1)*(tile_width+pad_size) + tile_width));
    clone->reorder_with_padding(tile_width, pad_size, false);
    EXPECT_EQ (clone->alloc_size, height*width);
    for(int i =0 ;i < char_img->dim.total(); i+=(char_img->dim.total()/20)) {
      EXPECT_EQ (char_img->data[i], clone->data[i]);
    }
  }
}

TEST(Reorder, With_padding_multi_tile) {
  const int tile_width = 128;
  const int width = 512;
  const int height= 512;
  const int pad_size = 3;
  std::unique_ptr<Matrix> char_img = Matrix::makeMatrix(Dimension{.rows = height, .cols = width}, 1);
  std::unique_ptr<Matrix> clone = char_img->clone();
  int num_tiles = width/tile_width;

  // 3x3 kernel to 13x13
  for(int pad_size = 1; pad_size < 7; pad_size++) {
    clone->reorder_with_padding(tile_width, pad_size, true);
    EXPECT_EQ (clone->alloc_size, height*( (num_tiles-1)*(tile_width+pad_size) + tile_width));
    clone->reorder_with_padding(tile_width, pad_size, false);
    EXPECT_EQ (clone->alloc_size, height*width);
    for(int i =0 ;i < char_img->dim.total(); i+=(char_img->dim.total()/20)) {
      EXPECT_EQ (char_img->data[i], clone->data[i]);
    }
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
