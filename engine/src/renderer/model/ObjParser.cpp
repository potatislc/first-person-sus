#include "ObjParser.h"

#include <fstream>
#include <iomanip>
#include <optional>
#include <array>
#include <unordered_map>

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

struct VertexKey {
    bool operator==(const VertexKey& other) const noexcept {
        return posIdx == other.posIdx &&
               texIdx == other.texIdx &&
               normIdx == other.normIdx;
    }

    struct Hash {
        std::size_t operator()(const VertexKey& k) const noexcept {
            auto h = std::hash<uint32_t>{}(k.posIdx);
            if (k.texIdx) {
                h ^= std::hash<uint32_t>{}(*k.texIdx + 1);
            }

            if (k.normIdx) {
                h ^= std::hash<uint32_t>{}(*k.normIdx + 2);
            }
            return h;
        }
    };

    uint32_t posIdx;
    std::optional<uint32_t> texIdx;
    std::optional<uint32_t> normIdx;
};

static VertexKey parseFaceElement(const std::string_view elem) {
    VertexKey key{};
    const auto firstSlash = elem.find('/');
    const auto secondSlash = elem.find('/', firstSlash + 1);

    // A position index must be provided otherwise parsing won't work
    key.posIdx = std::stoul(std::string(elem.substr(0, firstSlash))) - 1;

    if (firstSlash != std::string::npos && secondSlash > firstSlash + 1) {
        key.texIdx = std::stoul(std::string(elem.substr(firstSlash + 1, secondSlash - firstSlash - 1))) - 1;
    }

    if (secondSlash != std::string::npos && secondSlash + 1 < elem.size()) {
        key.normIdx = std::stoul(std::string(elem.substr(secondSlash + 1))) - 1;
    }

    return key;
}

uint32_t getOrCreateVertex(const VertexKey& key,
                           MeshData& mesh,
                           std::unordered_map<VertexKey, uint32_t, VertexKey::Hash>& vertexMap,
                           const std::vector<glm::vec3>& rawPositions,
                           const std::vector<glm::vec2>& rawTexCoords,
                           const std::vector<glm::vec3>& rawNormals) {
    auto [it, inserted] = vertexMap.try_emplace(key, static_cast<uint32_t>(mesh.positions.size()));
    if (inserted) {
        // Create vertex
        mesh.positions.emplace_back(rawPositions[key.posIdx]);

        if (key.texIdx) {
            mesh.textureCoords.emplace_back(rawTexCoords[*key.texIdx]);
        }

        if (key.normIdx) {
            mesh.normals.emplace_back(rawNormals[*key.normIdx]);
        }

        return it->second;
    }

    // Return already existing vertex
    return it->second;
}

MeshData ObjParser::operator()() {
    std::vector<glm::vec3> rawPositions;
    std::vector<glm::vec2> rawTexCoords;
    std::vector<glm::vec3> rawNormals;

    MeshData meshData;
    std::unordered_map<VertexKey, uint32_t, VertexKey::Hash> vertexMap;

    std::string line;
    while (std::getline(m_filePath, line)) {
        removeLineComment(line, "#");
        auto tokens = tokenize(line);

        if (tokens.empty()) {
            continue;
        }

        if (auto it = tokens.begin(); matchAndAdvance(it, tokens.end(), "v")) {
            rawPositions.emplace_back(parseVec<glm::vec3>(it, tokens.end()));
        } else if (matchAndAdvance(it, tokens.end(), "vt")) {
            rawTexCoords.emplace_back(parseVec<glm::vec2>(it, tokens.end()));
        } else if (matchAndAdvance(it, tokens.end(), "vn")) {
            rawNormals.emplace_back(parseVec<glm::vec3>(it, tokens.end()));
        } else if (matchAndAdvance(it, tokens.end(), "f")) {
            for (; it != tokens.end(); ++it) {
                const auto key = parseFaceElement(*it);
                const auto idx = getOrCreateVertex(key, meshData, vertexMap, rawPositions, rawTexCoords, rawNormals);
                meshData.indices.push_back(idx);
            }
        }
    }

    return meshData;
}
