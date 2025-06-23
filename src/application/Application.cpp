#include "Application.h"

#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "../renderer/GlRenderer.h"
#include "../scene/test/Test.h"

Application::Application(const std::string name, const unsigned int width, const unsigned int height,
                         auto rendererType) {
    switch (rendererType) {
        case Renderer::Type::OPEN_GL:
            m_Window = Renderer::GlRenderer::createWindow(name, width, height);
            m_Renderer = new Renderer::GlRenderer{m_Window};
            break;
        default:
            std::cerr << "Application could not start. No RendererType selected. \n";
            ASSERT(false);
            return;
    }

    m_BaseScene = new Scene::Test{};

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(m_Window.get(), m_Renderer->getContext());
    ImGui_ImplOpenGL3_Init("#version 330");
}

Application::~Application() {
    delete m_BaseScene;
    delete m_Renderer;
}

void Application::run() {
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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_Renderer->swapWindow(m_Window);

        uint64_t const frameEnd = SDL_GetPerformanceCounter();

        if (uint64_t const elapsedTicks = frameEnd - frameStart; elapsedTicks < targetTicks) {
            uint64_t const delayTicks = targetTicks - elapsedTicks;
            const auto delayMs = static_cast<uint32_t>((delayTicks * 1000) / perfFreq);
            SDL_Delay(delayMs);
        }

        m_FrameCount++;
        frameStart = SDL_GetPerformanceCounter();
    }
}
