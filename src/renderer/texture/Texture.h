
#ifndef RENDERER_TEXTURE_H
#define RENDERER_TEXTURE_H
#include <cstdint>
#include <string>
#include <glm/vec2.hpp>


namespace Renderer {
    class Texture {
    public:
        explicit Texture(const std::string& path);

        Texture(const Texture&) = delete;

        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept : m_Path{std::move(other.m_Path)}, m_Buffer{other.m_Buffer}, m_Id{other.m_Id},
                                            m_Size{other.m_Size}, m_Bpp{other.m_Bpp} {
            other.m_Id = {};
            other.m_Buffer = {};
        }

        Texture& operator=(Texture&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_Path = std::move(other.m_Path);
            m_Buffer = other.m_Buffer;
            other.m_Buffer = {};
            m_Id = other.m_Id;
            other.m_Id = {};
            return *this;
        }

        ~Texture();

        void bind(uint32_t slot = 0) const;

        static void unbind();

        [[nodiscard]] auto getSize() const {
            return m_Size;
        }

    private:
        std::string m_Path;
        uint8_t* m_Buffer{};
        uint32_t m_Id{};
        glm::ivec2 m_Size{};
        int m_Bpp{};
    };
}


#endif
