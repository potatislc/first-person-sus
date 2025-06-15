#include "VertexArray.h"

#include "Renderer.h"
#include "buffer/Index.h"
#include "buffer/Vertex.h"

Renderer::VertexArray::VertexArray() {
    RENDERER_API_CALL(glGenVertexArrays(1, &m_Id));
}

Renderer::VertexArray::~VertexArray() {
    RENDERER_API_CALL(glDeleteVertexArrays(1, &m_Id));
}

void Renderer::VertexArray::addBuffer(const Buffer::Vertex& vertexBuffer,
                                      const Buffer::Vertex::Layout& vertexLayout) const {
    bind();
    vertexBuffer.bind();

    const auto& elements = vertexLayout.getElements();
    for (size_t i{}, offset{}; const auto& [type, count, normalized]: elements) {
        RENDERER_API_CALL(glEnableVertexAttribArray(i));
        RENDERER_API_CALL(
            glVertexAttribPointer(i, count, type, normalized, vertexLayout.getStride(), reinterpret_cast<const void*>(
                    offset)
            ));

        i++;
        offset += count * Buffer::Vertex::Layout::Element::sizeOfType(type);
    }
}

void Renderer::VertexArray::bind() const {
    RENDERER_API_CALL(glBindVertexArray(m_Id));
}

void Renderer::VertexArray::unbind() {
    RENDERER_API_CALL(glBindVertexArray(0));
}
