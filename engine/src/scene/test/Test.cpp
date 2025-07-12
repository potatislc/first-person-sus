#include "Test.h"

#include "core/Application.h"
#include "renderer/shader/Parser.h"
#include <imgui.h>
#include <memory>

Engine::Scene::Test::Test() : m_shaderProgram{Renderer::Shader::Parser{"../engine/res/shader/test/Basic.glsl"}},
                              m_texture{
                                  Renderer::Texture::createGlTexture("../engine/res/texture/Melon.png")
                              } {
    const Renderer::Buffer::Vertex::Layout vertexLayout{
        glm::vec2{},
        glm::vec2{}
    };

    const std::vector krazyBufferData{
        Renderer::Buffer::copyBufferData(s_krazySquareVertices.data(), sizeof(s_krazySquareVertices)),
        Renderer::Buffer::copyBufferData(s_krazySquareTexCoords.data(), sizeof(s_krazySquareTexCoords))
    };

    const auto interleavedKrazyBufferData = Renderer::Buffer::Vertex::layoutInterleave(vertexLayout, krazyBufferData);

    const std::vector squaresData{
        Renderer::Buffer::copyBufferData(s_square.data(), sizeof(s_square)),
        Renderer::Buffer::copyBufferData(s_square2.data(),
                                         sizeof(s_square2)),
        interleavedKrazyBufferData
    };

    Renderer::Buffer::Vertex vertexBuffer{vertexLayout, Renderer::Buffer::batchBufferData(squaresData)};

    auto indexData = Renderer::Buffer::copyIndexData(s_indices.data(), s_indices.size(), 3);

    m_vertexArray = std::make_unique<Renderer::VertexArray>(std::move(vertexBuffer), indexData);
    m_texture.bind(0);
    m_shaderProgram.setUniform("u_Texture", 0);
}

void Engine::Scene::Test::update(const float deltaTime) {
}

void Engine::Scene::Test::render(const Renderer::Renderer& renderer) {
    const auto blue = static_cast<float>(Application::getInstance().getFrameCount() % 256) / 255.0f;

    renderer.clear({0.1f, 0.2f, blue, 1.0f});

    m_shaderProgram.bind();
    m_shaderProgram.setUniform("u_Color", glm::vec4{blue, .3f, .8f, 1.f});
    auto model = glm::translate(glm::mat4{1.f}, m_translation);
    auto mvp = m_projection * m_view * model;
    m_shaderProgram.setUniform("u_Mvp", mvp);
    renderer.draw(*m_vertexArray, m_shaderProgram);

    /*model = glm::translate(model, m_translation);
    mvp = m_projection * m_view * model;
    m_shaderProgram.setUniform("u_Mvp", mvp);
    renderer.draw(m_vertexArray, m_shaderProgram);*/
}

void Engine::Scene::Test::renderImGui() {
    ImGui::SliderFloat3("float", &m_translation[0], 0.0f, 960.0f);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
