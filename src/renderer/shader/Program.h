#pragma once

#include <iostream>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

#include "Uniform.h"
#include "../../core/Typedef.h"

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

        Program(Program&& other) noexcept : m_Uniforms{std::move(other.m_Uniforms)}, m_Id{other.m_Id} {
            other.m_Id = {};
        }

        Program& operator=(Program&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_Uniforms = std::move(other.m_Uniforms);
            m_Id = other.m_Id;
            other.m_Id = {};
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

        static void setUniform(int32_t location, int val);

        static void setUniform(int32_t location, float val);

        static void setUniform(int32_t location, const glm::vec4& val);

        static void setUniform(int32_t location, const glm::mat4& val);

        template<typename... T>
        void setUniform(const std::string& name, T... val) const {
            Program::setUniform(getUniformLocation(name), val...);
        }

    private:
        static constexpr char s_CreationFailStr[] = "Failed to create shader program.";

        [[nodiscard]] bool createProgram();

        [[nodiscard]] bool linkProgram() const;

        void attachShader(const Source& source);

        bool locateUniforms();

        std::vector<Uniform> m_Uniforms;
        Id m_Id{};
    };

    template<typename... Args>
    Program::Program(Args&... shaders) {
        if (!createProgram()) {
            return;
        }

        if (const bool success = ((shaders.compile(), shaders.isCompiled()) && ...); !success) {
            std::cerr << s_CreationFailStr << '\n';
            return;
        }

        (attachShader(shaders), ...);

        if (!linkProgram()) {
            return;
        }

        bind();

        if (!locateUniforms()) {
            return;
        }
    }
}
