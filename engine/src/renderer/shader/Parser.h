#pragma once

#include <fstream>
#include <unordered_map>
#include "Source.h"

namespace Engine::Renderer::Shader {
    class Parser {
    public:
        using ShaderStructsMap = std::unordered_map<std::string, std::vector<std::string> >;

        Parser() = delete;

        Parser(const Parser&) = delete;

        Parser& operator=(const Parser&) = delete;

        Parser(Parser&&) = delete;

        Parser& operator=(Parser&&) = delete;

        ~Parser();

        explicit Parser(const std::string& filePath,
                        const std::shared_ptr<ShaderStructsMap>& shaderStructs = std::make_shared<ShaderStructsMap>());

        void logParseFail(size_t lineNbr, std::string_view lineStr, const std::string& description) const;

        Source operator()();

        Source next() {
            return (*this)();
        }

    private:
        std::string m_filePath;
        std::ifstream m_istream;
        uint32_t m_nextShaderType{Source::s_shaderHeader};

        std::shared_ptr<std::unordered_map<std::string, std::vector<std::string> > > m_shaderStructs;
    };
}
