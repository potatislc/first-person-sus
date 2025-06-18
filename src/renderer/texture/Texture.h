
#ifndef RENDERER_TEXTURE_H
#define RENDERER_TEXTURE_H
#include <cstdint>
#include <string>
#include <glm/vec2.hpp>


namespace Renderer {
    class Texture {
    public:
        explicit Texture(const std::string& path);

        ~Texture();

        void bind(uint32_t slot = 0) const;

        static void unbind();

        [[nodiscard]] auto getSize() const {
            return m_Size;
        }

    private:
        std::string m_Path;
        uint32_t m_Id{};
        uint8_t* m_Buffer{};
        glm::ivec2 m_Size{};
        int m_Bpp{};
    };
}


#endif
