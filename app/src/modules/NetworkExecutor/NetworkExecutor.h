#pragma once

#include <memory>
#include <string>

namespace cv {
class Mat;
}

struct INetworkExecutor
{
    virtual ~INetworkExecutor() = default;

    virtual cv::Mat ProcessImage(const cv::Mat& img) = 0;

    static std::shared_ptr<INetworkExecutor> Create();
};
