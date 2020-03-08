#include "application.h"
#include "opencl_util.h"

#include <string>
#include <iostream>
#include <cassert>
#include "common.h"
#include "params.h"
#include <opencv2/opencv.hpp>


class Canny: public Application {
  public:
    Canny(int);
    GraphCut GetExecutionCut(const cl::Program&, 
    const cl::Pipe&, const cl::Pipe&, int, int) override;
    int GetCutInputTotal(int) override;
    int GetCutOutputTotal(int) override;
    bool NeedReordering(int) override;

    CutData GetInputData(int) override;
    CutData GetOutputData(int) override;
    void SetCutDataInfo(const cv::Mat&) override;

    void PreProcessCut(int, int) override;
    void PostProcessCut(int, int) override;

  private:
    int rows, cols;
};

Canny::Canny(int seq_size) : Application("app-canny", 1, seq_size, Color::GRAYSCALE), rows(0), cols(0)
{}

void Canny::SetCutDataInfo(const cv::Mat& in_img) {

  cut_inputs.clear();
  cut_outputs.clear();
  assert (in_img->elemSize() == 1);

  std::cout<<"Set cut info"<<std::endl;
  CutData cut_in, cut_out;

  cut_in.mat = in_img;
  cut_out.mat = in_img.clone();
  cut_inputs.push_back(cut_in);
  cut_outputs.push_back(cut_out);
}

CutData Canny::GetInputData(int i) {
  // Return input image for the first cut
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.Total() << std::endl;
  return cut;
}

CutData Canny::GetOutputData(int i) {
  // Allocate same size as the input image
  return cut_outputs.at(i);
}


void Canny::PreProcessCut(int seq_no, int cut_no) {
  assert(cut_no == 0);
  
  if (cut_inputs.at(0).mat.cols != Params::GetTileCols())
    do_reorder(cut_inputs.at(0).mat, Params::GetTileCols(), true).copyTo(cut_inputs.at(0).mat);

  cv::imwrite("in/frame_in" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
  
}

void Canny::PostProcessCut(int seq_no, int cut_no) {
  assert (cut_no == 0);
    
  if (cut_outputs.at(0).mat.cols != Params::GetTileCols())
    cut_outputs.at(0).mat = do_reorder(cut_outputs.at(0).mat, Params::GetTileCols(), false);

  cv::imwrite("out/canny_out" + std::to_string(seq_no) + ".jpg", cut_outputs[0].mat);
}

bool Canny::NeedReordering(int cut_index) {
  return true;
}

int Canny::GetCutInputTotal(int index) {
  return cut_inputs[index].Total();
}

int Canny::GetCutOutputTotal(int index) {
  return cut_outputs[index].Total();
}

GraphCut Canny::GetExecutionCut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, 
                                  int seq_no, int cut_id) {

  assert (cut_id == 0);

  cl_int status;
  GraphCut cut;

  cl_uint items = GetCutInputTotal(cut_id);
  cl_uint rows = cut_inputs[cut_id].mat.rows;
  cl_uint cols = cut_inputs[cut_id].mat.cols;

  auto kernel_info = {
    std::make_pair<std::string, ArgVector>("kernel_source", {
      std::make_pair(sizeof(cl_mem), (void *)&write_pipe()),
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }), 
    std::make_pair<std::string, ArgVector>("kernel_gblur5x5", {
      std::make_pair(sizeof(cl_uint), (void *)&rows),
      std::make_pair(sizeof(cl_uint), (void *)&cols)
    }),
    std::make_pair<std::string, ArgVector>("kernel_sobel3x3", {
      std::make_pair(sizeof(cl_uint), (void *)&rows),
      std::make_pair(sizeof(cl_uint), (void *)&cols)
    }),
    std::make_pair<std::string, ArgVector>("kernel_magnitude", {
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }),
    std::make_pair<std::string, ArgVector>("kernel_phase", {
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }),
    std::make_pair<std::string, ArgVector>("kernel_nonmax_suppression", {
      std::make_pair(sizeof(cl_uint), (void *)&rows),
      std::make_pair(sizeof(cl_uint), (void *)&cols)
    }),
    std::make_pair<std::string, ArgVector>("kernel_threshold", {
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
  return new Canny(1);
}
