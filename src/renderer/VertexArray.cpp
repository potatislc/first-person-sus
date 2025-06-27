#include "VertexArray.h"

#include <glad/glad.h>

#include "Renderer.h"
#include "buffer/Vertex.h"

Renderer::VertexArray::VertexArray(const std::shared_ptr<Buffer::Vertex>
                                   & vertexBuffer, Buffer::Index indexBuffer) : m_vertexBuffer{vertexBuffer},
    m_indexBuffer{std::move(indexBuffer)} {
    RENDERER_API_CALL(glGenVertexArrays(1, &m_id));
    attachVertexBuffer();
}

Renderer::VertexArray::~VertexArray() {
    RENDERER_API_CALL(glDeleteVertexArrays(1, &m_id));
}

void Renderer::VertexArray::attachVertexBuffer() const {
    bind();
    m_vertexBuffer->bind();

    const auto& vertexLayout = m_vertexBuffer->getLayout();
    const auto& elements = vertexLayout.getElements();
    Buffer::Size i{}, offset{};
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
    RENDERER_API_CALL(glBindVertexArray(m_id));
}

void Renderer::VertexArray::unbind() {
    RENDERER_API_CALL(glBindVertexArray(0));
}
