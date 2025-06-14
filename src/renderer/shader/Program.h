#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>

namespace Renderer {
    class Renderer;
}

namespace Renderer::Shader {
    class Parser;

    class Program {
    public:
        template<typename... Args>
        explicit Program(Args&... shaders);

        Program(const Renderer& renderer, Parser sourceParser);

        Program(const Program&) = delete;

        Program& operator=(const Program&) = delete;

        Program(Program&& other) noexcept : m_Renderer(other.m_Renderer), m_Id{other.m_Id} {
            other.m_Id = 0;
        }

        Program& operator=(Program&& other) noexcept {
            m_Id = other.m_Id;
            other.m_Id = 0;
            return *this;
        }

        explicit Program(const uint32_t id) : m_Id{id} {
        }

        ~Program();

        void use() const;

        explicit operator uint32_t() const {
            return m_Id;
        }

        [[nodiscard]] uint32_t getId() const {
            return m_Id;
        }

    private:
        const char* creationFailStr = "Failed to create shader program.";

        [[nodiscard]] bool createProgram();

        [[nodiscard]] bool linkProgram() const;

        void attachShader(uint32_t shaderId) const;

        const Renderer* m_Renderer{};
        uint32_t m_Id{};
    };

    template<typename... Args>
    Program::Program(Args&... shaders) {
        if (!createProgram()) {
            return;
        }

        if (const bool success = ((shaders.compile(), shaders.isCompiled()) && ...); !success) {
            std::cerr << creationFailStr << '\n';
            return;
        }

        (attachShader(shaders.getId()), ...);

        if (!linkProgram()) {
            return;
        }
    }
}

#endif //SHADERPROGRAM_H
