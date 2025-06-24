#include "Node.h"

Scene::Node::~Node() {
    deleteChildren();
    delete m_Scene;
}

void Scene::Node::update(const float deltaTime) {
    if (!m_Active) {
        return;
    }

    m_Scene->update(deltaTime);

    for (const auto& scene: m_Children) {
        scene->update(deltaTime);
    }
}

void Scene::Node::render(const Renderer::Renderer& renderer) {
    if (!m_Active) {
        return;
    }

    m_Scene->render(renderer);

    for (const auto& scene: m_Children) {
        scene->render(renderer);
    }
}

void Scene::Node::renderImGui() {
    if (!m_Active) {
        return;
    }

    m_Scene->renderImGui();

    for (const auto& scene: m_Children) {
        scene->renderImGui();
    }
}

void Scene::Node::addChildCopy(const Scene& scene) {
    if (const auto childCopy = scene.copy(); childCopy != nullptr) {
        m_Children.push_back(childCopy);
    }
}

std::vector<Scene::Scene*> Scene::Node::copyChildren() const {
    std::vector<Scene*> children;

    for (const auto& child: m_Children) {
        children.push_back(child->copy());
    }

    return children;
}

void Scene::Node::deleteChildren() const {
    for (const auto& child: m_Children) {
        delete &child;
    }
}
