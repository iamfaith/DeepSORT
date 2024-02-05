// 引入相关的头文件
#include <iostream>
#include <onnxruntime_cxx_api.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

#include <stdexcept>
#include "utils.h"


// #include <zlib.h>

// // 定义一个函数，用于压缩字符串
// std::string _compress(std::string& str, int compressionlevel = Z_BEST_COMPRESSION) {
//     z_stream zs;                        // z_stream is zlib's control structure
//     memset(&zs, 0, sizeof(zs));

//     if (deflateInit(&zs, compressionlevel) != Z_OK)
//         throw(std::runtime_error("deflateInit failed while compressing."));

//     zs.next_in = (Bytef*)str.data();
//     zs.avail_in = str.size();           // set the z_stream's input

//     int ret;
//     char outbuffer[32768];
//     std::string outstring;

//     // retrieve the compressed bytes blockwise
//     do {
//         zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
//         zs.avail_out = sizeof(outbuffer);

//         ret = deflate(&zs, Z_FINISH);

//         if (outstring.size() < zs.total_out) {
//             // append the block to the output string
//             outstring.append(outbuffer,
//                              zs.total_out - outstring.size());
//         }
//     } while (ret == Z_OK);

//     deflateEnd(&zs);

//     if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
//         std::ostringstream oss;
//         oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
//         throw(std::runtime_error(oss.str()));
//     }

//     return outstring;
// }



int main(void)
{
    // 定义 onnx 文件的路径
    const char *model_path = "/home/faith/DeepSORT/yolov5s.onnx";

    // 创建一个文件流对象，以二进制方式打开 onnx 文件
    std::ifstream model_file(model_path, std::ios::binary);

    // 检查文件是否打开成功
    if (!model_file.is_open())
    {
        std::cerr << "Failed to open " << model_path << std::endl;
        return -1;
    }

    // 将文件流的指针移动到文件末尾，获取文件的大小
    model_file.seekg(0, std::ios::end);
    size_t model_data_length = model_file.tellg();

    // 将文件流的指针移动回文件开头
    model_file.seekg(0, std::ios::beg);

    // 创建一个 char 数组，用于存储文件的字节流
    char *model_data = new char[model_data_length];

    // 将文件的内容读取到 char 数组中
    model_file.read(model_data, model_data_length);

    // 关闭文件流
    model_file.close();



    // 创建一个字符串流对象，用于存储十六进制字符串
    std::stringstream hex_stream;

    // 设置字符串流的格式为十六进制
    hex_stream << std::hex;

    // 遍历 char 数组，将每个字节转换为两位的十六进制字符串，并添加到字符串流中
    for (size_t i = 0; i < model_data_length; i++) {
        // 将 char 转换为 unsigned char，避免负数的问题
        unsigned char byte = static_cast<unsigned char>(model_data[i]);
        // 将每个字节转换为两位的十六进制字符串，不足两位的补零
        hex_stream << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    // 获取字符串流中的十六进制字符串
    std::string hex_string = hex_stream.str();

    // 定义 .h 文件的路径
    const char* header_path = "onnx.mem.h";

    // 创建一个文件流对象，以写入方式打开 .h 文件
    std::ofstream header_file(header_path);

    // 检查文件是否打开成功
    if (!header_file.is_open()) {
    std::cerr << "Failed to open " << header_path << std::endl;
    return -1;
    }


    std::cout << hex_string.size() << " " << encode(model_data, model_data_length).size() <<  "\n";
    // hex_string = bytes_to_base64((unsigned char *)model_data, model_data_length);
    hex_string = encode(model_data, model_data_length);

    // 写入 .h 文件的内容，定义一个静态的 const char 数组，将十六进制字符串赋值给它
    header_file << "static const size_t model_data_length = " << model_data_length << ";\n";
    header_file << "static const char model_data[] = \"" << hex_string << "\";\n";

    // 关闭文件流
    header_file.close();




    // FILE *cppfp = fopen("onnx.mem.h", "wb");
    // // fprintf(cppfp, "\n#ifdef _MSC_VER\n__declspec(align(4))\n#else\n__attribute__((aligned(4)))\n#endif\n");
    // fprintf(cppfp, "static const size_t model_data_length = %ld;\n", model_data_length);
    // fprintf(cppfp, "static const unsigned char %s[] = {\n", "model_data");
    // for (int i = 1; i <= model_data_length; i++) 
    // {
    //     char c = model_data[i - 1];
    //     fprintf(cppfp, "0x%x,", c);
    //     if (i % 16 == 0)
    //     {
    //         fprintf(cppfp, "\n");
    //     }
    // }

    // fprintf(cppfp, "};\n");
    // fclose(cppfp);

    // 创建一个 onnxruntime 的环境对象
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

    // 创建一个 onnxruntime 的会话选项对象
    Ort::SessionOptions session_options;

    // 创建一个 onnxruntime 的会话对象，传入环境，模型数据，模型长度和会话选项
    Ort::Session session(env, model_data, model_data_length, session_options);

    // 释放 char 数组的内存
    delete[] model_data;

    // 现在，您可以使用 session 对象来加载和运行 onnx 模型了

    return 0;
}