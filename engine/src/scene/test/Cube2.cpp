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

    /*const std::vector vertexData{
        Renderer::Buffer::copyBufferData(s_cubePositions.data(), sizeof(s_cubePositions)),
        Renderer::Buffer::copyBufferData(s_cubeUVs.data(), sizeof(s_cubeUVs)),
        Renderer::Buffer::copyBufferData(s_cubeNormals.data(), sizeof(s_cubeNormals)),
        Renderer::Buffer::copyBufferData(s_cubeColors.data(), sizeof(s_cubeColors))
    };*/

    std::vector vertexData{objParser.next().getVertexData()};
    vertexData.emplace_back(Renderer::Buffer::copyBufferData(s_cubeColors.data(), sizeof(s_cubeColors)));

    const auto interleavedVertexData = Renderer::Buffer::Vertex::layoutInterleave(
        layout, vertexData);

    Renderer::Buffer::Vertex vertexBuffer{layout, interleavedVertexData};

    auto indexData =
            Renderer::Buffer::copyIndexData(s_cubeIndices.data(), s_cubeIndices.size());
    m_vertexArray = std::make_unique<Renderer::VertexArray>(std::move(vertexBuffer), indexData);
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
    auto& inputMap = InputMap::getInstance();

    static const auto actionForward = inputMap.createAction("Forward");
    static const auto actionBackward = inputMap.createAction("Backward");
    static const auto actionRight = inputMap.createAction("Right");
    static const auto actionLeft = inputMap.createAction("Left");
    static const auto actionUp = inputMap.createAction("Up");
    static const auto actionDown = inputMap.createAction("Down");
    static const auto actionShoot = inputMap.createAction("Shoot");

    static bool inputsBound = false;
    if (!inputsBound) {
        inputMap.bindToKey(SDL_SCANCODE_W, actionForward);
        inputMap.bindToKey(SDL_SCANCODE_S, actionBackward);
        inputMap.bindToKey(SDL_SCANCODE_D, actionRight);
        inputMap.bindToKey(SDL_SCANCODE_A, actionLeft);
        inputMap.bindToKey(SDL_SCANCODE_SPACE, actionUp);
        inputMap.bindToKey(SDL_SCANCODE_LSHIFT, actionDown);
        inputMap.bindToMouseButton(SDL_BUTTON_LEFT, actionShoot);
        inputsBound = true;
    }

    if (inputMap.isActionJustPressed(actionShoot)) {
        LOG("Shoot!\n");
    }

    glm::vec3 forward = m_camera.getDirection();
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    glm::vec3 const right = glm::normalize(glm::cross(forward, Math::Vec3::up));

    auto velocity = glm::vec3{0.0f};
    if (inputMap.isActionPressed(actionForward)) {
        velocity += forward;
    }
    if (inputMap.isActionPressed(actionBackward)) {
        velocity -= forward;
    }
    if (inputMap.isActionPressed(actionRight)) {
        velocity += right;
    }
    if (inputMap.isActionPressed(actionLeft)) {
        velocity -= right;
    }

    if (inputMap.isActionPressed(actionUp)) {
        velocity += Math::Vec3::up;
    }

    if (inputMap.isActionPressed(actionDown)) {
        velocity -= Math::Vec3::up;
    }

    if (glm::length(velocity) > 0.0f) {
        velocity = glm::normalize(velocity);
    }

    constexpr float moveSpeed = 5.0f;
    m_camera.translate(velocity * moveSpeed * static_cast<float>(deltaTime));

    // Camera rotation
    const glm::vec2 mouseDelta = inputMap.getMouseVelocity();
    constexpr float mouseSensitivity = 10.0f;
    m_camera.rotateFromMouseDelta(mouseDelta * static_cast<float>(deltaTime), mouseSensitivity);
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
