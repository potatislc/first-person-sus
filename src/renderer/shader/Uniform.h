#pragma once

#include <string>

namespace Renderer::Shader {
    struct Uniform {
        static constexpr int32_t noLocation{-1};

        explicit Uniform(std::string name) : m_name{std::move(name)} {
        }

        bool locate(uint32_t programId);

        std::string m_name;
        // uint32_t typeId; // GLenum
        int32_t m_location{noLocation};
        // uint32_t m_size{1}; // For Arrays
    };
}
