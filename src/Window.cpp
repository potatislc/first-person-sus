#include "Window.h"

#include <iostream>
#include <utility>
#include <SDL3/SDL_init.h>

SDL_Window* Window::get() const {
    return m_Window;
}

Window::Window(std::string name, SDL_Window* window) : m_Name{std::move(name)}, m_Window{window} {
}

Window::~Window() {
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

Window Window::createGlWindow(const std::string& name, const int width, const int height) {
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
