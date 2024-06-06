#include "closed_gl.hpp"

#include <chrono>
#include <format>
#include <iostream>

auto
error(int errnum, const char* errmsg)
{
    std::cerr << errnum << ": " << errmsg << '\n';
}

auto
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // size of the screen
    // can implement resizing here!
}

auto
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

auto
main() -> int
{
    glfwSetErrorCallback(error);

    if (glfwInit() == 0) {
        return 1;
    }

    // setting the opengl version to 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    auto* window = glfwCreateWindow(1600, 900, "beaviewer", nullptr, nullptr);
    if (window == nullptr) {
#ifdef _DEBUG
        std::cerr << "failed to creat the window";
#endif
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    glbinding::initialize(glfwGetProcAddress, false);
    glbinding::aux::enableGetErrorCallback();

    std::cout << '\n'
              << "OpenGL Version:  " << glbinding::aux::ContextInfo::version() << '\n'
              << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor() << '\n'
              << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer() << '\n';


    while (glfwWindowShouldClose(window) == 0) {
        // setting `loop_start` to calculate `elapsed_time` later
        auto loop_start = std::chrono::steady_clock::now();

        glfwPollEvents();
        glfwSwapBuffers(window);

        // setting `time_point` and calculating `elapsed_time`
        auto loop_end     = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);

        // setting window title to current loop's `elapsed_time`
        glfwSetWindowTitle(window, std::format("frame time: {}", elapsed_time.count()).c_str());
    }

    gl::glDisableVertexAttribArray(0);

    glfwTerminate();
    return 0;
}