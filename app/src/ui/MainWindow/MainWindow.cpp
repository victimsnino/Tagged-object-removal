#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <rxqt.hpp>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    ui.reset();
}

rxcpp::observable<bool>  MainWindow::GetOnClickObservable() const
{
    return rxqt::from_signal(ui->Button, &QPushButton::clicked);
}

rxcpp::observer<std::string> MainWindow::GetOnTextObserver() const
{
    return rxcpp::make_observer_dynamic<std::string>([&](const std::string& text)
    {
        ui->DisplayText->setText(QString::fromStdString(text));
    });
}


void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    const auto min_width = static_cast<int>(width()*0.15);
    const auto max_width = static_cast<int>(width()*0.3);

    for(auto* menu : {ui->LeftMenu})
    {
        menu->setMinimumWidth(min_width);
        menu->setMaximumWidth(max_width);
    }
}
