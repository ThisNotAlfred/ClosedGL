#include "mesh.hpp"

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

                    fastgltf::iterateAccessorWithIndex<unsigned int>(
                        asset.get(), accessor,
                        [&](unsigned int index, std::size_t idx) { this->indices[idx] = index; });
                }

                this->indices_count = this->indices.size();
            }

            // vertices
            {
                const auto* position_it = primitive.findAttribute("POSITION");
                auto& position_accesor  = asset->accessors[position_it->second];

                if (!position_accesor.bufferViewIndex.has_value()) {
                    continue;
                }

                gl::glCreateBuffers(1, &this->vertex_buffer);
                gl::glNamedBufferData(this->vertex_buffer, position_accesor.count * sizeof(Vertex), nullptr,
                                      gl::GLenum::GL_STATIC_DRAW);

                auto* vertices =
                    static_cast<Vertex*>(gl::glMapNamedBuffer(this->vertex_buffer, gl::GLenum::GL_WRITE_ONLY));

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), position_accesor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
                        vertices[idx].position = pos;
                        vertices[idx].normal   = fastgltf::math::fvec3();
                        vertices[idx].uv       = fastgltf::math::fvec2();
                    });
            }

            // TODO: normals and uv coords
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
    gl::glDeleteBuffers(1, &this->index_buffer);
}

auto
Mesh::draw() const -> void
{
    gl::glBindVertexArray(this->vertex_array);
    gl::glDrawElements(gl::GL_TRIANGLES, this->indices_count, gl::GL_UNSIGNED_SHORT, this->indices.data());
    gl::glBindVertexArray(0);
}