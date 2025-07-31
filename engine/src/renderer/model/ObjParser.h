#pragma once

#include <fstream>
#include <string>

#include "MeshData.h"

namespace Engine::Renderer {
    class ObjParser {
    public:
        ObjParser(const std::string& path);

        MeshData operator()();

        MeshData next() {
            return (*this)();
        }

    private:
        std::ifstream m_filePath;
    };
}
