#include "NetworkExecutor.h"

#include <pybind11/embed.h>

namespace py = pybind11;

class NetworkExecutor final : public INetworkExecutor
{
public:
    NetworkExecutor() = default;

    std::string GetResult() override;

private:
    py::scoped_interpreter m_guard{};
    py::module m_module{py::module::import("python_module")};
};


std::string NetworkExecutor::GetResult()
{
    return m_module.attr("get_result")().cast<std::string>();
}

std::shared_ptr<INetworkExecutor> INetworkExecutor::Create()
{
    return std::make_shared<NetworkExecutor>();
}
