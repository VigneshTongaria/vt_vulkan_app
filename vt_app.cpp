#define GLFW_INCLUDE_VULKAN
#include "vt_app.hpp"
#include<iostream>
namespace vte
{
    Vtapp::Vtapp(std::string appName) // : appwindow(width,height,appName + "window")
    {
        appname = appName;
    }

    Vtapp::~Vtapp()
    {
        graphicsPP.cleanPP();
        device.cleanDevice();
    }

    void Vtapp::run()
    {
        while(!appwindow.ShouldClose())
        {
            glfwPollEvents();
        }
    }
} // namespace vte
