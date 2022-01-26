#include "rxqt_loop_singleton.h"

#include <rxqt_run_loop.hpp>

static rxqt::run_loop* s_run_loop{};

void SetUIRunLoop(rxqt::run_loop& run_loop)
{
    s_run_loop = &run_loop;
}

rxcpp::observe_on_one_worker ObserveOnUIRunLoop()
{
    if (!s_run_loop)
        throw std::logic_error{"You need to call SetUIRunLoop before usage ObserveOnUIRunLoop"};

    return s_run_loop->observe_on_run_loop();
}
