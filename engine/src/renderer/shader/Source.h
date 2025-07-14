#pragma once

#include <string>
#include <vector>

#include "Uniform.h"
#include "core/Typedef.h"

namespace Engine::Renderer {
    class Renderer;
}

namespace Engine::Renderer::Shader {
    class Source {
    public:
        using Type = uint32_t;

        static constexpr Type s_shaderHeader{0};

        Source() = default;

        Source(const Type type, std::string source, std::vector<Uniform> uniforms = {}) : m_source{
                std::move(source)
            }, m_uniforms{
                std::move(uniforms)
            }, m_type{type} {
        }

        Source(const Source&) = delete;

        Source& operator=(const Source&) = delete;

        Source(Source&& other) noexcept : m_source{std::move(other.m_source)}, m_uniforms{std::move(other.m_uniforms)},
                                          m_id{other.m_id},
                                          m_type{other.m_type} {
            other.m_id = {};
        }

        Source& operator=(Source&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_source = std::move(other.m_source);
            m_uniforms = std::move(other.m_uniforms);
            m_type = other.m_type;
            m_id = other.m_id;
            other.m_id = {};
            other.m_type = {};
            return *this;
        }

        void destroy();

        ~Source() {
            destroy();
        }

        [[nodiscard]] bool isCompiled() const {
            return !m_source.empty() && m_id != 0 && m_type != 0;
        }

        void compile();

        [[nodiscard]] auto getId() const {
            return m_id;
        }

        [[nodiscard]] auto getType() const {
            return m_type;
        }

        [[nodiscard]] auto getSource() const {
            return m_source;
        }

        explicit operator bool() const {
            return !m_source.empty();
        }

        [[nodiscard]] const std::vector<Uniform>& getUniforms() const;

    private:
        std::string m_source;
        std::vector<Uniform> m_uniforms;
        Id m_id{};
        Type m_type{s_shaderHeader};
    };
}
