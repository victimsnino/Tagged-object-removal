#include "NetworkExecutor.h"

#include <pybind11/embed.h>

#include <condition_variable>
#include <iostream>

namespace py = pybind11;

class NetworkExecutor final : public INetworkExecutor
{
public:
    NetworkExecutor();

    ~NetworkExecutor() override
    {
        py::gil_scoped_acquire gil{};
        m_state.reset();
    }

    std::string GetResult() override;

private:
    py::module& GetModule();
private:
    struct PythonState
    {
        py::scoped_interpreter guard{};
        py::module             module{};
        py::gil_scoped_release release{};
    };

    std::optional<PythonState> m_state{PythonState{}};
    std::once_flag             m_flag{};
    std::thread                m_loading{};
};


NetworkExecutor::NetworkExecutor()
: m_loading{&NetworkExecutor::GetModule, this}
{}


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
                       py::module                   sys = py::module::import("sys");
                       sys.attr("path").
                           attr("append")("C:/Coding/Study/Tagged-object-removal/app/_build/bin/Release/python");
                       m_state->module = py::module::import("python_module");
                       std::cout << "Python loading done" << std::endl;
                   });
    return m_state->module;
}

std::shared_ptr<INetworkExecutor> INetworkExecutor::Create()
{
    return std::make_shared<NetworkExecutor>();
}
