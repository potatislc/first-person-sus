#ifndef SHADER_H
#define SHADER_H

#include <string>

namespace Renderer {
    class Renderer;
}

namespace Renderer::Shader {
    class Source {
    public:
        Source() = delete;

        explicit Source(const Renderer& renderer) : m_Renderer{&renderer} {
        }

        Source(const Renderer& renderer, const unsigned int type, std::string source) : m_Source{
                std::move(source)
            },
            m_Renderer{&renderer}, m_Type{type} {
        }

        Source(const Source&) = delete;

        Source& operator=(const Source&) = delete;

        Source(Source&& other) noexcept : m_Source{std::move(other.m_Source)}, m_Renderer{m_Renderer},
                                          m_Type{other.m_Type},
                                          m_Id{other.m_Id} {
        }

        Source& operator=(Source&& other) noexcept {
            m_Source = std::move(other.m_Source);
            m_Type = other.m_Type;
            m_Id = other.m_Id;
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

    private:
        std::string m_Source;
        const Renderer* m_Renderer{};
        unsigned int m_Type{};
        unsigned int m_Id{};
    };
}

#endif //SHADER_H
