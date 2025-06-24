#pragma once

#include <vector>

#include "../Scene.h"

namespace Scene {
    class Node final : public Scene {
    public:
        Node() = delete;

        explicit Node(Scene& scene, const Scene& parent) : m_Scene{&scene}, m_Parent{&parent} {
        }

        Node(const Node& other) : m_Children{other.copyChildren()}, m_Scene{other.m_Scene->copy()},
                                  m_Parent{other.m_Parent}, m_Active{other.m_Active} {
        }

        Node& operator=(const Node& other) {
            if (&other == this) {
                return *this;
            }

            m_Children = other.copyChildren();
            m_Scene = other.m_Scene->copy();
            m_Parent = other.m_Parent;
            m_Active = other.m_Active;

            return *this;
        }

        Node(Node&& other) noexcept : m_Children{std::move(other.m_Children)}, m_Scene{other.m_Scene},
                                      m_Parent{other.m_Parent},
                                      m_Active{other.m_Active} {
            other.m_Scene = {};
            other.m_Parent = {};
            other.m_Active = {};
        }

        Node& operator=(Node&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_Children = std::move(other.m_Children);
            m_Scene = other.m_Scene;
            m_Parent = other.m_Parent;

            other.m_Scene = {};
            other.m_Parent = {};

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
            m_Active = true;
        }

        void deactivate() {
            m_Active = false;
        }

    private:
        [[nodiscard]] std::vector<Scene*> copyChildren() const;

        void deleteChildren() const;

        std::vector<Scene*> m_Children{};
        Scene* m_Scene{};
        const Scene* m_Parent{};
        bool m_Active = true;
    };
}
