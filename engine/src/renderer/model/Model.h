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

        const auto& getTextures() const {
            return m_textures;
        }

    private:
        explicit Model::Model(VertexArray vao, std::vector<Texture> textures = {}): m_vao(std::move(vao)),
            m_textures(std::move(textures)) {
        }

        VertexArray m_vao;
        std::vector<Texture> m_textures;
    };
}
