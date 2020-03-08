#pragma once
#include <string>

class Params {
public:
  static inline bool Verbose() { return _verbose;}
  static bool TestData() { return _test_data;}
  static std::string GetBinaryFile() {return binary_file;}
  static std::string GetApplicationPath() {return application_path;}
  static std::string GetInputFile() {return input_file;}
  static int GetTileCols() {return tile_cols;}
  static int GetTileSize() {return tile_cols*tile_cols;} //TODO rectangle tiles
  static void Set(const std::string& binary, 
                  const std::string& app, 
                  const std::string& input, int tiles, bool verbose){
    binary_file = binary;
    application_path = app;
    input_file = input;
    tile_cols = tiles;
    _verbose = verbose;
    _test_data = input.empty() ? true: false;
  }
private:
  static std::string binary_file;
  static std::string application_path;
  static std::string input_file;
  static int tile_cols;
  static bool _verbose;
  static bool _test_data;
};
