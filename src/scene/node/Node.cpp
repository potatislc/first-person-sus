#include "Node.h"

Scene::Node::~Node() {
    deleteChildren();
    delete m_scene;
}

void Scene::Node::update(const float deltaTime) {
    if (!m_active) {
        return;
    }

    m_scene->update(deltaTime);

    for (const auto& scene: m_children) {
        scene->update(deltaTime);
    }
}

void Scene::Node::render(const Renderer::Renderer& renderer) {
    if (!m_active) {
        return;
    }

    m_scene->render(renderer);

    for (const auto& scene: m_children) {
        scene->render(renderer);
    }
}

void Scene::Node::renderImGui() {
    if (!m_active) {
        return;
    }

    m_scene->renderImGui();

    for (const auto& scene: m_children) {
        scene->renderImGui();
    }
}

void Scene::Node::addChildCopy(const Scene& scene) {
    if (const auto childCopy = scene.copy(); childCopy != nullptr) {
        m_children.push_back(childCopy);
    }
}

std::vector<Scene::Scene*> Scene::Node::copyChildren() const {
    std::vector<Scene*> children;

    for (const auto& child: m_children) {
        children.push_back(child->copy());
    }

    return children;
}

void Scene::Node::deleteChildren() const {
    for (const auto& child: m_children) {
        delete &child;
    }
}
