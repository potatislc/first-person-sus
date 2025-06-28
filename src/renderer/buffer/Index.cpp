#include "Index.h"

#include <glad/glad.h>

#include "../Renderer.h"

Renderer::Buffer::DataBuffer Renderer::Buffer::Index::batch(const DataBatch& dataBatch) {
    auto batchedDataBuffer = Buffer::batch(dataBatch);

    size_t indexOffset{};
    size_t vertexOffset{};
    for (const auto& buffer: dataBatch) {
        const size_t indexCount = buffer.size() / sizeof(Count);
        size_t vertexCount{};

        for (size_t i{indexOffset}; i < indexOffset + indexCount; i++) {
            // Funny name
            Count& vertexIndex = *reinterpret_cast<Count*>(&batchedDataBuffer[i * sizeof(Count)]);
            vertexCount = (vertexIndex > vertexCount) ? vertexIndex : vertexCount;
            vertexIndex += vertexOffset;
            // std::cout << *reinterpret_cast<Count*>(&batchedDataBuffer[j * sizeof(Count)]) << '\n';
        }

        indexOffset += indexCount;
        vertexOffset += vertexCount + 1;
    }

    return batchedDataBuffer;
}

Renderer::Buffer::Index::Index(const void* data, const Size size) : m_count{
    static_cast<Count>(size / sizeof(Count))
} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

Renderer::Buffer::Index::Index(const DataBatch& dataBatch) {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));

    const auto batchedDataBuffer = batch(dataBatch);
    m_count = static_cast<Count>(batchedDataBuffer.size() / sizeof(Count));

    RENDERER_API_CALL(
        glBufferData(GL_ARRAY_BUFFER, batchedDataBuffer.size(), batchedDataBuffer.data(),
            GL_STATIC_DRAW));
}

void Renderer::Buffer::Index::destroy() {
    RENDERER_API_CALL(glDeleteBuffers(1, &m_id));
    m_id = {};
}

Renderer::Buffer::Index::~Index() {
    destroy();
}

void Renderer::Buffer::Index::bind() const {
    RENDERER_API_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
}

void Renderer::Buffer::Index::unbind() {
    RENDERER_API_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
