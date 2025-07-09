#include "Node.h"

Engine::Scene::Node::~Node() = default;

void Engine::Scene::Node::update(const float deltaTime) {
    if (!m_active) {
        return;
    }

    for (const auto& node: m_children) {
        node->update(deltaTime);
    }
}

void Engine::Scene::Node::render(const Renderer::Renderer& renderer) {
    if (!m_active) {
        return;
    }

    for (const auto& node: m_children) {
        node->render(renderer);
    }
}

void Engine::Scene::Node::renderImGui() {
    if (!m_active) {
        return;
    }

    for (const auto& node: m_children) {
        node->renderImGui();
    }
}
