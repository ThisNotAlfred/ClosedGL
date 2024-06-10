#pragma once

#include "closed_gl.hpp"

class UI
{
        public:
    explicit UI() = default;
    UI(GLFWwindow* window) : window(window) {};
    ~UI() = default;

    auto draw() -> void;
    static auto draw_debug(float delta_time, std::string_view info) -> void;

        private:
    auto top_menu() -> void;
    auto toolset() -> void;

    bool show_toolset = true;
    GLFWwindow* window {};
};