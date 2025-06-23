#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "../renderer/Renderer.h"

namespace Scene {
    class Scene;
}

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
        return m_FrameCount;
    }

    const Window& getWindow() const {
        return m_Window;
    }

    const Renderer::Renderer& getRenderer() const {
        return *m_Renderer;
    }

private:
    Window m_Window{};
    Renderer::Renderer* m_Renderer{};
    uint64_t m_FrameCount{};
    Scene::Scene* m_BaseScene{};
};


#endif
