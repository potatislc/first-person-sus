#include "Index.h"

#include <glad/glad.h>

#include "../Renderer.h"

Renderer::Buffer::Index::Index(const uint32_t* data, const uint32_t count) : m_Count{count} {
    RENDERER_API_CALL(glGenBuffers(1, &m_Id));
    bind();
    RENDERER_API_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(decltype(count)), data, GL_STATIC_DRAW));
}

Renderer::Buffer::Index::~Index() {
    unbind();
    RENDERER_API_CALL(glDeleteBuffers(1, &m_Id));
}

void Renderer::Buffer::Index::bind() const {
    RENDERER_API_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id));
}

void Renderer::Buffer::Index::unbind() {
    RENDERER_API_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
