#include "Test.h"

#include "../../core/Application.h"
#include "../../renderer/shader/Parser.h"
#include <imgui.h>

Scene::Test::Test() : m_shaderProgram{Renderer::Shader::Parser{"../res/shader/Basic.glsl"}},
                      m_texture{
                          Renderer::Texture::createGlTexture("../res/texture/Melon.png")
                      } {
    m_vertexLayout.push<float>(glm::vec2::length());
    m_vertexLayout.push<float>(glm::vec2::length());
    m_vertexArray.addBuffer(m_vertexBuffer, m_vertexLayout);

    m_texture.bind(0);
    m_shaderProgram.setUniform("u_Texture", 0);
}

void Scene::Test::update(const float deltaTime) {
}

void Scene::Test::render(const Renderer::Renderer& renderer) {
    const auto blue = static_cast<float>(Application::getUniqueInstance().getFrameCount() % 256) / 255.0f;

    renderer.clear({0.1f, 0.2f, blue, 1.0f});

    m_shaderProgram.bind();
    m_shaderProgram.setUniform("u_Color", glm::vec4{blue, .3f, .8f, 1.f});
    auto model = glm::translate(glm::mat4{1.f}, m_translation);
    auto mvp = m_projection * m_view * model;
    m_shaderProgram.setUniform("u_Mvp", mvp);
    renderer.draw(m_vertexArray, m_indexBuffer, m_shaderProgram);

    model = glm::translate(model, m_translation);
    mvp = m_projection * m_view * model;
    m_shaderProgram.setUniform("u_Mvp", mvp);
    renderer.draw(m_vertexArray, m_indexBuffer, m_shaderProgram);
}

void Scene::Test::renderImGui() {
    ImGui::SliderFloat3("float", &m_translation[0], 0.0f, 960.0f);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000. / io.Framerate, io.Framerate);
}
