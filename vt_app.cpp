#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include "vt_app.hpp"
namespace vte {
Vtapp::Vtapp(
    std::string appName) // : appwindow(width,height,appName + "window")
{
  appname = appName;
}

Vtapp::~Vtapp() {
  commandPool.cleanCommandPools();
  graphicsPP.cleanPP();
  device.cleanDevice();
}

void Vtapp::run() {
  while (!appwindow.ShouldClose()) {
    glfwPollEvents();
    commandPool.drawFrame();
  }

  vkDeviceWaitIdle(device.getVkDevice());
}
} // namespace vte
