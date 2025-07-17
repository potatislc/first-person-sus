#include "Cube.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "core/Application.h"
#include "renderer/Renderer.h"
#include "renderer/VertexArray.h"
#include "renderer/buffer/Vertex.h"
#include "renderer/shader/Parser.h"


Engine::Scene::Cube::Cube() : m_shaderProgram{Renderer::Shader::Parser{"../engine/res/shader/test/CubeTest.glsl"}} {
    const Renderer::Buffer::Vertex::Layout cubeLayout{*s_cubePositions.data(), *s_cubeColors.data()};

    const std::vector vertexData{
        Renderer::Buffer::copyBufferData(s_cubePositions.data(), sizeof(s_cubePositions)),
        Renderer::Buffer::copyBufferData(s_cubeColors.data(), sizeof(s_cubeColors))
    };

    const auto interleavedVertexData = Renderer::Buffer::Vertex::layoutInterleave(cubeLayout, vertexData);

    Renderer::Buffer::Vertex vertexBuffer{cubeLayout, interleavedVertexData};

    m_vertexArray = std::make_unique<Renderer::VertexArray>(std::move(vertexBuffer),
                                                            Renderer::Buffer::copyIndexData(
                                                                s_cubeIndices.data(), s_cubeIndices.size()));
    m_camera.setPosition(glm::vec3{0.f, 0.f, 3.f});
}

void Engine::Scene::Cube::update(const double deltaTime) {
    Scene::update(deltaTime);
}

void Engine::Scene::Cube::render(const Renderer::Renderer& renderer) {
    const auto animSpeed{static_cast<float>(Application::getInstance().getFrameCount()) * .005f};

    glm::mat4 model{1.0f};
    model = glm::rotate(model, animSpeed,
                        glm::vec3(0.5f, 1.0f, 0.0f));

    const glm::vec3 camPos{glm::cos(animSpeed) * s_camRadius, 0, glm::sin(animSpeed) * s_camRadius};
    m_camera.setPosition(camPos);
    m_camera.lookAt(Math::Vec3::zero);

    m_shaderProgram.bind();
    m_shaderProgram.setUniform("model", model);
    m_shaderProgram.setUniform("view", m_camera.getView());
    m_shaderProgram.setUniform("projection", m_camera.getProjection());

    renderer.clear(glm::vec4{0.f, .5f, 1.f, 1.f});
    renderer.draw(*m_vertexArray, m_shaderProgram);
}

void Engine::Scene::Cube::renderImGui() {
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
