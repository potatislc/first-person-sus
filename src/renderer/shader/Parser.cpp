#include "Parser.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

Renderer::Shader::Parser::Parser(const std::string& filePath) : m_Stream{filePath} {
    if (m_Stream.bad()) {
        std::cerr << "Invalid shader source path" << filePath << '\n';
    }

    assert(m_Stream.good());
}

Renderer::Shader::Source Renderer::Shader::Parser::operator()() {
    auto fail = [this](const std::string& description) {
        std::cerr << "Failed to parse shader source: " << description << " At line "
                << m_LineCount << ". " << "[LINE SOURCE]: " << m_Line << '\n';
        return Source{};
    };

    std::stringstream ss;
    uint32_t shaderType = m_NextShaderType;

    while (std::getline(m_Stream, m_Line)) {
        ++m_LineCount;

        if (std::ranges::all_of(m_Line,
                                [](const unsigned char c) { return std::isspace(c); })) {
            continue;
        }

        if (m_Line.contains("#shader")) {
            if (m_Line.contains("vertex")) {
                m_NextShaderType = GL_VERTEX_SHADER;
            } else if (m_Line.contains("fragment")) {
                m_NextShaderType = GL_FRAGMENT_SHADER;
            } else {
                return fail("Could not deduce shader type.");
            }

            if (shaderType != 0) {
                break;
            }

            shaderType = m_NextShaderType;

            continue;
        }

        ss << m_Line << '\n';
    }

    if (shaderType == 0) {
        return fail("Shader type is not set.");
    }

    return Source{shaderType, ss.str()};
}
