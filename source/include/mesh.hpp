#pragma once

#include "closed_gl.hpp"

#include "fastgltf/tools.hpp"
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <filesystem>

struct Vertex {
    fastgltf::math::fvec3 position;
    fastgltf::math::fvec3 normal;
    fastgltf::math::fvec2 uv;
};

class Mesh
{
        public:
    Mesh(std::filesystem::path& path) : path(path) {};
    ~Mesh() = default;

    [[nodiscard]] auto create_buffers() -> bool;

    // TODO implementing other draw types .e.g instancing
    auto draw() const -> void;
    auto destroy() -> void;

        private:
    std::filesystem::path path;

    int indices_count;

    gl::GLuint vertex_buffer;
    gl::GLuint index_buffer;
};