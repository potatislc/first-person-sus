#pragma once

#include <fstream>
#include "Source.h"

namespace Engine::Renderer::Shader {
    class Parser {
    public:
        Parser() = delete;

        Parser(const Parser&) = delete;

        Parser& operator=(const Parser&) = delete;

        Parser(Parser&&) = delete;

        Parser& operator=(Parser&&) = delete;

        ~Parser() = default;

        explicit Parser(const std::string& filePath);

        Source operator()();

        Source next() {
            return (*this)();
        }

    private:
        std::ifstream m_stream;
        uint32_t m_nextShaderType{};
    };
}
