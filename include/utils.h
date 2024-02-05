#pragma once
#include <numeric> // accumulate
#include <vector>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <cctype>

#define utils_flip(arr)               \
    for (int j = 0; j < 8; j++) \
        arr ^= (1 << j);

std::string encode(const char *bytes_to_encode, unsigned int in_len);
std::string decode(const std::string &encoded_string);

uint64_t time_get(void);


template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
{
    os << "[";
    for (int i = 0; i < v.size(); ++i)
    {
        os << v[i];
        if (i != v.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}



template <typename T>
T vectorProduct(const std::vector<T> &v)
{
    return std::accumulate(v.begin(), v.end(), 1, std::multiplies<T>());
}
