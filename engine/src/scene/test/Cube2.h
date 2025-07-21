#pragma once
#include <array>
#include <glm/vec3.hpp>

#include "core/InputMap.h"
#include "scene/Scene.h"
#include "renderer/shader/Program.h"
#include "renderer/Camera.h"
#include "renderer/Texture.h"

namespace Engine::Renderer {
    class VertexArray;
}

namespace Engine::Scene {
    class Cube2 final : public Scene {
    public:
        Cube2();

        ~Cube2() override = default;

        Cube2(const Cube2&) = delete;

        Cube2(Cube2&&) = delete;

        Cube2& operator=(const Cube2&) = delete;

        Cube2& operator=(Cube2&&) = delete;

        void update(double deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

    private:
        // Positions (24 unique)
        static constexpr std::array<glm::vec3, 24> s_cubePositions = {
            {
                // Front face
                {-0.5f, -0.5f, 0.5f},
                {0.5f, -0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {-0.5f, 0.5f, 0.5f},

                // Back face
                {0.5f, -0.5f, -0.5f},
                {-0.5f, -0.5f, -0.5f},
                {-0.5f, 0.5f, -0.5f},
                {0.5f, 0.5f, -0.5f},

                // Left face
                {-0.5f, -0.5f, -0.5f},
                {-0.5f, -0.5f, 0.5f},
                {-0.5f, 0.5f, 0.5f},
                {-0.5f, 0.5f, -0.5f},

                // Right face
                {0.5f, -0.5f, 0.5f},
                {0.5f, -0.5f, -0.5f},
                {0.5f, 0.5f, -0.5f},
                {0.5f, 0.5f, 0.5f},

                // Bottom face
                {-0.5f, -0.5f, -0.5f},
                {0.5f, -0.5f, -0.5f},
                {0.5f, -0.5f, 0.5f},
                {-0.5f, -0.5f, 0.5f},

                // Top face
                {-0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, -0.5f},
                {-0.5f, 0.5f, -0.5f}
            }
        };

        // UVs
        static constexpr std::array<glm::vec2, 24> s_cubeUVs = {
            {
                // Front face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

                // Back face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

                // Left face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

                // Right face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

                // Bottom face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},

                // Top face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
            }
        };

        // Colors (one color per face × 4 vertices)
        static constexpr std::array<glm::vec3, 24> s_cubeColors = {
            {
                // Front (red)
                {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},

                // Back (green)
                {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},

                // Left (blue)
                {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},

                // Right (yellow)
                {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},

                // Bottom (cyan)
                {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f},

                // Top (magenta)
                {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}
            }
        };

        static constexpr std::array<uint32_t, 36> s_cubeIndices = {
            {
                // Front face
                0, 1, 2, 2, 3, 0,

                // Back face
                4, 5, 6, 6, 7, 4,

                // Left face
                8, 9, 10, 10, 11, 8,

                // Right face
                12, 13, 14, 14, 15, 12,

                // Bottom face
                16, 17, 18, 18, 19, 16,

                // Top face
                20, 21, 22, 22, 23, 20
            }
        };

        // Normals (one per face × 4 vertices)
        static constexpr std::array<glm::vec3, 24> s_cubeNormals = {
            {
                // Front (+Z)
                {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},

                // Back (−Z)
                {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},

                // Left (−X)
                {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},

                // Right (+X)
                {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},

                // Bottom (−Y)
                {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},

                // Top (+Y)
                {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
            }
        };


        /*std::array<glm::vec3, 10> s_cubeOrigins{
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
        };*/

        static constexpr float s_camRadius{3.f};

        Renderer::Camera m_camera;
        Renderer::Shader::Program m_cubeShader;
        Renderer::Texture m_color;
        Renderer::Texture m_diffuse;
        Renderer::Texture m_specular;
        Renderer::Texture m_emission;
        std::unique_ptr<Renderer::VertexArray> m_vertexArray;
        glm::vec3 m_lightColor{1.f, 1.f, 1.f};
        InputMap::ActionId m_actionJump;
        InputMap::ActionId m_actionRelease;
    };
}
