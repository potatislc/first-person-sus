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

Engine::Renderer::ObjParser::ObjParser(const std::string& path) : m_file(path) {
    ASSERT_MSG(path.ends_with(".obj"),
               "In Engine::Renderer::Model::loadObjModel(): File is not of supported type '.obj': " << path);
    ASSERT_MSG(m_file.is_open(),
               "In Engine::Renderer::Model::loadObjModel(): Could not open file with path: " << path);
}

template<typename T, typename = std::enable_if_t<std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3>> >
static T parseVec(auto& it, const auto& end) {
    T vec{};

    for (size_t i{}; i < T::length(); i++) {
        float val{};
        auto res = std::from_chars(it->data(), it->data() + it->size(), val);
        vec[i] = val;

        ASSERT_MSG(res.ec == std::errc(), "Malformed float: ptr -> " << std::quoted(res.ptr));

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

// Apparently you may need to account for negative indices which mean an index previous to the latest
static uint32_t rebase_index(const int i, const size_t n) {
    // OBJ is 1-based.
    const int idx = (i > 0) ? (i - 1) : (static_cast<int>(n) + i);
    ASSERT_MSG(idx >= 0 && std::cmp_less(idx , n), "Index out of range");
    return static_cast<uint32_t>(idx);
}

static VertexKey parseFaceElement(std::string_view elem,
                                  const size_t nPos, const size_t nTex, const size_t nNorm) {
    VertexKey key{};
    const auto firstSlash = elem.find('/');
    const auto secondSlash = (firstSlash == std::string::npos)
                                 ? std::string::npos
                                 : elem.find('/', firstSlash + 1);

    auto to_int = [](const std::string_view s) -> int {
        int v{};
        auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
        ASSERT_MSG(ec == std::errc(), "Malformed integer in face element");
        return v;
    };


    // v
    key.posIdx = rebase_index(to_int(elem.substr(0, firstSlash)), nPos);

    // vt
    if (firstSlash != std::string::npos && secondSlash > firstSlash + 1) {
        key.texIdx = rebase_index(to_int(elem.substr(firstSlash + 1,
                                                     secondSlash - firstSlash - 1)), nTex);
    }

    // vn
    if (secondSlash != std::string::npos && secondSlash + 1 < elem.size()) {
        key.normIdx = rebase_index(to_int(elem.substr(secondSlash + 1)), nNorm);
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
        mesh.positions.emplace_back(rawPositions[key.posIdx]);
        // Keep arrays aligned
        mesh.textureCoords.emplace_back(key.texIdx ? rawTexCoords[*key.texIdx] : glm::vec2(0.0f));
        mesh.normals.emplace_back(key.normIdx ? rawNormals[*key.normIdx] : glm::vec3(0.0f));
    }
    return it->second;
}


MeshData ObjParser::operator()() {
    std::vector<glm::vec3> rawPositions;
    std::vector<glm::vec2> rawTexCoords;
    std::vector<glm::vec3> rawNormals;

    MeshData meshData;
    std::unordered_map<VertexKey, uint32_t, VertexKey::Hash> vertexMap;

    std::string line;
    while (std::getline(m_file, line)) {
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
            std::vector<uint32_t> face;
            for (; it != tokens.end(); ++it) {
                const auto key = parseFaceElement(*it, rawPositions.size(), rawTexCoords.size(), rawNormals.size());
                face.push_back(getOrCreateVertex(key, meshData, vertexMap, rawPositions, rawTexCoords, rawNormals));
            }

            // triangulate: (0, i-1, i)
            for (size_t i = 2; i < face.size(); ++i) {
                meshData.indices.push_back(face[0]);
                meshData.indices.push_back(face[i - 1]);
                meshData.indices.push_back(face[i]);
            }
        }
    }

    return meshData;
}
