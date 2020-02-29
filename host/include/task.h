#pragma once

class CPUTask{
  public:
  	int operator() (void* data_in, void* data_out, int len);
  	int tile_width;
  	int tile_height;
};


