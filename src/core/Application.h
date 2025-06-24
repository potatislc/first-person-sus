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

        uint64_t getFrameCount() const {
            return m_frameCount;
        }

        static const Application& getUniqueInstance() {
            CORE_ASSERT_MSG(s_uniqueInstance != nullptr, "Unique application instance is null. \n");
            return *s_uniqueInstance;
        }


        const Window& getWindow() const {
            return m_window;
        }

        const Renderer::Renderer& getRenderer() const {
            return *m_renderer;
        }

    private:
        static Application* s_uniqueInstance;

        Window m_window{};
        Renderer::Renderer* m_renderer{};
        uint64_t m_frameCount{};
        Scene::Scene* m_baseScene{};
    };
}
