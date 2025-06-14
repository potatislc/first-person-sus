#include "Index.h"

#include <glad/glad.h>

#include "../Renderer.h"

Renderer::Buffer::Index::Index(const Renderer& renderer, const uint32_t* data, const uint32_t count) : m_Renderer{
    &renderer
} {
    RENDERER_API_CALL(renderer, glGenBuffers(1, &m_BufferId));
    bind();
    RENDERER_API_CALL(renderer,
                      glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW));
}

Renderer::Buffer::Index::~Index() {
    unbind();
    RENDERER_API_CALL(*m_Renderer, glDeleteBuffers(1, &m_BufferId));
}

void Renderer::Buffer::Index::bind() const {
    RENDERER_API_CALL(*m_Renderer, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId));
}

void Renderer::Buffer::Index::unbind() const {
    RENDERER_API_CALL(*m_Renderer, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
