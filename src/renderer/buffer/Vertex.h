#ifndef RENDERER_BUFFER_VERTEX_H
#define RENDERER_BUFFER_VERTEX_H
#include <cstdint>
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

            [[nodiscard]] auto getElements() const { return m_Elements; }

            [[nodiscard]] auto getStride() const { return m_Stride; }

        private:
            std::vector<Element> m_Elements;
            size_t m_Stride{};
        };

        Vertex(const void* data, uint32_t size);

        ~Vertex();

        void bind() const;

        static void unbind();

    private:
        uint32_t m_Id{};
    };

    template<>
    void Vertex::Layout::push<float>(uint32_t count);

    template<>
    void Vertex::Layout::push<uint32_t>(uint32_t count);

    template<>
    void Vertex::Layout::push<uint8_t>(uint32_t count);
}

#endif
