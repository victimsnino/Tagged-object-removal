#pragma once

#include <ViewInterface.h>

#include <QMainWindow>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow final : public QMainWindow, public ViewInterface
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    rxcpp::observable<QImage>                   GetOnImageObservable() const override;
    rxcpp::observable<std::vector<std::string>> GetOnProcessTagsObservable() const override;
    rxcpp::observer<QImage>                     GetOnProcessedImageObserver() override;
    rxcpp::observer<std::vector<std::string>>   GetOnTagsListObserver() override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateImages() const;

private:
    std::unique_ptr<Ui::MainWindow>             m_ui;
    rxcpp::observable<QImage>                   m_on_image_observable;
    rxcpp::observable<std::vector<std::string>> m_on_process_tags;

    QPixmap m_pixmap_before{};
    QPixmap m_pixmap_after{};
};

