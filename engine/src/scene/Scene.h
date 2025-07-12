#pragma once

namespace Engine::Renderer {
    class Renderer;
}

namespace Engine::Scene {
    class Scene {
    public:
        Scene() = default;

        Scene(const Scene&) = delete;

        Scene& operator=(const Scene&) = delete;

        Scene(Scene&&) = default;

        Scene& operator=(Scene&&) = default;

        virtual ~Scene() = default;

        virtual void update(float deltaTime) {
        }

        virtual void render(const Renderer::Renderer& renderer) {
        }

        virtual void renderImGui() {
        }
    };
}
