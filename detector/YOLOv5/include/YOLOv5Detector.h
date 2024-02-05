#pragma once
#include <onnxruntime_cxx_api.h>
#include <vector>
// #include <numeric>
// #include <string>
// #include <stdexcept> 

#include "dataType.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include <sstream>
class detect_result
{
public:
    int classId;
    float confidence;
    cv::Rect_<float> box;
};



class YOLOv5Detector
{
public:
    void init(std::string onnxpath);
    void detect(cv::Mat & frame, std::vector<detect_result> &result);
     std::vector<std::string> classes_;
     void draw_frame(cv::Mat & frame, std::vector<detect_result> &results);
     YOLOv5Detector();
private:
    void initOnnxModel();

private:

    cv::dnn::Net net;

    const float confidence_threshold_ =0.25f;
    const float nms_threshold_ = 0.4f;

    const int model_input_width_ = 640;
    const int model_input_height_ = 640;


    Ort::Env env;
    Ort::Session session_{env, k_detect_model_path.c_str(), Ort::SessionOptions{nullptr}};
    std::vector<const char*> input_node_names;
    // Ort::Value input_tensor_{nullptr};
    std::vector<int64_t> input_node_dims;


    const char *output_names[1] = {"output"};
    std::vector<int64_t> output_node_dims;
    // Ort::Value output_tensor_{nullptr};




};
