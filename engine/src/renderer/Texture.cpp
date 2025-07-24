#include "Texture.h"

#include <glad/glad.h>

#include "Renderer.h"
#include "stb_image.h"

Engine::Renderer::Texture::GlSource::~GlSource() {
    RENDERER_API_CALL(glDeleteTextures(1, &m_id));
}

Engine::Renderer::Texture Engine::Renderer::Texture::createGlTexture(const std::string& path) {
    stbi_set_flip_vertically_on_load(1);
    auto source = std::make_shared<GlSource>();
    int bpp{};
    void* buffer = stbi_load(path.c_str(), &source->m_size.x, &source->m_size.y, &bpp, 4);

    RENDERER_API_CALL(glGenTextures(1, &source->m_id));
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, source->m_id));

    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    RENDERER_API_CALL(
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source->m_size.x, source->m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            buffer));
    unbind();

    if (buffer != nullptr) {
        stbi_image_free(buffer);
    } else {
        LOG_ERR("Texture not found, from path: " << path << '\n');
    }

    return Texture{std::move(source)};
}

void Engine::Renderer::Texture::bind(const uint32_t slot) const {
    RENDERER_API_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, m_source->m_id));
}

void Engine::Renderer::Texture::unbind() {
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
