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

    rxcpp::observable<bool>  GetOnClickObservable() const override;
    rxcpp::observer<std::string> GetOnTextObserver() const override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    std::unique_ptr<Ui::MainWindow> ui;
};