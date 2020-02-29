#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>


#include "common.h"
#include "app_manager.h"
#include "cl_manager.h"
#include "clio_manager.h"
#include "cut.h"
#include "perf.h"
#include "application.h"
#include "params.h"

#include "args/argx.hxx"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#define KB (1024)
#define MB (1024*KB)
#define GB (1024*MB)
#define packet_sz 32

// Functions accessed through ICD

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
std::string Params::app_path;
std::string Params::input_file;
int Params::tile_cols;
bool Params::_verbose;
bool Params::_test_data;
int Params::_seq_size;

void dump_error(const char *str, cl_int status) {
  printf("%s\n", str);
  printf("Error code: %d\n", status);
}


int main(int argc, char *argv[]) {
  
  const int num_workers = 1;
  const int num_threads = 1;
  
  args::ArgumentParser parser("Host progam for FPGA computer vision algorithm acceleration using Intel OpenCL SDK.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::ValueFlag<std::string> binary_file(parser, "binary", "FPGA .aocx binary file", {'b'});
  args::ValueFlag<std::string> app_path(parser, "app", "Application to run (path to dll file): \n ", {'a'});
  args::ValueFlag<std::string> input_file(parser, "input", "Input video file", {'i'});
  args::ValueFlag<int> tile_cols(parser, "tile_cols", "Input tile dimension", {'t'});
  args::ValueFlag<int> seq_size(parser, "seq_size", "Number of frames in input sequence", {'n'});

  args::Group group(parser, "This group is all exclusive:", args::Group::Validators::DontCare);
  args::Flag verbose(group, "verbose", "Verbose mode", {'v'});
  args::Flag emulator(group, "emulator", "Run in emulator", {'e'});
  args::Flag dump_perf(group, "perf", "Dump perf stats", {'p'});

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
    if (binary_file && app_path) {;
      Params::set(args::get(binary_file),
                  args::get(app_path),
                  input_file? args::get(input_file): std::string(),
                  tile_cols? args::get(tile_cols) : 128,
                  seq_size? args::get(seq_size) : INT_MAX,
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
  CLManager cl_manager;
  CLIOManager clio_manager;
  cl_manager.init_cl(Params::get_binary_file());
  if (Params::verbose())
    cl_manager.get_device_info();
  clio_manager.init(cl_manager.context);

  // Program the board with a binary that supports the entire program
  // No PR at this point
  if (Params::verbose())
    std::cout<<"Programming device"<<std::endl;
  cl_manager.program_board(0);
  
  // App manager
  ApplicationManager app_manager(Params::get_app_path(), 
                                num_workers, 
                                num_threads, 
                                Params::seq_size(),
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
 
  Application* app = app_manager.get_app();
  cv::Mat mat, dmat;
  

  GraphCut cut[10];  //TODO: fix
   

  for(int seq =0; seq  < std::min(Params::seq_size(), no_frames);  seq++) {
    printf ("Processing frame %d \n", seq);
  
    // Not necessarily
    cv::waitKey(30);
    cap >> mat;
    // Mat is a BGR image
    assert(mat.channels() == 3);

    // if the app requires Grayscale images
    if (app->get_color() == Color::GRAYSCALE)
      cv::cvtColor(mat, dmat, cv::COLOR_BGR2GRAY);
    else if (app->get_color() == Color::RGBX) {
      cv::cvtColor(mat, dmat, cv::COLOR_BGR2RGBA);
      std::cout<<"Converted to RGBA"<<std::endl;
    } else {
      throw std::runtime_error("Unexpected color format");
    }

    app->set_cut_data_info(dmat);
    cv::imwrite("in/frame_in_" + std::to_string(seq) + ".jpg", mat);
      
    // Process cuts
    // GraphCut cut;
    for(int i =0; i < app->get_cuts_number(); i++) {
      // Reset pipes for later cuts

      if (seq == 0){
        cut[i] = app->get_execution_cut(cl_manager.program, 
          clio_manager.read_pipe, 
          clio_manager.write_pipe, 
          0, i);
      }

      // Allocate/prepare input/output buffers
      // Can come from input file or previous cut execution
      CutData cut_in = app->get_input_data(i);
      CutData cut_out = app->get_output_data(i);

      // Reording using CPU
      app->pre_process_cut(seq, i);

      int in_size = cut_in.total_bytes();
      int out_size = cut_out.total_bytes();
      
      printf("insize = %d bytes\n", in_size);
      printf("outsize = %d bytes\n", out_size); 

      cl_manager.enqueue_cut(cut[i]);
      
      printf ("Beginning to push and pull data\n");
      clio_manager.overlapped_io(cut_in.mat.data, cut_out.mat.data, in_size, out_size);

      app->post_process_cut(seq, i);

      std::cout<<"Waiting for all queues to finish"<<std::endl;
      cl_manager.wait_finish(cut[i].kernels.size());
    }
  }

  //clio_manager.release();  
  cl_manager.cleanup();

  if(dump_perf)
    Perf::dump_events();

  //cv::waitKey(0);
  return 0;
}
