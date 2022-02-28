#pragma once

#include <rxcpp/rx-observable.hpp>

class QImage;

struct ViewInterface
{
    virtual ~ViewInterface() = default;

    virtual rxcpp::observable<QImage>                   GetOnImageObservable() const = 0;
    virtual rxcpp::observable<std::vector<std::string>> GetOnProcessTagsObservable() const = 0;
    virtual rxcpp::observer<QImage>                     GetOnProcessedImageObserver() = 0;
    virtual rxcpp::observer<std::vector<std::string>>   GetOnTagsListObserver() = 0;
};
