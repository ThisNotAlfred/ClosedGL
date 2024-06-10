#pragma once

#include "closed_gl.hpp"

#include "camera.hpp"
#include "gltf.hpp"
#include "tools.hpp"
#include "user_interface.hpp"

class Engine
{
        public:
    Engine(GLFWwindow* window);
    ~Engine() = default;

    auto init() -> void;
    auto frame() -> void;

    // drawing meshes and other components
    auto draw_meshes() const -> void;
    auto emit_lights() const -> void;

    auto check_for_input() -> void;

        private:
    GLFWwindow* window;
    int width;
    int height;

    // environment variables
    UI user_interface;
    Camera camera;

    // TODO: use a better structur rather than a simple vector for handling nodes and shaders
    std::vector<gl::GLuint> shaders;
    gl::GLuint shadow_shader;

    float delta_time = 0.0F;
    float total_time = 0.0F; 

    // input variables
    bool is_left_alt_down   = false;
    bool is_left_ctrl_down  = false;
    bool is_left_shift_down = false;
};