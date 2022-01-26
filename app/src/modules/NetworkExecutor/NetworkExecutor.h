#pragma once

#include <memory>
#include <string>

struct INetworkExecutor
{
    virtual ~INetworkExecutor() = default;

    virtual std::string GetResult() = 0;

    static std::shared_ptr<INetworkExecutor> Create();
};
