# AFFIX
FPGA acceleration framework for computer vision aglorithms described in graphical form according to OpenVX spec. This repo is work in progress, better documentation and more features coming soon.

### FPGA Support
FPGA backend utilizes OpenCL to target Intel FPGAs. Tests been performed on Arria10 GX dev kit.

### Components
1. fpga: OpenCL library for describing computational pipelines
2. host: host program with OpenCL runtime and I/O management

### Dependencies
1. CMake
2. OpenVX runtime
3. OpenCV library
4. Google Test
5. Intel OpenCL SDK (tested against version 18.1 and 19.1)

### License
University of California BSD 3-Clause
