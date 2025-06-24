#pragma once

#include <vector>

namespace Renderer {
    class Renderer;
}

namespace Renderer::Buffer {
    class Vertex {
    public:
        class Layout {
        public:
            struct Element {
                uint32_t type{};
                uint32_t count{};
                bool normalized{};

                static size_t sizeOfType(uint32_t type);
            };

            template<typename T>
            void push(uint32_t count) {
                static_assert(false);
            }

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

    private:
        uint32_t m_id{};
    };

    template<>
    void Vertex::Layout::push<float>(uint32_t count);

    template<>
    void Vertex::Layout::push<uint32_t>(uint32_t count);

    template<>
    void Vertex::Layout::push<uint8_t>(uint32_t count);
}
