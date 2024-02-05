/*!
    @Description : https://github.com/shaoshengsong/
    @Author      : shaoshengsong
    @Date        : 2022-09-23 02:52:41
*/
#include <YOLOv5Detector.h>
#include <iostream>


void YOLOv5Detector::initOnnxModel() 
{
    size_t num_input_nodes = session_.GetInputCount();
    // std::vector<const char*> input_node_names(num_input_nodes);

    // std::vector<int64_t> input_node_dims;  // simplify... this model has only 1 input node [1, 3, 640, 640]
                                            // Otherwise need vector<vector<>>
    // std::vector<int64_t> output_node_dims;    

    printf("Number of inputs = %zu\n", num_input_nodes);
    Ort::AllocatorWithDefaultOptions allocator;
    // iterate over all input nodes
    for (int i = 0; i < num_input_nodes; i++) {
        // print input node names
        auto input_name = session_.GetInputNameAllocated(i, allocator);
        printf("Input %d : name=%s\n", i, input_name.get());
        // input_node_names[i] = input_name.get();
        // input_node_names.push_back(input_name.get());
        input_node_names.push_back("images");

        // print input node types
        Ort::TypeInfo type_info = session_.GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        printf("Input %d : type=%d\n", i, type);

        // print input shapes/dims
        input_node_dims = tensor_info.GetShape();
        printf("Input %d : num_dims=%zu\n", i, input_node_dims.size());
        for (int j = 0; j < input_node_dims.size(); j++)
        printf("Input %d : dim %d=%jd\n", i, j, input_node_dims[j]);
    }

    printf("---------------------\n");
    size_t num_output_nodes = session_.GetOutputCount();
    printf("Number of output = %zu\n", num_output_nodes);
    for (int i = 0; i < num_output_nodes; i++) {
        auto output_name = session_.GetOutputNameAllocated(i, allocator);
        printf("Output %d : name=%s\n", i, output_name.get());

        Ort::TypeInfo type_info = session_.GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        printf("Output %d : type=%d\n", i, type);

        // print output shapes/dims
        output_node_dims = tensor_info.GetShape();
        printf("Output %d : num_dims=%zu\n", i, output_node_dims.size());
        for (int j = 0; j < output_node_dims.size(); j++)
        printf("Output %d : dim %d=%jd\n", i, j, output_node_dims[j]);

        printf("\n\n");
        if (strcmp(output_name.get(), output_names[i]) == 0) {
            break;
        }
    }

}

YOLOv5Detector::YOLOv5Detector(void) {
    // output_names = {"output"}; //输出节点名
}

void YOLOv5Detector::init(std::string onnxpath)
{
    std::cout << "read " << onnxpath << std::endl; 
    // this->net = cv::dnn::readNetFromONNX(onnxpath);

    initOnnxModel();



    std::string file="/home/faith/DeepSORT/coco_80_labels_list.txt";
    std::ifstream ifs(file);
    if (!ifs.is_open())
        CV_Error(cv::Error::StsError, "File " + file + " not found");
    std::string line;
    while (std::getline(ifs, line))
    {
        classes_.push_back(line);
    }
}

