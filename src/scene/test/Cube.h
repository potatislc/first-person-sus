#include <array>
#include <glm/vec3.hpp>

#include "../Scene.h"
#include "../../renderer/shader/Program.h"

namespace Renderer {
    class VertexArray;
}

namespace Scene {
    class Cube final : public Scene {
    public:
        Cube();

        ~Cube() override = default;

        void update(float deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

    private:
        static constexpr std::array<glm::vec3, 48> s_cubePositions{
            {
                {-0.5f, -0.5f, 0.5f},
                {0.5f, -0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {-0.5f, -0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {-0.5f, 0.5f, 0.5f},

                {0.5f, -0.5f, -0.5f},
                {-0.5f, -0.5f, -0.5f},
                {-0.5f, 0.5f, -0.5f},
                {0.5f, -0.5f, -0.5f},
                {-0.5f, 0.5f, -0.5f},
                {0.5f, 0.5f, -0.5f},

                {-0.5f, -0.5f, -0.5f},
                {-0.5f, -0.5f, 0.5f},
                {-0.5f, 0.5f, 0.5f},
                {-0.5f, -0.5f, -0.5f},
                {-0.5f, 0.5f, 0.5f},
                {-0.5f, 0.5f, -0.5f},

                {0.5f, -0.5f, 0.5f},
                {0.5f, -0.5f, -0.5f},
                {0.5f, 0.5f, -0.5f},
                {0.5f, -0.5f, 0.5f},
                {0.5f, 0.5f, -0.5f},
                {0.5f, 0.5f, 0.5f},

                {-0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, -0.5f},
                {-0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, -0.5f},
                {-0.5f, 0.5f, -0.5f},

                {-0.5f, -0.5f, -0.5f},
                {0.5f, -0.5f, -0.5f},
                {0.5f, -0.5f, 0.5f},
                {-0.5f, -0.5f, -0.5f},
                {0.5f, -0.5f, 0.5f},
                {-0.5f, -0.5f, 0.5f}
            }
        };

        static constexpr std::array<glm::vec3, 48> s_cubeColors{
            {
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},

                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},

                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},

                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},

                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},

                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f},
                {0.5f, 0.5f, 0.5f}
            }
        };

        static constexpr std::array<uint32_t, 36> s_cubeIndices{
            {
                // Front face
                0, 1, 2,
                0, 2, 3,

                // Right face
                1, 5, 6,
                1, 6, 2,

                // Back face
                5, 4, 7,
                5, 7, 6,

                // Left face
                4, 0, 3,
                4, 3, 7,

                // Top face
                3, 2, 6,
                3, 6, 7,

                // Bottom face
                4, 5, 1,
                4, 1, 0
            }
        };

        std::unique_ptr<Renderer::VertexArray> m_vertexArray;
        Renderer::Shader::Program m_shaderProgram;
    };
}
