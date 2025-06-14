#include "GLRenderer.h"

#include <ios>
#include <iostream>
#include <glad/glad.h>

Window Renderer::GLRenderer::createWindow(const std::string& name, const int width, const int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
        SDL_Quit();
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int major{};
    int minor{};
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    std::cout << "Requested OpenGL version: " << major << "." << minor << '\n';

    SDL_Window* window = SDL_CreateWindow(name.c_str(), width, height,
                                          SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << '\n';
        SDL_Quit();
    }

    return Window{name, window};
}

Renderer::GLRenderer Renderer::GLRenderer::create(const Window& window) {
    return GLRenderer{window};
}

Renderer::GLRenderer::GLRenderer(const Window& window) : m_Context(SDL_GL_CreateContext(window.get())) {
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

void Renderer::GLRenderer::clearErrors() const {
    while (glGetError() != GL_NO_ERROR) {
    }
}

bool Renderer::GLRenderer::logErrors(const char* functionName, const char* fileName,
                                     const size_t line) const {
    auto emptyLog = true;
    while (const auto err = glGetError()) {
        std::cerr << "OpenGL error: (0x" << std::hex << err << std::dec << ")" << " from function: "
                << functionName << " inside file: " << fileName << "at line: " << line << '\n';
        emptyLog = false;
    }

    return emptyLog;
}

void Renderer::GLRenderer::swapWindow(const Window& window) const {
    SDL_GL_SwapWindow(window.get());
}
