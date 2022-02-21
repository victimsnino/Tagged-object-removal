#include "Controller.h"

#include <NetworkExecutor.h>
#include <rxqt_loop_singleton.h>
#include <ViewInterface.h>
#include <opencv2/imgproc.hpp>

#include <QImage>

static cv::Mat QImageToMat(const QImage& original)
{
    QImage  image = original.convertToFormat(QImage::Format_RGB888);
    cv::Mat view(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine());
    cvtColor(view, view, cv::COLOR_RGB2BGR);
    return view.clone();
}

static QImage MatToQImage(cv::Mat image)
{
    switch(image.type())
    {
        case CV_8UC4:
        {
            return QImage(image.data, image.cols, image.rows, image.step[0], QImage::Format_ARGB32);
        }
        case CV_8UC3:
        {
            cvtColor(image, image, cv::COLOR_BGR2BGRA); //COLOR_BGR2RGB doesn't behave so use RGBA
            return QImage(image.data, image.cols, image.rows, image.step[0], QImage::Format_ARGB32);
        }
        case CV_8UC1:
        {
            cvtColor(image, image, cv::COLOR_GRAY2BGRA);
            return QImage(image.data, image.cols, image.rows, image.step[0], QImage::Format_ARGB32);
        }
        default:
        {
            throw std::invalid_argument("Image format not supported");
        }
    }
}


Controller::Controller(ViewInterface& view)
{
    auto network_executor = INetworkExecutor::Create();

    view.GetOnImageObservable()
        .observe_on(rxcpp::observe_on_new_thread())
        .map(&QImageToMat)
        .map([network_executor](const cv::Mat& mat) { return network_executor->ProcessImage(mat); })
        .map(&MatToQImage)
        .observe_on(ObserveOnUIRunLoop())
        .subscribe(view.GetOnProcessedImageObserver());
}
