#include <sstream>
#include <glad/glad.h>

#include "Parser.h"
#include "core/Assert.h"

namespace Shader = Engine::Renderer::Shader;
using size_type = std::string::size_type;

struct StringReplace {
    const char* from;
    const char* to;
};

static constexpr StringReplace engineResPath{.from = "ENGINE_RES_PATH", .to = ENGINE_RES_PATH};

Shader::Parser::~Parser() {
    m_istream.close();
}

Shader::Parser::Parser(const std::string& filePath) : m_istream{filePath} {
    if (!m_istream.is_open()) {
        LOG_ERR("Invalid shader source path: " << filePath << '\n');
    }

    ASSERT(m_istream.is_open());
}

static void removeLineComment(std::string& line) {
    static constexpr auto commentChar{"//"};

    if (const auto commentStart = line.find(commentChar); commentStart != std::string::npos) {
        line.resize(commentStart);
    }
}

static void ltrim(std::string_view& s) {
    s.remove_prefix(std::min(s.find_first_not_of(" \t\n\r\f\v;"), s.size()));
}

struct Token {
    std::string_view name;
    size_t lineIndex;
};

using Statement = std::vector<Token>;

static std::vector<std::string_view> tokenize(std::string_view line) {
    std::vector<std::string_view> tokens;
    ltrim(line);

    while (!line.empty()) {
        if (const size_type tokenEnd = line.find_first_of(" \t\n\r\f\v;"); tokenEnd != std::string::npos) {
            tokens.emplace_back(line.substr(0, tokenEnd));
            line.remove_prefix(tokenEnd);
        } else {
            tokens.emplace_back(line);
            break;
        }

        ltrim(line);
    }

    return tokens;
}

uint32_t toGlShaderType(const std::string_view& type) {
    if (type == "vertex") {
        return GL_VERTEX_SHADER;
    }

    if (type == "fragment") {
        return GL_FRAGMENT_SHADER;
    }

    return {};
}

static void findAndReplace(std::string& str, const std::string& toFind, const std::string& toReplace) {
    if (const size_type pos = str.find(toFind); pos != std::string::npos) {
        str.replace(pos, toFind.size(), toReplace);
    }
}

Shader::Source Shader::Parser::operator()() {
    size_t lineNumber{1};
    std::string line;

    uint32_t shaderType{}; // This probably not needed if you play your cards right
    std::stringstream resultStream;
    std::vector<Uniform> uniforms;

    if (m_nextShaderType != 0) {
        shaderType = m_nextShaderType;
    }

    while (std::getline(m_istream, line)) {
        removeLineComment(line);
        if (line.empty()) {
            continue;
        }

        const auto tokens{tokenize(line)};

        if (tokens.size() <= 1) {
            resultStream << line << '\n';
            lineNumber++;
            continue;
        }

        for (size_t i{}; i < tokens.size() - 1; i++) {
            const auto token = tokens[i];

            if (token == "uniform") {
                // Scariest 2 of all time
                uniforms.emplace_back(std::string{tokens[i + 2]});
                continue;
            }

            if (token == "#include") {
                const auto nextTokenCopy{std::string{tokens[i + 1]}};

                auto includePath{nextTokenCopy};
                if (includePath.starts_with(engineResPath.from)) {
                    includePath.replace(0, std::strlen(engineResPath.from), engineResPath.to);
                }

                findAndReplace(line, std::string{token}, "");
                findAndReplace(line, nextTokenCopy, "");

                Parser includeParser{includePath};
                while (const auto source{includeParser.next()}) {
                    resultStream << source.getSource();
                    uniforms.insert(uniforms.end(), source.getUniforms().begin(), source.getUniforms().end());
                }
            }

            if (token == "#shader") {
                if (shaderType != 0) {
                    m_nextShaderType = toGlShaderType(tokens[i + 1]);
                    LOG(resultStream.str());
                    return Source{shaderType, resultStream.str(), uniforms};
                }

                shaderType = toGlShaderType(tokens[i + 1]);

                if (shaderType == 0) {
                    // Fail!
                    return Source{};
                }

                const auto nextTokenCopy{std::string{tokens[i + 1]}};
                findAndReplace(line, std::string{token}, "");
                findAndReplace(line, nextTokenCopy, "");
            }
        }

        resultStream << line << '\n';
        lineNumber++;
    }

    // Don't care about simply shader data files only full sources
    if (shaderType != 0) {
        LOG(resultStream.str());
    }

    return Source{shaderType, resultStream.str(), uniforms};
}
