#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "renderer/buffer/Vertex.h"

namespace Engine::Renderer {
    struct MeshData {
        static Buffer::Vertex::Layout baseLayout() {
            return Buffer::Vertex::Layout{
                decltype(positions)::value_type{},
                decltype(textureCoords)::value_type{}, decltype(normals)::value_type{}
            };
        }

        [[nodiscard]] bool isEmpty() const {
            return positions.empty() && normals.empty() && textureCoords.empty();
        }

        [[nodiscard]] std::vector<Buffer::BufferData> getVertexData() const {
            return std::vector{
                Buffer::copyBufferData(positions), Buffer::copyBufferData(textureCoords),
                Buffer::copyBufferData(normals)
            };
        }

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> textureCoords;
        std::vector<glm::vec3> normals;
        std::vector<uint32_t> indices;
    };
}
