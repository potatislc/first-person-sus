#pragma once

#include <iostream>

namespace Renderer {
    class Renderer;
}

namespace Scene {
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

        virtual Scene* copy() const {
            std::cerr << "Scene instance is not copyable with Scene::Scene::copy().\n";
            return nullptr;
        }
    };
}
