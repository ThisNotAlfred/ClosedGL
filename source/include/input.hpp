#pragma once

#include "closed_gl.hpp"

class Input
{
        public:
    explicit Input(GLFWwindow* window) : window(window) {};
    ~Input() = default;

    auto check_for_input() -> void;

    [[nodiscard]] auto get_left_ctrl_status() const -> bool;
    [[nodiscard]] auto get_left_shift_status() const -> bool;
    [[nodiscard]] auto get_left_alt_status() const -> bool;

        private:
    GLFWwindow* window;

    bool is_left_shift_down = false;
    bool is_left_ctrl_down  = false;
    bool is_left_alt_down   = false;
};
