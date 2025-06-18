#ifndef RENDERER_SHADER_PROGRAM_H
#define RENDERER_SHADER_PROGRAM_H

#include <iostream>
#include <vector>
#include <glm/vec4.hpp>

#include "Uniform.h"

namespace Renderer {
    class Renderer;
}

namespace Renderer::Shader {
    class Source;
    class Parser;

    class Program {
    public:
        template<typename... Args>
        explicit Program(Args&... shaders);

        explicit Program(Parser sourceParser);

        Program(const Program&) = delete;

        Program& operator=(const Program&) = delete;

        Program(Program&& other) noexcept : m_Id{other.m_Id} {
            other.m_Id = 0;
        }

        Program& operator=(Program&& other) noexcept {
            m_Id = other.m_Id;
            other.m_Id = 0;
            return *this;
        }

        explicit Program(const uint32_t id) : m_Id{id} {
        }

        void destroy() const;

        ~Program();

        void bind() const;

        static void unbind();

        explicit operator uint32_t() const {
            return m_Id;
        }

        [[nodiscard]] uint32_t getId() const {
            return m_Id;
        }

        [[nodiscard]] int32_t getUniformLocation(const std::string& name) const;

        void setUniform(int32_t location, const glm::vec4& vec4) const;

        template<typename... T>
        void setUniform(const std::string& name, T... val) const {
            setUniform(getUniformLocation(name), val...);
        }

    private:
        const char* creationFailStr = "Failed to create shader program.";

        [[nodiscard]] bool createProgram();

        [[nodiscard]] bool linkProgram() const;

        void attachShader(const Source& source);

        bool locateUniforms();

        uint32_t m_Id{};
        std::vector<Uniform> m_Uniforms;
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

        (attachShader(shaders), ...);

        if (!linkProgram()) {
            return;
        }

        if (!locateUniforms()) {
            return;
        }
    }
}

#endif
