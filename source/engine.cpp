#include "engine.hpp"

Engine::Engine(GLFWwindow* window)
{
    // getting size of the current window
    glfwGetWindowSize(window, &this->width, &this->height);

    // initilizing default values
    this->user_interface = UI(window);
    this->camera         = Camera(10, 3, 0.3F);
}

