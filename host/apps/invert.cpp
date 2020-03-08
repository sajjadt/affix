#include "application.h"
#include "opencl_util.h"
#include <string>
#include <iostream>
#include <cassert>

class Invert: public Application {
  public:
    Invert(int);
    GraphCut GetExecutionCut(const cl::Program&, const cl::Pipe&, const cl::Pipe&, int, int) override;
    int GetCutInputTotal(int) override;
    int GetCutOutputTotal(int) override;
    bool NeedReordering(int) override;

    CutData GetInputData(int) override;
    CutData GetOutputData(int) override;
    void SetCutDataInfo(const cv::Mat&) override;

    void set_global_buffers();

    void PreProcessCut(int, int) override;
    void PostProcessCut(int, int) override;
  private:
    int rows, cols;
};

Invert::Invert(int seq_size) :
 Application("invert", 1, seq_size, Color::GRAYSCALE), rows(0), cols(0) {}

void Invert::SetCutDataInfo(const cv::Mat& in_img) {
  cut_inputs.clear();
  cut_outputs.clear();
  
  CutData cut_in, cut_out;

  cut_in.mat = in_img;
  cut_out.mat = in_img.clone();

  cut_inputs.push_back(cut_in);
  cut_outputs.push_back(cut_out);
}

CutData Invert::GetInputData(int i) {
  // Return input image for the first cut
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.Total() << std::endl;
  return cut;
}

CutData Invert::GetOutputData(int i) {
  // Allocate same size as the input image
  return cut_outputs.at(i);
}

void Invert::PreProcessCut(int seq_no, int cut_no) {
  assert(cut_no == 0);
  std::cout<<"pre procss"<<std::endl;
}

void Invert::PostProcessCut(int seq_no, int cut_no) {
  assert(cut_no == 0);
  std::cout<<"post procss"<<std::endl;

  //cv::cvtColor(cut_outputs[0].mat, cut_outputs[0].mat, cv::COLOR_RGBA2BGR);
  cv::imwrite("out/invert_out_" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
  
}

bool Invert::NeedReordering(int cut_index) {
  return false;
}

int Invert::GetCutInputTotal(int index) {
  return cut_inputs[index].Total();
}
int Invert::GetCutOutputTotal(int index) {
  return cut_outputs[index].Total();
}

GraphCut Invert::GetExecutionCut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, 
                                  int seq_no, int cut_id) {

  assert (cut_id == 0);

  cl_int status;
  GraphCut cut;
  cl_ulong items = GetCutInputTotal(0);

  auto kernel_info = {
    std::make_pair<std::string, ArgVector>("kernel_source", {
      std::make_pair(sizeof(cl_mem), (void *)&write_pipe()),
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }), 
    std::make_pair<std::string, ArgVector>("kernel_invert", {
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }),
    std::make_pair<std::string, ArgVector>("kernel_sink", {
      std::make_pair(sizeof(cl_mem), (void *)&read_pipe()),
      std::make_pair(sizeof(cl_uint), (void *)&items)
    })
  };
  
  cut.kernels = setup_kernels(program, kernel_info);
  return cut;
}

Application* GetAppliaction(OpenCLManager* _) {
  return new Invert(1);
}
