#pragma once

#include <cstdint>
#include <utility>
#include <glm/vec4.hpp>

#include "core/Assert.h"

namespace Engine {
    class Window;
}

namespace Engine::Renderer {
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
        }

        std::unreachable();
    }

    class Renderer {
    public:
        static Renderer* getActiveRenderer() {
            return s_ActiveRenderer;
        }

        template<typename Func>
        static void apiCall(const Renderer* renderer, Func&& func, const char* code, const char* file,
                            const size_t line) {
            ASSERT_MSG(renderer != nullptr, "In Renderer::Debug::apiCall: Renderer is not set.\n");

            renderer->clearErrors();
            std::forward<Func>(func)();
            ASSERT(renderer->logErrors(code, file, line));
        }

        template<typename Func>
        static auto apiCallReturn(const Renderer* renderer, Func&& func, const char* code, const char* file,
                                  const size_t line) -> decltype(func()) {
            ASSERT_MSG(renderer != nullptr, "In Renderer::Debug::apiCallReturn: Renderer is not set.\n");

            renderer->clearErrors();
            auto result = std::forward<Func>(func)();
            ASSERT(renderer->logErrors(code, file, line));
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

        [[nodiscard]] virtual void* getContext() const = 0;

        virtual void swapWindow(const Window& window) const = 0;

        virtual void clear(glm::vec4 color) const = 0;

        virtual void draw(const VertexArray& vertexArray, const Shader::Program& shaderProgram) const = 0;

    protected:
        static inline Renderer* s_ActiveRenderer{};
    };
}

#ifdef NDEBUG

#define RENDERER_API_CALL(x) (x)
#define RENDERER_API_CALL_RETURN(x) (x)

#else

#define RENDERER_API_CALL(x) Renderer::Renderer::apiCall(Renderer::Renderer::getActiveRenderer(), [&] { x; }, #x, __FILE__, __LINE__)
#define RENDERER_API_CALL_RETURN(x) Renderer::Renderer::apiCallReturn(Renderer::Renderer::getActiveRenderer(), [&] { return (x); }, #x, __FILE__, __LINE__)

#endif
