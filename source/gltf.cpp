#include "gltf.hpp"

#include "tools.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <print>

auto
Gltf::load_nodes() -> bool
{
    auto fastgltf_extensions = fastgltf::Extensions::KHR_materials_transmission |
                               fastgltf::Extensions::KHR_materials_variants |
                               fastgltf::Extensions::KHR_materials_volume | fastgltf::Extensions::KHR_lights_punctual;

    fastgltf::Parser parser(fastgltf_extensions);

    auto data = fastgltf::GltfDataBuffer::FromPath(this->path);
    if (data.error() != fastgltf::Error::None) {
        return false;
    }

    auto fastgltf_options = fastgltf::Options::DecomposeNodeMatrices;

    auto asset = parser.loadGltf(data.get(), path.parent_path(), fastgltf_options);
    if (auto error = asset.error(); error != fastgltf::Error::None) {
        return false;
    }

    // gathering data for all the nodes
    std::vector<std::size_t> node_indices;
    for (const auto& scene : asset->scenes) {
        node_indices.insert(node_indices.end(), scene.nodeIndices.begin(), scene.nodeIndices.end());
    }

    std::vector<std::size_t> children_indices;
    for (const auto& node : asset->nodes) {
        children_indices.insert(children_indices.end(), node.children.begin(), node.children.end());
    }

    // connect all the nodes in one vector
    node_indices.insert(node_indices.end(), children_indices.begin(), children_indices.end());

    // loading the nodes
    for (const auto& node_index : node_indices) {
        const auto& node = asset->nodes[node_index];

        if (node.lightIndex.has_value()) {
            // getting the rotation, scale, and translation
            assert(std::holds_alternative<fastgltf::TRS>(node.transform));
            const auto& translation = std::get<fastgltf::TRS>(node.transform).translation;
            const auto& scale       = std::get<fastgltf::TRS>(node.transform).scale;
            const auto& rotation    = std::get<fastgltf::TRS>(node.transform).rotation;

            const auto& intensity = asset->lights[node.lightIndex.value()].intensity;
            const auto& color     = asset->lights[node.lightIndex.value()].color;
            this->lights.push_back(PointLight { translation, scale, rotation, color, intensity });
        }

        if (node.meshIndex.has_value()) {
        }
    }

    // loading all the textures
    for (const auto& texture : asset->textures) {
        this->textures.push_back(texture);
    }

#ifdef _DEBUG
    assert(fastgltf::validate(asset.get()) == fastgltf::Error::None);
#endif

    return true;
}

auto
Gltf::load_meshes() -> Vertex
{
}

auto
Gltf::load_lights() -> std::vector<std::variant<PointLight>>
{
    std::vector<std::variant<PointLight>> lights;

    for (const auto& light : this->lights) {
    }

    return lights;
}

auto
compile_shader(std::filesystem::path& path, gl::GLenum shader_type) -> gl::GLuint
{
    auto content                         = read_file_binary(path);
    std::array<char*, 1> shader_contents = { content.data() };
    std::array<int, 1> shader_size       = { static_cast<int>(content.size()) };

    auto shader = gl::glCreateShader(shader_type);
    gl::glShaderSource(shader, 1, shader_contents.data(), shader_size.data());

    gl::glCompileShader(shader);

    gl::GLboolean is_compiled;
    gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == gl::GL_FALSE) {
        auto max_length = 1024;
        gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &max_length);

        std::vector<char> error_log(max_length);
        gl::glGetShaderInfoLog(shader, max_length, &max_length, error_log.data());

        std::string_view error(error_log);
        std::print(stderr, "{}", error);

        gl::glDeleteShader(shader);

        exit(EXIT_FAILURE);
    }

    return shader;
}
