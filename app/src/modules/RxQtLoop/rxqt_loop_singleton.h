#pragma once

#include <rxcpp/operators/rx-observe_on.hpp>


namespace rxqt
{
class run_loop;
}

void SetUIRunLoop(rxqt::run_loop& run_loop);
rxcpp::observe_on_one_worker ObserveOnUIRunLoop();