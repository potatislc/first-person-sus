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

        explicit VertexArray(Buffer::Index indexBuffer,
                             std::initializer_list<std::shared_ptr<Buffer::Vertex> > vertexBuffers = {});

        VertexArray(const VertexArray&) = delete;

        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept : m_vertexBuffers{std::move(other.m_vertexBuffers)},
                                                    m_indexBuffer{std::move(other.m_indexBuffer)}, m_id{other.m_id} {
            other.m_id = {};
        }

        VertexArray& operator=(VertexArray&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_vertexBuffers = std::move(other.m_vertexBuffers);
            m_indexBuffer = std::move(other.m_indexBuffer);
            m_id = other.m_id;
            other.m_id = {};
            return *this;
        }

        ~VertexArray();

        void bind() const;

        static void unbind();

        const Buffer::Index& getIndexBuffer() const {
            return m_indexBuffer;
        }

    private:
        void addBuffer(const std::shared_ptr<Buffer::Vertex>& vertexBuffer);

        std::vector<std::shared_ptr<Buffer::Vertex> > m_vertexBuffers;
        Buffer::Index m_indexBuffer;
        Id m_id{};
    };
}
