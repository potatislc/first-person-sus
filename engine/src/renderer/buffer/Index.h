#pragma once

#include "Buffer.h"
#include "../../core/Typedef.h"

namespace Engine::Renderer {
    class Renderer;
}

namespace Engine::Renderer::Buffer {
    class Index {
    public:
        Index() = delete;

        explicit Index(uint32_t count);

        Index(const Index&) = delete;

        Index& operator=(const Index&) = delete;

        Index(Index&& other) noexcept : m_id{other.m_id}, m_count{other.m_count} {
            other.m_id = {};
            other.m_count = {};
        }

        Index& operator=(Index&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_id = other.m_id;
            other.m_id = {};
            m_count = other.m_count;
            other.m_count = {};
            return *this;
        }

        void destroy();

        ~Index();

        void bind() const;

        static void unbind();

        [[nodiscard]] Id getId() const {
            return m_id;
        }

        [[nodiscard]] uint32_t getCount() const {
            return m_count;
        }

        [[nodiscard]] bool isValid() const {
            return m_id != 0 && m_count > 0;
        }

    private:
        Id m_id{};
        uint32_t m_count{};
    };
}
