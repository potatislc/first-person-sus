#include "ModelTest.h"

#include <imgui.h>

#include "core/Application.h"
#include "renderer/model/ObjParser.h"

Engine::ModelTest::ModelTest() : m_shader{
    ENGINE_RES_PATH"/shader/source/Base.vert", ENGINE_RES_PATH"/shader/source/Base.frag"
} {
    Renderer::ObjParser parser{ENGINE_RES_PATH"/model/Eye.obj"};

    m_model = Renderer::Model::generate(parser.next(), {
                                            Renderer::Texture::loadGlTexture(ENGINE_RES_PATH"/texture/Wall.png")
                                        });

    m_model->getTextures().front().bind(0);
    m_shader.bind();
    m_shader.setUniform("u_texture1", 0);
    m_shader.setUniform("u_material.diffuse", glm::vec3{.8f});
    m_shader.setUniform("u_material.specular", glm::vec3{.9f});
    m_shader.setUniform("u_material.emission", glm::vec3{.0f});
    m_shader.setUniform("u_material.shine", 32.f);

    m_shader.setUniform("u_light.ambient", glm::vec3{0.4f, 0.4f, 0.4f});
    m_shader.setUniform("u_light.diffuse", glm::vec3{0.5f, 0.5f, 0.5f});
    m_shader.setUniform("u_light.specular", glm::vec3{1.0f, 1.0f, 1.0f});
    m_shader.setUniform("u_light.direction", normalize(glm::vec3{-1.f, -1.f, -1.f}));

    m_instancePositions.resize(100);
    for (size_t i{}; i < m_instancePositions.size(); i++) {
        m_instancePositions[i].x = static_cast<float>(i);
    }

    const Renderer::Buffer::Vertex::Layout instanceLayout{*m_instancePositions.data()};
    Renderer::Buffer::Vertex instanceBuffer{
        instanceLayout, m_instancePositions.data(),
        static_cast<uint32_t>(m_instancePositions.size() * instanceLayout.getStride())
    };
    m_model->setInstanceBuffer(std::move(instanceBuffer));
}

void Engine::ModelTest::update(const double deltaTime) {
    m_camera.debugMove(deltaTime, 5.f, 10.f);
}

void Engine::ModelTest::render(const Renderer::Renderer& renderer) {
    glm::mat4 model{1.0f};
    const auto animSpeed = static_cast<float>(Application::getInstance().getTimeSinceInit());
    model = glm::rotate(model, animSpeed, glm::vec3(0.5f, 1.0f, 0.0f));

    m_shader.setUniform("u_viewPos", m_camera.getPosition());

    m_shader.bind();

    m_shader.setUniform("u_model", model);
    m_shader.setUniform("u_view", m_camera.getView());
    m_shader.setUniform("u_projection", m_camera.getProjection());
    renderer.clear(glm::vec4{1.f, .3f, .2f, 1.f} * .1f);
    m_model->drawInstanced(m_shader, m_instancePositions.data(), static_cast<uint32_t>(m_instancePositions.size()));
}

void Engine::ModelTest::renderImGui() {
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
