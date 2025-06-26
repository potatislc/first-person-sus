#pragma once

#include <glm/vec4.hpp>
#include <SDL3/SDL_video.h>

#include "Renderer.h"

namespace Renderer {
    class GlRenderer final : public Renderer {
    public:
        static Core::Window createWindow(const std::string& name, int width, int height);

        explicit GlRenderer(const Core::Window& window);

        GlRenderer(const GlRenderer&) = delete;

        GlRenderer& operator=(const GlRenderer&) = delete;

        GlRenderer(GlRenderer&& other) noexcept : m_context(other.m_context) {
            other.m_context = {};
        }

        GlRenderer& operator=(GlRenderer&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_context = other.m_context;
            other.m_context = {};
            return *this;
        }

        ~GlRenderer() override {
            SDL_GL_DestroyContext(m_context);
        }

        void clearErrors() const override;

        bool logErrors(const char* functionName, const char* fileName, size_t line) const override;

        bool isValid() override {
            return m_context != nullptr && m_glLoaderInitialized;
        }

        void* getContext() const override;

        void swapWindow(const Core::Window& window) const override;

        void clear(glm::vec4 color) const override;

        void draw(const VertexArray& vertexArray, const Shader::Program& shaderProgram) const override;

    private:
        SDL_GLContext m_context{};
        bool m_glLoaderInitialized{};
    };
}
