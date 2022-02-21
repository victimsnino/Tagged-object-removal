#include "NetworkExecutor.h"

#include <opencv2/imgproc.hpp>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>

#include <condition_variable>
#include <filesystem>
#include <iostream>

#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#elif
#include <unistd.h>
#endif

static std::filesystem::path GetExeDirectory()
{
#ifdef _WIN32
    // Windows specific
    wchar_t szPath[MAX_PATH];
    GetModuleFileNameW( NULL, szPath, MAX_PATH );
#else
    // Linux specific
    char szPath[PATH_MAX];
    ssize_t count = readlink( "/proc/self/exe", szPath, PATH_MAX );
    if( count < 0 || count >= PATH_MAX )
        return {}; // some error
    szPath[count] = '\0';
#endif
    return std::filesystem::path{ szPath }.parent_path() / ""; // to finish the folder path with (back)slash
}

namespace py = pybind11;

static py::dtype determine_np_dtype(int depth)
{
    switch (depth) {
    case CV_8U: return py::dtype::of<uint8_t>();
    case CV_8S: return py::dtype::of<int8_t>();
    case CV_16U: return py::dtype::of<uint16_t>();
    case CV_16S: return py::dtype::of<int16_t>();
    case CV_32S: return py::dtype::of<int32_t>();
    case CV_32F: return py::dtype::of<float>();
    case CV_64F: return py::dtype::of<double>();
    default:
        throw std::invalid_argument("Unsupported data type.");
    }
}

static std::vector<std::size_t> determine_shape(const cv::Mat& m)
{
    if (m.channels() == 1) {
        return {
            static_cast<size_t>(m.rows)
            , static_cast<size_t>(m.cols)
        };
    }
    auto channels =[&]()
    {
        switch(m.type())
        {
        case CV_8UC1:
            return 1;
        case CV_8UC2:
            return 2;
        case CV_8UC3:
            return 3;
        case CV_8UC4:
            return 4;
        }
        throw std::logic_error{"Unsupported channels types"};
    }();
    return {
        static_cast<size_t>(m.rows)
        , static_cast<size_t>(m.cols)
        , static_cast<size_t>(channels)
    };
}

static py::capsule make_capsule(const cv::Mat& m)
{
    return py::capsule(&m);
}

static py::array mat_to_nparray(const cv::Mat& m)
{
    if (!m.isContinuous()) {
        throw std::invalid_argument("Only continuous Mats supported.");
    }

    return py::array(determine_np_dtype(m.depth())
        , determine_shape(m)
        , m.data
        , make_capsule(m));
}

class NetworkExecutor final : public INetworkExecutor
{
public:
    NetworkExecutor();

    ~NetworkExecutor() override
    {
        py::gil_scoped_acquire gil{};
        m_state.reset();
    }

    cv::Mat ProcessImage(const cv::Mat& img) override;

private:
    py::module& GetModule();
private:
    struct PythonState
    {
        py::scoped_interpreter guard{};
        py::module             module{};
        py::gil_scoped_release release{};
    };

    std::optional<PythonState> m_state{};
    std::once_flag             m_flag{};
    std::thread                m_loading{};
};


NetworkExecutor::NetworkExecutor()
    : m_state{std::in_place_t{}}
    , m_loading{&NetworkExecutor::GetModule, this} {}


cv::Mat NetworkExecutor::ProcessImage(const cv::Mat& original_img)
{
    auto& module = GetModule();

    pybind11::gil_scoped_acquire acquire{};
    auto img = module.attr("process_image")(mat_to_nparray(original_img)).cast<py::array_t<uint8_t>>();

    //auto im = img.unchecked<3>();
    auto rows = img.shape(0);
    auto cols = img.shape(1);
    auto type = CV_MAKETYPE(CV_8U,img.shape(2));

    return cv::Mat(rows, cols, type, (unsigned char*)img.data());
}

py::module& NetworkExecutor::GetModule()
{
    std::call_once(m_flag,
                   [&]()
                   {
                       pybind11::gil_scoped_acquire acquire{};
                       py::module                   sys = py::module::import("sys");

                       sys.attr("path").
                           attr("append")((GetExeDirectory()/"python").string());

                       std::cout << "Python loading started" << std::endl;
                       m_state->module = py::module::import("python_module");
                       std::cout << "Python loading done" << std::endl;
                   });
    return m_state->module;
}

std::shared_ptr<INetworkExecutor> INetworkExecutor::Create()
{
    return std::make_shared<NetworkExecutor>();
}
