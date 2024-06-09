#include "input.hpp"

auto
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}