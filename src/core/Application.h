#pragma once

#include <string>

#include "../renderer/Renderer.h"
#include "Window.h"

namespace Scene {
    class Scene;
}

namespace Core {
    class Application {
    public:
        Application(const std::string& name, unsigned int width, unsigned int height,
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

        [[nodiscard]] static const Application& getUniqueInstance() {
            CORE_ASSERT_MSG(s_uniqueInstance != nullptr, "Unique application instance is null. \n");
            return *s_uniqueInstance;
        }

        void setBaseScene(std::unique_ptr<Scene::Scene> baseScene);

        [[nodiscard]] const Window& getWindow() const {
            return m_window;
        }

        [[nodiscard]] const Renderer::Renderer& getRenderer() const {
            return *m_renderer;
        }

    private:
        static Application* s_uniqueInstance;

        Window m_window;
        std::unique_ptr<Renderer::Renderer> m_renderer;
        std::unique_ptr<Scene::Scene> m_baseScene;
        uint64_t m_frameCount{};
    };
}
