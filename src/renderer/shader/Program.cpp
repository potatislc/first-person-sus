#include "Program.h"

#include <array>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Parser.h"
#include "../Renderer.h"
#include "Source.h"

void Renderer::Shader::Program::setUniform(const int32_t location, const int val) {
    RENDERER_API_CALL(glUniform1i(location, val));
}

void Renderer::Shader::Program::setUniform(const int32_t location, const float val) {
    RENDERER_API_CALL(glUniform1f(location, val));
}

void Renderer::Shader::Program::setUniform(const int32_t location, const glm::vec4& val) {
    RENDERER_API_CALL(glUniform4fv(location, 1, &val[0]));
}

void Renderer::Shader::Program::setUniform(const int32_t location, const glm::mat4& val) {
    RENDERER_API_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, &val[0][0]));
}

int32_t Renderer::Shader::Program::getUniformLocation(const std::string& name) const {
    for (const auto& uniform: m_uniforms) {
        if (uniform.m_name == name) {
            return uniform.m_location;
        }
    }

    std::cerr << "Uniform with name: " << name << ". not found.\n";

    return Uniform::noLocation;
}

bool Renderer::Shader::Program::createProgram() {
    m_id = RENDERER_API_CALL_RETURN(glCreateProgram());
    if (m_id == 0) {
        std::cerr << s_CreationFailStr << '\n';
    }

    return static_cast<bool>(m_id);
}

bool Renderer::Shader::Program::linkProgram() const {
    RENDERER_API_CALL(glLinkProgram(m_id));

    int linkStatus = 0;
    RENDERER_API_CALL(glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus));
    if (linkStatus == 0) {
        std::cerr << "Failed to link shader program." << '\n';
        std::array<char, 512> log{};
        glGetProgramInfoLog(m_id, 512, nullptr, log.data());
        std::cerr << "Link error: " << log.data() << '\n';
    }

    return static_cast<bool>(linkStatus);
}

void Renderer::Shader::Program::attachShader(const Source& source) {
    auto appendUniforms = source.getUniforms();
    m_uniforms.insert(m_uniforms.end(), appendUniforms.begin(), appendUniforms.end());

    RENDERER_API_CALL(glAttachShader(m_id, source.getId()));
}

bool Renderer::Shader::Program::locateUniforms() {
    for (auto& uniform: m_uniforms) {
        if (!uniform.locate(m_id)) {
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
            std::cerr << s_CreationFailStr << '\n';
            return;
        }

        attachShader(shader);
    }

    if (!linkProgram()) {
        return;
    }

    bind();

    if (!locateUniforms()) {
        return;
    }
}

void Renderer::Shader::Program::bind() const {
    RENDERER_API_CALL(glUseProgram(m_id));
}

void Renderer::Shader::Program::unbind() {
    RENDERER_API_CALL(glUseProgram(0));
}

void Renderer::Shader::Program::destroy() const {
    if (m_id == 0) {
        return;
    }

    unbind();
    RENDERER_API_CALL(glDeleteProgram(m_id));
}

Renderer::Shader::Program::~Program() {
    destroy();
}
