#pragma once

#include"vte_window.hpp"
#include "vte_device.hpp"

namespace vte
{
    class Vtapp
    {
        public :
        Vtapp(std::string appName);
        void run();
        static constexpr u_int32_t width = 800;
        static constexpr u_int32_t height = 600;

        private :
        Vtewindow appwindow {width,height,"vt game engine"};
        std::string appname;
        VteDevice device{appname,appwindow};
    };
}