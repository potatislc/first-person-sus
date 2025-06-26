#pragma once

#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glad/glad.h>

#include "../../core/Assert.h"

namespace Renderer::Shader {
    template<typename T>
    constexpr bool is_valid_scalar_v =
            std::is_same_v<T, float> || std::is_same_v<T, int>;

    enum class DataType : uint8_t {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    inline uint32_t dataTypeSize(const DataType type) {
        switch (type) {
            case DataType::Float: return 4;
            case DataType::Float2: return 4 * 2;
            case DataType::Float3: return 4 * 3;
            case DataType::Float4: return 4 * 4;
            case DataType::Mat3: return 4 * 3 * 3;
            case DataType::Mat4: return 4 * 4 * 4;
            case DataType::Int: return 4;
            case DataType::Int2: return 4 * 2;
            case DataType::Int3: return 4 * 3;
            case DataType::Int4: return 4 * 4;
            case DataType::Bool: return 1;
            default: break;
        }

        CORE_ASSERT_MSG(false, "Unknown Shader::DataType!");
        return 0;
    }

    inline uint32_t dataTypeComponentCount(const DataType type) {
        switch (type) {
            case DataType::Float: return 1;
            case DataType::Float2: return 2;
            case DataType::Float3: return 3;
            case DataType::Float4: return 4;
            case DataType::Mat3: return 3 * 3;
            case DataType::Mat4: return 4 * 4;
            case DataType::Int: return 1;
            case DataType::Int2: return 2;
            case DataType::Int3: return 3;
            case DataType::Int4: return 4;
            case DataType::Bool: return 1;
            default: break;
        }

        CORE_ASSERT_MSG(false, "Unknown Shader::DataType!");
        return 0;
    }


    template<typename T>
    consteval DataType toShaderDataType() {
        static_assert(std::is_arithmetic_v<T>);
        return DataType::None;
    }

    template<>
    consteval DataType toShaderDataType<float>() {
        return DataType::Float;
    }

    template<>
    consteval DataType toShaderDataType<glm::vec2>() {
        return DataType::Float2;
    }

    template<>
    consteval DataType toShaderDataType<glm::vec3>() {
        return DataType::Float3;
    }

    template<>
    inline DataType toShaderDataType<glm::vec4>() {
        return DataType::Float4;
    }

    template<>
    inline DataType toShaderDataType<glm::mat3>() {
        return DataType::Mat3;
    }

    template<>
    inline DataType toShaderDataType<glm::mat4>() {
        return DataType::Mat4;
    }

    template<>
    inline DataType toShaderDataType<int>() {
        return DataType::Int;
    }

    template<>
    inline DataType toShaderDataType<glm::ivec2>() {
        return DataType::Int2;
    }

    template<>
    inline DataType toShaderDataType<glm::ivec3>() {
        return DataType::Int3;
    }

    template<>
    inline DataType toShaderDataType<glm::ivec4>() {
        return DataType::Int4;
    }

    template<>
    inline DataType toShaderDataType<bool>() {
        return DataType::Bool;
    }

    inline GLenum toGlDataType(const DataType type) {
        switch (type) {
            case DataType::None: break;
            case DataType::Float: return GL_FLOAT;
            case DataType::Float2: return GL_FLOAT;
            case DataType::Float3: return GL_FLOAT;
            case DataType::Float4: return GL_FLOAT;
            case DataType::Mat3: return GL_FLOAT;
            case DataType::Mat4: return GL_FLOAT;
            case DataType::Int: return GL_INT;
            case DataType::Int2: return GL_INT;
            case DataType::Int3: return GL_INT;
            case DataType::Int4: return GL_INT;
            case DataType::Bool: return GL_BOOL;
            default: break;
        }

        CORE_ASSERT_MSG(false, "Unknown Shader::DataType!");
        return 0;
    }
}
