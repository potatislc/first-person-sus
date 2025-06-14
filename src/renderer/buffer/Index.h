#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H
#include <cstdint>

namespace Renderer {
    class Renderer;
}

namespace Renderer::Buffer {
    class Index {
    public:
        Index(const Renderer& renderer, const uint32_t* data, uint32_t count);

        ~Index();

        void bind() const;

        void unbind() const;

    private:
        const Renderer* m_Renderer{};
        uint32_t m_BufferId{};
    };
}

#endif //VERTEXBUFFER_H
