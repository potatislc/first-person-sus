#include "VertexArray.h"

#include <glad/glad.h>

#include "Renderer.h"
#include "buffer/Vertex.h"

Renderer::VertexArray::VertexArray() {
    RENDERER_API_CALL(glGenVertexArrays(1, &m_id));
}

Renderer::VertexArray::~VertexArray() {
    RENDERER_API_CALL(glDeleteVertexArrays(1, &m_id));
}

void Renderer::VertexArray::addBuffer(const Buffer::Vertex& vertexBuffer) const {
    bind();
    vertexBuffer.bind();

    const auto& vertexLayout = vertexBuffer.getLayout();
    const auto& elements = vertexLayout.getElements();
    for (size_t i{}, offset{}; const auto& [type, count, normalized]: elements) {
        RENDERER_API_CALL(glEnableVertexAttribArray(i));
        RENDERER_API_CALL(
            glVertexAttribPointer(i, count, Shader::toGlDataType(type), normalized, vertexLayout.getStride(),
                reinterpret_cast<const void*>(
                    offset)
            ));

        i++;
        offset += count * Shader::dataTypeSize(type);
    }
}

void Renderer::VertexArray::bind() const {
    RENDERER_API_CALL(glBindVertexArray(m_id));
}

void Renderer::VertexArray::unbind() {
    RENDERER_API_CALL(glBindVertexArray(0));
}
