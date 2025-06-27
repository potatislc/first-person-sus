#include "Vertex.h"

#include <glad/glad.h>

#include "../Renderer.h"

Renderer::Buffer::Vertex::Vertex(const Layout& layout, const void* data, const uint32_t size) : m_layout{layout} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

Renderer::Buffer::Vertex::Vertex(const Layout& layout, const DataBatch& dataBatch) : m_layout{layout} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();

    size_t totalSize{};
    for (const auto [_, size]: dataBatch) {
        totalSize += size;
    }

    std::vector<uint8_t> batchedDataBuffer;
    batchedDataBuffer.reserve(totalSize);
    size_t offset{};
    for (const auto& [data, size]: dataBatch) {
        batchedDataBuffer.resize(offset + size);
        std::memcpy(batchedDataBuffer.data() + offset, data, size);
        offset += size;
    }

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
