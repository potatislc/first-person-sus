#include "Texture.h"

#include <glad/glad.h>

#include "Renderer.h"
#include "stb_image.h"

Engine::Renderer::Texture Engine::Renderer::Texture::createGlTexture(const std::string& path) {
    stbi_set_flip_vertically_on_load(1);
    Texture texture{};
    texture.m_buffer = stbi_load(path.c_str(), &texture.m_size.x, &texture.m_size.y, &texture.m_bpp, 4);

    RENDERER_API_CALL(glGenTextures(1, &texture.m_id));
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, texture.m_id));

    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    RENDERER_API_CALL(
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.m_size.x, texture.m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            texture.m_buffer));
    unbind();

    if (texture.m_buffer != nullptr) {
        stbi_image_free(texture.m_buffer);
    } else {
        LOG_ERR("Texture not found, from path: " << path << '\n');
    }

    return texture;
}

Engine::Renderer::Texture::~Texture() {
    RENDERER_API_CALL(glDeleteTextures(1, &m_id));
}

void Engine::Renderer::Texture::bind(const uint32_t slot) const {
    RENDERER_API_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
}

void Engine::Renderer::Texture::unbind() {
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
