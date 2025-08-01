#include "ObjParser.h"

#include <fstream>
#include <iomanip>
#include <optional>
#include <array>

#include "core/Assert.h"
#include "core/StringUtils.h"

using MeshData = Engine::Renderer::MeshData;
using ObjParser = Engine::Renderer::ObjParser;


Engine::Renderer::ObjParser::ObjParser(const std::string& path) : m_filePath(path) {
    ASSERT_MSG(path.ends_with(".obj"),
               "In Engine::Renderer::Model::loadObjModel(): File is not of supported type '.obj': " << path);
    ASSERT_MSG(m_filePath.is_open(),
               "In Engine::Renderer::Model::loadObjModel(): Could not open file with path: " << path);
}

template<typename T>
static T parseVec(auto& it, const auto& end) {
    T vec{};

    for (int i{}; i < glm::vec3::length(); i++) {
        float val{};
        auto res = std::from_chars(it->data(), it->data() + it->size(), val);
        vec[i] = val;

        if (res.ec != std::errc()) {
            LOG_ERR("Malformed float: ptr -> " << std::quoted(res.ptr));
            break;
        }

        ++it;
        if (it >= end) {
            break;
        }
    }

    return vec;
}

static bool matchAndAdvance(auto& it, const auto& end, const std::string& token) {
    if (it != end && *it == token) {
        ++it;
        return true;
    }

    return false;
}

static void appendIndices(MeshData& mesh, std::string_view faceStr) {
    std::array faceData{
        &mesh.positionIndices,
        &mesh.textureCoordIndices,
        &mesh.normalIndices
    };

    auto faceIt = faceData.begin();
    size_t endPos = faceStr.find('/');

    auto advanceFaceIt = [&] {
        ++faceIt;
        if (endPos == std::string::npos) {
            faceStr = {};
        } else {
            faceStr = faceStr.substr(endPos + 1);
            endPos = faceStr.find('/');
        }
    };

    // Face data is one-indexed inside the .obj-format, while opengl needs a zero-indexed index buffer
    auto toZeroIndexed = [](const auto val) {
        return val - 1;
    };

    while (faceIt != faceData.end()) {
        uint32_t val{};
        const std::string_view parseStr = (endPos == std::string::npos) ? faceStr : faceStr.substr(0, endPos);
        if (parseStr.empty()) {
            advanceFaceIt();
            continue;
        }

        if (const auto [ptr, ec] = std::from_chars(parseStr.data(), parseStr.data() + parseStr.size(), val);
            ec != std::errc()) {
            LOG_ERR("Malformed uint32_t: " << std::quoted(parseStr));
            advanceFaceIt();
            continue;
        }

        (*faceIt)->emplace_back(toZeroIndexed(val));
        advanceFaceIt();
    }
}

MeshData ObjParser::operator()() {
    MeshData mesh;
    std::string line;

    while (std::getline(m_filePath, line)) {
        removeLineComment(line, "#");
        const auto tokens = tokenize(line);

        if (tokens.size() < 2) {
            continue;
        }

        auto tokenIt = tokens.begin();

        if (matchAndAdvance(tokenIt, tokens.end(), "v")) {
            mesh.positions.emplace_back(parseVec<decltype(mesh.positions)::value_type>(tokenIt, tokens.end()));
            continue;
        }

        if (matchAndAdvance(tokenIt, tokens.end(), "vn")) {
            mesh.normals.emplace_back(parseVec<decltype(mesh.normals)::value_type>(tokenIt, tokens.end()));
            continue;
        }

        if (matchAndAdvance(tokenIt, tokens.end(), "vt")) {
            mesh.textureCoords.emplace_back(parseVec<decltype(mesh.textureCoords)::value_type>(tokenIt, tokens.end()));
            continue;
        }

        if (matchAndAdvance(tokenIt, tokens.end(), "f")) {
            appendIndices(mesh, *tokenIt);
            continue;
        }

        if (matchAndAdvance(tokenIt, tokens.end(), "o")) {
            if (!mesh.isEmpty()) {
                return mesh;
            }
        }
    }

    return mesh;
}
