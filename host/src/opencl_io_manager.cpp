#include "opencl_io_manager.h"
#include "opencl_manager.h"

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
   
#include "CL/cl2.hpp"
#include "CL/opencl.h"

#include <iostream>
//extern void dump_error(const char *str, cl_int status);

// Functions accessed through ICD
void * (*map_pipe_foo) (cl_mem, cl_map_flags, size_t, size_t *, cl_int *);
cl_int (*unmap_pipe_foo) (cl_mem, void *, size_t, size_t *);

void OpenCLIOManager::Init(const cl::Context& context) {
  cl_int status;
  
  map_pipe_foo = (void * (*) (cl_mem, cl_map_flags, size_t, size_t *, cl_int *)) clGetExtensionFunctionAddress("clMapHostPipeIntelFPGA");
  unmap_pipe_foo = (cl_int (*) (cl_mem, void *, size_t, size_t *)) clGetExtensionFunctionAddress("clUnmapHostPipeIntelFPGA");

  cl_mem read_mem = clCreatePipe(context(), CL_MEM_HOST_READ_ONLY, packet_sz, (10*MB)/packet_sz, NULL, &status);
  std::cout<<"Read pipe status: "<<status << std::endl; 
  cl_mem write_mem = clCreatePipe(context(), CL_MEM_HOST_WRITE_ONLY, packet_sz, (10*MB)/packet_sz, NULL, &status);
  std::cout<<"Write pipe status: "<<status << std::endl; 

  read_pipe = cl::Pipe(read_mem);
  write_pipe = cl::Pipe(write_mem);

  std::cout<< "Created read/write pipes" << std::endl;
}

void OpenCLIOManager::Release() {
  read_pipe.~Pipe();
  write_pipe.~Pipe();
}


void OpenCLIOManager::OverlappedIO(unsigned char* inbuffer, unsigned char* outbuffer, cl_ulong insize, cl_ulong outsize) {
  size_t total_mapped_size_wr = 0;
  size_t total_mapped_size_rd = 0;
  size_t unmapped_size;
  size_t mapped_size;
  cl_int errcode;
  cl_ulong * buffer;
  int error = 0;
  size_t prev;
  
  int read_progress = 0;
  while (total_mapped_size_wr < insize || total_mapped_size_rd < outsize ) {
    if ( total_mapped_size_wr != insize ) {
      buffer = (cl_ulong *)(*map_pipe_foo) (write_pipe(), 0, insize - total_mapped_size_wr, &mapped_size, &errcode);
      if (errcode && errcode != CL_OUT_OF_RESOURCES) {
        printf("Write MAP failed with error code: %d\n", errcode);
        return;
      }
      if (errcode != CL_OUT_OF_RESOURCES) {
        //printf("Mapped %d bytes to write into pipe\n", mapped_size);
        memcpy ( buffer, (inbuffer + (total_mapped_size_wr)), mapped_size );
        total_mapped_size_wr += mapped_size;

        unmapped_size = 0;
        while (unmapped_size != mapped_size) {
          errcode = (*unmap_pipe_foo) (write_pipe(), buffer, mapped_size-unmapped_size, &unmapped_size);
          if (errcode && errcode != CL_OUT_OF_RESOURCES) {
            printf("Write UNMAP failed with error code: %d\n", errcode);
            return ;
          }
        }
      }
    }
    buffer = (cl_ulong *)(*map_pipe_foo) (read_pipe(), 0, outsize - total_mapped_size_rd, &mapped_size, &errcode);
    if (errcode && errcode != CL_OUT_OF_RESOURCES) {
      printf("Read MAP failed with error code: %d\n", errcode);
      return ;
    }

    if (errcode != CL_OUT_OF_RESOURCES) {
      memcpy (outbuffer+ total_mapped_size_rd, buffer, mapped_size );
      
      total_mapped_size_rd += mapped_size;
      read_progress += mapped_size;

      if(read_progress > outsize/10) {
        read_progress = 0;
        printf("total_mapped_size_rd = %d and mapped_size = %d\n", total_mapped_size_rd, mapped_size);
      }

      unmapped_size = 0;
      while (unmapped_size != mapped_size) {
        errcode = (*unmap_pipe_foo) (read_pipe(), buffer, mapped_size-unmapped_size, &unmapped_size);
        if (errcode && errcode != CL_OUT_OF_RESOURCES) {
          printf("Read UNMAP failed with error code: %d\n", errcode);
          return ;
        }
      }
    }
  }
}


