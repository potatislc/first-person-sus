#pragma once

#include <vector>

#include "../Scene.h"

namespace Scene {
    class Node final : public Scene {
    public:
        Node() = delete;

        explicit Node(Scene& scene, const Scene& parent) : m_scene{&scene}, m_parent{&parent} {
        }

        Node(const Node& other) : m_children{other.copyChildren()}, m_scene{other.m_scene->copy()},
                                  m_parent{other.m_parent}, m_active{other.m_active} {
        }

        Node& operator=(const Node& other) {
            if (&other == this) {
                return *this;
            }

            m_children = other.copyChildren();
            m_scene = other.m_scene->copy();
            m_parent = other.m_parent;
            m_active = other.m_active;

            return *this;
        }

        Node(Node&& other) noexcept : m_children{std::move(other.m_children)}, m_scene{other.m_scene},
                                      m_parent{other.m_parent},
                                      m_active{other.m_active} {
            other.m_scene = {};
            other.m_parent = {};
            other.m_active = {};
        }

        Node& operator=(Node&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_children = std::move(other.m_children);
            m_scene = other.m_scene;
            m_parent = other.m_parent;

            other.m_scene = {};
            other.m_parent = {};

            return *this;
        }

        ~Node() override;

        void update(float deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

        Scene* copy() const override {
            return new Node{*this};
        }

        void addChildCopy(const Scene& scene);

        void activate() {
            m_active = true;
        }

        void deactivate() {
            m_active = false;
        }

    private:
        [[nodiscard]] std::vector<Scene*> copyChildren() const;

        void deleteChildren() const;

        std::vector<Scene*> m_children{};
        Scene* m_scene{};
        const Scene* m_parent{};
        bool m_active = true;
    };
}
