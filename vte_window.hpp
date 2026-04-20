#pragma once
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <vector>

namespace vte {
class Vtewindow {
public:
  Vtewindow(u_int32_t w, u_int32_t h, std::string name);
  ~Vtewindow();

  bool ShouldClose() { return glfwWindowShouldClose(window); }
  GLFWwindow *window = nullptr;

  static std::vector<std::function<void(int, int)>> resizeCallbacks;

private:
  void initwindow();
  u_int32_t width, height;
  std::string windowname;
  static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
};
} // namespace vte
