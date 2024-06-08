#include "mesh.hpp"

#include "tools.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <print>

auto
Mesh::create_buffers() -> bool
{
    fastgltf::Parser parser;

    auto data = fastgltf::GltfDataBuffer::FromPath(this->path);
    if (data.error() != fastgltf::Error::None) {
        return false;
    }

    auto asset = parser.loadGltf(data.get(), path.parent_path());
    if (auto error = asset.error(); error != fastgltf::Error::None) {
        return false;
    }

    for (auto& image : asset->images) {
        gl::GLuint texture;
        glCreateTextures(gl::GL_TEXTURE_2D, 1, &texture);
        std::visit(fastgltf::visitor {
                       [](auto& arg) {},
                       [&](fastgltf::sources::URI& filePath) {
                           // TODO:
                           assert(false);
                       },
                       [&](fastgltf::sources::Array& vector) { assert(false); },
                       [&](fastgltf::sources::BufferView& view) {
                           auto& buf_view = asset->bufferViews[view.bufferViewIndex];
                           auto& buffer   = asset->buffers[buf_view.bufferIndex];
                           // TODO(StaticSaga): i took this code from an example, this is doing needless copying which
                           // should be avoided
                           std::visit(
                               fastgltf::visitor { [](auto& arg) {},
                                                   [&](fastgltf::sources::Array& array) {
                                                       int width, height, channels;
                                                       unsigned char* data = stbi_load_from_memory(
                                                           array.bytes.data() + buf_view.byteOffset,
                                                           (int)buf_view.byteLength, &width, &height, &channels, 4);
                                                       int levels = 1 + (int)log2(std::max(width, height));
                                                       glTextureStorage2D(texture, levels, gl::GL_RGBA8, width, height);
                                                       glTextureSubImage2D(texture, 0, 0, 0, width, height, gl::GL_RGBA,
                                                                           gl::GL_UNSIGNED_BYTE, data);
                                                       stbi_image_free(data);
                                                   } },
                               buffer.data);
                       },
                   },
                   image.data);

        gl::glGenerateTextureMipmap(texture);
        this->textures.push_back(texture);
    }

    for (const auto& mesh : asset->meshes) {
        for (auto&& primitive : mesh.primitives) {

            // indices
            {
                if (primitive.indicesAccessor.has_value()) {
                    auto& accessor = asset->accessors[primitive.indicesAccessor.value()];
                    this->indices.resize(accessor.count);

                    fastgltf::iterateAccessorWithIndex<std::uint32_t>(
                        asset.get(), accessor,
                        [&](std::uint32_t index, std::size_t idx) { this->indices[idx] = index; });
                }

                this->indices_count = this->indices.size();
            }

            // vertices
            {
                const auto* position_it = primitive.findAttribute("POSITION");
                auto& position_accessor = asset->accessors[position_it->second];

                const auto* normal_it = primitive.findAttribute("NORMAL");
                auto& normal_accessor = asset->accessors[position_it->second];

                const auto* uv_it = primitive.findAttribute("TEXCOORD_0");
                auto& uv_accessor = asset->accessors[uv_it->second];

                assert(position_accessor.bufferViewIndex.has_value());
                assert(normal_accessor.bufferViewIndex.has_value());
                assert(uv_accessor.bufferViewIndex.has_value());

                gl::glCreateBuffers(1, &this->vertex_buffer);
                gl::glNamedBufferData(this->vertex_buffer, position_accessor.count * sizeof(Vertex), nullptr,
                                      gl::GLenum::GL_STATIC_DRAW);

                auto* vertices =
                    static_cast<Vertex*>(gl::glMapNamedBuffer(this->vertex_buffer, gl::GLenum::GL_WRITE_ONLY));

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), position_accessor,
                    [&](const fastgltf::math::fvec3& pos, std::size_t idx) { vertices[idx].position = pos; });

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), normal_accessor,
                    [&](const fastgltf::math::fvec3& norm, std::size_t idx) { vertices[idx].normal = norm; });
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                    asset.get(), uv_accessor,
                    [&](const fastgltf::math::fvec2& uv, std::size_t idx) { vertices[idx].uv = uv; });

                gl::glUnmapNamedBuffer(this->vertex_buffer);
            }
        }
    }

    gl::glCreateVertexArrays(1, &this->vertex_array);

    gl::glVertexArrayVertexBuffer(this->vertex_array, 0, this->vertex_buffer, 0, sizeof(Vertex));

    gl::glEnableVertexArrayAttrib(this->vertex_array, 0);
    gl::glEnableVertexArrayAttrib(this->vertex_array, 1);
    gl::glEnableVertexArrayAttrib(this->vertex_array, 2);

    gl::glVertexArrayAttribFormat(this->vertex_array, 0, 3, gl::GL_FLOAT, gl::GL_FALSE, offsetof(Vertex, position));
    gl::glVertexArrayAttribFormat(this->vertex_array, 1, 3, gl::GL_FLOAT, gl::GL_FALSE, offsetof(Vertex, normal));
    gl::glVertexArrayAttribFormat(this->vertex_array, 2, 2, gl::GL_FLOAT, gl::GL_FALSE, offsetof(Vertex, uv));

    gl::glVertexArrayAttribBinding(this->vertex_array, 0, 0);
    gl::glVertexArrayAttribBinding(this->vertex_array, 1, 0);
    gl::glVertexArrayAttribBinding(this->vertex_array, 2, 0);

#ifdef _DEBUG
    assert(fastgltf::validate(asset.get()) == fastgltf::Error::None);
#endif

    return true;
}

auto
Mesh::destroy() -> void
{
    gl::glDeleteBuffers(1, &this->vertex_array);
    gl::glDeleteBuffers(1, &this->vertex_buffer);
}

auto
Mesh::draw() const -> void
{
    // TODO(StaticSaga): do not hardcode this
    int unit = 0;
    gl::glBindTextureUnit(unit, this->textures[1]);
    gl::glBindVertexArray(this->vertex_array);
    gl::glUniform1i(3, unit);

    gl::glDrawElements(gl::GL_TRIANGLES, this->indices_count, gl::GL_UNSIGNED_INT, this->indices.data());
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
        gl::glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);

        std::string_view error(error_log);
        std::print(stderr, "{}", error);

        gl::glDeleteShader(shader);

        exit(EXIT_FAILURE);
    }

    return shader;
}
