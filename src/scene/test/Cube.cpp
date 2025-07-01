#include "Cube.h"

#include <imgui.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../core/Application.h"
#include "../../renderer/Renderer.h"
#include "../../renderer/VertexArray.h"
#include "../../renderer/buffer/Vertex.h"
#include "../../renderer/shader/Parser.h"

Scene::Cube::Cube() : m_shaderProgram{Renderer::Shader::Parser{"../res/shader/CubeTest.glsl"}} {
    const Renderer::Buffer::Vertex::Layout cubeLayout{*s_cubePositions.data(), *s_cubeColors.data()};

    const std::vector vertexData{
        Renderer::Buffer::copyBufferData(s_cubePositions.data(), sizeof(s_cubePositions)),
        Renderer::Buffer::copyBufferData(s_cubeColors.data(), sizeof(s_cubeColors))
    };

    const auto interleavedVertexData = Renderer::Buffer::Vertex::layoutInterleave(cubeLayout, vertexData);

    std::cout << interleavedVertexData.size() << " = " << (vertexData[0].size() + vertexData[1].size()) << "\n";

    Renderer::Buffer::Vertex vertexBuffer{cubeLayout, interleavedVertexData};

    m_vertexArray = std::make_unique<Renderer::VertexArray>(std::move(vertexBuffer),
                                                            Renderer::Buffer::copyIndexData(
                                                                s_cubeIndices.data(), s_cubeIndices.size()));
    m_camera.setPosition(glm::vec3{0.f, 0.f, 3.f});
}

void Scene::Cube::update(const float deltaTime) {
    Scene::update(deltaTime);
}

void Scene::Cube::render(const Renderer::Renderer& renderer) {
    glm::mat4 model{1.0f};
    model = glm::rotate(model, static_cast<float>(Core::Application::getUniqueInstance().getFrameCount()) * .005f,
                        glm::vec3(0.5f, 1.0f, 0.0f));

    m_shaderProgram.bind();
    m_shaderProgram.setUniform("model", model);
    m_shaderProgram.setUniform("view", m_camera.getView());
    m_shaderProgram.setUniform("projection", m_camera.getProjection());

    renderer.clear(glm::vec4{0.f, .5f, 1.f, 1.f});
    renderer.draw(*m_vertexArray, m_shaderProgram);
}

void Scene::Cube::renderImGui() {
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
