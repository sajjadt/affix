#include "application.h"
#include <string>
#include <iostream>
#include <cassert>
#include "Vision/canny.hpp"
#include "params.h"


class Filter: public Application {
  public:
    Filter(int);
    //std::vector<std::future<void>> tile_process(int, int, Tile, std::unique_ptr<Task::Scheduler>&) override;
    GraphCut get_execution_cut(
      const cl::Program&, 
      const cl::Pipe&, 
      const cl::Pipe&, int,  int) override;
    int get_cut_input_total(int) override;
    int get_cut_output_total(int) override;
    bool need_reordering(int) override;

    CutData get_input_data(int) override;
    CutData get_output_data(int) override;
    void set_cut_data_info(const cv::Mat&) override;

    void pre_process_cut(int, int) override;
    void post_process_cut(int, int) override;
  private:
    int rows, cols;
};

Filter::Filter(int seq_size) : Application("filter", 1, seq_size, Color::GRAYSCALE), rows(0), cols(0) {}

void Filter::set_cut_data_info(const cv::Mat& in_img) {
  std::cout<<"Set cut info"<<std::endl;
  CutData cut_in, cut_out;

  in_img.copyTo(cut_in.mat);
  cut_out.mat = in_img.clone();

  cut_inputs.push_back(cut_in);
  cut_outputs.push_back(cut_out);
}

CutData Filter::get_input_data(int i) {
  // Return input image for the first cut
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.total() << std::endl;
  return cut;
}

CutData Filter::get_output_data(int i) {
  // Allocate same size as the input image
  return cut_outputs.at(i);
}

void Filter::pre_process_cut(int seq_no, int cut_no) {
  assert(cut_no == 0);
  std::cout<<"pre procss"<<std::endl;

  cut_inputs.at(0).mat = do_reorder(cut_inputs.at(0).mat, Params::get_tile_cols(), true);
}


void Filter::post_process_cut(int seq_no, int cut_no) {
  assert (cut_no == 0);
  if (cut_outputs.at(0).mat.cols != Params::get_tile_cols())
    cut_outputs.at(0).mat = do_reorder(cut_outputs.at(0).mat, Params::get_tile_cols(), false);
  cv::imwrite("out/filter_out_" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
}

bool Filter::need_reordering(int cut_index) {
  return true;
}

//std::vector<std::future<void>> Filter::tile_process(int num_workers, int cut_no, Tile tile, std::unique_ptr<Task::Scheduler>& scheduler) {
//  std::vector<std::future<void>> futures;
//  return futures;
//}

int Filter::get_cut_input_total(int index) {
  return cut_inputs[index].total();
}
int Filter::get_cut_output_total(int index) {
  return cut_outputs[index].total();
}

GraphCut Filter::get_execution_cut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, int seq_no, int cut_id) {

  assert (cut_id == 0);

  cl_int status;
  GraphCut cut;

  std::vector<std::string> app_kernels  = {
                    "kernel_source",
                    "kernel_filter",
                    "kernel_sink",
                    };

  std::vector<cl::Kernel> kernels;

  for(int i =0;i < app_kernels.size(); i++) {
    kernels.push_back(cl::Kernel(program, app_kernels[i].c_str(), &status));
    if(status != CL_SUCCESS) {
      throw std::runtime_error(Formatter() << "failed to create kernel" << app_kernels[i]);
    }
  }

  cl_uint rows = cut_inputs[cut_id].mat.rows;
  cl_uint cols = cut_inputs[cut_id].mat.cols;
  cl_uint items = rows*cols;

  std::vector<std::vector<KernelArg> > kernel_args(app_kernels.size());
  typedef std::vector<KernelArg> ArgVector;
  kernel_args[0] = ArgVector({
    KernelArg{.size=sizeof(cl_mem), .value=(void *)&write_pipe()},
    KernelArg{.size=sizeof(cl_uint), .value=(void *)&items}
  });
  kernel_args[1] = ArgVector({
    KernelArg{.size=sizeof(cl_uint), .value=(void *)&rows},
    KernelArg{.size=sizeof(cl_uint), .value=(void *)&cols}
  });
  kernel_args[2] = ArgVector({
    KernelArg{.size=sizeof(cl_mem), .value=(void *)&read_pipe()},
    KernelArg{.size=sizeof(cl_uint), .value=(void *)&items}
  });

// set kernel args
  int i = 0;
  for(auto it = kernels.begin(); it != kernels.end(); it++) {
    for (int j=0;j < kernel_args[i].size(); j++) {
      KernelArg arg = kernel_args[i][j];
      status = it->setArg(j, arg.size, arg.value);
      std::cout<<status << std::endl;
    }
    i++;
  }

  cut.kernels = kernels;
}

Application* get_app(CLManager* _) {
  return new Filter(1);
}
