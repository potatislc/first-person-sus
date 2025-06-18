#ifndef RENDERER_H
#define RENDERER_H
#include <cassert>
#include <iostream>
#include <utility>
#include <glm/vec4.hpp>
#include <SDL3/SDL.h>

#include "../Window.h"

namespace Renderer {
    class GlRenderer;
    class VertexArray;

    namespace Buffer {
        class Index;
    }

    namespace Shader {
        class Program;
    }

    enum class Type : uint8_t {
        NONE = 0,
        OPEN_GL = 1,
        SDL_GPU = 2
    };

    inline const char* toString(const Type e) {
        switch (e) {
            case Type::NONE: return "NONE";
            case Type::OPEN_GL: return "OPEN_GL";
            case Type::SDL_GPU: return "SDL_GPU";
            default: return "unknown";
        }
    }

    class Renderer {
    public:
        static Renderer* getActiveRenderer() {
            return s_ActiveRenderer;
        }

        template<typename Func>
        static void apiCall(const Renderer* renderer, Func&& func, const char* code, const char* file,
                            const size_t line) {
            if (renderer == nullptr) {
                std::cerr << "In Renderer::Debug::apiCall: Renderer is not set.\n";
                return;
            }

            renderer->clearErrors();
            std::forward<Func>(func)();
            assert(renderer->logErrors(code, file, line));
        }

        template<typename Func>
        static auto apiCallReturn(const Renderer* renderer, Func&& func, const char* code, const char* file,
                                  const size_t line) -> decltype(func()) {
            if (renderer == nullptr) {
                std::cerr << "In Renderer::Debug::apiCallReturn: Renderer is not set.\n";
                return decltype(func()){};
            }

            renderer->clearErrors();
            auto result = std::forward<Func>(func)();
            assert(renderer->logErrors(code, file, line));
            return result;
        }

        Renderer() = default;

        Renderer(const Renderer&) = delete;

        Renderer& operator=(const Renderer&) = delete;

        // Moves should be legal eventually. To be able to switch apis on the fly.
        Renderer(Renderer&&) = delete;

        Renderer& operator=(Renderer&&) = delete;

        virtual ~Renderer() {
            s_ActiveRenderer = nullptr;
        }

        virtual void clearErrors() const = 0;

        [[nodiscard]] virtual bool logErrors(const char* functionName, const char* fileName, size_t line) const = 0;

        virtual bool isValid() = 0;

        virtual void swapWindow(const Window& window) const = 0;

        virtual void clear(glm::vec4 color) const = 0;

        virtual void draw(const VertexArray& vertexArray, const Buffer::Index& indexBuffer,
                          const Shader::Program& shaderProgram) const = 0;

    protected:
        static inline Renderer* s_ActiveRenderer{};
    };
}

#ifdef NDEBUG

#define RENDERER_API_CALL(renderer, x) (x)
#define RENDERER_API_CALL_RETURN(renderer, x) (x)

#else

#define RENDERER_API_CALL(x) Renderer::Renderer::apiCall(Renderer::Renderer::getActiveRenderer(), [&] { x; }, #x, __FILE__, __LINE__)
#define RENDERER_API_CALL_RETURN(x) Renderer::Renderer::apiCallReturn(Renderer::Renderer::getActiveRenderer(), [&] { return (x); }, #x, __FILE__, __LINE__)

#endif

#endif
