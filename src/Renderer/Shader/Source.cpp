#include "Source.h"

#include <array>
#include <iostream>
#include <glad/glad.h>
#include "../Renderer.h"

void Renderer::Shader::Source::destroy() {
    if (m_Id == 0) {
        return;
    }

    RENDERER_API_CALL(*m_Renderer, glDeleteShader(m_Id));
    m_Id = 0;
}

void Renderer::Shader::Source::compile() {
    if (!hasValidSource()) {
        std::cerr << "Shader compilation failed: " << "Invalid source\n";
        return;
    }

    const auto shaderId = RENDERER_API_CALL_RETURN(*m_Renderer, glCreateShader(m_Type));
    const auto* const src = m_Source.c_str();
    RENDERER_API_CALL(*m_Renderer, glShaderSource(shaderId, 1, &src, nullptr));
    RENDERER_API_CALL(*m_Renderer, glCompileShader(shaderId));

    int success{};
    RENDERER_API_CALL(*m_Renderer, glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success));
    if (success == 0) {
        std::array<char, 512> infoLog{};
        RENDERER_API_CALL(*m_Renderer,
                          glGetShaderInfoLog(shaderId, infoLog.size(), nullptr, infoLog.data()));
        std::cerr << "Shader compilation failed:\n" << infoLog.data() << "\n";
        return;
    }

    m_Id = shaderId;
}
