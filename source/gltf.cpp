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

    auto fastgltf_options = fastgltf::Options::DecomposeNodeMatrices | fastgltf::Options::GenerateMeshIndices;

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
            const auto& primtives = asset->meshes[node.meshIndex.value()].primitives;

            for (const auto& primitive : primtives) {
                auto* position          = primitive.findAttribute("POSITION");
                auto& position_accessor = asset->accessors[position->second];
                // every gltf mesh has POSITION
                assert(position_accessor.bufferViewIndex.has_value());

                gl::GLuint vertex_buffer = 0;
                gl::glCreateBuffers(1, &vertex_buffer);

                gl::glNamedBufferData(vertex_buffer, position_accessor.count * sizeof(Vertex), nullptr,
                                      gl::GLenum::GL_STATIC_DRAW);

                auto* vertices = static_cast<Vertex*>(gl::glMapNamedBuffer(vertex_buffer, gl::GL_WRITE_ONLY));

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), position_accessor,
                    [&](const fastgltf::math::fvec3& pos, std::size_t idx) { vertices[idx].position = pos; });

                auto normal           = primitive.findAttribute("NORMAL");
                auto& normal_accessor = asset->accessors[normal->second];
                if (normal_accessor.bufferViewIndex.has_value()) {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                        asset.get(), normal_accessor,
                        [&](const fastgltf::math::fvec3& nor, std::size_t idx) { vertices[idx].normal = nor; });
                }

                auto tangent           = primitive.findAttribute("TANGENT");
                auto& tangent_accessor = asset->accessors[tangent->second];
                if (tangent_accessor.bufferViewIndex.has_value()) {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                        asset.get(), tangent_accessor,
                        [&](const fastgltf::math::fvec4& tan, std::size_t idx) { vertices[idx].tangent = tan; });
                }

                auto texcoord           = primitive.findAttribute("TEXCOORD_0");
                auto& texcoord_accessor = asset->accessors[texcoord->second];
                if (texcoord_accessor.bufferViewIndex.has_value()) {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                        asset.get(), texcoord_accessor,
                        [&](const fastgltf::math::fvec2& tex, std::size_t idx) { vertices[idx].texcoord = tex; });
                }

                gl::glUnmapNamedBuffer(vertex_buffer);

                // settning up vertex array
                gl::GLuint vertex_array;
                gl::glCreateVertexArrays(1, &vertex_array);

                gl::glVertexArrayVertexBuffer(vertex_array, 0, vertex_buffer, 0, sizeof(Vertex));

                gl::glEnableVertexArrayAttrib(vertex_array, 0);
                gl::glEnableVertexArrayAttrib(vertex_array, 1);
                gl::glEnableVertexArrayAttrib(vertex_array, 2);
                gl::glEnableVertexArrayAttrib(vertex_array, 3);

                gl::glVertexArrayAttribFormat(vertex_array, 0, 3, gl::GL_FLOAT, gl::GL_FALSE,
                                              offsetof(Vertex, position));
                gl::glVertexArrayAttribFormat(vertex_array, 1, 3, gl::GL_FLOAT, gl::GL_FALSE, offsetof(Vertex, normal));
                gl::glVertexArrayAttribFormat(vertex_array, 2, 4, gl::GL_FLOAT, gl::GL_FALSE,
                                              offsetof(Vertex, tangent));
                gl::glVertexArrayAttribFormat(vertex_array, 3, 2, gl::GL_FLOAT, gl::GL_FALSE,
                                              offsetof(Vertex, texcoord));

                gl::glVertexArrayAttribBinding(vertex_array, 0, 0);
                gl::glVertexArrayAttribBinding(vertex_array, 1, 0);
                gl::glVertexArrayAttribBinding(vertex_array, 2, 0);
                gl::glVertexArrayAttribBinding(vertex_array, 3, 0);

                // getting indicies
                std::vector<std::uint32_t> index_array;
                assert(primitive.indicesAccessor.has_value());
                auto& index_accesor = asset->accessors[primitive.indicesAccessor.value()];
                index_array.resize(index_accesor.count);

                fastgltf::iterateAccessorWithIndex<std::uint32_t>(
                    asset.get(), index_accesor,
                    [&](std::uint32_t index, std::size_t idx) { index_array[idx] = index; });

                // getting the rotation, scale, and translation
                assert(std::holds_alternative<fastgltf::TRS>(node.transform));
                const auto& translation = std::get<fastgltf::TRS>(node.transform).translation;
                const auto& scale       = std::get<fastgltf::TRS>(node.transform).scale;
                const auto& rotation    = std::get<fastgltf::TRS>(node.transform).rotation;

                // creating mesh object
                this->meshes.push_back(Scene { translation, scale, rotation, vertex_array, index_array });
            }
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
Gltf::get_scenes() -> std::vector<Scene>
{
    return this->meshes;
}

auto
Gltf::get_lights() -> std::vector<std::variant<PointLight>>
{
    return this->lights;
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
