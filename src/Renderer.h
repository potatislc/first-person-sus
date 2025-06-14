#ifndef RENDERER_H
#define RENDERER_H
#include <cassert>
#include <iostream>
#include <utility>
#include <SDL3/SDL.h>

#include "Window.h"

class GlRenderer;

class Renderer {
public:
    static GlRenderer createGlRenderer(const Window& window);

#ifndef NDEBUG
    template<typename Func>
    void apiCall(Func&& func, const char* code, const char* file, size_t line) const;

    template<typename Func>
    [[nodiscard]] auto apiCallReturn(Func&& func, const char* code, const char* file,
                                     size_t line) const -> decltype(func());
#endif

    Renderer() = default;

    Renderer(const Renderer&) = delete;

    Renderer& operator=(const Renderer&) = delete;

    // Moves should be legal eventually. To be able to switch apis on the fly.
    Renderer(Renderer&&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    virtual ~Renderer() = default;

    virtual void clearErrors() const = 0;

    [[nodiscard]] virtual bool logErrors(const char* functionName, const char* fileName, size_t line) const = 0;

    virtual bool isValid() = 0;

    virtual void swapWindow(const Window& window) const = 0;
};

#ifdef NDEBUG

#define RENDERER_API_CALL(renderer, x) (x)
#define RENDERER_API_CALL_RETURN(renderer, x) (x)

#else

template<typename Func>
void Renderer::apiCall(Func&& func, const char* code, const char* file, const size_t line) const {
    clearErrors();
    std::forward<Func>(func)();
    assert(logErrors(code, file, line));
}

template<typename Func>
auto Renderer::apiCallReturn(Func&& func, const char* code, const char* file,
                             const size_t line) const -> decltype(func()) {
    clearErrors();
    auto result = std::forward<Func>(func)();
    assert(logErrors(code, file, line));
    return result;
}

#define RENDERER_API_CALL(renderer, x) (renderer).apiCall([&] { x; }, #x, __FILE__, __LINE__)
#define RENDERER_API_CALL_RETURN(renderer, x) (renderer).apiCallReturn([&] { return (x); }, #x, __FILE__, __LINE__)

#endif

class GlRenderer final : public Renderer {
public:
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

#endif //RENDERER_H