void YOLOv5Detector::detect(cv::Mat & frame, std::vector<detect_result> &results)
{

    std::cout << frame.size() << std::endl;
    int w = frame.cols;
    int h = frame.rows;
    int _max = std::max(h, w);
    cv::Mat image = cv::Mat::zeros(cv::Size(_max, _max), CV_8UC3);
    cv::Rect roi(0, 0, w, h);
    frame.copyTo(image(roi));


    float x_factor = static_cast<float>(image.cols) / model_input_width_;
    float y_factor = static_cast<float>(image.rows) / model_input_height_;


    std::cout<<x_factor<<std::endl;
    std::cout<<y_factor<<std::endl;

    cv::Mat blob = cv::dnn::blobFromImage(image, 1 / 255.0, cv::Size(model_input_width_, model_input_height_), cv::Scalar(0, 0, 0), true, false);



    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeDefault);
    // output_tensor_ = Ort::Value::CreateTensor<float>(memory_info, results_.data(), results_.size(), output_node_dims.data(), output_node_dims.size());

    std::vector<Ort::Value> inputTensors;
    auto inputTensorSize = vectorProduct(input_node_dims);
    std::vector<float> inputTensorValues;
    inputTensorValues.assign(blob.begin<float>(),
                             blob.end<float>());
    inputTensors.push_back(Ort::Value::CreateTensor<float>(
        memory_info, inputTensorValues.data(), inputTensorSize, input_node_dims.data(),
        input_node_dims.size()));


    std::cout<<inputTensorSize << " " << input_node_dims.size() << " " << input_node_dims <<std::endl;


    auto outputTensors =  session_.Run(Ort::RunOptions{nullptr}, input_node_names.data(), inputTensors.data(), 1, output_names, 1);

    auto* rawOutput = outputTensors[0].GetTensorData<float>();
    std::vector<int64_t> outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
    size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
    // std::vector<float> output(rawOutput, rawOutput + count);

    std::cout << outputShape << std::endl;

    cv::Mat det_output(outputShape[1], outputShape[2], CV_32F, (void*)rawOutput);


    // session_.Run(Ort::RunOptions{nullptr}, input_node_names.data(), inputTensors.data(), 1, output_names, &output_tensor_, 1);

    // this->net.setInput(blob);
    // cv::Mat preds = this->net.forward("output");//outputname
    // cv::Mat det_output(preds.size[1], preds.size[2], CV_32F, preds.ptr<float>());

    std::vector<cv::Rect> boxes;
    std::vector<int> classIds;
    std::vector<float> confidences;
    for (int i = 0; i < det_output.rows; i++)
    {
        float box_conf = det_output.at<float>(i, 4);
        if (box_conf < nms_threshold_)
        {
            continue;
        }

        cv::Mat classes_confidences = det_output.row(i).colRange(5, 85);
        cv::Point classIdPoint;
        double cls_conf;
        cv::minMaxLoc(classes_confidences, 0, &cls_conf, 0, &classIdPoint);


        if (cls_conf > confidence_threshold_)
        {
            float cx = det_output.at<float>(i, 0);
            float cy = det_output.at<float>(i, 1);
            float ow = det_output.at<float>(i, 2);
            float oh = det_output.at<float>(i, 3);
            int x = static_cast<int>((cx - 0.5 * ow) * x_factor);
            int y = static_cast<int>((cy - 0.5 * oh) * y_factor);
            int width = static_cast<int>(ow * x_factor);
            int height = static_cast<int>(oh * y_factor);
            cv::Rect box;
            box.x = x;
            box.y = y;
            box.width = width;
            box.height = height;

            boxes.push_back(box);
            classIds.push_back(classIdPoint.x);
            confidences.push_back(cls_conf * box_conf);
        }
    }

    std::vector<int> indexes;
    cv::dnn::NMSBoxes(boxes, confidences, confidence_threshold_, nms_threshold_, indexes);
    for (size_t i = 0; i < indexes.size(); i++)
    {
        detect_result dr;
        int index = indexes[i];
        int idx = classIds[index];
        dr.box = boxes[index];
        dr.classId = idx;
        dr.confidence = confidences[index];
        results.push_back(dr);
    }

}
void YOLOv5Detector::draw_frame(cv::Mat & frame, std::vector<detect_result> &results)
{

    for(auto dr : results)
    {

        cv::rectangle(frame, dr.box , cv::Scalar(0, 0, 255), 2, 8);
        // cv::rectangle(frame, cv::Point(dr.box .tl().x, dr.box .tl().y - 20), cv::Point(dr.box .br().x, dr.box .tl().y), cv::Scalar(255, 0, 0), -1);

        std::string label = cv::format("%.2f", dr.confidence);
        label = classes_[dr.classId ] + ":" + label;

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        int x = dr.box.x;
        int y = dr.box.y - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > frame.cols)
            x = frame.cols - label_size.width;


        cv::putText(frame, label, cv::Point(dr.box.x, dr.box.y + label_size.height), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0));

    }
}
