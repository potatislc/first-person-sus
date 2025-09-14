#include "Model.h"

#include <fstream>

#include "../Renderer.h"
#include "../shader/Program.h"

Engine::Renderer::Model Engine::Renderer::Model::generate(const MeshData& meshData, std::vector<Texture> textures) {
    const auto layout = MeshData::baseLayout();

    const auto interleavedData = Buffer::Vertex::layoutInterleave(layout, meshData.getVertexData());

    Buffer::Vertex vertexBuffer{layout, interleavedData};

    return Model{
        VertexArray{std::move(vertexBuffer), meshData.indices},
        std::move(textures)
    };
}

void Engine::Renderer::Model::draw(const Shader::Program& shaderProgram) const {
    m_vertexArray.bind();
    shaderProgram.bind();
    RENDERER_API_CALL(
        glDrawElements(GL_TRIANGLES, m_vertexArray.getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr));
}

void Engine::Renderer::Model::drawInstanced(const Shader::Program& shaderProgram, const void* instanceData,
                                            const uint32_t instanceCount) const {
    ASSERT_MSG(m_vertexArray.isInstantiable(), "Model cannot be drawn instanced. No instance buffer is set.");

    m_vertexArray.bind();
    m_vertexArray.updateInstanceBuffer(instanceData, instanceCount);
    shaderProgram.bind();
    RENDERER_API_CALL(
        glDrawElementsInstanced(GL_TRIANGLES, m_vertexArray.getIndexBuffer().getCount(), GL_UNSIGNED_INT,
            nullptr, instanceCount));
}

void Engine::Renderer::Model::setInstanceBuffer(Buffer::Vertex instanceBuffer) {
    m_vertexArray.setInstanceBuffer(std::move(instanceBuffer));
}
