#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>

#include "common.h"
#include "application_manager.h"
#include "opencl_manager.h"
#include "opencl_io_manager.h"
#include "cut.h"
#include "application.h"
#include "params.h"

#include "args/argx.hxx"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#define KB (1024)
#define MB (1024*KB)
#define GB (1024*MB)
#define packet_sz 32

// Set to false to temporarily disable printing of error notification callbacks
bool g_enable_notifications = false;
void ocl_notify(
    const char *errinfo,
    const void *private_info,
    size_t cb,
    void *user_data) {
  if(g_enable_notifications) {
    printf("  OpenCL Notification Callback:");
    printf(" %s\n", errinfo);
  }
}

std::string Params::binary_file;
std::string Params::application_path;
std::string Params::input_file;
int Params::tile_cols;
bool Params::_verbose;
bool Params::_test_data;

void dump_error(const char *str, cl_int status) {
  printf("%s\n", str);
  printf("Error code: %d\n", status);
}

int main(int argc, char *argv[]) {
  
  const int num_workers = 1;
  const int num_threads = 1;
  
  args::ArgumentParser parser("Host progam for FPGA computer vision algorithm acceleration using Intel OpenCL SDK.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<std::string> binary_file(parser, "binary", "FPGA binary file (path to .aocx file)", {'b'});
  args::ValueFlag<std::string> application_path(parser, "app", "Application to run (path to .so file): \n ", {'a'});
  args::ValueFlag<std::string> input_file(parser, "input", "Input file to stream", {'i'});
  args::ValueFlag<int> tile_cols(parser, "tile_cols", "Input tile dimension", {'t'});

  args::Flag verbose(parser, "verbose", "Verbose mode", {'v'});
  args::Flag emulator(parser, "emulator", "Run in emulator", {'e'});

  cv::VideoCapture cap;

  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help) {
    std::cout << parser;
    return 0;
  } catch (args::ParseError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  } catch (args::ValidationError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  try {
    if (binary_file && application_path) {;
      Params::Set(args::get(binary_file),
                  args::get(application_path),
                  input_file? args::get(input_file): std::string(),
                  tile_cols? args::get(tile_cols) : 128,
                  verbose);
    } else {
      std::cout << parser;
      throw std::runtime_error("Bad command line input");
    }

  } catch (const std::exception& e) {
    std::cout<<e.what()<<std::endl;
    return 0;
  }

  if (emulator) {
    std::cout<<"Emulator device is selected." << std::endl;
    putenv("CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1");
    putenv("CL_CONTEXT_EMULATOR_DEVICE_ALTERA=1");
  }

  std::cout<<"Initializing OpenCL" << std::endl;
  OpenCLManager cl_manager;
  OpenCLIOManager clio_manager;
  cl_manager.Init(Params::GetBinaryFile());
  if (Params::Verbose())
    cl_manager.GetDeviceInfo();
  clio_manager.Init(cl_manager.context);

  // Program the board with a binary that supports the entire program
  // No PR at this point
  if (Params::Verbose())
    std::cout<<"Programming device"<<std::endl;
  cl_manager.ConfigureFPGA(0);
  
  // App manager
  ApplicationManager application_manager(Params::GetApplicationPath(), 
                                num_workers, 
                                num_threads,
                                &cl_manager);

  // I/O device
  cap.open(args::get(input_file));
  assert (cap.isOpened() == true);
  int fps = cap.get(CV_CAP_PROP_FPS);
  int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  int no_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
  int format ;
  std::cout<<"loaded media:" << args::get(input_file) <<", frames:" << no_frames << ", FPS:" << fps << ", Dim:" << frame_width << "," << frame_height << std::endl;
 
  Application* app = application_manager.GetAppliaction();
  cv::Mat mat, dmat;
  
  GraphCut cut[10];  //TODO: fix
   
  for(int seq =0; seq  < no_frames;  seq++) {
    cap >> mat;
    // Mat is a BGR image
    assert(mat.channels() == 3);

    // if the app requires Grayscale images
    if (app->GetColor() == Color::GRAYSCALE)
      cv::cvtColor(mat, dmat, cv::COLOR_BGR2GRAY);
    else if (app->GetColor() == Color::RGBX)
      cv::cvtColor(mat, dmat, cv::COLOR_BGR2RGBA);
    else
      throw std::runtime_error("Unexpected color format");

    app->SetCutDataInfo(dmat);
      
    // Process cuts
    for(int i =0; i < app->GetCutsNumber(); i++) {
      // Reset pipes for later cuts
      if (seq == 0){
        cut[i] = app->GetExecutionCut(cl_manager.program, 
          clio_manager.read_pipe, 
          clio_manager.write_pipe, 
          0, i);
      }

      // Allocate/prepare input/output buffers
      // Can come from input file or previous cut execution
      CutData cut_in = app->GetInputData(i);
      CutData cut_out = app->GetOutputData(i);

      // Reording using CPU
      app->PreProcessCut(seq, i);

      int in_size = cut_in.total_bytes();
      int out_size = cut_out.total_bytes();
 
      cl_manager.EnqueueCut(cut[i]);
      
      printf ("Beginning to push and pull data\n");
      clio_manager.OverlappedIO(cut_in.mat.data, cut_out.mat.data, in_size, out_size);

      app->PostProcessCut(seq, i);

      std::cout<<"Waiting for all queues to finish"<<std::endl;
      cl_manager.WaitUntilFinished(cut[i].kernels.size());
    }
  }

  //clio_manager.release();  
  cl_manager.Cleanup();

  return 0;
}
