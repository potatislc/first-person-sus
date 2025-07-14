#include <sstream>
#include <unordered_map>
#include <unordered_set>
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

Shader::Parser::Parser(const std::string& filePath, const std::shared_ptr<ParseCache>& parseCache) : m_filePath{
        filePath
    },
    m_istream{filePath}, m_parseCache{parseCache} {
    if (!m_istream.is_open()) {
        LOG_ERR("Invalid shader source path: " << filePath << '\n');
    }

    ASSERT(m_istream.is_open());

    m_parseCache->includedPaths.emplace(filePath);
}

static void removeLineComment(std::string& line) {
    static constexpr auto commentChar{"//"};

    if (const auto commentStart = line.find(commentChar); commentStart != std::string::npos) {
        line.resize(commentStart);
    }
}

static void ltrim(std::string_view& s, const std::string& predicate = " \t\n\r\f\v;") {
    s.remove_prefix(std::min(s.find_first_not_of(predicate), s.size()));
}

struct Token {
    std::string_view name;
    size_t lineIndex;
};

using Statement = std::vector<Token>;

static std::vector<std::string_view> tokenize(std::string_view line, const std::string& predicate = " \t\n\r\f\v;") {
    std::vector<std::string_view> tokens;
    ltrim(line);

    while (!line.empty()) {
        if (const size_type tokenEnd = line.find_first_of(predicate); tokenEnd != std::string::npos) {
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

void Shader::Parser::logParseFail(const size_t lineNbr, const std::string_view lineStr,
                                  const std::string& description) const {
    LOG_ERR("Failed to parse shader source: " << description << "Inside file: " << m_filePath << ". At line "
        << lineNbr << ". [LINE SOURCE]: " << lineStr << '\n');
}

template<typename... Args>
static void findAndRemoveTokens(std::string& line, Args&&... tokens) {
    (findAndReplace(line, std::forward<Args>(tokens), ""), ...);
}

Shader::Source Shader::Parser::buildSource(const uint32_t shaderType, const std::string& sourceString,
                                           const std::vector<Uniform>& uniforms) const {
    if (m_parseCache.use_count() == 1) {
        m_parseCache->clear();
    }

    return Source{shaderType, sourceString, uniforms};
}

class LineStream {
public:
    void operator<<(const std::string& line) {
        resultStream << line;
        m_lineNbr++;
    }

    [[nodiscard]] const std::stringstream& getResult() const {
        return resultStream;
    }

    [[nodiscard]] size_t getLineNbr() const {
        return m_lineNbr;
    }

private:
    std::stringstream resultStream;
    size_t m_lineNbr{1};
};

Shader::Source Shader::Parser::operator()() {
    std::string line;
    LineStream lineStream;

    uint32_t shaderType{m_nextShaderType};
    std::vector<Uniform> uniforms;
    std::string shaderStructTypeName;

    while (std::getline(m_istream, line)) {
        removeLineComment(line);
        if (line.empty()) {
            continue;
        }

        if (!shaderStructTypeName.empty()) {
            const auto tokens{tokenize(line, " \t\n\f\v")};

            auto it = tokens.begin();
            while (it != tokens.end()) {
                if (it->contains('}')) {
                    shaderStructTypeName = "";
                    break;
                }

                if (it->ends_with(';')) {
                    const auto trimmedToken = it->substr(0, it->find_first_of(';'));
                    m_parseCache->shaderStructs[shaderStructTypeName].emplace_back(trimmedToken);

                    ++it;
                    continue;
                }

                if (const auto nextIt = it + 1; nextIt != tokens.end() && nextIt->starts_with(';')) {
                    m_parseCache->shaderStructs[shaderStructTypeName].emplace_back(*it);
                }

                ++it;
            }
        }

        const auto tokens{tokenize(line)};

        if (tokens.size() <= 1) {
            lineStream << line + '\n';
            continue;
        }

        for (size_t i{}; i < tokens.size() - 1; i++) {
            const auto token = tokens[i];

            if (token == "uniform") {
                if (i + 2 >= tokens.size()) {
                    logParseFail(lineStream.getLineNbr(), line,
                                 "Incomplete uniform declaration. Uniform name not found.");
                }

                const auto uniformType{std::string{tokens[i + 1]}};
                const auto uniformName{std::string{tokens[i + 2]}};

                if (m_parseCache->shaderStructs.contains(uniformType)) {
                    for (const auto& memberName: m_parseCache->shaderStructs[uniformType]) {
                        uniforms.emplace_back(uniformName + "." + memberName);
                    }
                } else {
                    uniforms.emplace_back(uniformName);
                }
                continue;
            }

            if (token == "#include") {
                const auto nextTokenCopy{std::string{tokens[i + 1]}};

                if (m_parseCache->includedPaths.contains(nextTokenCopy)) {
                    findAndRemoveTokens(line, std::string{token}, nextTokenCopy);
                    continue;
                }

                auto includePath{nextTokenCopy};
                if (includePath.starts_with(engineResPath.from)) {
                    includePath.replace(0, std::strlen(engineResPath.from), engineResPath.to);
                }

                Parser includeParser{includePath, m_parseCache};
                while (const auto source{includeParser.next()}) {
                    lineStream << source.getSource();
                    uniforms.insert(uniforms.end(), source.getUniforms().begin(), source.getUniforms().end());
                }

                findAndRemoveTokens(line, std::string{token}, nextTokenCopy);
                continue;
            }

            if (token == "#shader") {
                const auto nextTokenCopy{std::string{tokens[i + 1]}};

                if (shaderType != Source::s_shaderHeader) {
                    m_nextShaderType = toGlShaderType(nextTokenCopy);
                    LOG(lineStream.getResult().str());
                    return buildSource(shaderType, lineStream.getResult().str(), uniforms);
                }

                shaderType = toGlShaderType(nextTokenCopy);

                if (shaderType == Source::s_shaderHeader) {
                    logParseFail(lineStream.getLineNbr(), line,
                                 "Shader type evaluated to 0 (shader header), which is not a compilable shader type. ");
                    return Source{};
                }

                findAndRemoveTokens(line, std::string{token}, nextTokenCopy);
                continue;
            }

            if (token == "struct") {
                shaderStructTypeName = std::string{tokens[i + 1]};
            }
        }

        lineStream << line + '\n';
    }

    if (shaderType != Source::s_shaderHeader) {
        LOG(lineStream.getResult().str());
    }

    return buildSource(shaderType, lineStream.getResult().str(), uniforms);
}
