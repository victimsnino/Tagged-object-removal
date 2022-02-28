#include "Controller.h"

#include <NetworkExecutor.h>
#include <rxqt_loop_singleton.h>
#include <ViewInterface.h>
#include <opencv2/imgproc.hpp>

#include <QImage>


static QImage MatToQimageRef(cv::Mat& mat, QImage::Format format)
{
    return QImage(mat.data,
                  mat.cols,
                  mat.rows,
                  mat.step,
                  format);
}

static QImage MatToQimageRef(cv::Mat& mat)
{
    if (mat.empty())
        return {};
    switch (mat.type())
    {
    case CV_8UC3:
        return MatToQimageRef(mat, QImage::Format_RGB888).rgbSwapped();
    case CV_8U:
        return MatToQimageRef(mat, QImage::Format_Indexed8);
    case CV_8UC4:
        return MatToQimageRef(mat, QImage::Format_ARGB32);
    default:
        return {};
    }
}

static cv::Mat QImageToMatRef(QImage& img, int format)
{
    return cv::Mat(img.height(),
                   img.width(),
                   format,
                   img.bits(),
                   img.bytesPerLine());
}

static cv::Mat QImageToMatRef(QImage& img)
{
    if (img.isNull())
        return cv::Mat();

    switch (img.format())
    {
    case QImage::Format_RGB888:
    {
        auto result = QImageToMatRef(img, CV_8UC3);
        cvtColor(result, result, cv::COLOR_RGB2BGR);
        return result;
    }
    case QImage::Format_Indexed8:
    {
        return QImageToMatRef(img, CV_8U);
    }
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        auto res = QImageToMatRef(img, CV_8UC4);
        cvtColor(res, res, cv::COLOR_BGRA2BGR);
        return res;
    }
    default:
        break;
    }

    return {};
}

static QImage MatToQImageCopy(cv::Mat mat)
{
    return MatToQimageRef(mat).copy();
}

static cv::Mat QImageToMatCopy(QImage img)
{
    return QImageToMatRef(img).clone();
}

Controller::Controller(ViewInterface& view)
{
    auto network_executor = INetworkExecutor::Create();

    view.GetOnProcessTagsObservable().observe_on(rxcpp::observe_on_new_thread()).
         with_latest_from(view.GetOnImageObservable()
                              .observe_on(rxcpp::observe_on_new_thread())
                              .map(&QImageToMatCopy))
         .map([network_executor](const std::tuple<std::vector<std::string>, cv::Mat>& tags_with_img) { return network_executor->ProcessImage(tags_with_img); })
         .map(&MatToQImageCopy)
         .observe_on(ObserveOnUIRunLoop())
         .subscribe(view.GetOnProcessedImageObserver());

    rxcpp::observable<>::defer([network_executor]()
    {
        return rxcpp::observable<>::just(network_executor->GetListOfTags());
    })
    .subscribe_on(rxcpp::observe_on_new_thread())
    .observe_on(ObserveOnUIRunLoop())
    .subscribe(view.GetOnTagsListObserver());
}
