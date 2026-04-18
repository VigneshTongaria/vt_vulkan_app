cmake_minimum_required(VERSION 3.20)
project(VulkanTest)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
   main.cpp
   vt_app.cpp
   vte_window.cpp
   vte_device.cpp
   vte_graphicspp.cpp
   vte_utlis.hpp
)

# Find dependencies
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(VulkanTest ${SOURCE_FILES})

# Link Vulkan and GLFW
target_link_libraries(VulkanTest PRIVATE Vulkan::Vulkan glfw)

# Optional: Add GLM headers
find_path(GLM_INCLUDE_DIR glm/glm.hpp)
target_include_directories(VulkanTest PRIVATE ${GLM_INCLUDE_DIR})

# Check if the build type is Debug
add_definitions(-DNDEBUG)
