#ifndef WINDOW_H
#define WINDOW_H
#include <string>
#include <SDL3/SDL.h>

class Window {
public:
    Window(std::string name, SDL_Window* window) : m_Name{std::move(name)}, m_Window{window} {
    }

    Window() = default;

    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window&&) = delete;

    ~Window() {
        SDL_DestroyWindow(m_Window);
        SDL_Quit();
    }

    [[nodiscard]] bool isValid() const {
        return m_Window != nullptr;
    }

    [[nodiscard]] SDL_Window* get() const {
        return m_Window;
    }

private:
    std::string m_Name;
    SDL_Window* m_Window{};
};

#endif
