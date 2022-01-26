#pragma once

#include <rxcpp/rx-observable.hpp>

struct ViewInterface
{
    virtual ~ViewInterface() = default;

    virtual rxcpp::observable<bool>  GetOnClickObservable() const = 0;
    virtual rxcpp::observer<std::string> GetOnTextObserver() const = 0;
};
