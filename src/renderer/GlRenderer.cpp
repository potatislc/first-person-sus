#include "GlRenderer.h"

#include <iostream>
#include <glad/glad.h>
#include <SDL3/SDL_init.h>

#include "VertexArray.h"
#include "buffer/Index.h"
#include "shader/Program.h"
#include "../core/Window.h"

Window Renderer::GlRenderer::createWindow(const std::string& name, const int width, const int height) {
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

Renderer::GlRenderer::GlRenderer(const Window& window) : m_context(SDL_GL_CreateContext(window.get())) {
    if (m_context == nullptr) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << '\n';
        return;
    }

    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0) {
        std::cerr << "Failed to initialize GLAD\n";
        m_glLoaderInitialized = false;
        return;
    }

    s_ActiveRenderer = this;

    std::cout << "GL Version: " << RENDERER_API_CALL_RETURN(glGetString(GL_VERSION)) << '\n';
    std::cout << "GLSL Version: " << RENDERER_API_CALL_RETURN(glGetString(GL_SHADING_LANGUAGE_VERSION)) << '\n';

    // Temporary blend mode set
    RENDERER_API_CALL(glEnable(GL_BLEND));
    RENDERER_API_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void Renderer::GlRenderer::clearErrors() const {
    while (glGetError() != GL_NO_ERROR) {
    }
}

bool Renderer::GlRenderer::logErrors(const char* functionName, const char* fileName,
                                     const size_t line) const {
    auto emptyLog = true;
    while (const auto err = glGetError()) {
        std::cerr << "OpenGL error: " << err << ")" << " from function: "
                << functionName << " inside file: " << fileName << "at line: " << line << '\n';
        emptyLog = false;
    }

    return emptyLog;
}

void* Renderer::GlRenderer::getContext() const {
    return m_context;
}

void Renderer::GlRenderer::swapWindow(const Window& window) const {
    SDL_GL_SwapWindow(window.get());
}

void Renderer::GlRenderer::clear(const glm::vec4 color) const {
    RENDERER_API_CALL(glClearColor(color.r, color.g, color.b, color.a));
    RENDERER_API_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::GlRenderer::draw(const VertexArray& vertexArray, const Buffer::Index& indexBuffer,
                                const Shader::Program& shaderProgram) const {
    vertexArray.bind();
    indexBuffer.bind();
    shaderProgram.bind();
    RENDERER_API_CALL(glDrawElements(GL_TRIANGLES, indexBuffer.getCount(), GL_UNSIGNED_INT, nullptr));
}
