#pragma once
#include <string>
#include <vector>

// Memory allocation 
// Number of partitions
// List of kernel information (name, num args, their type and value)
// Parition mapping {CPU, FPGA}
// Parition IO {size, type}

class ApplicationInfo {
public:
  std::string name;
  int cuts;
  std::vector<int> cut_input_size;
  std::vector<int> cut_output_size;
};


