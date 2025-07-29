#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <glm/vec2.hpp>

#include "core/Rect.h"
#include "core/Typedef.h"

namespace Engine::Renderer {
    class Texture {
    public:
        class GlSource {
        public:
            GlSource() = default;

            GlSource(const GlSource&) = delete;

            GlSource& operator=(const GlSource&) = delete;

            GlSource& operator=(GlSource&&) = delete;

            GlSource(GlSource&&) = delete;

            ~GlSource();

            [[nodiscard]] Id getId() const {
                return m_id;
            }

            [[nodiscard]] glm::vec2 getSize() const {
                return m_size;
            }

            friend Texture;

        private:
            Id m_id{};
            glm::ivec2 m_size{};
        };

        static Texture loadGlTexture(const std::string& path);

        void bind(uint32_t slot = 0) const;

        static void unbind();

        [[nodiscard]] const GlSource& getSource() const {
            return *m_source;
        }

    private:
        explicit Texture(std::shared_ptr<GlSource> source) : m_source(std::move(source)),
                                                             m_subRect{glm::vec2{}, m_source->getSize()} {
        }

        std::shared_ptr<GlSource> m_source;
        Rect m_subRect{};
    };
}
