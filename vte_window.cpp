#include "vte_window.hpp"

namespace vte {
Vtewindow::Vtewindow(u_int32_t w, u_int32_t h, std::string name) {
  width = w;
  height = h;
  windowname = name;
  initwindow();
}

void Vtewindow::initwindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window =
      glfwCreateWindow(width, height, windowname.c_str(), nullptr, nullptr);
}

Vtewindow::~Vtewindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
} // namespace vte
