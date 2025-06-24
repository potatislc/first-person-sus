#include "Parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>

#include  "../../core/Assert.h"

Renderer::Shader::Parser::Parser(const std::string& filePath) : m_stream{filePath} {
    if (m_stream.bad()) {
        std::cerr << "Invalid shader source path: " << filePath << '\n';
    }

    ASSERT(m_stream.good());
}

Renderer::Shader::Source Renderer::Shader::Parser::operator()() {
    auto fail = [this](const std::string& description) {
        std::cerr << "Failed to parse shader source: " << description << " At line "
                << m_lineCount << ". " << "[LINE SOURCE]: " << m_line << '\n';
        return Source{};
    };

    std::stringstream ss;
    uint32_t shaderType = m_nextShaderType;
    std::vector<Uniform> uniforms;

    while (std::getline(m_stream, m_line)) {
        ++m_lineCount;

        if (std::ranges::all_of(m_line,
                                [](const unsigned char c) { return std::isspace(c); })) {
            continue;
        }

        if (m_line.contains("#shader")) {
            if (m_line.contains("vertex")) {
                m_nextShaderType = GL_VERTEX_SHADER;
            } else if (m_line.contains("fragment")) {
                m_nextShaderType = GL_FRAGMENT_SHADER;
            } else {
                return fail("Could not deduce shader type.");
            }

            if (shaderType != 0) {
                break;
            }

            shaderType = m_nextShaderType;

            continue;
        }

        if (m_line.contains("uniform ")) {
            auto uniformName = m_line.substr(m_line.find_last_of(' ') + 1);
            if (const size_t end = uniformName.find_first_of(';'); end != std::string::npos) {
                uniformName.erase(end);
            }

            uniforms.emplace_back(uniformName);
        }

        ss << m_line << '\n';
    }

    if (shaderType == 0) {
        return fail("Shader type is not set.");
    }

    return Source{shaderType, ss.str(), uniforms};
}
