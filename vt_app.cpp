#define GLFW_INCLUDE_VULKAN
#include "vt_app.hpp"
#include<iostream>
namespace vte
{
    Vtapp::Vtapp(std::string appName) // : appwindow(width,height,appName + "window")
    {
        appname = appName;
    }

    void Vtapp::run()
    {
        u_int32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount,nullptr);

        std::cout<<"\n Supported Extensions : "<<extensionCount << std::endl;

        while(!appwindow.ShouldClose())
        {
            glfwPollEvents();
        }
    }
} // namespace vte
