#include "Application.h"

#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "../renderer/GlRenderer.h"
#include "../scene/test/Test.h"
#include "../scene/test/Cube.h"

Application* Application::s_instance{};

Application::Application(const std::string& name, const unsigned int width, const unsigned int height,
                         const Renderer::Type rendererType) {
    switch (rendererType) {
        case Renderer::Type::OPEN_GL:
            m_window = Renderer::GlRenderer::createWindow(name, static_cast<int>(width), static_cast<int>(height));
            m_renderer = std::make_unique<Renderer::GlRenderer>(m_window);

            break;
        default:
            LOG_ERR("Application could not start. No RendererType selected. ");
            return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(m_window.get(), m_renderer->getContext());
    ImGui_ImplOpenGL3_Init("#version 330");
}

Application& Application::initialize(const std::string& name, const unsigned int width,
                                     const unsigned int height, const Renderer::Type rendererType) {
    static Application instance{name, width, height, rendererType};
    s_instance = &instance;
    return instance;
}

Application::~Application() {
    m_baseScene.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    m_renderer.reset();
    m_window.destroy();

    SDL_Quit();
}

void Application::run() {
    if (m_baseScene == nullptr) {
        LOG_ERR("Application is performing default behaviour. No base scene is assigned!");
    }

    auto running = true;
    SDL_Event event{};

    const uint64_t perfFreq = SDL_GetPerformanceFrequency();
    constexpr unsigned int fps = 60;
    constexpr double targetFrameTime = 1.0 / fps;
    const auto targetTicks = static_cast<uint64_t>(targetFrameTime * static_cast<double>(perfFreq));
    uint64_t frameStart = SDL_GetPerformanceCounter();

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        if (m_baseScene != nullptr) {
            m_baseScene->update(0.f);
            m_baseScene->render(*m_renderer);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (m_baseScene != nullptr) {
            m_baseScene->renderImGui();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_renderer->swapWindow(m_window);

        uint64_t const frameEnd = SDL_GetPerformanceCounter();

        if (uint64_t const elapsedTicks = frameEnd - frameStart; elapsedTicks < targetTicks) {
            uint64_t const delayTicks = targetTicks - elapsedTicks;
            const auto delayMs = static_cast<uint32_t>((delayTicks * 1000) / perfFreq);
            SDL_Delay(delayMs);
        }

        m_frameCount++;
        frameStart = SDL_GetPerformanceCounter();
    }
}

void Application::setBaseScene(std::unique_ptr<Scene::Scene> baseScene) {
    m_baseScene = std::move(baseScene);
}
