#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>

namespace Renderer {
    class Renderer;

    namespace Shader {
        class Parser;
    }
}

namespace Renderer::Shader {
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

        explicit Program(const unsigned int id) : m_Id{id} {
        }

        ~Program();

        void use() const;

        explicit operator unsigned int() const {
            return m_Id;
        }

        [[nodiscard]] unsigned int getId() const {
            return m_Id;
        }

    private:
        const char* creationFailStr = "Failed to create shader program.";

        [[nodiscard]] bool createProgram();

        [[nodiscard]] bool linkProgram() const;

        void attachShader(unsigned int shaderId) const;

        const Renderer* m_Renderer{};
        unsigned int m_Id{};
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
