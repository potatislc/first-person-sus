#include "Parser.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>

#include  "core/Assert.h"

using size_type = std::string::size_type;

struct StringReplace {
    const char* from;
    const char* to;
};

static constexpr StringReplace engineResPath{.from = "ENGINE_RES_PATH", .to = ENGINE_RES_PATH};

Engine::Renderer::Shader::Parser::~Parser() {
    m_istream.close();
}

Engine::Renderer::Shader::Parser::Parser(const std::string& filePath) : m_istream{filePath} {
    if (!m_istream.is_open()) {
        LOG_ERR("Invalid shader source path: " << filePath << '\n');
    }

    ASSERT(m_istream.is_open());
}

// Maybe used for other parsers in the future
class ParseLine {
public:
    explicit ParseLine(std::istream& is) : m_istream(&is) {
    }

    [[nodiscard]] bool isEmpty() const {
        return m_line.empty();
    }

    bool next() {
        const auto validLine = static_cast<bool>(std::getline(*m_istream, m_line));
        m_lineCount++;

        // Remove blank lines? Who Cares?
        /*while (std::ranges::all_of(m_line,
                                   [](const unsigned char c) { return std::isspace(c); })) {
            // Idk
        }*/

        if (const auto commentStart = m_line.find(s_supportedCommentStyle); commentStart != std::string::npos) {
            m_line = m_line.substr(0, commentStart);
        }

        return validLine;
    }

    [[nodiscard]] const std::string& getString() const {
        return m_line;
    }

    [[nodiscard]] size_t getLineCount() const {
        return m_lineCount;
    }

    // Ignores ';'
    [[nodiscard]] std::string getLastToken() const {
        auto token = m_line.substr(m_line.find_last_of(' ') + 1);

        if (const size_t end = token.find_first_of(';'); end != std::string::npos) {
            token.erase(end);
        }

        return token;
    }

private:
    static constexpr auto s_supportedCommentStyle{"//"};

    std::istream* m_istream{};
    std::string m_line;
    size_t m_lineCount{};
};

[[nodiscard]] static bool processUniform(const ParseLine& parseLine,
                                         std::vector<Engine::Renderer::Shader::Uniform>& uniforms) {
    if (parseLine.getString().starts_with("uniform ")) {
        const auto uniformName = parseLine.getLastToken();
        uniforms.emplace_back(uniformName);
        return true;
    }

    return false;
}

Engine::Renderer::Shader::Source Engine::Renderer::Shader::Parser::operator()() {
    auto fail = [this](const ParseLine& parseLine, const std::string& description) {
        LOG_ERR("Failed to parse shader source: " << description << " At line "
            << parseLine.getLineCount() << ". " << "[LINE SOURCE]: " << parseLine.getString() << '\n');
        return Source{};
    };

    ParseLine parseLine{m_istream};
    std::stringstream resStream;
    uint32_t shaderType = m_nextShaderType;
    std::vector<Uniform> uniforms;

    while (parseLine.next()) {
        if (parseLine.getString().starts_with("#shader")) {
            if (parseLine.getString().contains("vertex")) {
                m_nextShaderType = GL_VERTEX_SHADER;
            } else if (parseLine.getString().contains("fragment")) {
                m_nextShaderType = GL_FRAGMENT_SHADER;
            } else {
                return fail(parseLine, "Could not deduce shader type.");
            }

            if (shaderType != 0) {
                break;
            }

            shaderType = m_nextShaderType;

            continue;
        }

        if (parseLine.getString().starts_with("#include ")) {
            std::string includePath = parseLine.getLastToken();
            if (const size_type pos = includePath.find(engineResPath.from); pos != std::string::npos) {
                includePath.replace(pos, std::strlen(engineResPath.from), engineResPath.to);
            }

            std::ifstream includeStream{includePath};
            if (!includeStream.is_open()) {
                includeStream.close();
                return fail(parseLine, "Invalid include path: " + includePath + ".");
            }

            ParseLine parseIncludeLine{includeStream};
            while (parseIncludeLine.next()) {
                [[maybe_unused]] const auto uniformFound{processUniform(parseIncludeLine, uniforms)};
                resStream << parseIncludeLine.getString() << '\n';
            }

            includeStream.close();
            continue;
        }

        [[maybe_unused]] const auto uniformFound{processUniform(parseLine, uniforms)};

        resStream << parseLine.getString() << '\n';
    }

    if (shaderType == 0) {
        return fail(parseLine, "Shader type is not set.");
    }

    LOG(resStream.str());

    return Source{shaderType, resStream.str(), uniforms};
}
