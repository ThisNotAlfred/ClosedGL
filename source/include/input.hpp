#pragma once

#include "closed_gl.hpp"

#include "engine.hpp"

class Input
{
        public:
    explicit Input(GLFWwindow* window, Engine& engine) : window(window), engine(engine) {};
    ~Input() = default;

    auto check_for_input() -> void;

        private:
    GLFWwindow* window;
    Engine& engine;

    bool is_left_shift_down = false;
    bool is_left_ctrl_down  = false;
    bool is_left_alt_down   = false;
};
