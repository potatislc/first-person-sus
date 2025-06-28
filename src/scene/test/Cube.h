#include <array>
#include <glm/vec3.hpp>

#include "../Scene.h"

class Cube final : Scene::Scene {
public:
    ~Cube() override;

    void update(float deltaTime) override;

    void render(const Renderer::Renderer& renderer) override;

    void renderImGui() override;

private:
    static constexpr std::array<glm::vec3, 48> s_cubeVertices{
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
};
