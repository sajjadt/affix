#include "opencl_manager.h"
#include <cstring>
#include <iostream>
#include "common.h"
#include "params.h"
#include <stdarg.h>
#include <stdio.h> 
#include <unistd.h> // readlink, chdir

bool OpenCLManager::FileExists(const char *file_name) {
  return access(file_name, R_OK) != -1;
}

// Loads a file in binary form.
char* OpenCLManager::LoadBinaryFile(const char *file_name, size_t *size) {
  // Open the File
  FILE* fp = fopen(file_name, "rb");
  if(fp == NULL) {
    return NULL;
  }

  // Get the size of the file
  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);
  std::cout<<*size<<std::endl;
  // Allocate space for the binary
  char *binary = new char[*size];

  // Go back to the file start
  rewind(fp);
  // Read the file into the binary
  int read = fread(binary, 1, *size,  fp);
  if( read == 0) {
    delete[] binary;
    fclose(fp);
    return NULL;
  }
  return binary;
}


cl::Program OpenCLManager::CreateProgramFromBinary(const cl::Context& context, const std::string& binary_file_name, const std::vector<cl::Device>& devices) {
  // Early exit for potentially the most common way to fail: AOCX does not exist.
  if(!FileExists(binary_file_name.c_str())) {
    std::cout<<"AOCX file does not exist"<<std::endl;
  }
  cl::Program program;
  uint num_devices = devices.size();

  // Load the binary.
  size_t binary_size = 0;
  char * binary_data = LoadBinaryFile(binary_file_name.c_str(), &binary_size);
  
  if (binary_size <= 0) {
    throw std::runtime_error("Error loading file" );
  } else {
    std::vector<unsigned char> binary(binary_data, binary_data+binary_size);
 
    cl_int status;

    std::vector<std::vector<unsigned char>> binaries;
    
    binaries.push_back(binary);
    std::cout<<binaries.size() <<", "<< binaries[0].size() << ", ";
    program = cl::Program(context, devices, binaries, NULL, NULL);
  }
  return program;
}

extern void dump_error(const char *str, cl_int status);
extern void ocl_notify(
    const char *errinfo,
    const void *private_info,
    size_t cb,
    void *user_data);

void pfn_notify(cl_program, void *user_data) {
  std::cout<<"Finished programming"<<std::endl;
}

void OpenCLManager::Init(const std::string& binary_file){

  cl_uint num_platforms;
  cl_uint num_devices;
  // Get the OpenCL platform.
  
  cl::vector<cl::Platform> platforms;
  cl::vector<cl::Device> devices;  

  cl::Platform::get(&platforms);
  std::cout<<"Found " << platforms.size()<< " devices..." << std::endl;
  cl::Platform plat;
  
  int platform_id = 0;
  int device_id = 0;
  cl_int status;

  // Search for FPGA OpenCL device
  for(cl::vector<cl::Platform>::iterator it = platforms.begin(); it != platforms.end(); ++it){

    if(it->getInfo<CL_PLATFORM_NAME>() == "Intel(R) FPGA SDK for OpenCL(TM)") {
      platform =  *it;
       
      std::cout << "Platform ID: " << platform_id++ << std::endl;  
      std::cout << "Platform Name: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;  
      std::cout << "Platform Vendor: " << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;  

      platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);  

      for(cl::vector<cl::Device>::iterator it2 = devices.begin(); it2 != devices.end(); ++it2){
        device = *it2;
      }
    }
  }

  // create a context
  context = cl::Context(devices, NULL, &ocl_notify, NULL, &status);
  if(status != CL_SUCCESS) {
    printf("Failed clCreateContext.", status);
    Cleanup();
   return;
  }

  for(int i=0; i< OpenCLManager::MAX_QUEUES; i++) {
    queues.push_back(cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &status));
  }

  // create the program
  printf("Using AOCX: %s\n", binary_file.c_str());
  program = CreateProgramFromBinary(context, binary_file.c_str(), devices);
  
}

void OpenCLManager::EnqueueCut(GraphCut cut){

  cl_int status;

  for (int i=0; i < cut.kernels.size(); i++){
    if (Params::Verbose())
      std::cout<<"Adding kernel " << cut.kernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>() << " to Q " << i << std::endl;
    status = queues[i].enqueueTask(cut.kernels[i]);
    if (status != CL_SUCCESS) {
      throw std::runtime_error(Formatter() << "failed to launch kernel (index): "<< i <<", (code): " << status);
    }
  }
}

void OpenCLManager::GetDeviceInfo() {
  std::cout << "\t\tDevice Name: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;  
  std::cout << "\t\tDevice Type: " << device.getInfo<CL_DEVICE_TYPE>();
  std::cout << " (GPU: " << CL_DEVICE_TYPE_GPU << ", ACCELERATOR:, "<< CL_DEVICE_TYPE_ACCELERATOR << ", CPU: " << CL_DEVICE_TYPE_CPU << ")" << std::endl;  
  std::cout << "\t\tDevice Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
  std::cout << "\t\tDevice Max Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
  std::cout << "\t\tDevice Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
  std::cout << "\t\tDevice Max Clock Frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
  std::cout << "\t\tDevice Max Allocateable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
  std::cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
  std::cout << "\t\tDevice Available: " << device.getInfo< CL_DEVICE_AVAILABLE>() << std::endl;     
}

void OpenCLManager::ConfigureFPGA(int no) {
  // build the program
}

void OpenCLManager::WaitUntilFinished(int num_queues) {
  cl_int status;
  for (auto it = queues.begin(); it != queues.end(); it++){
    //if (Params::verbose())
    //  std::cout<<"Finishing Q " << std::endl;
    status = it->finish();
  }
}

// free the resources allocated during initialization
void OpenCLManager::Cleanup() {
/*
  for (int i=0;i < MAX_KERNELS; i++) {
    if (kernels[i])
       clReleaseKernel(kernels[i]);
  }
  
  if(program)
    clReleaseProgram(OpenCLManager::program);

  for (int i=0;i < MAX_QUEUES; i++) {
    if (queues[i])
      clReleaseCommandQueue(OpenCLManager::queues[i]);
  }

  if(context)
    clReleaseContext(OpenCLManager::context);
    */
}

cl::Buffer OpenCLManager::CreateFpgaBuffer(cl_mem_flags flags, size_t size) {
  cl_int status;
  return cl::Buffer(context, flags, size, NULL, &status);
}

cl::Buffer OpenCLManager::CreateFpgaBuffer(
  cl_mem_flags flags,
  size_t size,
  void* host_ptr) {
  cl_int status;
  return cl::Buffer(context, flags, size, host_ptr, &status);
}