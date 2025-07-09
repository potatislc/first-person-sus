#include <array>
#include <glm/vec3.hpp>

#include "../Scene.h"
#include "../../renderer/shader/Program.h"
#include "../../renderer/Camera.h"

namespace Engine::Renderer {
    class VertexArray;
}

namespace Engine::Scene {
    class Cube final : public Scene {
    public:
        Cube();

        ~Cube() override = default;

        void update(float deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

    private:
        static constexpr std::array<glm::vec3, 8> s_cubePositions = {
            glm::vec3{-0.5f, -0.5f, -0.5f}, // 0
            glm::vec3{0.5f, -0.5f, -0.5f}, // 1
            glm::vec3{0.5f, 0.5f, -0.5f}, // 2
            glm::vec3{-0.5f, 0.5f, -0.5f}, // 3
            glm::vec3{-0.5f, -0.5f, 0.5f}, // 4
            glm::vec3{0.5f, -0.5f, 0.5f}, // 5
            glm::vec3{0.5f, 0.5f, 0.5f}, // 6
            glm::vec3{-0.5f, 0.5f, 0.5f} // 7
        };

        static constexpr std::array<glm::vec3, 8> s_cubeColors = {
            glm::vec3{1.0f, 0.0f, 0.0f}, // red
            glm::vec3{0.0f, 1.0f, 0.0f}, // green
            glm::vec3{0.0f, 0.0f, 1.0f}, // blue
            glm::vec3{1.0f, 1.0f, 0.0f}, // yellow
            glm::vec3{1.0f, 0.0f, 1.0f}, // magenta
            glm::vec3{0.0f, 1.0f, 1.0f}, // cyan
            glm::vec3{1.0f, 1.0f, 1.0f}, // white
            glm::vec3{0.0f, 0.0f, 0.0f} // black
        };

        static constexpr std::array<uint32_t, 36> s_cubeIndices = {
            // front face
            0, 1, 2, 2, 3, 0,
            // right face
            1, 5, 6, 6, 2, 1,
            // back face
            5, 4, 7, 7, 6, 5,
            // left face
            4, 0, 3, 3, 7, 4,
            // top face
            3, 2, 6, 6, 7, 3,
            // bottom face
            4, 5, 1, 1, 0, 4
        };

        static constexpr float s_camRadius{3.f};

        Renderer::Camera m_camera;
        Renderer::Shader::Program m_shaderProgram;
        std::unique_ptr<Renderer::VertexArray> m_vertexArray;
    };
}
