#pragma once
#include "MeshData.h"
#include "../Texture.h"
#include "../VertexArray.h"

namespace Engine::Renderer {
    namespace Shader {
        class Program;
    }

    class Model {
    public:
        static Model generate(const MeshData& meshData, std::vector<Texture> textures = {});

        void draw(const Shader::Program& shaderProgram) const;

        void drawInstanced(const Shader::Program& shaderProgram, const void* instanceData,
                           uint32_t instanceCount) const;

        void setInstanceBuffer(Buffer::Vertex instanceBuffer);

        const auto& getTextures() const {
            return m_textures;
        }

    private:
        explicit Model::Model(VertexArray vao, std::vector<Texture> textures = {}) : m_vertexArray(std::move(vao)),
            m_textures(std::move(textures)) {
        }

        VertexArray m_vertexArray;
        std::vector<Texture> m_textures;
    };
}
