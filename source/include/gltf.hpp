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

struct Mesh {
    fastgltf::math::fvec3 translation;
    fastgltf::math::fvec3 scale;
    fastgltf::math::fquat rotation;
    
    gl::GLuint vertex_array;
    gl::GLuint index_array;
};

struct PointLight {
    fastgltf::math::fvec3 translation;
    fastgltf::math::fvec3 scale;
    fastgltf::math::fquat rotation;
    
    fastgltf::math::fvec3 color;
    fastgltf::num intensity;
};

class Gltf
{
        public:
    Gltf(std::filesystem::path& path) : path(path) {};
    ~Gltf() = default;

    [[nodiscard]] auto load_nodes() -> bool;

    [[nodiscard]] auto load_meshes() -> Vertex;
    [[nodiscard]] auto load_lights() -> std::vector<std::variant<PointLight>>;

        private:
    std::filesystem::path path;

    std::vector<std::variant<PointLight>> lights;
    std::vector<Mesh> meshes;
    std::vector<fastgltf::Texture> textures;
};

auto compile_shader(std::filesystem::path& path, gl::GLenum shader_type) -> gl::GLuint;
