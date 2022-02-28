#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cv {
class Mat;
}

struct INetworkExecutor
{
    virtual ~INetworkExecutor() = default;

    virtual cv::Mat                  ProcessImage(const std::tuple<std::vector<std::string>, cv::Mat>& tags_with_img) = 0;
    virtual std::vector<std::string> GetListOfTags() = 0;

    static std::shared_ptr<INetworkExecutor> Create();
};
