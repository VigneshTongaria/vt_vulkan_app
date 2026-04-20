#include "vte_window.hpp"
#include <GLFW/glfw3.h>
#include <functional>
#include <vector>

namespace vte {
Vtewindow::Vtewindow(u_int32_t w, u_int32_t h, std::string name) {
  width = w;
  height = h;
  windowname = name;
  initwindow();
}

std::vector<std::function<void(int, int)>> Vtewindow::resizeCallbacks;

void Vtewindow::initwindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

  window = glfwCreateWindow(width, height, windowname.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
}

Vtewindow::~Vtewindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
void Vtewindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    //auto vtewindow = reinterpret_cast<Vtewindow*>(glfwGetWindowUserPointer(window));
    for(auto& cb : resizeCallbacks)
    {
        cb(width,height);
    }
}
} // namespace vte
