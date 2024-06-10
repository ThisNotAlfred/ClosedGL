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
    fastgltf::math::fvec2 texcoord;
};

struct Scene {
    fastgltf::math::fvec3 translation;
    fastgltf::math::fvec3 scale;
    fastgltf::math::fquat rotation;
    
    gl::GLuint vertex_array;
    std::vector<std::uint32_t> index_array;
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

    [[nodiscard]] auto get_scenes() -> std::vector<Scene>;
    [[nodiscard]] auto get_lights() -> std::vector<std::variant<PointLight>>;

        private:
    std::filesystem::path path;

    std::vector<Scene> meshes;
    std::vector<fastgltf::Texture> textures;
    std::vector<std::variant<PointLight>> lights;
};

auto compile_shader(std::filesystem::path& path, gl::GLenum shader_type) -> gl::GLuint;
