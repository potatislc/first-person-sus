#include "Program.h"

#include <iostream>
#include <glad/glad.h>

#include "Parser.h"
#include "../Renderer.h"
#include "Source.h"

void Renderer::Shader::Program::setUniform(const int32_t location, const glm::vec4& vec4) const {
    RENDERER_API_CALL(glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w));
}

int32_t Renderer::Shader::Program::getUniformLocation(const std::string& name) const {
    for (const auto& uniform: m_Uniforms) {
        if (uniform.m_Name == name) {
            return uniform.m_Location;
        }
    }

    std::cerr << "Uniform with name: " << name << "not found.\n";

    return Uniform::noLocation;
}

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

void Renderer::Shader::Program::attachShader(const Source& source) {
    auto appendUniforms = source.getUniforms();
    m_Uniforms.insert(m_Uniforms.end(), appendUniforms.begin(), appendUniforms.end());
    RENDERER_API_CALL(glAttachShader(m_Id, source.getId()));
}

bool Renderer::Shader::Program::locateUniforms() {
    for (auto& uniform: m_Uniforms) {
        if (!uniform.locate(m_Id)) {
            return false;
        }
    }

    return true;
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

        attachShader(shader);
    }

    if (!linkProgram()) {
        return;
    }

    if (!locateUniforms()) {
        return;
    }
}

void Renderer::Shader::Program::bind() const {
    RENDERER_API_CALL(glUseProgram(m_Id));
}

void Renderer::Shader::Program::unbind() {
    RENDERER_API_CALL(glUseProgram(0));
}

void Renderer::Shader::Program::destroy() const {
    if (m_Id == 0) {
        return;
    }

    unbind();
    RENDERER_API_CALL(glDeleteProgram(m_Id));
}

Renderer::Shader::Program::~Program() {
    destroy();
}
