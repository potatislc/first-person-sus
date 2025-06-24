#pragma once

#include <cstdint>
#include <string>
#include <glm/vec2.hpp>

#include "../../core/Typedef.h"

namespace Renderer {
    class Texture {
    public:
        static Texture createGlTexture(const std::string& path);

        Texture() = default;

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
        Id m_Id{};
        glm::ivec2 m_Size{};
        int m_Bpp{};
    };
}
