#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include "../renderer/Renderer.h"

namespace Scene {
    class Scene;
}

class Application {
public:
    Application(std::string name, unsigned int width, unsigned int height, auto rendererType = Renderer::Type::OPEN_GL);

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

    Application(Application&&) = delete;

    Application& operator=(Application&&) = delete;

    ~Application();

    void run();

private:
    Window m_Window{};
    Renderer::Renderer* m_Renderer{};
    uint64_t m_FrameCount{};
    Scene::Scene* m_BaseScene{};
};


#endif
