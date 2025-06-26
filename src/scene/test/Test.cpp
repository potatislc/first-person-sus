#include "Test.h"

#include "../../core/Application.h"
#include "../../renderer/shader/Parser.h"
#include <imgui.h>
#include <memory>

// I know this is the most horrible member initializer list you have ever seen,
// but it is only to demonstrate that you can construct a vertex array completely in-place.
Scene::Test::Test() : m_vertexArray{
                          std::make_shared<Renderer::Buffer::Vertex>(
                              s_Square.data(), sizeof(s_Square),
                              Renderer::Buffer::Vertex::Layout{
                                  /* You can push elements to layout in either of these two ways. */
                                  s_Square.front(),
                                  Renderer::Buffer::Vertex::Layout::Element{Renderer::Shader::DataType::Float2}
                              }),
                          Renderer::Buffer::Index{s_Indices.data(), s_Indices.size()}
                      }, m_shaderProgram{Renderer::Shader::Parser{"../res/shader/Basic.glsl"}},
                      m_texture{
                          Renderer::Texture::createGlTexture("../res/texture/Melon.png")
                      } {
    m_texture.bind(0);
    m_shaderProgram.setUniform("u_Texture", 0);
}

void Scene::Test::update(const float deltaTime) {
}

void Scene::Test::render(const Renderer::Renderer& renderer) {
    const auto blue = static_cast<float>(Core::Application::getUniqueInstance().getFrameCount() % 256) / 255.0f;

    renderer.clear({0.1f, 0.2f, blue, 1.0f});

    m_shaderProgram.bind();
    m_shaderProgram.setUniform("u_Color", glm::vec4{blue, .3f, .8f, 1.f});
    auto model = glm::translate(glm::mat4{1.f}, m_translation);
    auto mvp = m_projection * m_view * model;
    m_shaderProgram.setUniform("u_Mvp", mvp);
    renderer.draw(m_vertexArray, m_shaderProgram);

    model = glm::translate(model, m_translation);
    mvp = m_projection * m_view * model;
    m_shaderProgram.setUniform("u_Mvp", mvp);
    renderer.draw(m_vertexArray, m_shaderProgram);
}

void Scene::Test::renderImGui() {
    ImGui::SliderFloat3("float", &m_translation[0], 0.0f, 960.0f);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
