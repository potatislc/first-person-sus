#pragma once

#include "buffer/Vertex.h"
#include "../core/Typedef.h"
#include "buffer/Index.h"

namespace Buffer {
    class Vertex;
}

namespace Renderer {
    class VertexArray {
    public:
        VertexArray();

        VertexArray(const VertexArray&) = delete;

        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept : m_id{other.m_id} {
            other.m_id = {};
        }

        VertexArray& operator=(VertexArray&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_id = other.m_id;
            other.m_id = {};
            return *this;
        }

        ~VertexArray();

        void addBuffer(const Buffer::Vertex& vertexBuffer, const Buffer::Vertex::Layout& vertexLayout) const;

        void bind() const;

        static void unbind();

    private:
        Id m_id{};
        std::vector<Buffer::Vertex> m_vertexBuffers;
        // Buffer::Index m_indexBuffer;
    };
}
