#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H
#include <cstdint>

namespace Renderer {
    class Renderer;
}

namespace Renderer::Buffer {
    class Vertex {
    public:
        Vertex(const Renderer& renderer, const void* data, uint32_t size);

        ~Vertex();

        void bind() const;

        void unbind() const;

    private:
        const Renderer* m_Renderer{};
        uint32_t m_BufferId{};
    };
}

#endif //VERTEXBUFFER_H
