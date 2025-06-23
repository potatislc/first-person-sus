#ifndef NODE_H
#define NODE_H

#include <vector>

#include "../Scene.h"

namespace Scene {
    [[maybe_unused]] class Node final : public Scene {
    public:
        explicit Node(Scene& scene) : m_Scene{&scene} {
        }

        Node(const Node& other) : m_Scene{other.m_Scene}, m_SubScenes{other.m_SubScenes} {
        }

        ~Node() override {
            delete m_Scene;
        }

        void update(const float deltaTime) override {
            m_Scene->update(deltaTime);

            for (const auto& scene: m_SubScenes) {
                scene->update(deltaTime);
            }
        }

        void onRender() override {
            m_Scene->onRender();

            for (const auto& scene: m_SubScenes) {
                scene->onRender();
            }
        }

        void onImGuiRender() override {
            m_Scene->onImGuiRender();

            for (const auto& scene: m_SubScenes) {
                scene->onImGuiRender();
            }
        }

        /*Scene* copy() const override {
            return new Node{*this};
        }*/

        void addSubSceneCopy(const Scene& scene) {
            // m_SubScenes.push_back(scene.copy());
        }

    private:
        Scene* m_Scene{};
        std::vector<Scene*> m_SubScenes{};
    };
}

#endif
