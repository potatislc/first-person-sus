#include "VertexArray.h"

#include <glad/glad.h>

#include "Renderer.h"
#include "buffer/Vertex.h"

Renderer::VertexArray::VertexArray(Buffer::Vertex vertexBuffer, Buffer::Index indexBuffer) : m_vertexBuffer{
        std::move(vertexBuffer)
    }, m_indexBuffer{std::move(indexBuffer)} {
    RENDERER_API_CALL(glGenVertexArrays(1, &m_id));
    attachVertexBuffer();
}

Renderer::VertexArray::VertexArray(Buffer::Vertex vertexBuffer,
                                   const Buffer::IndexData& indexData) : m_vertexBuffer{
                                                                             std::move(vertexBuffer)
                                                                         },
                                                                         m_indexBuffer{
                                                                             static_cast<uint32_t>(indexData.
                                                                                 size())
                                                                         } {
    RENDERER_API_CALL(glGenVertexArrays(1, &m_id));
    attachVertexBuffer();

    m_indexBuffer.bind();
    RENDERER_API_CALL(
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(Buffer::IndexData::value_type), indexData.data()
            , GL_STATIC_DRAW
        ));
}

Renderer::VertexArray::~VertexArray() {
    RENDERER_API_CALL(glDeleteVertexArrays(1, &m_id));
}

void Renderer::VertexArray::attachVertexBuffer() const {
    bind();
    m_vertexBuffer.bind();

    const auto& vertexLayout = m_vertexBuffer.getLayout();
    const auto& elements = vertexLayout.getAttributes();
    size_t i{};
    size_t offset{};
    for (const auto& [type, normalized]: elements) {
        RENDERER_API_CALL(glEnableVertexAttribArray(i));
        RENDERER_API_CALL(
            glVertexAttribPointer(i, Shader::componentCount(type), Shader::toGlDataType(type),
                normalized, vertexLayout.getStride(),
                reinterpret_cast<const void*>(
                    offset)
            ));

        i++;
        offset += Shader::dataTypeSize(type);
    }
}

void Renderer::VertexArray::bind() const {
    m_indexBuffer.bind();
    RENDERER_API_CALL(glBindVertexArray(m_id));
}

void Renderer::VertexArray::unbind() {
    RENDERER_API_CALL(glBindVertexArray(0));
}
