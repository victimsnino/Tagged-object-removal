#include <rxqt_loop_singleton.h>
#include <rxqt_run_loop.hpp>

#include <Controller.h>
#include <MainWindow.h>

#include <QApplication>



int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    auto run_loop = rxqt::run_loop{};
    SetUIRunLoop(run_loop);

    MainWindow window{};

    [[maybe_unused]]Controller controller{window};

    window.show();

    return a.exec();
}
