#pragma once
#include <GLFW/glfw3.h>
#include <string>
namespace vte
{
    class Vtewindow
    {
        public:
        Vtewindow(u_int32_t w, u_int32_t h, std::string name);
        ~Vtewindow();

        bool ShouldClose()
        {
            return glfwWindowShouldClose(window);
        }
        GLFWwindow* window = nullptr;

        private :
        void initwindow();
        u_int32_t width,height;
        std::string windowname;
    };
}