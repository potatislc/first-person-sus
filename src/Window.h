#ifndef WINDOW_H
#define WINDOW_H
#include <string>
#include <SDL3/SDL_video.h>


class Window {
public:
    static Window createGlWindow(const std::string& name, int width, int height);

    Window() = default;

    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;

    Window& operator=(Window&&) = delete;

    ~Window();

    [[nodiscard]] auto createGlContext() const {
        return SDL_GL_CreateContext(m_Window);
    }

    [[nodiscard]] bool isValid() const {
        return m_Window != nullptr;
    }

    SDL_Window* get() const;

private:
    explicit Window(std::string name, SDL_Window* window);

    std::string m_Name;
    SDL_Window* m_Window{};
};


#endif //WINDOW_H
