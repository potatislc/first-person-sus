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

    Window(Window&& other) noexcept : m_Name{std::move(other.m_Name)}, m_Window{other.m_Window} {
        other.m_Window = {};
    }

    Window& operator=(Window&& other) noexcept {
        if (&other == this) {
            return *this;
        }

        m_Name = std::move(other.m_Name);
        m_Window = other.m_Window;
        other.m_Window = {};

        return *this;
    }

    void destroy() {
        if (m_Window != nullptr) {
            SDL_DestroyWindow(m_Window);
            m_Window = {};
        }
    }

    ~Window() {
        destroy();
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
