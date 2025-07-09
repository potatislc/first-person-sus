#pragma once

#include <vector>

#include "../Scene.h"

namespace Engine::Scene {
    class Node : public Scene {
    public:
        Node() = default;

        template<typename NodeType, typename... ConstructorArgs>
        auto create(Node& parent, ConstructorArgs... constructorArgs);

        Node(const Node& other) : m_children{other.copyChildren()}, m_parent{other.m_parent}, m_active{other.m_active} {
        }

        Node& operator=(const Node& other) {
            if (&other == this) {
                return *this;
            }

            m_children = other.copyChildren();
            m_parent = other.m_parent;
            m_active = other.m_active;

            // Create copies of the children

            return *this;
        }

        Node(Node&& other) noexcept : m_children{std::move(other.m_children)},
                                      m_parent{other.m_parent},
                                      m_active{other.m_active} {
            other.m_parent = {};
            other.m_active = {};
        }

        Node& operator=(Node&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            m_children = std::move(other.m_children);
            m_parent = other.m_parent;
            other.m_parent = {};

            return *this;
        }

        ~Node() override;

        void update(float deltaTime) override;

        void render(const Renderer::Renderer& renderer) override;

        void renderImGui() override;

        virtual Node copy() {
            return Node{*this};
        }

        void activate() {
            m_active = true;
        }

        void deactivate() {
            m_active = false;
        }

        std::vector<std::unique_ptr<Node> > copyChildren() const {
            std::vector<std::unique_ptr<Node> > copied;
            for (const auto& child: m_children) {
                copied.push_back(std::make_unique<Node>(child->copy()));
            }

            return copied;
        }

    private:
        std::vector<std::unique_ptr<Node> > m_children;
        const Node* m_parent{};
        bool m_active = true;
    };

    template<typename NodeType, typename... ConstructorArgs>
    auto Node::create(Node& parent, ConstructorArgs... constructorArgs) {
        NodeType node{constructorArgs...};
        node.m_parent = &parent;
        return node;
    }
}
