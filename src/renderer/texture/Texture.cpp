#include "Texture.h"

#include <glad/glad.h>

#include <utility>
#include "../Renderer.h"
#include "stb_image.h"

Renderer::Texture Renderer::Texture::createGlTexture(const std::string& path) {
    stbi_set_flip_vertically_on_load(1);
    Texture texture{};
    texture.m_Buffer = stbi_load(path.c_str(), &texture.m_Size.x, &texture.m_Size.y, &texture.m_Bpp, 4);

    RENDERER_API_CALL(glGenTextures(1, &texture.m_Id));
    RENDERER_API_CALL(glBindTexture(GL_TEXTURE_2D, texture.m_Id));

    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    RENDERER_API_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    RENDERER_API_CALL(
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.m_Size.x, texture.m_Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            texture.m_Buffer));
    unbind();

    if (texture.m_Buffer != nullptr) {
        stbi_image_free(texture.m_Buffer);
    } else {
        std::cerr << "Texture not found, from path: " << path << '\n';
    }

    return texture;
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
