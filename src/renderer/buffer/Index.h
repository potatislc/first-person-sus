#ifndef RENDERER_BUFFER_INDEX_H
#define RENDERER_BUFFER_INDEX_H
#include <cstdint>

namespace Renderer {
    class Renderer;
}

namespace Renderer::Buffer {
    class Index {
    public:
        Index(const uint32_t* data, uint32_t count);

        ~Index();

        void bind() const;

        static void unbind();

        [[nodiscard]] auto getCount() const {
            return m_Count;
        }

    private:
        uint32_t m_Id{};
        uint32_t m_Count{};
    };
}

#endif
