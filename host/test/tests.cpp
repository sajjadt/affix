#include "image.h"
#include <gtest/gtest.h>
#include <cstdlib>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

TEST(Reorder, grayscale) {
  const int tile_width = 128;
  const int width = 256;
  const int height= 512;
  std::unique_ptr<Matrix> char_img = Matrix::randomMatrix(Dimension{.rows = height, .cols = width}, 1);
  std::unique_ptr<Matrix> clone = char_img->clone();

  clone->reorder(128, true);
  clone->reorder(128, false);

  for(int i =0 ;i < char_img->dim.Total(); i++) {
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

  for(int i =0 ;i < char_img->dim.Total(); i++) {
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
  
  for(int i =0 ;i < char_img->dim.Total(); i++) {
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

  for(int i =0 ;i < color_img->dim.Total(); i++) {
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
  
  for(int i =0 ;i < char_img->dim.Total(); i+=(char_img->dim.Total()/20)) {
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
    for(int i =0 ;i < char_img->dim.Total(); i+=(char_img->dim.Total()/20)) {
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
    for(int i =0 ;i < char_img->dim.Total(); i+=(char_img->dim.Total()/20)) {
      EXPECT_EQ (char_img->data[i], clone->data[i]);
    }
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
