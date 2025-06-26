#pragma once

#include <cstdint>
#include "../../core/Typedef.h"

namespace Renderer {
    class Renderer;
}

namespace Renderer::Buffer {
    class Index {
    public:
        Index() = delete;

        Index(const uint32_t* data, uint32_t count);

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

        [[nodiscard]] auto getCount() const {
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
