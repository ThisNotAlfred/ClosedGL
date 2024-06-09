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
    int width = 0;
    int height = 0;

    UI user_interface;
    Camera camera;
};