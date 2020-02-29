#include "application.h"
#include "cl_util.h"

#include <string>
#include <iostream>
#include <cassert>
#include "params.h"
#include "cl_manager.h"
#include <opencv2/opencv.hpp>

#define GMEM_SIZE (8*1024*1024)

class AutoC: public Application {
  public:
    AutoC(CLManager * cl_manager);
    //std::vector<std::future<void>> tile_process(int, int, Tile, std::unique_ptr<Task::Scheduler>&) override;
    GraphCut get_execution_cut(const cl::Program&,
                              const cl::Pipe&,
                              const cl::Pipe&, int, int) override;
    int get_cut_input_total(int) override;
    int get_cut_output_total(int) override;
    bool need_reordering(int) override;

    CutData get_input_data(int) override;
    CutData get_output_data(int) override;
    void set_cut_data_info(const cv::Mat&) override;

    void pre_process_cut(int, int) override;
    void post_process_cut(int, int) override;

    void set_global_buffers();

  private:
    int rows, cols;
    cl::Buffer fpga_global_buffers[4];
    CLManager* cl_manager;
};

AutoC::AutoC(CLManager * cl_manager) : Application("app-autoc", 2, seq_size, Color::RGBX), rows(0), cols(0), 
cl_manager(cl_manager)
{
  set_global_buffers();
}

void AutoC::set_global_buffers() {
  cl_int status;
  fpga_global_buffers[0] = cl_manager->create_fpga_buffer(CL_MEM_READ_WRITE, GMEM_SIZE);
  fpga_global_buffers[1] = cl_manager->create_fpga_buffer(CL_MEM_READ_WRITE, GMEM_SIZE);
  fpga_global_buffers[2] = cl_manager->create_fpga_buffer(CL_MEM_READ_WRITE, GMEM_SIZE);
}

void AutoC::set_cut_data_info(const cv::Mat& in_img) {
  cut_inputs.clear();
  cut_outputs.clear();

  assert (in_img.elemSize() == 4);
  CutData cut_in_1, cut_out_1, cut_in_2, cut_out_2;

  cut_in_1.mat = in_img;
  cut_out_1.mat = cv::Mat();
  cut_in_2.mat = cv::Mat();
  cut_out_2.mat = in_img.clone();

  cut_inputs.push_back(cut_in_1);
  cut_inputs.push_back(cut_in_2);
  cut_outputs.push_back(cut_out_1);
  cut_outputs.push_back(cut_out_2);
}

CutData AutoC::get_input_data(int i) {
  // Return input image for the first cut
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.total() << std::endl;
  return cut;
}

CutData AutoC::get_output_data(int i) {
  return cut_outputs.at(i);
}


void AutoC::pre_process_cut(int seq_no, int cut_no) {}

void AutoC::post_process_cut(int seq_no, int cut_no) {
  assert (cut_no < 2);
  if (cut_no == 1) {
    std::cout<<"Saving image"<<std::endl;
    cv::cvtColor(cut_outputs.at(1).mat, cut_outputs.at(1).mat, cv::COLOR_RGBA2BGR);
    cv::imwrite("out/autoc_out_" + std::to_string(seq_no) + ".jpg", cut_outputs.at(1).mat);
  }
}

bool AutoC::need_reordering(int cut_index) {
  return false;
}

int AutoC::get_cut_input_total(int index) {
  assert(index < num_cuts);
  return cut_inputs[index].total();
}
int AutoC::get_cut_output_total(int index) {
  assert(index < num_cuts);
  return cut_outputs[index].total();
}

GraphCut AutoC::get_execution_cut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, int seq_no, int cut_index) {

  assert(cut_index < num_cuts);

  cl_int status;
  GraphCut cut;

  std::vector<std::string> app_kernels;
  cl_uint items = get_cut_input_total(0);
  
  if (cut_index ==0) {
    auto kernel_info = {
      std::make_pair<std::string, ArgVector>("kernel_source", {
        std::make_pair(sizeof(cl_mem), (void *)&write_pipe()),
        std::make_pair(sizeof(cl_uint), (void *)&items)
      }), 
      std::make_pair<std::string, ArgVector>("kernel_rgbx_to_yuv4", {
        std::make_pair(sizeof(cl_uint), (void *)&items)
      }),
      std::make_pair<std::string, ArgVector>("kernel_replicate_y", {
        std::make_pair(sizeof(cl_uint), (void *)&items)
      }),
      std::make_pair<std::string, ArgVector>("kernel_hist", {
        std::make_pair(sizeof(cl_uint), (void *)&items)
      }),
      std::make_pair<std::string, ArgVector>("kernel_save_y", {
        std::make_pair(sizeof(cl_uint), (void *)&items),
        std::make_pair(sizeof(cl_mem), (void *)&fpga_global_buffers[0]())
      }),
      std::make_pair<std::string, ArgVector>("kernel_save_u", {
        std::make_pair(sizeof(cl_uint), (void *)&items),
        std::make_pair(sizeof(cl_mem), (void *)&fpga_global_buffers[1]())
      }),
      std::make_pair<std::string, ArgVector>("kernel_save_v", {
        std::make_pair(sizeof(cl_uint), (void *)&items),
        std::make_pair(sizeof(cl_mem), (void *)&fpga_global_buffers[2]())
      })
    };
    cut.kernels = setup_kernels(program, kernel_info);

  } else if (cut_index == 1) {
    auto kernel_info = {
      std::make_pair<std::string, ArgVector>("kernel_load_y", {
        std::make_pair(sizeof(cl_uint), (void *)&items),
        std::make_pair(sizeof(cl_mem), (void *)&fpga_global_buffers[0]())
      }),
      std::make_pair<std::string, ArgVector>("kernel_load_u", {
        std::make_pair(sizeof(cl_uint), (void *)&items),
        std::make_pair(sizeof(cl_mem), (void *)&fpga_global_buffers[1]())
      }),
      std::make_pair<std::string, ArgVector>("kernel_load_v", {
        std::make_pair(sizeof(cl_uint), (void *)&items),
        std::make_pair(sizeof(cl_mem), (void *)&fpga_global_buffers[2]())
      }),
      std::make_pair<std::string, ArgVector>("kernel_equalize", {
        std::make_pair(sizeof(cl_uint), (void *)&items)
      }), 
      std::make_pair<std::string, ArgVector>("kernel_yuv4_to_rgbx", {
        std::make_pair(sizeof(cl_uint), (void *)&items)
      }), 
      std::make_pair<std::string, ArgVector>("kernel_sink", {
        std::make_pair(sizeof(cl_mem), (void *)&read_pipe()),
        std::make_pair(sizeof(cl_uint), (void *)&items)
      }),
      
    };
    cut.kernels = setup_kernels(program, kernel_info);
  }
  return cut;
}

Application* get_app(CLManager* cl_manager) {
  return new AutoC(cl_manager);
}
