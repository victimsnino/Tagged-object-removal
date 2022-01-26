#include <rxqt_loop_singleton.h>
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    auto& run_loop = GetUIRunLoop();


    return a.exec();
}
