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
        VertexArray() = delete;

        VertexArray(Buffer::Vertex vertexBuffer, uint32_t indexCount);

        VertexArray(Buffer::Vertex vertexBuffer, const Buffer::IndexData& indexData);

        VertexArray(const VertexArray&) = delete;

        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept : m_vertexBuffer{std::move(other.m_vertexBuffer)},
                                                    m_indexBuffer{std::move(other.m_indexBuffer)}, m_id{other.m_id} {
            other.m_id = {};
        }

        VertexArray& operator=(VertexArray&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_vertexBuffer = std::move(other.m_vertexBuffer);
            m_indexBuffer = std::move(other.m_indexBuffer);
            m_id = other.m_id;
            other.m_id = {};
            return *this;
        }

        ~VertexArray();

        void bind() const;

        static void unbind();

        [[nodiscard]] const Buffer::Index& getIndexBuffer() const {
            return m_indexBuffer;
        }

    private:
        void attachVertexBuffer() const;

        void attachIndexBuffer(const Buffer::IndexData& indexData) const;

        Buffer::Vertex m_vertexBuffer;
        Buffer::Index m_indexBuffer;
        Id m_id{};
    };
}
