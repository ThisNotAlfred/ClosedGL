#pragma once

#include "closed_gl.hpp"

#include "camera.hpp"
#include "mesh.hpp"
#include "tools.hpp"
#include "user_interface.hpp"

class Engine
{
        public:
    Engine(GLFWwindow* window);
    ~Engine() = default;

    auto init() -> void;
    auto frame() -> void;

        private:
    GLFWwindow* window;
    int width;
    int height;

    UI user_interface;
    Camera camera;
    // TODO use a better structur rather than a simple vector for handling mehses and shaders
    std::vector<Mesh> meshes;
    std::vector<gl::GLuint> shaders;

    // environment booleans
    float delta_time = 0.0F;
};