#include "Vertex.h"

#include <glad/glad.h>

#include <utility>

#include "../Renderer.h"

Renderer::Buffer::Vertex::Vertex(Layout layout, const void* data, const uint32_t size) : m_layout{std::move(layout)} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

Renderer::Buffer::Vertex::Vertex(Layout layout, const DataBatch& dataBatch) : m_layout{std::move(layout)} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    const auto batchedDataBuffer = batch(dataBatch);
    RENDERER_API_CALL(
        glBufferData(GL_ARRAY_BUFFER, batchedDataBuffer.size(), batchedDataBuffer.data(), GL_STATIC_DRAW));
}

Renderer::Buffer::Vertex::~Vertex() {
    RENDERER_API_CALL(glDeleteBuffers(1, &m_id));
}

void Renderer::Buffer::Vertex::bind() const {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
}

void Renderer::Buffer::Vertex::unbind() {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
