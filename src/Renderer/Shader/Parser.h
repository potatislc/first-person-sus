#ifndef SOURCEPARSER_H
#define SOURCEPARSER_H

#include <fstream>
#include <iostream>
#include "Source.h"

namespace Renderer::Shader {
    class Parser {
    public:
        Parser() = delete;

        Parser(const Parser&) = delete;

        Parser& operator=(const Parser&) = delete;

        Parser(Parser&&) = delete;

        Parser& operator=(Parser&&) = delete;

        ~Parser() = default;

        explicit Parser(const std::string& filePath);

        Source operator()(const Renderer& renderer);

        Source next(const Renderer& renderer) {
            return (*this)(renderer);
        }

    private:
        std::ifstream m_Stream;
        std::string m_Line;
        size_t m_LineCount{};
        unsigned int m_NextShaderType{};
    };
}

#endif //SOURCEPARSER_H
