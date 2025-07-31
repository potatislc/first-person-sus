#include "VertexArray.h"

#include <numeric>
#include <glad/glad.h>

#include "Renderer.h"
#include "buffer/Vertex.h"

Engine::Renderer::VertexArray Engine::Renderer::VertexArray::withGeneratedDefaultIndices(Buffer::Vertex vertexBuffer,
    const uint32_t indexCount) {
    Buffer::IndexData defaultIndexData(indexCount);
    std::ranges::iota(defaultIndexData, 0);
    return VertexArray{std::move(vertexBuffer), defaultIndexData};
}

Engine::Renderer::VertexArray::VertexArray(Buffer::Vertex vertexBuffer,
                                           const Buffer::IndexData& indexData) : m_vertexBuffer{
        std::move(vertexBuffer)
    },
    m_indexBuffer{
        static_cast<uint32_t>(indexData.
            size())
    } {
    RENDERER_API_CALL(glGenVertexArrays(1, &m_id));
    attachVertexBuffer();
    attachIndexBuffer(indexData);
}

Engine::Renderer::VertexArray::~VertexArray() {
    RENDERER_API_CALL(glDeleteVertexArrays(1, &m_id));
}

void Engine::Renderer::VertexArray::attachVertexBuffer() const {
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

void Engine::Renderer::VertexArray::attachIndexBuffer(const Buffer::IndexData& indexData) const {
    m_indexBuffer.bind();
    RENDERER_API_CALL(
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(Buffer::IndexData::value_type), indexData.data()
            , GL_STATIC_DRAW
        ));
}

void Engine::Renderer::VertexArray::bind() const {
    m_indexBuffer.bind();
    RENDERER_API_CALL(glBindVertexArray(m_id));
}

void Engine::Renderer::VertexArray::unbind() {
    RENDERER_API_CALL(glBindVertexArray(0));
}
