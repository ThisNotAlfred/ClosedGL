#include "mesh.hpp"

#include "tools.hpp"

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
                auto& position_accesor  = asset->accessors[position_it->second];

                const auto* normal_it = primitive.findAttribute("NROMAL");
                auto& normal_accesor  = asset->accessors[position_it->second];

                if (!position_accesor.bufferViewIndex.has_value()) {
                    continue;
                }

                if (!normal_accesor.bufferViewIndex.has_value()) {
                    continue;
                }

                gl::glCreateBuffers(1, &this->vertex_buffer);
                gl::glNamedBufferData(this->vertex_buffer, position_accesor.count * sizeof(Vertex), nullptr,
                                      gl::GLenum::GL_STATIC_DRAW);

                auto* vertices =
                    static_cast<Vertex*>(gl::glMapNamedBuffer(this->vertex_buffer, gl::GLenum::GL_WRITE_ONLY));

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), position_accesor,
                    [&](const fastgltf::math::fvec3& pos, std::size_t idx) { vertices[idx].position = pos; });

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), normal_accesor, [&](const fastgltf::math::fvec3& norm, std::size_t idx) {
                        vertices[idx].normal = norm;
                        vertices[idx].uv     = fastgltf::math::fvec2();
                    });

                gl::glUnmapNamedBuffer(this->vertex_buffer);
            }

            // TODO: textures
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
    gl::glBindVertexArray(this->vertex_array);
    gl::glDrawElements(gl::GL_TRIANGLES, this->indices_count, gl::GL_UNSIGNED_INT, this->indices.data());
}

auto
compile_shader(std::filesystem::path& path, gl::GLenum shader_type) -> gl::GLuint
{
    auto content                         = read_file_binary(path);
    std::array<char*, 1> shader_contents = { content.data() };
    std::array<int, 1> shader_size       = { content.size() };

    auto shader = gl::glCreateShader(shader_type);

    gl::glShaderSource(shader, shader_contents.size(), shader_contents.data(), shader_size.data());

    gl::glCompileShader(shader);

    auto is_compiled = gl::GL_FALSE;
    gl::glGetShaderiv(shader, gl::GLenum::GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled == gl::GL_FALSE) {
        auto max_length = 0;
        gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &max_length);

        std::vector<char> infolog(max_length);
        gl::glGetShaderInfoLog(shader, max_length, &max_length, infolog.data());

        gl::glDeleteShader(shader);

        return -1;
    }

    return shader;
}
