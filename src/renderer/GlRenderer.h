#ifndef RENDERER_GLRENDERER_H
#define RENDERER_GLRENDERER_H

#include <glm/vec4.hpp>

#include "Renderer.h"

namespace Renderer {
    class GlRenderer final : public Renderer {
    public:
        static Window createWindow(const std::string& name, const int width, const int height);

        static GlRenderer create(const Window& window);

        explicit GlRenderer(const Window& window);

        ~GlRenderer() override {
            SDL_GL_DestroyContext(m_Context);
        }

        void clearErrors() const override;

        bool logErrors(const char* functionName, const char* fileName, size_t line) const override;

        bool isValid() override {
            return m_Context != nullptr && m_GlLoaderInitialized;
        }

        void swapWindow(const Window& window) const override;

        void clear(glm::vec4 color) const override;

        void draw(const VertexArray& vertexArray, const Buffer::Index& indexBuffer,
                  const Shader::Program& shaderProgram) const override;

    private:
        SDL_GLContext m_Context{};
        bool m_GlLoaderInitialized{};
    };
}

#endif
