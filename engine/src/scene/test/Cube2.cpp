#include "Cube2.h"

#include <imgui.h>
#include <random>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../core/Application.h"
#include "../../renderer/Renderer.h"
#include "../../renderer/VertexArray.h"
#include "../../renderer/buffer/Vertex.h"
#include "../../renderer/shader/Parser.h"
#include "core/InputMap.h"
#include "renderer/model/ObjParser.h"


std::array<glm::vec3, 64> generateRandomPositions() {
    std::array<glm::vec3, 64> positions;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (auto& pos: positions) {
        pos = glm::vec3(dist(gen), dist(gen), dist(gen));
    }

    return positions;
}

Engine::Scene::Cube2::Cube2() : m_cubeShader{Renderer::Shader::Parser{ENGINE_RES_PATH"/shader/test/CubeTest2.glsl"}},
                                m_color{
                                    Renderer::Texture::loadGlTexture(ENGINE_RES_PATH"/texture/Wall.png")
                                },
                                m_diffuse{
                                    Renderer::Texture::loadGlTexture(ENGINE_RES_PATH"/texture/Wall-diffuse.png")
                                }, m_specular{
                                    Renderer::Texture::loadGlTexture(ENGINE_RES_PATH"/texture/Wall-border.png")
                                }, m_emission{
                                    Renderer::Texture::loadGlTexture(ENGINE_RES_PATH"/texture/Wall-graffiti.png")
                                } {
    Renderer::ObjParser objParser{ENGINE_RES_PATH"/model/Cube.obj"};

    Renderer::Buffer::Vertex::Layout layout{
        Renderer::MeshData::baseLayout()
    };
    layout.push(*s_cubeColors.data());

    const auto meshData{objParser.next()};
    auto vertexData{meshData.getVertexData()};
    vertexData.push_back(Renderer::Buffer::copyBufferData(s_cubeColors.data(), sizeof(s_cubeColors)));
    const auto interleavedVertexData = Renderer::Buffer::Vertex::layoutInterleave(
        layout, vertexData);
    Renderer::Buffer::Vertex vertexBuffer{layout, interleavedVertexData};
    m_vertexArray = std::make_unique<Renderer::VertexArray>(std::move(vertexBuffer), meshData.indices);

    m_color.bind(0);
    m_diffuse.bind(1);
    m_specular.bind(2);
    m_emission.bind(3);
    m_cubeShader.bind();
    m_cubeShader.setUniform("u_texture1", 0);
    m_cubeShader.setUniform("u_matMap.diffuse", 1);
    m_cubeShader.setUniform("u_matMap.specular", 2);
    m_cubeShader.setUniform("u_matMap.emission", 3);
    m_cubeShader.setUniform("u_matMap.shine", 32.f);

    // Light default values
    m_cubeShader.setUniform("u_light.position", glm::vec3{10.0f, 10.0f, 10.0f});
    m_cubeShader.setUniform("u_light.ambient", glm::vec3{0.2f, 0.2f, 0.2f});
    m_cubeShader.setUniform("u_light.diffuse", glm::vec3{0.5f, 0.5f, 0.5f});
    m_cubeShader.setUniform("u_light.specular", glm::vec3{1.0f, 1.0f, 1.0f});
    m_cubeShader.setUniform("u_light.constant", 1.0f);
    m_cubeShader.setUniform("u_light.linear", 0.09f);
    m_cubeShader.setUniform("u_light.quadratic", 0.032f);

    m_camera.setPosition(glm::vec3{0.f, 0.f, s_camRadius});
    SDL_SetWindowRelativeMouseMode(Application::getInstance().getWindow().getSdlWindow(), true);

    s_cubes = generateRandomPositions();
}

void Engine::Scene::Cube2::update(const double deltaTime) {
    m_camera.debugMove(deltaTime, 5.f, 10.f);
}


void Engine::Scene::Cube2::render(const Renderer::Renderer& renderer) {
    glm::mat4 model{1.0f};
    const auto animSpeed = static_cast<float>(Application::getInstance().getTimeSinceInit());
    model = glm::rotate(model, animSpeed, glm::vec3(0.5f, 1.0f, 0.0f));

    m_cubeShader.setUniform("u_viewPos", m_camera.getPosition());

    const glm::vec3 lightPos{glm::cos(-animSpeed * 4) * 2.f, .5f, glm::sin(-animSpeed * 4) * 2.f};
    m_cubeShader.setUniform("u_light.position", lightPos);

    m_cubeShader.bind();

    m_cubeShader.setUniform("u_model", model);
    m_cubeShader.setUniform("u_view", m_camera.getView());
    m_cubeShader.setUniform("u_projection", m_camera.getProjection());
    renderer.clear(glm::vec4{1.f, .3f, .2f, 1.f} * .1f);
    renderer.draw(*m_vertexArray, m_cubeShader);

    for (const auto cubeModel: s_cubes) {
        model = glm::mat4{1.f};
        model = glm::translate(model, cubeModel * 16.f);
        model = glm::rotate(model, animSpeed, glm::vec3(0.5f, 1.0f, 0.0f));
        m_cubeShader.setUniform("u_model", model);
        renderer.draw(*m_vertexArray, m_cubeShader);
    }
}

void Engine::Scene::Cube2::renderImGui() {
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
