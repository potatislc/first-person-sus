#include "Index.h"

#include <glad/glad.h>

#include "../Renderer.h"

Engine::Renderer::Buffer::Index::Index(const uint32_t count) : m_count{count} {
    RENDERER_API_CALL(glGenBuffers(1, &m_id));
}

void Engine::Renderer::Buffer::Index::destroy() {
    RENDERER_API_CALL(glDeleteBuffers(1, &m_id));
    m_id = {};
}

Engine::Renderer::Buffer::Index::~Index() {
    destroy();
}

void Engine::Renderer::Buffer::Index::bind() const {
    RENDERER_API_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
}

void Engine::Renderer::Buffer::Index::unbind() {
    RENDERER_API_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
