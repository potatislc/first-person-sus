#include "Renderer.h"

#include <ios>
#include <iostream>
#include <glad/glad.h>

GlRenderer Renderer::createGlRenderer(const Window& window) {
    return GlRenderer{window};
}

GlRenderer::GlRenderer(const Window& window) : m_Context(window.createGlContext()) {
    if (m_Context == nullptr) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << '\n';
        return;
    }

    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0) {
        std::cerr << "Failed to initialize GLAD\n";
        m_GlLoaderInitialized = false;
        return;
    }

    std::cout << "GL Version: " << RENDERER_API_CALL_RETURN(*this, glGetString(GL_VERSION)) << '\n';
    std::cout << "GLSL Version: " << RENDERER_API_CALL_RETURN(*this, glGetString(GL_SHADING_LANGUAGE_VERSION)) << '\n';
}

void GlRenderer::clearErrors() const {
    while (glGetError() != GL_NO_ERROR) {
    }
}

bool GlRenderer::logErrors(const char* functionName, const char* fileName,
                           const size_t line) const {
    auto emptyLog = true;
    while (const auto err = glGetError()) {
        std::cerr << "OpenGL error: (0x" << std::hex << err << std::dec << ")" << " from function: "
                << functionName << " inside file: " << fileName << "at line: " << line << '\n';
        emptyLog = false;
    }

    return emptyLog;
}

void GlRenderer::swapWindow(const Window& window) const {
    SDL_GL_SwapWindow(window.get());
}
