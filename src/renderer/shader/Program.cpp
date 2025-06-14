#include "Program.h"

#include <iostream>
#include <glad/glad.h>

#include "Parser.h"
#include "../Renderer.h"

bool Renderer::Shader::Program::createProgram() {
    m_Id = RENDERER_API_CALL_RETURN(*m_Renderer, glCreateProgram());
    if (m_Id == 0) {
        std::cerr << creationFailStr << '\n';
    }

    return static_cast<bool>(m_Id);
}

bool Renderer::Shader::Program::linkProgram() const {
    RENDERER_API_CALL(*m_Renderer, glLinkProgram(m_Id));

    int linkStatus = 0;
    RENDERER_API_CALL(*m_Renderer, glGetProgramiv(m_Id, GL_LINK_STATUS, &linkStatus));
    if (linkStatus == 0) {
        std::cerr << "Failed to link shader program." << '\n';
    }

    return static_cast<bool>(linkStatus);
}

void Renderer::Shader::Program::attachShader(const uint32_t shaderId) const {
    RENDERER_API_CALL(*m_Renderer, glAttachShader(m_Id, shaderId));
}

Renderer::Shader::Program::Program(const Renderer& renderer, Parser sourceParser) : m_Renderer{&renderer} {
    if (!createProgram()) {
        return;
    }

    while (auto shader{sourceParser.next(renderer)}) {
        shader.compile();
        if (!shader.isCompiled()) {
            std::cerr << creationFailStr << '\n';
            return;
        }

        attachShader(shader.getId());
    }

    if (!linkProgram()) {
        return;
    }
}

void Renderer::Shader::Program::use() const {
    RENDERER_API_CALL(*m_Renderer, glUseProgram(m_Id));
}

Renderer::Shader::Program::~Program() {
    if (m_Id == 0) {
        return;
    }

    RENDERER_API_CALL(*m_Renderer, glDeleteProgram(m_Id));
}
