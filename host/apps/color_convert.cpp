#include "application.h"
#include "opencl_util.h"

#include <string>
#include <iostream>
#include <cassert>

class ColorConvert: public Application {
  public:
    ColorConvert(int);
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

ColorConvert::ColorConvert(int seq_size) :
 Application("cc", 1, seq_size, Color::RGBX), 
 rows(0), cols(0)
{
}

void ColorConvert::SetCutDataInfo(const cv::Mat& in_img) {
  cut_inputs.clear();
  cut_outputs.clear();

  CutData cut_in, cut_out;

  cut_in.mat = in_img;
  cut_out.mat = in_img.clone();

  cut_inputs.push_back(cut_in);
  cut_outputs.push_back(cut_out);
}

CutData ColorConvert::GetInputData(int i) {
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.Total() << std::endl;
  return cut;
}

CutData ColorConvert::GetOutputData(int i) {
  return cut_outputs.at(i);
}

void ColorConvert::PreProcessCut(int seq_no, int cut_no) {
  assert(cut_no == 0);
}

void ColorConvert::PostProcessCut(int seq_no, int cut_no) {
  assert(cut_no == 0);

  cv::cvtColor(cut_outputs[0].mat, cut_outputs[0].mat, cv::COLOR_RGBA2BGR);
  cv::imwrite("out/cc_out_" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
  
}

bool ColorConvert::NeedReordering(int cut_index) {
  return false;
}

int ColorConvert::GetCutInputTotal(int index) {
  return cut_inputs[index].Total();
}
int ColorConvert::GetCutOutputTotal(int index) {
  return cut_outputs[index].Total();
}

GraphCut ColorConvert::GetExecutionCut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, 
                                  int seq_no, int cut_id) {

  assert (cut_id == 0);

  cl_int status;
  GraphCut cut;

  cl_uint items = GetCutInputTotal(0);

  auto kernel_info = {
    std::make_pair<std::string, ArgVector>("kernel_source", {
      std::make_pair(sizeof(cl_mem), (void *)&write_pipe()),
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }), 
    std::make_pair<std::string, ArgVector>("kernel_rgbx_to_yuv4", {
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }),
    std::make_pair<std::string, ArgVector>("kernel_yuv4_to_rgbx", {
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
  return new ColorConvert(1);
}
