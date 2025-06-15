#include "Program.h"

#include <iostream>
#include <glad/glad.h>

#include "Parser.h"
#include "../Renderer.h"

bool Renderer::Shader::Program::createProgram() {
    m_Id = RENDERER_API_CALL_RETURN(glCreateProgram());
    if (m_Id == 0) {
        std::cerr << creationFailStr << '\n';
    }

    return static_cast<bool>(m_Id);
}

bool Renderer::Shader::Program::linkProgram() const {
    RENDERER_API_CALL(glLinkProgram(m_Id));

    int linkStatus = 0;
    RENDERER_API_CALL(glGetProgramiv(m_Id, GL_LINK_STATUS, &linkStatus));
    if (linkStatus == 0) {
        std::cerr << "Failed to link shader program." << '\n';
    }

    return static_cast<bool>(linkStatus);
}

void Renderer::Shader::Program::attachShader(const uint32_t shaderId) const {
    RENDERER_API_CALL(glAttachShader(m_Id, shaderId));
}

Renderer::Shader::Program::Program(Parser sourceParser) {
    if (!createProgram()) {
        return;
    }

    while (auto shader{sourceParser.next()}) {
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
    RENDERER_API_CALL(glUseProgram(m_Id));
}

Renderer::Shader::Program::~Program() {
    if (m_Id == 0) {
        return;
    }

    RENDERER_API_CALL(glDeleteProgram(m_Id));
}
