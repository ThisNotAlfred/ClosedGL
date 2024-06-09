#include "input.hpp"

auto
Input::check_for_input() -> void
{
    while (true) {
        // handling left shift
        if (glfwGetKey(this->window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            this->is_left_alt_down = true;
        }

        if (glfwGetKey(this->window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE) {
            this->is_left_alt_down = false;
        }

        // handling left ctrl
        if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            this->is_left_ctrl_down = true;
        }

        if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
            this->is_left_ctrl_down = false;
        }

        // handling left shift
        if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            this->is_left_shift_down = true;
        }

        if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
            this->is_left_shift_down = false;
        }

        // check for escape and quit if pressed escape
        // TODO make a proper application that takes an existing status
        if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            this->engine.quit();
            break;
        }
    }
}