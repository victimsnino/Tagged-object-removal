#pragma once

#include <rxqt_run_loop.hpp>

inline rxqt::run_loop& GetUIRunLoop()
{
    static rxqt::run_loop s_run_loop{};
    return s_run_loop;
}