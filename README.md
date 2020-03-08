# AFFIX
FPGA acceleration framework for computer vision aglorithms described in graphical form followin the OpenVX spec. 

## Components
1. [fpga](fpga)    : OpenCL library for describing computational vision pipelines on FPGAs
2. [host](host)    : A sample host program that uses OpenCL runtime to offload computation to FPGA pipelines

## Dependencies
1. CMake
3. OpenCV library
4. Google Test
5. Intel OpenCL SDK (tested against version 19.1)

## Usage
1. Build the host application. Make sure that aocl is in the path.
```bash
cd host
mkdir build && cd build
cmake ..
make
```

2. Develop and build a pipeline. Each pipeline has two components that need to be build separately.
   1. Implement the FPGA pipeline in OpenCL and build with aocl to a FPGA binary file. Refer to [docs](docs/fpga.md) for more information.
   2. Describe the FPGA pipeline and implement pre/post processing logic for the host program in C++ and compile it to a shared library. Refer to [docs](docs/host.md) for more information.

3. Run the host program with proper input
   * input/output path
   * Path to generated FPGA binary (.aocx file)
   * Path to generated host component (.so file)
   * binary flag to specify hardware/emulator setup

``` bash 
./host -b [FPGA .aocx file] -a [Path to application .so file] -i [Path to input (image/video)] [Other flags]
```

The following picture shows this process.

![components.png](docs/images/comp.png)

## FPGA Board Support
This project is using OpenCL to configure and control the FPGA acceleration. We have used Arria10 GX dev kit (a10gx_hostch board variant) during development. We have tried to use the vendor specific extension as little as possible to make it easier to port this work to other development kits or different FPGA vendors.

### License
University of California BSD 3-Clause
