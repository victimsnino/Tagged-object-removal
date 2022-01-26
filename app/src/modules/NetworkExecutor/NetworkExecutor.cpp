#include "NetworkExecutor.h"

#include <pybind11/embed.h>

#include <condition_variable>
#include <iostream>

namespace py = pybind11;

class NetworkExecutor final : public INetworkExecutor
{
public:
    NetworkExecutor();

    std::string GetResult() override;
private:
    py::module& GetModule();
private:
    py::scoped_interpreter m_guard{};
    std::once_flag         m_flag{};
    py::gil_scoped_release m_release{}; // immediately release gil to provide ability to load module from another thread
    py::module             m_module{};
};


NetworkExecutor::NetworkExecutor()
{
    std::thread(&NetworkExecutor::GetModule, this).detach();
}

std::string NetworkExecutor::GetResult()
{
    auto& module = GetModule();

    pybind11::gil_scoped_acquire acquire{};
    return module.attr("get_result")().cast<std::string>();
}

py::module& NetworkExecutor::GetModule()
{
    std::call_once(m_flag,
                   [&]()
                   {
                       pybind11::gil_scoped_acquire acquire{};
                       m_module = py::module::import("python_module");
                       std::cout << "Python loading done" << std::endl;
                   });
    return m_module;
}

std::shared_ptr<INetworkExecutor> INetworkExecutor::Create()
{
    return std::make_shared<NetworkExecutor>();
}
