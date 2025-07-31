#include "Model.h"

#include <fstream>

#include "../Renderer.h"
#include "../shader/Program.h"

Engine::Renderer::Model Engine::Renderer::Model::generate(const MeshData& meshData, std::vector<Texture> textures) {
    const auto layout = MeshData::baseLayout();

    const auto interleavedData = Buffer::Vertex::layoutInterleave(layout, meshData.getVertexData());

    Buffer::Vertex vertexBuffer{layout, interleavedData};

    return Model{
        VertexArray{std::move(vertexBuffer), static_cast<uint32_t>(meshData.positionIndices.size())},
        std::move(textures)
    };
}

void Engine::Renderer::Model::draw(const Shader::Program& shaderProgram) const {
    m_vao.bind();
    shaderProgram.bind();
    RENDERER_API_CALL(glDrawElements(GL_TRIANGLES, m_vao.getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr));
}
