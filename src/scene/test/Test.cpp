#include "Test.h"

#include "../../application/Application.h"
#include "../../renderer/shader/Parser.h"
#include <imgui.h>

Scene::Test::Test() : shaderProgram{Renderer::Shader::Parser{"../res/shader/Basic.glsl"}},
                      texture{
                          Renderer::Texture::createGlTexture("../res/texture/Melon.png")
                      } {
    m_VertexLayout.push<float>(glm::vec2::length());
    m_VertexLayout.push<float>(glm::vec2::length());
    m_VertexArray.addBuffer(m_VertexBuffer, m_VertexLayout);

    texture.bind(0);
    shaderProgram.setUniform("u_Texture", 0);
}

void Scene::Test::update(const float deltaTime) {
}

void Scene::Test::render(const Renderer::Renderer& renderer) {
    const auto blue = static_cast<float>(Application::getGlobalInstance().getFrameCount() % 256) / 255.0f;

    renderer.clear({0.1f, 0.2f, blue, 1.0f});

    shaderProgram.bind();
    shaderProgram.setUniform("u_Color", glm::vec4{blue, .3f, .8f, 1.f});
    const auto model = glm::translate(glm::mat4{1.f}, translation);
    const auto mvp = m_Projection * m_View * model;
    shaderProgram.setUniform("u_Mvp", mvp);
    renderer.draw(m_VertexArray, m_IndexBuffer, shaderProgram);
}

void Scene::Test::renderImGui() {
    ImGui::SliderFloat3("float", &translation[0], 0.0f, 960.0f);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000. / io.Framerate, io.Framerate);
}
