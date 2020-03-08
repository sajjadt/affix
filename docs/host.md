A sample host program provides utility function for executing the appliaction on CPU and FPGA. Functionalities include:

1. Input/output on the host using OpenCV
2. Orchestratig execution of algorithms on CPU and FPGA 
4. Simultanious I/O to FPGA using PCIe connection 

Host program is using Intel OpenCL implementation to communicate with and control the FPGA instance.

Individual applications orchestrate the host by implementing the ```Application``` class interface. This class will be compiled to a dynamically linked library that will be loaded by the host at run-time. Vision algorithms are omimplemented as DAGs. Such DAGs will translate to one or multiple graph cuts, where each cut is implemented as a pipeline on FPGA. The appllication implments pre-processing and post-processing steps for each graph cut as well as provides the arguments for OpenCL functions. 

```cpp
class Application {
 public:

  virtual void PreProcessCut(int, int) = 0;
  virtual void PostProcessCut(int, int) = 0;
  virtual GraphCut GetExecutionCut(const cl::Program&, const cl::Pipe&, const cl::Pipe&, int, int) = 0;

  // ...
 protected:
  OpenCLManager * cl_manager; 
  // ...
};
```

### Build Sample Applications
```bash
cd host/apps
mkdir build && cd build
cmake ..
make [target]
```