#pragma once
#include <numeric> // accumulate
#include <vector>
#include <iostream>
#include <sys/time.h>

static inline uint64_t time_get(void)
{
    struct timeval tv;

    gettimeofday(&tv, 0);
    return (uint64_t)(tv.tv_sec * 1000000000ULL + tv.tv_usec * 1000);
}


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
