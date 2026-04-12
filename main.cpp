#include <iostream>
#include <string>
#include "vt_app.hpp"
#include <GLFW/glfw3.h>
int main()
{
    vte::Vtapp myApp("VT GameEngine");

    try
    {
        myApp.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    // if (!glfwInit()) return -1;

    // // 1. DO NOT set GLFW_NO_API (Default is OpenGL)
    // // 2. Create the window
    // GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Test Window", NULL, NULL);
    // if (!window) {
    //     glfwTerminate();
    //     return -1;
    // }

    // // 3. Make the OpenGL context current
    // glfwMakeContextCurrent(window);

    // while (!glfwWindowShouldClose(window)) {
    //     // 4. Clear the screen to a nice "CachyOS Green" or Blue
    //     // This proves the GPU is actually drawing to the window
    //     glClearColor(0.1f, 0.4f, 0.1f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT);

    //     // 5. Swap buffers (This maps the window to the screen)
    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }

    // glfwTerminate();
    // return 0;
}
