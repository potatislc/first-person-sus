#include "Program.h"

#include <array>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Parser.h"
#include "renderer/Renderer.h"
#include "Source.h"

void Engine::Renderer::Shader::Program::setUniform(const int32_t location, const int val) {
    RENDERER_API_CALL(glUniform1i(location, val));
}

void Engine::Renderer::Shader::Program::setUniform(const int32_t location, const float val) {
    RENDERER_API_CALL(glUniform1f(location, val));
}

void Engine::Renderer::Shader::Program::setUniform(const int32_t location, const glm::vec3& val) {
    RENDERER_API_CALL(glUniform3fv(location, 1, glm::value_ptr(val)));
}

void Engine::Renderer::Shader::Program::setUniform(const int32_t location, const glm::vec4& val) {
    RENDERER_API_CALL(glUniform4fv(location, 1, glm::value_ptr(val)));
}

void Engine::Renderer::Shader::Program::setUniform(const int32_t location, const glm::mat4& val) {
    RENDERER_API_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val)));
}

int32_t Engine::Renderer::Shader::Program::getUniformLocation(const std::string& name) const {
    for (const auto& uniform: m_uniforms) {
        if (uniform.m_name == name) {
            return uniform.m_location;
        }
    }

    LOG_ERR("Uniform with name: " << name << " not found.\n");

    return Uniform::noLocation;
}

bool Engine::Renderer::Shader::Program::createProgram() {
    m_id = RENDERER_API_CALL_RETURN(glCreateProgram());
    if (m_id == 0) {
        LOG_ERR(&s_CreationFailStr << '\n');
    }

    return static_cast<bool>(m_id);
}

bool Engine::Renderer::Shader::Program::linkProgram() const {
    RENDERER_API_CALL(glLinkProgram(m_id));

    int linkStatus = 0;
    RENDERER_API_CALL(glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus));
    if (linkStatus == 0) {
        LOG_ERR("Failed to link shader program." << '\n');
        std::array<char, 512> log{};
        glGetProgramInfoLog(m_id, 512, nullptr, log.data());
        LOG_ERR("Link error: " << log.data() << '\n');
    }

    return static_cast<bool>(linkStatus);
}

void Engine::Renderer::Shader::Program::attachShader(const Source& source) {
    auto appendUniforms = source.getUniforms();
    m_uniforms.insert(m_uniforms.end(), appendUniforms.begin(), appendUniforms.end());

    RENDERER_API_CALL(glAttachShader(m_id, source.getId()));
}

bool Engine::Renderer::Shader::Program::locateUniforms() {
    for (auto& uniform: m_uniforms) {
        if (!uniform.locate(m_id)) {
            return false;
        }
    }

    return true;
}

Engine::Renderer::Shader::Program::Program(Parser sourceParser) {
    if (!createProgram()) {
        return;
    }

    while (auto shader{sourceParser.next()}) {
        shader.compile();
        if (!shader.isCompiled()) {
            LOG_ERR(&s_CreationFailStr << '\n');
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

Engine::Renderer::Shader::Program::Program(const std::initializer_list<std::string> paths) {
    if (!createProgram()) {
        return;
    }

    for (const auto& path: paths) {
        Parser parser{path};

        while (auto shader{parser.next()}) {
            shader.compile();
            if (!shader.isCompiled()) {
                LOG_ERR(&s_CreationFailStr << '\n');
                return;
            }

            attachShader(shader);
        }
    }

    if (!linkProgram()) {
        return;
    }

    bind();

    if (!locateUniforms()) {
        return;
    }
}

void Engine::Renderer::Shader::Program::bind() const {
    static Id s_bound{};

    if (m_id == s_bound) {
        return;
    }

    RENDERER_API_CALL(glUseProgram(m_id));
    s_bound = m_id;
}

void Engine::Renderer::Shader::Program::unbind() {
    RENDERER_API_CALL(glUseProgram(0));
}

void Engine::Renderer::Shader::Program::destroy() const {
    if (m_id == 0) {
        return;
    }

    unbind();
    RENDERER_API_CALL(glDeleteProgram(m_id));
}

Engine::Renderer::Shader::Program::~Program() {
    destroy();
}
