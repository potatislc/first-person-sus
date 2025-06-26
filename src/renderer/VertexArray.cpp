#include "VertexArray.h"

#include <glad/glad.h>

#include "Renderer.h"
#include "buffer/Vertex.h"

Renderer::VertexArray::VertexArray(Buffer::Index indexBuffer,
                                   const std::initializer_list<std::shared_ptr<Buffer::Vertex> >
                                   vertexBuffers) : m_indexBuffer{std::move(indexBuffer)} {
    RENDERER_API_CALL(glGenVertexArrays(1, &m_id));
    for (const auto& vertexBuffer: vertexBuffers) {
        addBuffer(vertexBuffer);
    }
}

Renderer::VertexArray::~VertexArray() {
    RENDERER_API_CALL(glDeleteVertexArrays(1, &m_id));
}

void Renderer::VertexArray::addBuffer(const std::shared_ptr<Buffer::Vertex>& vertexBuffer) {
    bind();
    vertexBuffer->bind();

    const auto& vertexLayout = vertexBuffer->getLayout();
    const auto& elements = vertexLayout.getElements();
    for (size_t i{}, offset{}; const auto& [type, count, normalized]: elements) {
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

    m_vertexBuffers.push_back(vertexBuffer);
}

void Renderer::VertexArray::bind() const {
    RENDERER_API_CALL(glBindVertexArray(m_id));
}

void Renderer::VertexArray::unbind() {
    RENDERER_API_CALL(glBindVertexArray(0));
}
