#pragma once

#include <string>
#include <SDL3/SDL.h>

namespace Engine {
    class Window {
    public:
        Window(std::string name, SDL_Window* window) : m_name{std::move(name)}, m_window{window} {
        }

        Window() = default;

        Window(const Window&) = delete;

        Window& operator=(const Window&) = delete;

        Window(Window&& other) noexcept : m_name{std::move(other.m_name)}, m_window{other.m_window} {
            other.m_window = {};
        }

        Window& operator=(Window&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_name = std::move(other.m_name);
            m_window = other.m_window;
            other.m_window = {};

            return *this;
        }

        void destroy() {
            if (m_window != nullptr) {
                SDL_DestroyWindow(m_window);
                m_window = {};
            }
        }

        ~Window() {
            destroy();
        }

        [[nodiscard]] bool isValid() const {
            return m_window != nullptr;
        }

        [[nodiscard]] SDL_Window* get() const {
            return m_window;
        }

    private:
        std::string m_name;
        SDL_Window* m_window{};
    };
}
