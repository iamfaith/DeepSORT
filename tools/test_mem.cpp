// 引入相关的头文件
#include <iostream>
#include <onnxruntime_cxx_api.h>
#include <fstream>
#include "onnx.mem.h"
#include "utils.h"

// // 定义一个函数，用于解压缩字符串
// std::string _decompress(std::string &str)
// {
//     z_stream zs; // z_stream is zlib's control structure
//     memset(&zs, 0, sizeof(zs));

//     if (inflateInit(&zs) != Z_OK)
//         throw(std::runtime_error("inflateInit failed while decompressing."));

//     zs.next_in = (Bytef *)str.data();
//     zs.avail_in = str.size();

//     int ret;
//     char outbuffer[32768];
//     std::string outstring;

//     // get the decompressed bytes blockwise using repeated calls to inflate
//     do
//     {
//         zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
//         zs.avail_out = sizeof(outbuffer);

//         ret = inflate(&zs, 0);

//         if (outstring.size() < zs.total_out)
//         {
//             outstring.append(outbuffer,
//                              zs.total_out - outstring.size());
//         }

//     } while (ret == Z_OK);

//     inflateEnd(&zs);

//     if (ret != Z_STREAM_END)
//     { // an error occurred that was not EOF
//         std::ostringstream oss;
//         oss << "Exception during zlib decompression: (" << ret << ") "
//             << zs.msg;
//         throw(std::runtime_error(oss.str()));
//     }

//     return outstring;
// }



int main(void)
{

    // 定义字符串的 model_data
    // std::string model_data1 = std::string(model_data);

    // // 定义一个 char 数组，用于存储转换后的字节流
    // std::vector<char> array;

    // // 遍历字符串的 model_data，每两个字符一组
    // for (int i = 0; i < model_data1.length(); i += 2)
    // {
    //     // 获取当前的子字符串
    //     std::string sub = model_data1.substr(i, 2);
    //     // 将子字符串转换为十六进制数值
    //     int num = std::stoi(sub, nullptr, 16);
    //     // 将数值转换为 char 类型，并存储到数组中
    //     array.push_back(static_cast<char>(num));
    // }



    std::string array = decode(model_data);

    std::cout << model_data_length <<  " " << array.size() << "\n"; 
    // 创建一个 onnxruntime 的环境对象
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

    // 创建一个 onnxruntime 的会话选项对象
    Ort::SessionOptions session_options;

    // 创建一个 onnxruntime 的会话对象，传入环境，模型数据，模型长度和会话选项
    Ort::Session session(env, array.data(), model_data_length, session_options);  // c_str 会加后缀
    // Ort::Session session(env, array, model_data_length, session_options);
    return 0;
}