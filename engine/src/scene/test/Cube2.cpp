#include "Cube2.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../core/Application.h"
#include "../../renderer/Renderer.h"
#include "../../renderer/VertexArray.h"
#include "../../renderer/buffer/Vertex.h"
#include "../../renderer/shader/Parser.h"


Engine::Scene::Cube2::Cube2() : m_cubeShader{Renderer::Shader::Parser{ENGINE_RES_PATH"/shader/test/CubeTest2.glsl"}},
                                m_texture{
                                    Renderer::Texture::createGlTexture(ENGINE_RES_PATH"/texture/Wall.png")
                                } {
    const Renderer::Buffer::Vertex::Layout layout{
        *s_cubePositions.data(), *s_cubeColors.data(), *s_cubeUVs.data(), *s_cubeNormals.data()
    };

    const std::vector vertexData{
        Renderer::Buffer::copyBufferData(s_cubePositions.data(), sizeof(s_cubePositions)),
        Renderer::Buffer::copyBufferData(s_cubeColors.data(), sizeof(s_cubeColors)),
        Renderer::Buffer::copyBufferData(s_cubeUVs.data(), sizeof(s_cubeUVs)),
        Renderer::Buffer::copyBufferData(s_cubeNormals.data(), sizeof(s_cubeNormals))
    };

    const auto interleavedVertexData = Renderer::Buffer::Vertex::layoutInterleave(layout, vertexData);

    Renderer::Buffer::Vertex vertexBuffer{layout, interleavedVertexData};

    auto indexData =
            Renderer::Buffer::copyIndexData(s_cubeIndices.data(), s_cubeIndices.size());
    m_vertexArray = std::make_unique<Renderer::VertexArray>(std::move(vertexBuffer), indexData);
    m_texture.bind(0);
    m_cubeShader.bind();
    m_cubeShader.setUniform("u_texture1", 0);

    // Material default values
    m_cubeShader.setUniform("u_material.ambient", glm::vec3{0.1f, 0.1f, 0.1f});
    m_cubeShader.setUniform("u_material.diffuse", glm::vec3{0.8f, 0.8f, 0.8f});
    m_cubeShader.setUniform("u_material.specular", glm::vec3{1.0f, 1.0f, 1.0f});
    m_cubeShader.setUniform("u_material.emission", glm::vec3{0.0f, 0.0f, 0.0f});
    m_cubeShader.setUniform("u_material.shine", 32.0f);

    // Light default values
    m_cubeShader.setUniform("u_light.position", glm::vec3{10.0f, 10.0f, 10.0f});
    m_cubeShader.setUniform("u_light.ambient", glm::vec3{0.2f, 0.2f, 0.2f});
    m_cubeShader.setUniform("u_light.diffuse", glm::vec3{0.5f, 0.5f, 0.5f});
    m_cubeShader.setUniform("u_light.specular", glm::vec3{1.0f, 1.0f, 1.0f});


    m_camera.setPosition(glm::vec3{0.f, 0.f, s_camRadius});
}

void Engine::Scene::Cube2::update(const double deltaTime) {
}

void Engine::Scene::Cube2::render(const Renderer::Renderer& renderer) {
    glm::mat4 model{1.0f};
    const auto animSpeed = static_cast<float>(Application::getInstance().getTimeSinceInit());
    model = glm::rotate(model, animSpeed, glm::vec3(0.5f, 1.0f, 0.0f));

    const glm::vec3 camPos{glm::cos(animSpeed) * s_camRadius, 0, glm::sin(animSpeed) * s_camRadius};
    m_camera.setPosition(camPos);
    m_camera.lookAt(Math::Vec3::zero);
    m_cubeShader.setUniform("u_viewPos", m_camera.getPosition());

    const glm::vec3 lightPos{glm::cos(-animSpeed * 4) * 2.f, .5f, glm::sin(-animSpeed * 4) * 2.f};
    m_cubeShader.setUniform("u_light.position", lightPos);

    m_cubeShader.bind();
    m_cubeShader.setUniform("u_model", model);
    m_cubeShader.setUniform("u_view", m_camera.getView());
    m_cubeShader.setUniform("u_projection", m_camera.getProjection());

    renderer.clear(glm::vec4{1.f, .3f, .2f, 1.f} * .1f);
    renderer.draw(*m_vertexArray, m_cubeShader);
}

void Engine::Scene::Cube2::renderImGui() {
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
