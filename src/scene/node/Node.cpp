#include "Node.h"

Scene::Node::~Node() = default;

void Scene::Node::update(const float deltaTime) {
    if (!m_active) {
        return;
    }

    for (const auto& node: m_children) {
        node->update(deltaTime);
    }
}

void Scene::Node::render(const Renderer::Renderer& renderer) {
    if (!m_active) {
        return;
    }

    for (const auto& node: m_children) {
        node->render(renderer);
    }
}

void Scene::Node::renderImGui() {
    if (!m_active) {
        return;
    }

    for (const auto& node: m_children) {
        node->renderImGui();
    }
}
