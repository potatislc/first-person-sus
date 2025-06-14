#include "Vertex.h"

#include <glad/glad.h>

#include "../Renderer.h"

Renderer::Buffer::Vertex::Vertex(const Renderer& renderer, const void* data, const uint32_t size) : m_Renderer{
    &renderer
} {
    RENDERER_API_CALL(renderer, glGenBuffers(1, &m_BufferId));
    bind();
    RENDERER_API_CALL(renderer,
                      glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

Renderer::Buffer::Vertex::~Vertex() {
    unbind();
    RENDERER_API_CALL(*m_Renderer, glDeleteBuffers(1, &m_BufferId));
}

void Renderer::Buffer::Vertex::bind() const {
    RENDERER_API_CALL(*m_Renderer, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId));
}

void Renderer::Buffer::Vertex::unbind() const {
    RENDERER_API_CALL(*m_Renderer, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
