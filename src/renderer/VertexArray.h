#ifndef RENDERER_VERTEXARRAY_H
#define RENDERER_VERTEXARRAY_H
#include "buffer/Vertex.h"

namespace Buffer {
    class Vertex;
}

namespace Renderer {
    class VertexArray {
    public:
        VertexArray();

        VertexArray(const VertexArray&) = delete;

        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept : m_Id{other.m_Id} {
            other.m_Id = {};
        }

        VertexArray& operator=(VertexArray&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_Id = other.m_Id;
            other.m_Id = {};
            return *this;
        }

        ~VertexArray();

        void addBuffer(const Buffer::Vertex& vertexBuffer, const Buffer::Vertex::Layout& vertexLayout) const;

        void bind() const;

        static void unbind();

    private:
        uint32_t m_Id{};
    };
}
#endif
