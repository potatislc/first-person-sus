#pragma once

#include <vector>
#include "Buffer.h"
#include "../shader/Shader.h"

namespace Renderer {
    class Renderer;
}

namespace Renderer::Buffer {
    class Vertex {
    public:
        class Layout {
        public:
            struct Attribute {
                Attribute() = delete;

                explicit Attribute(const Shader::DataType dataType,
                                   const bool normalized = {}) : dataType{
                                                                     dataType
                                                                 }, normalized{normalized} {
                }

                Shader::DataType dataType{};
                bool normalized{};
            };

            Layout() = delete;

            void push(const Attribute& element);

            template<glm::length_t L, typename T, glm::qualifier Q = glm::defaultp>
            void push(glm::vec<L, T, Q> /*unused*/, bool normalized = {});

            template<typename... Args>
            explicit Layout(Args... args) {
                (push(std::forward<Args>(args)), ...);
            }

            [[nodiscard]] auto getAttributes() const { return m_attributes; }

            [[nodiscard]] auto getStride() const { return m_stride; }

        private:
            std::vector<Attribute> m_attributes;
            size_t m_stride{};
        };

        static BufferData layoutInterleave(const Layout& layout, const std::vector<BufferData>& dataBatch);

        Vertex() = delete;

        Vertex(Layout layout, const void* data, uint32_t size);

        Vertex(Layout layout, const BufferData& bufferData);

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

        [[nodiscard]] const Layout& getLayout() const {
            return m_layout;
        }

    private:
        Layout m_layout;
        Id m_id{};
    };

    inline void Vertex::Layout::push(const Attribute& element) {
        m_attributes.push_back(element);
        m_stride += Shader::dataTypeSize(element.dataType);
    }

    template<glm::length_t L, typename T, glm::qualifier Q>
    void Vertex::Layout::push(glm::vec<L, T, Q> /*unused*/, bool normalized) {
        using VecType = glm::vec<L, T, Q>;
        const auto dataType = Shader::toShaderDataType<VecType>();
        static_assert(dataType != Shader::DataType::None);

        m_attributes.emplace_back(dataType, normalized);
        m_stride += Shader::dataTypeSize(dataType);
    }
}
