#pragma once

#include <iostream>
#include <vector>
#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

#include "Uniform.h"
#include "../../core/Typedef.h"
#include "../../core/Log.h"

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

        Program(Program&& other) noexcept : m_uniforms{std::move(other.m_uniforms)}, m_id{other.m_id} {
            other.m_id = {};
        }

        Program& operator=(Program&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_uniforms = std::move(other.m_uniforms);
            m_id = other.m_id;
            other.m_id = {};
            return *this;
        }

        explicit Program(const uint32_t id) : m_id{id} {
        }

        void destroy() const;

        ~Program();

        void bind() const;

        static void unbind();

        explicit operator uint32_t() const {
            return m_id;
        }

        [[nodiscard]] uint32_t getId() const {
            return m_id;
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

        std::vector<Uniform> m_uniforms;
        Id m_id{};
    };

    template<typename... Args>
    Program::Program(Args&... shaders) {
        if (!createProgram()) {
            return;
        }

        if (const bool success = ((shaders.compile(), shaders.isCompiled()) && ...); !success) {
            LOG_ERR(&s_CreationFailStr << '\n');
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
