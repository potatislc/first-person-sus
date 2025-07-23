#include "Application.h"

#include <chrono>
#include <imgui.h>
#include <print>
#include <thread>

#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "InputMap.h"

#include "renderer/GlRenderer.h"
#include "scene/test/Test.h"
#include "scene/test/Cube.h"

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<double>;

Engine::Application* Engine::Application::s_instance{};

Engine::Application::Application(const std::string& name, const unsigned int width, const unsigned int height,
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

Engine::Application& Engine::Application::initialize(const std::string& name, const unsigned int width,
                                                     const unsigned int height, const Renderer::Type rendererType) {
    static Application instance{name, width, height, rendererType};
    s_instance = &instance;
    return instance;
}

Engine::Application::~Application() {
    m_baseScene.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    m_renderer.reset();
    m_window.destroy();

    SDL_Quit();
}

void Engine::Application::run() {
    if (m_baseScene == nullptr) {
        LOG_ERR("Application is performing default behaviour. No base scene is assigned!");
    }

    auto running{true};
    SDL_Event event{};

    constexpr unsigned int fps{120};
    constexpr Duration targetFrameTime{1.0 / fps};
    auto lastFrameStart{Clock::now()};

    while (running) {
        const auto frameStart{Clock::now()};

        const Duration deltaTime{frameStart - lastFrameStart};
        m_timeSinceInit += deltaTime.count();
        lastFrameStart = frameStart;
        auto& inputMap{InputMap::getInstance()};
        inputMap.updateActionState();

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            switch (event.type) {
                case SDL_EVENT_KEY_DOWN: {
                    if (!event.key.repeat) {
                        LOG("Key press detected\n");
                        inputMap.getKeyBind(event.key.key /* event.key.scancode */).setActionState(
                            inputMap, InputMap::ActionState::JUST_PRESSED);
                    }
                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    }
                }
                break;

                case SDL_EVENT_KEY_UP:
                    LOG("Key release detected\n");
                    inputMap.getKeyBind(event.key.key).setActionState(
                        inputMap, InputMap::ActionState::JUST_RELEASED);
                    break;

                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                default:
                    break;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (m_baseScene != nullptr) {
            m_baseScene->update(deltaTime.count());
            m_baseScene->render(*m_renderer);
        }

        if (m_baseScene != nullptr) {
            m_baseScene->renderImGui();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_renderer->swapWindow(m_window);

        const auto frameEnd = Clock::now();

        if (const Duration elapsed{frameEnd - frameStart}; elapsed < targetFrameTime) {
            static constexpr auto sleepPrecision{std::chrono::milliseconds(2)};
            if (const auto remaining = targetFrameTime - elapsed; remaining > sleepPrecision) {
                std::this_thread::sleep_for(remaining - sleepPrecision);
            }

            // Spin wait (maybe don't do this for portable devices)
            while (Clock::now() - frameStart < targetFrameTime) {
            }
        }

        m_frameCount++;
    }
}

void Engine::Application::setBaseScene(std::unique_ptr<Scene::Scene> baseScene) {
    m_baseScene = std::move(baseScene);
}
