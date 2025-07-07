#include <array>
#include <glm/vec3.hpp>

#include "../Scene.h"
#include "../../renderer/shader/Program.h"
#include "../../renderer/Camera.h"
#include "../../renderer/Texture.h"

namespace Renderer {
    class VertexArray;
}

namespace Scene {
    class Cube2 final : public Scene {
    public:
        Cube2();

        ~Cube2() override = default;

        void update(float deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

    private:
        /*// Positions (x, y, z)
        static constexpr std::array<glm::vec3, 36> s_cubePositions = {
            {
                // Front face
                {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
                {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},

                // Back face
                {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f},

                // Left face
                {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},
                {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},

                // Right face
                {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
                {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},

                // Bottom face
                {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f},
                {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, -0.5f},

                // Top face
                {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, -0.5f}
            }
        };

        // Texture Coordinates (u, v)
        static constexpr std::array<glm::vec2, 36> s_cubeUVs = {
            {
                // Front face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
                {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},

                // Back face
                {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
                {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},

                // Left face
                {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},

                // Right face
                {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},

                // Bottom face
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
                {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f},

                // Top face
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
                {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}
            }
        };

        // Colors (r, g, b) — one color per face
        static constexpr std::array<glm::vec3, 36> s_cubeColors = {
            {
                // Front (red)
                {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
                {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},

                // Back (green)
                {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},

                // Left (blue)
                {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
                {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},

                // Right (yellow)
                {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
                {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},

                // Bottom (cyan)
                {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f},
                {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f},

                // Top (magenta)
                {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}
            }
        };*/

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


        static constexpr float s_camRadius{3.f};

        Renderer::Camera m_camera;
        Renderer::Shader::Program m_shaderProgram;
        Renderer::Texture m_texture;
        std::unique_ptr<Renderer::VertexArray> m_vertexArray;
    };
}
