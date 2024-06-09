#pragma once

#include "closed_gl.hpp"

#include "camera.hpp"
#include "tools.hpp"
#include "user_interface.hpp"

class Engine
{
        public:
    Engine(GLFWwindow* window);
    ~Engine() = default;

    auto frame() -> void;

        private:
    GLFWwindow* window;
    int width;
    int height;

    UI user_interface;
    Camera camera;

    // environment booleans
    bool run = true;
    float delta_time = 0.0F;
};