#pragma once

#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include "Source.h"

namespace Engine::Renderer::Shader {
    class Parser {
    public:
        using ShaderStructsMap = std::unordered_map<std::string, std::vector<std::string> >;
        using ShaderDependencySet = std::unordered_set<std::string>;

        struct ParseCache {
            void clear() {
                includedPaths.clear();
                shaderStructs.clear();
            }

            ShaderDependencySet includedPaths;
            ShaderStructsMap shaderStructs;
        };

        Parser() = delete;

        Parser(const Parser&) = delete;

        Parser& operator=(const Parser&) = delete;

        Parser(Parser&&) = delete;

        Parser& operator=(Parser&&) = delete;

        ~Parser();

        explicit Parser(const std::string& filePath,
                        const std::shared_ptr<ParseCache>& parseCache = std::make_shared<ParseCache>());

        void logParseFail(size_t lineNbr, std::string_view lineStr, const std::string& description) const;

        [[nodiscard]] Source buildSource(uint32_t shaderType, const std::string& sourceString,
                                         const std::vector<Uniform>& uniforms) const;

        Source operator()();

        Source next() {
            return (*this)();
        }

    private:
        std::string m_filePath;
        std::ifstream m_istream;
        uint32_t m_nextShaderType{Source::s_shaderHeader};
        std::shared_ptr<ParseCache> m_parseCache;
    };
}
