#include "Cube2.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../../core/Application.h"
#include "../../renderer/Renderer.h"
#include "../../renderer/VertexArray.h"
#include "../../renderer/buffer/Vertex.h"
#include "../../renderer/shader/Parser.h"
#include "core/InputMap.h"


Engine::Scene::Cube2::Cube2() : m_cubeShader{Renderer::Shader::Parser{ENGINE_RES_PATH"/shader/test/CubeTest2.glsl"}},
                                m_color{
                                    Renderer::Texture::createGlTexture(ENGINE_RES_PATH"/texture/Wall.png")
                                },
                                m_diffuse{
                                    Renderer::Texture::createGlTexture(ENGINE_RES_PATH"/texture/Wall-diffuse.png")
                                }, m_specular{
                                    Renderer::Texture::createGlTexture(ENGINE_RES_PATH"/texture/Wall-border.png")
                                }, m_emission{
                                    Renderer::Texture::createGlTexture(ENGINE_RES_PATH"/texture/Wall-graffiti.png")
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
}

void Engine::Scene::Cube2::update(const double deltaTime) {
    auto& inputMap = InputMap::getInstance();
    static const auto actionForward{inputMap.createAction("Forward")};
    static const auto actionBackward{inputMap.createAction("Backward")};
    static const auto actionRight{inputMap.createAction("Right")};
    static const auto actionLeft{inputMap.createAction("Left")};
    static const auto actionUp{inputMap.createAction("Up")};
    static const auto actionDown{inputMap.createAction("Down")};

    static auto a = [&inputMap]() {
        /*inputMap.bind(SDLK_W, actionForward);
        inputMap.bind(SDLK_S, actionBackward);
        inputMap.bind(SDLK_D, actionRight);
        inputMap.bind(SDLK_A, actionLeft);
        inputMap.bind(SDLK_SPACE, actionUp);
        inputMap.bind(SDLK_LSHIFT, actionDown);*/

        inputMap.bind(SDL_SCANCODE_W, actionForward);
        inputMap.bind(SDL_SCANCODE_S, actionBackward);
        inputMap.bind(SDL_SCANCODE_D, actionRight);
        inputMap.bind(SDL_SCANCODE_A, actionLeft);
        inputMap.bind(SDL_SCANCODE_SPACE, actionUp);
        inputMap.bind(SDL_SCANCODE_LSHIFT, actionDown);
    };

    a();

    const glm::ivec3 inputDir{
        static_cast<int>(inputMap.isActionPressed(actionRight)) - static_cast<int>(inputMap.
            isActionPressed(actionLeft)),
        static_cast<int>(inputMap.isActionPressed(actionUp)) - static_cast<int>(inputMap.
            isActionPressed(actionDown)),
        static_cast<int>(inputMap.isActionPressed(actionBackward)) - static_cast<int>(inputMap.isActionPressed(
            actionForward))
    };


    constexpr auto moveSpeed{5.f};
    const glm::vec3 velocity{glm::vec3{inputDir} * moveSpeed * static_cast<float>(deltaTime)};
    /*if (inputDir != glm::ivec3{Math::Vec3::zero}) {
        LOG("x: " << velocity.x << ", z: " << velocity.z << '\n');
    }*/
    m_camera.translate(velocity);
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
}

void Engine::Scene::Cube2::renderImGui() {
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.f / io.Framerate),
                static_cast<double>(io.Framerate));
}
