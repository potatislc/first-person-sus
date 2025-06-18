#include "Texture.h"

#include <glad/glad.h>

#include <utility>
#include "../Renderer.h"
#include "stb_image.h"

Renderer::Texture::Texture(const std::string& path) : m_Path{path} {
    stbi_set_flip_vertically_on_load(1);
    m_Buffer = stbi_load(path.c_str(), &m_Size.x, &m_Size.y, &m_Bpp, 4);

    RENDERER_API_CALL(glGenTextures(1, &m_Id));
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, m_Id));

    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    RENDERER_API_CALL(
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Size.x, m_Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Buffer));
    unbind();

    if (m_Buffer != nullptr) {
        stbi_image_free(m_Buffer);
    } else {
        std::cerr << "Texture not found, from path: " << path << '\n';
    }
}

Renderer::Texture::~Texture() {
    RENDERER_API_CALL(glDeleteTextures(1, &m_Id));
}

void Renderer::Texture::bind(const uint32_t slot) const {
    RENDERER_API_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, m_Id));
}

void Renderer::Texture::unbind() {
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
