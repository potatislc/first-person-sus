#ifndef RENDERER_VERTEXARRAY_H
#define RENDERER_VERTEXARRAY_H
#include "buffer/Index.h"
#include "buffer/Vertex.h"

namespace Buffer {
    class Vertex;
    class Index;
}

namespace Renderer {
    class VertexArray {
    public:
        VertexArray();

        ~VertexArray();

        void addBuffer(const Buffer::Vertex& vertexBuffer, const Buffer::Vertex::Layout& vertexLayout) const;

        void bind() const;

        static void unbind();

    private:
        uint32_t m_Id{};
    };
}
#endif //VERTEXARRAY_H
