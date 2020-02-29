#pragma once
#include <string>

class Params {
public:
  static inline bool verbose() { return _verbose;}
  static bool test_data() { return _test_data;}
  static std::string get_binary_file() {return binary_file;}
  static std::string get_app_path() {return app_path;}
  static std::string get_input_file() {return input_file;}
  static int get_tile_cols() {return tile_cols;}
  static int get_tile_size() {return tile_cols*tile_cols;} //TODO rectangle tiles
  static int seq_size() {return _seq_size;}
  static void set(const std::string& binary, 
                  const std::string& app, 
                  const std::string& input, int tiles, int seq_size, bool verbose){
    binary_file = binary;
    app_path = app;
    input_file = input;
    tile_cols = tiles;
    _verbose = verbose;
    _seq_size = seq_size;
    _test_data = input.empty() ? true: false;
  }
private:
  static std::string binary_file;
  static std::string app_path;
  static std::string input_file;
  static int tile_cols;
  static int _seq_size;
  static bool _verbose;
  static bool _test_data;
};
