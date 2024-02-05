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
    void detect(cv::Mat &frame, std::vector<detect_result> &result);
    std::vector<std::string> classes_;
    void draw_frame(cv::Mat &frame, std::vector<detect_result> &results);
    YOLOv5Detector();

private:
    void initOnnxModel();

private:
    cv::dnn::Net net;

    // nms threshold: 0.3 ~ 0.7，较高的值会保留更多的边界框，较低的值会剔除更多的边界框。
    // confidence threshold: 0.1 ~ 0.5，较高的值会提高检测的准确性，较低的值会提高检测的召回率。
    //  使用的是yolov5s模型，您可以参考官方文档中的默认设置，即nms threshold为0.45，confidence threshold为0.25。
    // const float confidence_threshold_ = 0.25f;
    const float confidence_threshold_ = 0.7;
    const float nms_threshold_ = 0.4f;

    const int model_input_width_ = 640;
    const int model_input_height_ = 640;

    Ort::Env env;
    Ort::Session session_{env, k_detect_model_path.c_str(), Ort::SessionOptions{nullptr}};
    std::vector<const char *> input_node_names;
    // Ort::Value input_tensor_{nullptr};
    std::vector<int64_t> input_node_dims;

    const char *output_names[1] = {"output"};
    std::vector<int64_t> output_node_dims;
    // Ort::Value output_tensor_{nullptr};
};
