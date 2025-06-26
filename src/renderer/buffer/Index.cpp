#include "Index.h"

#include <glad/glad.h>

#include "../Renderer.h"

Renderer::Buffer::Index::Index(const uint32_t* data, const uint32_t count) : m_count{count} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, count * sizeof(decltype(count)), data, GL_STATIC_DRAW));
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
