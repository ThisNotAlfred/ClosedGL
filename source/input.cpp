#include "input.hpp"

auto
Input::check_for_input() -> void
{
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

    // check for escape and exit if pressed escape
    // TODO make a proper application that takes an existing status
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        // this->exit_status = true;
        exit(EXIT_SUCCESS);
    }
}

auto
Input::get_left_ctrl_status() const -> bool
{
    return this->is_left_ctrl_down;
}

auto
Input::get_left_shift_status() const -> bool
{
    return this->is_left_shift_down;
}

auto
Input::get_left_alt_status() const -> bool
{
    return this->is_left_alt_down;
}