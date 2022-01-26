#include "Controller.h"

#include <NetworkExecutor.h>

#include <ViewInterface.h>

#include <rxqt_loop_singleton.h>

using namespace std::string_literals;

Controller::Controller(ViewInterface& view)
{
    auto network_executor = INetworkExecutor::Create();

    view.GetOnClickObservable()
        .observe_on(rxcpp::observe_on_new_thread())
        .tap([](const auto&)
        {
            std::this_thread::sleep_for(std::chrono::seconds{3});
        }).map([network_executor](const auto&)
        {
            return network_executor->GetResult() +
                    std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        })
        .observe_on(ObserveOnUIRunLoop()).subscribe(view.GetOnTextObserver());
}
