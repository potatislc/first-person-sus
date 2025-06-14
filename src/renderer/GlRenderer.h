#ifndef GLRENDERER_H
#define GLRENDERER_H

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

    private:
        SDL_GLContext m_Context{};
        bool m_GlLoaderInitialized{};
    };
}

#endif //GLRENDERER_H
