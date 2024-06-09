#pragma once

#include "closed_gl.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

#include <filesystem>

struct Vertex {
    fastgltf::math::fvec3 position;
    fastgltf::math::fvec3 normal;
    fastgltf::math::fvec4 tangent;
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

    std::vector<std::uint32_t> indices;
    std::vector<gl::GLuint> textures;

    gl::GLuint vertex_array;
    gl::GLuint vertex_buffer;
};

auto compile_shader(std::filesystem::path& path, gl::GLenum shader_type) -> gl::GLuint;
