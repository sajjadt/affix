#include "application.h"
#include "cl_util.h"

#include "common.h"
#include "params.h"

#include <string>
#include <iostream>
#include <cassert>

#include <opencv2/opencv.hpp>

class Census: public Application {
  public:
    Census(int);
    //std::vector<std::future<void>> tile_process(int, int, Tile, std::unique_ptr<Task::Scheduler>&) override;
    GraphCut get_execution_cut(const cl::Program&, 
      const cl::Pipe&, 
      const cl::Pipe&, 
      int, int) override;
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

Census::Census(int seq_size) : Application("app-census", 1, seq_size, Color::GRAYSCALE), rows(0), cols(0)
{}

void Census::set_cut_data_info(const cv::Mat& in_img) {
  cut_inputs.clear();
  cut_outputs.clear();
  
  assert (in_img->elemSize() == 1);

  CutData cut_in, cut_out;
  cut_in.mat = in_img;
  cut_out.mat = in_img.clone();

  cut_inputs.push_back(cut_in);
  cut_outputs.push_back(cut_out);
}

CutData Census::get_input_data(int i) {
  // Return input image for the first cut
  std::cout<<"get in data "<<i << std::endl;
  CutData cut = cut_inputs.at(i);
  std::cout<< cut.total() << std::endl;
  return cut;
}

CutData Census::get_output_data(int i) {
  // Allocate same size as the input image
  return cut_outputs.at(i);
}

void Census::pre_process_cut(int seq_no, int cut_no) {
  assert(cut_no == 0);
  std::cout<<"pre procss"<<std::endl;
  //Perf::record_event(Event{.type = DATA_REORDER_STARTED, .time_point=std::chrono::high_resolution_clock::now(), .cut_index=0});
  //cut_inputs.at(0).mat->reorder_tiled(Params::get_tile_cols(), true);
  if (cut_inputs.at(0).mat.cols != Params::get_tile_cols())
    do_reorder(cut_inputs.at(0).mat, Params::get_tile_cols(), true).copyTo(cut_inputs.at(0).mat);
  //Perf::record_event(Event{.type = DATA_REORDER_FINISHED, .time_point=std::chrono::high_resolution_clock::now(), .cut_index=0});
  cv::imwrite("in/frame_in" + std::to_string(seq_no) + ".jpg", cut_outputs.at(0).mat);
}

void Census::post_process_cut(int seq_no, int cut_no) {
  assert (cut_no == 0);
  if (cut_outputs.at(0).mat.cols != Params::get_tile_cols())
    cut_outputs.at(0).mat = do_reorder(cut_outputs.at(0).mat, Params::get_tile_cols(), false);

  cv::imwrite("out/census_out" + std::to_string(seq_no) + ".jpg", cut_outputs[0].mat);
}

bool Census::need_reordering(int cut_index) {
  return true;
}

int Census::get_cut_input_total(int index) {
  return cut_inputs[index].total();
}
int Census::get_cut_output_total(int index) {
  return cut_outputs[index].total();
}

GraphCut Census::get_execution_cut(const cl::Program& program,
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
    std::make_pair<std::string, ArgVector>("kernel_sobel3x3", {
      std::make_pair(sizeof(cl_uint), (void *)&rows),
      std::make_pair(sizeof(cl_uint), (void *)&cols)
    }),
    std::make_pair<std::string, ArgVector>("kernel_magnitude", {
      std::make_pair(sizeof(cl_uint), (void *)&items)
    }),
    std::make_pair<std::string, ArgVector>("kernel_census3x3", {
      std::make_pair(sizeof(cl_uint), (void *)&rows),
      std::make_pair(sizeof(cl_uint), (void *)&cols)
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
  return new Census(1);
}
