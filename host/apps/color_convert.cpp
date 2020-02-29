#include "application.h"
#include "cl_util.h"

#include <string>
#include <iostream>
#include <cassert>

class ColorConvert: public Application {
  public:
    ColorConvert(int);
    //std::vector<std::future<void>> tile_process(int, int, Tile, std::unique_ptr<Task::Scheduler>&) override;
    GraphCut get_execution_cut(const cl::Program&, const cl::Pipe&, const cl::Pipe&, int, int) override;
    int get_cut_input_total(int) override;
    int get_cut_output_total(int) override;
    bool need_reordering(int) override;

    CutData get_input_data(int) override;
    CutData get_output_data(int) override;
    void set_cut_data_info(const cv::Mat&) override;

    void set_global_buffers();

    void pre_process_cut(int, int) override;
    void post_process_cut(int, int) override;
  private:
    int rows, cols;
};

ColorConvert::ColorConvert(int seq_size) :
 Application("cc", 1, seq_size, Color::RGBX), 
 rows(0), cols(0)
{
}

void ColorConvert::set_cut_data_info(const cv::Mat& in_img) {
  cut_inputs.clear();
  cut_outputs.clear();

  CutData cut_in, cut_out;

  cut_in.mat = in_img;
  cut_out.mat = in_img.clone();

  cut_inputs.push_back(cut_in);
  cut_outputs.push_back(cut_out);
}

CutData ColorConvert::get_input_data(int i) {
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.total() << std::endl;
  return cut;
}

CutData ColorConvert::get_output_data(int i) {
  return cut_outputs.at(i);
}

void ColorConvert::pre_process_cut(int seq_no, int cut_no) {
  assert(cut_no == 0);
}

void ColorConvert::post_process_cut(int seq_no, int cut_no) {
  assert(cut_no == 0);

  cv::cvtColor(cut_outputs[0].mat, cut_outputs[0].mat, cv::COLOR_RGBA2BGR);
  cv::imwrite("out/cc_out_" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
  
}

bool ColorConvert::need_reordering(int cut_index) {
  return false;
}

int ColorConvert::get_cut_input_total(int index) {
  return cut_inputs[index].total();
}
int ColorConvert::get_cut_output_total(int index) {
  return cut_outputs[index].total();
}

GraphCut ColorConvert::get_execution_cut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, 
                                  int seq_no, int cut_id) {

  assert (cut_id == 0);

  cl_int status;
  GraphCut cut;

  cl_uint items = get_cut_input_total(0);

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

Application* get_app(CLManager* _) {
  return new ColorConvert(1);
}
