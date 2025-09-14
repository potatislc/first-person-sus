#include "VertexArray.h"

#include <numeric>
#include <glad/glad.h>

#include "Renderer.h"
#include "buffer/Vertex.h"
#include "shader/Source.h"

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

void Engine::Renderer::VertexArray::setInstanceBuffer(Buffer::Vertex instanceBuffer) {
    m_instanceBuffer = std::move(instanceBuffer);
    const uint32_t attributeStart{static_cast<uint32_t>(m_vertexBuffer.getLayout().getAttributes().size())};
    attachInstanceBuffer(attributeStart);
}

void Engine::Renderer::VertexArray::attachIndexBuffer(const Buffer::IndexData& indexData) const {
    bind();
    RENDERER_API_CALL(
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(Buffer::IndexData::value_type), indexData.data()
            , GL_STATIC_DRAW
        ));
}

namespace {
    using Engine::Renderer::Renderer;

    // Template based on if we are passing an instance buffer or vertex buffer, both buffers sadly and gladly have the same type
    template<bool Instanced>
    void defineBufferAttributes(const Engine::Renderer::Buffer::Vertex& buffer, const uint32_t attributeStart) {
        const auto& vertexLayout = buffer.getLayout();
        const auto& elements = vertexLayout.getAttributes();
        size_t attributeIndex{attributeStart};
        size_t attributeOffset{};
        for (const auto& [type, normalized]: elements) {
            RENDERER_API_CALL(glEnableVertexAttribArray(attributeIndex));
            RENDERER_API_CALL(
                glVertexAttribPointer(attributeIndex, Engine::Renderer::Shader::componentCount(type), Engine::Renderer::
                    Shader::toGlDataType(type),
                    normalized, vertexLayout.getStride(),
                    reinterpret_cast<const void*>(
                        attributeOffset)
                ));

            if constexpr (Instanced) {
                RENDERER_API_CALL(glVertexAttribDivisor(attributeIndex, 1));
            }

            attributeIndex++;
            attributeOffset += Engine::Renderer::Shader::dataTypeSize(type);
        }
    }
}

void Engine::Renderer::VertexArray::attachVertexBuffer(const uint32_t attributeStart) const {
    bind();
    m_vertexBuffer.bind();
    defineBufferAttributes<false>(m_vertexBuffer, attributeStart);
}

void Engine::Renderer::VertexArray::attachInstanceBuffer(const uint32_t attributeStart) const {
    bind();
    m_instanceBuffer->bind();
    defineBufferAttributes<true>(*m_instanceBuffer, attributeStart);
}

void Engine::Renderer::VertexArray::bind() const {
    // Unnecessary to rebind if it is the same
    static Id boundId{};
    if (boundId == m_id) {
        return;
    }

    boundId = m_id;

    RENDERER_API_CALL(glBindVertexArray(m_id));
    m_indexBuffer.bind();
}

void Engine::Renderer::VertexArray::unbind() {
    RENDERER_API_CALL(glBindVertexArray(0));
}
