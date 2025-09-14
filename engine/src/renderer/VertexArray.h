#pragma once

#include "buffer/Vertex.h"
#include "core/Typedef.h"
#include "buffer/Index.h"

namespace Engine::Renderer::Buffer {
    class Vertex;
}

namespace Engine::Renderer {
    class VertexArray {
    public:
        VertexArray() = delete;

        static VertexArray withGeneratedDefaultIndices(Buffer::Vertex vertexBuffer, uint32_t indexCount);

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

        void setInstanceBuffer(Buffer::Vertex instanceBuffer);

        void updateInstanceBuffer(const void* data, uint32_t count) const;

        [[nodiscard]] bool isInstantiable() const {
            return m_instanceBuffer.has_value();
        }

    private:
        void attachIndexBuffer(const Buffer::IndexData& indexData) const;

        void attachVertexBuffer(uint32_t attributeStart = 0) const;

        void attachInstanceBuffer(uint32_t attributeStart) const;

        Buffer::Vertex m_vertexBuffer;
        std::optional<Buffer::Vertex> m_instanceBuffer; // Specifies per-instance attributes
        Buffer::Index m_indexBuffer;
        Id m_id{};
    };
}
