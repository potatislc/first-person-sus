#include "Vertex.h"

#include <glad/glad.h>

#include "../Renderer.h"

template<>
void Renderer::Buffer::Vertex::Layout::push<float>(uint32_t count) {
    m_elements.emplace_back(GL_FLOAT, count, false);
    m_stride += Element::sizeOfType(GL_FLOAT) * count;
}

template<>
void Renderer::Buffer::Vertex::Layout::push<uint32_t>(uint32_t count) {
    m_elements.emplace_back(GL_UNSIGNED_INT, count, false);
    m_stride += Element::sizeOfType(GL_UNSIGNED_INT) * count;
}

template<>
void Renderer::Buffer::Vertex::Layout::push<uint8_t>(uint32_t count) {
    m_elements.emplace_back(GL_UNSIGNED_BYTE, count, true);
    m_stride += Element::sizeOfType(GL_UNSIGNED_BYTE) * count;
}

size_t Renderer::Buffer::Vertex::Layout::Element::sizeOfType(const uint32_t type) {
    switch (type) {
        case GL_FLOAT: return sizeof(GLfloat);
        case GL_UNSIGNED_INT: return sizeof(GLuint);
        case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
        default: return 0;
    }
}

Renderer::Buffer::Vertex::Vertex(const void* data, const uint32_t size) {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
    bind();
    RENDERER_API_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

Renderer::Buffer::Vertex::~Vertex() {
    RENDERER_API_CALL(glDeleteBuffers(1, &m_id));
}

void Renderer::Buffer::Vertex::bind() const {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_id));
}

void Renderer::Buffer::Vertex::unbind() {
    RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
