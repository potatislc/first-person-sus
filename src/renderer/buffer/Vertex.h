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
                friend class Layout;

                Element() = delete;

                explicit Element(const Shader::DataType dataType,
                                 const bool normalized = {}) : dataType{
                                                                   dataType
                                                               },
                                                               count{
                                                                   Shader::componentCount(dataType)
                                                               }, normalized{normalized} {
                }

                Shader::DataType dataType{};
                Shader::DataTypeComponentCount count{};
                bool normalized{};
            };

            Layout() = delete;

            void push(const Element& element);

            template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
            void push(glm::vec<L, T, Q>, bool normalized = {});

            template<typename... Args>
            explicit Layout(Args... args) {
                (push(std::forward<Args>(args)), ...);
            }

            [[nodiscard]] auto getElements() const { return m_elements; }

            [[nodiscard]] auto getStride() const { return m_stride; }

        private:
            std::vector<Element> m_elements;
            size_t m_stride{};
        };

        Vertex() = delete;

        Vertex(const void* data, uint32_t size, const Layout& layout);

        Vertex(const Vertex&) = delete;

        Vertex& operator=(const Vertex&) = delete;

        Vertex(Vertex&& other) noexcept : m_layout{std::move(other.m_layout)}, m_id{other.m_id} {
            other.m_id = {};
        }

        Vertex& operator=(Vertex&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_id = other.m_id;
            m_layout = std::move(other.m_layout);
            other.m_id = {};
            return *this;
        }

        ~Vertex();

        void bind() const;

        static void unbind();

        const Layout& getLayout() const {
            return m_layout;
        }

    private:
        Layout m_layout;
        uint32_t m_id{};
    };

    inline void Vertex::Layout::push(const Element& element) {
        m_elements.push_back(element);
        m_stride += Shader::dataTypeSize(element.dataType);
    }

    template<glm::length_t L, typename T, glm::qualifier Q>
    void Vertex::Layout::push(glm::vec<L, T, Q>, bool normalized) {
        using VecType = glm::vec<L, T, Q>;
        const auto dataType = Shader::toShaderDataType<VecType>();
        static_assert(dataType != Shader::DataType::None);

        m_elements.emplace_back(dataType, normalized);
        m_stride += Shader::dataTypeSize(dataType);
    }
}
