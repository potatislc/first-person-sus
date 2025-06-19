#ifndef RENDERER_SHADER_SOURCE_H
#define RENDERER_SHADER_SOURCE_H

#include <string>
#include <vector>

#include "Uniform.h"

namespace Renderer {
    class Renderer;
}

namespace Renderer::Shader {
    class Source {
    public:
        Source() = default;

        Source(const uint32_t type, std::string source, std::vector<Uniform> uniforms = {}) : m_Source{
                std::move(source)
            }, m_Uniforms{
                std::move(uniforms)
            }, m_Type{type} {
        }

        Source(const Source&) = delete;

        Source& operator=(const Source&) = delete;

        Source(Source&& other) noexcept : m_Source{std::move(other.m_Source)}, m_Uniforms{std::move(other.m_Uniforms)},
                                          m_Id{other.m_Id},
                                          m_Type{other.m_Type} {
            other.m_Id = {};
        }

        Source& operator=(Source&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_Source = std::move(other.m_Source);
            m_Uniforms = std::move(other.m_Uniforms);
            m_Type = other.m_Type;
            m_Id = other.m_Id;
            other.m_Id = {};
            other.m_Type = {};
            return *this;
        }

        void destroy();

        ~Source() {
            destroy();
        }

        [[nodiscard]] bool hasValidSource() const {
            return !m_Source.empty() && m_Type != 0;
        }

        [[nodiscard]] bool isCompiled() const {
            return hasValidSource() && m_Id != 0 && m_Type != 0;
        }

        void compile();

        [[nodiscard]] auto getId() const {
            return m_Id;
        }

        [[nodiscard]] auto getType() const {
            return m_Type;
        }

        [[nodiscard]] auto getSource() const {
            return m_Source;
        }

        explicit operator bool() const {
            return hasValidSource();
        }

        [[nodiscard]] const std::vector<Uniform>& getUniforms() const;

    private:
        std::string m_Source;
        std::vector<Uniform> m_Uniforms;
        uint32_t m_Id{};
        uint32_t m_Type{};
    };
}

#endif
