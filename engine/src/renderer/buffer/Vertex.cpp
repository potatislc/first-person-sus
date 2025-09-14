#include "Vertex.h"

#include <glad/glad.h>

#include <utility>

#include "renderer/Renderer.h"

Engine::Renderer::Buffer::BufferData Engine::Renderer::Buffer::Vertex::layoutInterleave(
    const Layout& layout, const std::vector<BufferData>& dataBatch) {
    ASSERT(layout.getAttributes().size() == dataBatch.size());

    BufferData combinedBuffer;

    size_t minElements{std::numeric_limits<size_t>::max()};
    for (size_t i = 0; i < dataBatch.size(); i++) {
        const size_t attrSize = Shader::dataTypeSize(layout.getAttributes()[i].dataType);
        ASSERT(attrSize > 0);
        ASSERT(dataBatch[i].size() % attrSize == 0); // Do they share the same alignment?
        minElements = std::min(minElements, dataBatch[i].size() / attrSize);
    }

    combinedBuffer.resize(minElements * layout.getStride());

    for (ptrdiff_t i = 0; std::cmp_less(i, minElements); i++) {
        ptrdiff_t offsetInStride = 0;
        for (size_t j = 0; j < layout.getAttributes().size(); j++) {
            const auto attributeSize = Shader::dataTypeSize(layout.getAttributes()[j].dataType);
            std::copy_n(dataBatch[j].begin() + i * attributeSize,
                        attributeSize,
                        combinedBuffer.begin() + i * static_cast<ptrdiff_t>(layout.getStride()) +
                        offsetInStride);
            offsetInStride += attributeSize;
        }
    }

    return combinedBuffer;
}

Engine::Renderer::Buffer::Vertex::Vertex(Layout layout, const void* data, const uint32_t size) : m_layout{
    std::move(layout)
} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

Engine::Renderer::Buffer::Vertex::Vertex(Layout layout, const BufferData& bufferData) : m_layout{std::move(layout)} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, bufferData.size(), bufferData.data(), GL_STATIC_DRAW));
}

Engine::Renderer::Buffer::Vertex::~Vertex() {
    RENDERER_API_CALL(glDeleteBuffers(1, &m_id));
}

void Engine::Renderer::Buffer::Vertex::bind() const {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
}

void Engine::Renderer::Buffer::Vertex::unbind() {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void Engine::Renderer::Buffer::Vertex::update(const void* vertexData, const uint32_t vertexCount) const {
    bind();
    RENDERER_API_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * m_layout.getStride(), vertexData));
}
