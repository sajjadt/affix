#include "application.h"
#include <string>
#include <iostream>
#include <cassert>
#include "params.h"
#include "opencl_util.h"

class Filter: public Application {
  public:
    Filter(int);
    GraphCut GetExecutionCut(
      const cl::Program&, 
      const cl::Pipe&, 
      const cl::Pipe&, int,  int) override;
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

Filter::Filter(int seq_size) : Application("filter", 1, seq_size, Color::GRAYSCALE), rows(0), cols(0) {}

void Filter::SetCutDataInfo(const cv::Mat& in_img) {
  std::cout<<"Set cut info"<<std::endl;
  CutData cut_in, cut_out;

  in_img.copyTo(cut_in.mat);
  cut_out.mat = in_img.clone();

  cut_inputs.push_back(cut_in);
  cut_outputs.push_back(cut_out);
}

CutData Filter::GetInputData(int i) {
  // Return input image for the first cut
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.Total() << std::endl;
  return cut;
}

CutData Filter::GetOutputData(int i) {
  // Allocate same size as the input image
  return cut_outputs.at(i);
}

void Filter::PreProcessCut(int seq_no, int cut_no) {
  assert(cut_no == 0);
  std::cout<<"pre procss"<<std::endl;

  cut_inputs.at(0).mat = do_reorder(cut_inputs.at(0).mat, Params::GetTileCols(), true);
}

void Filter::PostProcessCut(int seq_no, int cut_no) {
  assert (cut_no == 0);
  if (cut_outputs.at(0).mat.cols != Params::GetTileCols())
    cut_outputs.at(0).mat = do_reorder(cut_outputs.at(0).mat, Params::GetTileCols(), false);
  cv::imwrite("out/filter_out_" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
}

bool Filter::NeedReordering(int cut_index) {
  return true;
}

int Filter::GetCutInputTotal(int index) {
  return cut_inputs[index].Total();
}
int Filter::GetCutOutputTotal(int index) {
  return cut_outputs[index].Total();
}

GraphCut Filter::GetExecutionCut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, int seq_no, int cut_id) {

  assert (cut_id == 0);

  cl_int status;
  GraphCut cut;

  cl_uint rows = cut_inputs[cut_id].mat.rows;
  cl_uint cols = cut_inputs[cut_id].mat.cols;
  cl_uint items = rows*cols;

  auto kernel_info = {
    std::make_pair<std::string, ArgVector>("kernel_source", {
      std::make_pair(sizeof(cl_mem), (void *)&write_pipe()),
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }), 
    std::make_pair<std::string, ArgVector>("kernel_filter", {
      std::make_pair(sizeof(cl_uint), (void *)&rows),
      std::make_pair(sizeof(cl_uint), (void *)&cols)
    }),
    std::make_pair<std::string, ArgVector>("kernel_sink", {
      std::make_pair(sizeof(cl_mem), (void *)&read_pipe()),
      std::make_pair(sizeof(cl_uint), (void *)&items)
    })
  };

  cut.kernels = setup_kernels(program, kernel_info);
}

Application* GetAppliaction(OpenCLManager* _) {
  return new Filter(1);
}
