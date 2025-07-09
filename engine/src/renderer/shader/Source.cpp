#include "Source.h"

#include <array>
#include <iostream>
#include <glad/glad.h>
#include "../Renderer.h"

void Engine::Renderer::Shader::Source::destroy() {
    if (m_id == 0) {
        return;
    }

    RENDERER_API_CALL(glDeleteShader(m_id));
    m_id = 0;
}

void Engine::Renderer::Shader::Source::compile() {
    if (!hasValidSource()) {
        LOG_ERR("Shader compilation failed: " << "Invalid source\n");
        return;
    }

    const auto shaderId = RENDERER_API_CALL_RETURN(glCreateShader(m_type));
    const auto* const src = m_source.c_str();
    RENDERER_API_CALL(glShaderSource(shaderId, 1, &src, nullptr));
    RENDERER_API_CALL(glCompileShader(shaderId));

    int success{};
    RENDERER_API_CALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success));
    if (success == 0) {
        std::array<char, 512> infoLog{};
        RENDERER_API_CALL(
            glGetShaderInfoLog(shaderId, infoLog.size(), nullptr, infoLog.data()));
        LOG_ERR("Shader compilation failed:\n" << infoLog.data() << "\n");
        return;
    }

    m_id = shaderId;
}

const std::vector<Engine::Renderer::Shader::Uniform>& Engine::Renderer::Shader::Source::getUniforms() const {
    return m_uniforms;
}
