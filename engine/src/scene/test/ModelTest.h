#pragma once
#include "renderer/Camera.h"
#include "renderer/model/Model.h"
#include "renderer/shader/Program.h"
#include "scene/Scene.h"

namespace Engine {
    class ModelTest final : public Scene::Scene {
    public:
        ModelTest();

        ~ModelTest() override = default;

        void update(double deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

    private:
        Renderer::Camera m_camera;
        std::optional<Renderer::Model> m_model;
        std::vector<glm::vec3> m_instancePositions;
        Renderer::Shader::Program m_shader;
    };
}
