
#include "utils.h"

// std::string replace_illegal_char(unsigned char &c)
// {
//   std::string str;
//   switch (c)
//   {                // 判断字符是否是不合法的字符
//   case '\"':       // 如果是双引号
//     return "\\\""; // 用 \" 替换

//   case '\\':       // 如果是反斜杠
//     return "\\\\"; // 用 \\ 替换

//   case '\n':      // 如果是换行符
//     return "\\n"; // 用 \n 替换
//   // 其他不合法的字符可以类似处理，这里省略
//   default: // 如果不是不合法的字符
//     str += c;
//     return str; // 保持不变
//   }
// }

uint64_t time_get(void)
{
  struct timeval tv;

  gettimeofday(&tv, 0);
  return (uint64_t)(tv.tv_sec * 1000000000ULL + tv.tv_usec * 1000);
}

// base64 字符集，共 64 个
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// 判断一个字符是否是 base64 字符集中的字符
inline bool is_base64(unsigned char c)
{
  return (isalnum(c) || (c == '+') || (c == '/'));
}

// base64 编码函数，将字节流转换为 base64 字符串
// 参数：bytes_to_encode：要编码的字节流，in_len：字节流的长度
// 返回值：编码后的 base64 字符串
std::string encode(const char *bytes_to_encode, unsigned int in_len)
{
  std::string ret;               // 存储结果
  int i = 0;                     // 字节流的索引
  int j = 0;                     // base64 字符的索引
  unsigned char char_array_3[3]; // 存储 3 个字节
  unsigned char char_array_4[4]; // 存储 4 个 base64 字符

  // 遍历字节流，每次取 3 个字节
  while (in_len--)
  {
    unsigned char _tmp = *(bytes_to_encode++);
    utils_flip(_tmp);
    char_array_3[i++] = _tmp;
    // 如果取到了 3 个字节，就进行编码
    if (i == 3)
    {
      // 将 3 个字节分为 4 个 6 位的块
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      // 根据字符集映射每个 6 位的块为一个字符，并追加到结果中
      for (i = 0; (i < 4); i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  // 如果最后剩余的字节数不足 3 个，就需要补充 0 并用 = 号表示
  if (i)
  {
    // 补充 0
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    // 分为 4 个 6 位的块
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    // 映射前 i + 1 个字符，剩余的用 = 号表示
    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }

  return ret;
}

// base64 解码函数，将 base64 字符串转换为字节流
// 参数：encoded_string：要解码的 base64 字符串
// 返回值：解码后的字节流
std::string decode(const std::string &encoded_string)
{
  int in_len = encoded_string.size(); // base64 字符串的长度
  int i = 0;                          // base64 字符的索引
  int j = 0;                          // 字节流的索引
  int in_ = 0;                        // base64 字符串的指针
  unsigned char char_array_4[4];      // 存储 4 个 base64 字符
  unsigned char char_array_3[3];      // 存储 3 个字节
  std::string ret;                    // 存储结果

  // 遍历 base64 字符串，每次取 4 个字符
  while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
  {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    // 如果取到了 4 个字符，就进行解码
    if (i == 4)
    {
      // 根据字符集反映射每个字符为一个 6 位的块
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

      // 将 4 个 6 位的块拼接为 3 个字节
      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      // 追加到结果中
      for (i = 0; (i < 3); i++)
      {
        unsigned char _tmp = char_array_3[i];
        utils_flip(_tmp);

        ret += _tmp;
      }
      i = 0;
    }
  }

  // 如果最后剩余的字符数不足 4 个，就需要去掉补充的 0
  if (i)
  {
    // 反映射每个字符为一个 6 位的块
    for (j = 0; j < i; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

    // 拼接为字节
    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

    // 追加前 i - 1 个字节到结果中
    for (j = 0; (j < i - 1); j++)
    {
      unsigned char _tmp = char_array_3[j];
      utils_flip(_tmp);

      ret += _tmp;
    }
  }

  return ret;
}
