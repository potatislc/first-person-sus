#pragma once

#include <vector>
#include "../shader/Shader.h"

namespace Renderer {
    class Renderer;
}

namespace Renderer::Buffer {
    class Vertex {
    public:
        class Layout {
        public:
            struct Element {
                Shader::DataType type{};
                uint8_t count{}; // Don't do anything stupid!
                bool normalized{};
            };

            template<typename T, std::enable_if_t<Shader::is_valid_scalar_v<T>, int>  = 0>
            void push(uint32_t count = 1);

            template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp, std::enable_if_t<
                Shader::is_valid_scalar_v<T>, int>  = 0>
            void push(glm::vec<L, T, Q>);

            [[nodiscard]] auto getElements() const { return m_elements; }

            [[nodiscard]] auto getStride() const { return m_stride; }

        private:
            std::vector<Element> m_elements;
            size_t m_stride{};
        };

        Vertex(const void* data, uint32_t size);

        Vertex(const Vertex&) = delete;

        Vertex& operator=(const Vertex&) = delete;

        Vertex(Vertex&& other) noexcept : m_id{other.m_id} {
            other.m_id = {};
        }

        Vertex& operator=(Vertex&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_id = other.m_id;
            other.m_id = {};
            return *this;
        }

        ~Vertex();

        void bind() const;

        static void unbind();

        Layout& getLayout() {
            return m_layout;
        }

        const Layout& getLayout() const {
            return m_layout;
        }

    private:
        Layout m_layout;
        uint32_t m_id{};
    };

    template<typename T, std::enable_if_t<Shader::is_valid_scalar_v<T>, int> >
    void Vertex::Layout::push(const uint32_t count) {
        CORE_ASSERT_MSG(count > 0, "Cannot push vertex layout of count 0.");
        const auto dataType = Shader::toShaderDataType<T>();

        m_elements.emplace_back(dataType, count);
        m_stride += Shader::dataTypeSize(dataType) * count;
    }

    template<glm::length_t L, typename T, glm::qualifier Q, std::enable_if_t<Shader::is_valid_scalar_v<T>, int> >
    void Vertex::Layout::push(glm::vec<L, T, Q>) {
        const auto dataType = Shader::toShaderDataType<T>();

        m_elements.emplace_back(dataType, L);
        m_stride += Shader::dataTypeSize(dataType) * L;
    }
}
