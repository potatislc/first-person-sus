#pragma once

#include <string>

namespace Renderer::Shader {
    struct Uniform {
        static constexpr int32_t noLocation{-1};

        explicit Uniform(std::string name) : m_Name{std::move(name)} {
        }

        bool locate(uint32_t programId);

        std::string m_Name;
        // uint32_t typeId; // GLenum
        int32_t m_Location{noLocation};
        // uint32_t m_Size{1}; // For Arrays
    };
}
