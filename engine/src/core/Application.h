#pragma once

#include <string>

#include "../renderer/Renderer.h"
#include "../scene/Scene.h"
#include "Window.h"

namespace Engine::Scene {
    class Node;
    class Scene;
}

namespace Engine {
    class Application {
    public:
        static Application& initialize(const std::string& name, unsigned int width,
                                       unsigned int height,
                                       Renderer::Type rendererType = Renderer::Type::OPEN_GL);

        Application(const Application&) = delete;

        Application& operator=(const Application&) = delete;

        Application(Application&&) = delete;

        Application& operator=(Application&&) = delete;

        ~Application();

        void run();

        [[nodiscard]] uint64_t getFrameCount() const {
            return m_frameCount;
        }

        [[nodiscard]] static Application& getInstance() {
            if (s_instance == nullptr) {
                LOG_ERR("In Core::Application::getInstance(): Application is not initialized\n");
                exit(1);
            }

            return *s_instance;
        }

        void setBaseScene(std::unique_ptr<Scene::Scene> baseScene);

        [[nodiscard]] const Window& getWindow() const {
            return m_window;
        }

        [[nodiscard]] const Renderer::Renderer& getRenderer() const {
            return *m_renderer;
        }

    private:
        Application() = default;

        Application(const std::string& name, unsigned int width, unsigned int height,
                    Renderer::Type rendererType = Renderer::Type::OPEN_GL);

        static Application* s_instance;

        Window m_window;
        std::unique_ptr<Renderer::Renderer> m_renderer;
        std::unique_ptr<Scene::Scene> m_baseScene;
        uint64_t m_frameCount{};
    };
}
