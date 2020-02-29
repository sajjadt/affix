#include "application.h"
#include "cl_util.h"

#include <string>
#include <iostream>
#include <cassert>
#include "common.h"
#include "params.h"
#include <opencv2/opencv.hpp>


class Canny: public Application {
  public:
    Canny(int);
    //std::vector<std::future<void>> tile_process(int, int, Tile, std::unique_ptr<Task::Scheduler>&) override;
    GraphCut get_execution_cut(const cl::Program&, 
    const cl::Pipe&, const cl::Pipe&, int, int) override;
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

Canny::Canny(int seq_size) : Application("app-canny", 1, seq_size, Color::GRAYSCALE), rows(0), cols(0)
{}

void Canny::set_cut_data_info(const cv::Mat& in_img) {

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

CutData Canny::get_input_data(int i) {
  // Return input image for the first cut
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.total() << std::endl;
  return cut;
}

CutData Canny::get_output_data(int i) {
  // Allocate same size as the input image
  return cut_outputs.at(i);
}


void Canny::pre_process_cut(int seq_no, int cut_no) {
  assert(cut_no == 0);
  
  if (cut_inputs.at(0).mat.cols != Params::get_tile_cols())
    do_reorder(cut_inputs.at(0).mat, Params::get_tile_cols(), true).copyTo(cut_inputs.at(0).mat);

  cv::imwrite("in/frame_in" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
  
}

void Canny::post_process_cut(int seq_no, int cut_no) {
  assert (cut_no == 0);
    
  if (cut_outputs.at(0).mat.cols != Params::get_tile_cols())
    cut_outputs.at(0).mat = do_reorder(cut_outputs.at(0).mat, Params::get_tile_cols(), false);

  cv::imwrite("out/canny_out" + std::to_string(seq_no) + ".jpg", cut_outputs[0].mat);
}

bool Canny::need_reordering(int cut_index) {
  return true;
}

int Canny::get_cut_input_total(int index) {
  return cut_inputs[index].total();
}

int Canny::get_cut_output_total(int index) {
  return cut_outputs[index].total();
}

GraphCut Canny::get_execution_cut(const cl::Program& program,
                                  const cl::Pipe& read_pipe,
                                  const cl::Pipe& write_pipe, 
                                  int seq_no, int cut_id) {

  assert (cut_id == 0);

  cl_int status;
  GraphCut cut;

  cl_uint items = get_cut_input_total(cut_id);
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

Application* get_app(CLManager* _) {
  return new Canny(1);
}
