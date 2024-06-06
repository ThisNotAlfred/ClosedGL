#include "closed_gl.hpp"

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
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    gl::glDisableVertexAttribArray(0);

    glfwTerminate();
    return 0;
}