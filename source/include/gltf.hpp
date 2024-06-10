#pragma once

#include "closed_gl.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>

#include <filesystem>

struct Gltf {
    fastgltf::math::fvec3 position;
    fastgltf::math::fvec3 normal;
    fastgltf::math::fvec4 tangent;
    fastgltf::math::fvec2 uv;
};

struct PointLight {
    fastgltf::math::fvec3 position;
    fastgltf::math::fvec3 color;
    fastgltf::math::fvec3 intensity;
};

class Gltf
{
        public:
    Gltf(std::filesystem::path& path) : path(path) {};
    ~Gltf() = default;

    [[nodiscard]] auto load_nodes() -> bool;

    
    
    auto destroy() -> void;

        private:
    std::filesystem::path path;

    std::vector<fastgltf::Camera> cameras;
    std::vector<fastgltf::Light> lights;
    std::vector<fastgltf::Mesh> meshes;
};

auto compile_shader(std::filesystem::path& path, gl::GLenum shader_type) -> gl::GLuint;
