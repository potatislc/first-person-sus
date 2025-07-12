#pragma once

#include <cstdint>
#include <string>
#include <glm/vec2.hpp>

#include "core/Typedef.h"

namespace Engine::Renderer {
    class Texture {
    public:
        static Texture createGlTexture(const std::string& path);

        Texture() = default;

        Texture(const Texture&) = delete;

        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept : m_path{std::move(other.m_path)}, m_buffer{other.m_buffer}, m_id{other.m_id},
                                            m_size{other.m_size}, m_bpp{other.m_bpp} {
            other.m_id = {};
            other.m_buffer = {};
        }

        Texture& operator=(Texture&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_path = std::move(other.m_path);
            m_buffer = other.m_buffer;
            other.m_buffer = {};
            m_id = other.m_id;
            other.m_id = {};
            return *this;
        }

        ~Texture();

        void bind(uint32_t slot = 0) const;

        static void unbind();

        [[nodiscard]] auto getSize() const {
            return m_size;
        }

    private:
        std::string m_path;
        uint8_t* m_buffer{};
        Id m_id{};
        glm::ivec2 m_size{};
        int m_bpp{};
    };
}
