#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <array>
#include <string>
#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "Window.h"
#include "renderer/GlRenderer.h"
#include "renderer/buffer/Index.h"
#include "renderer/buffer/Vertex.h"
#include "renderer/shader/Parser.h"
#include "renderer/shader/Program.h"
#include "renderer/VertexArray.h"
#include "renderer/texture/Texture.h"

int main(int /*unused*/, char** /*unused*/) {
#ifdef __linux__
    setenv("ASAN_OPTIONS", "detect_leaks=1", 1);
#endif
    const Window window = Renderer::GlRenderer::createWindow("Hej", 960, 540);
    const auto renderer = Renderer::GlRenderer{window};

    std::array<glm::vec2, 8> square{
        {
            {-50.f, -50.f}, {0.f, 0.f}, // Top-left
            {50.f, -50.f}, {1.f, 0.f}, // Top-right
            {50.f, 50.f}, {1.f, 1.f}, // Bottom-right
            {-50.f, 50.f}, {0.f, 1.f} // Bottom-left
        }
    };

    const std::array<uint32_t, 6> indices = {
        0, 1, 2,
        2, 3, 0
    };

    const Renderer::VertexArray vertexArray;

    const Renderer::Buffer::Vertex vertexBuffer{square.data(), sizeof(square)};
    Renderer::Buffer::Vertex::Layout vertexLayout{};
    vertexLayout.push<float>(glm::vec2::length());
    vertexLayout.push<float>(glm::vec2::length());
    vertexArray.addBuffer(vertexBuffer, vertexLayout);

    const Renderer::Buffer::Index indexBuffer{indices.data(), indices.size()};

    const auto projection = glm::ortho(0.f, 960.f, 0.f, 540.f, -1.f, 1.f);
    const auto view = glm::translate(glm::mat4{1.f}, glm::vec3{0, 0, 0});

    const Renderer::Shader::Program shaderProgram{Renderer::Shader::Parser{"../res/shader/Basic.glsl"}};

    const Renderer::Texture texture{"../res/texture/Melon.png"};
    texture.bind(0);
    shaderProgram.setUniform("u_Texture", 0);

    glm::vec3 translation{200, 200, 0};

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window.get(), renderer.getContext());
    ImGui_ImplOpenGL3_Init("#version 330");

    bool running = true;
    SDL_Event event;

    const uint64_t perfFreq = SDL_GetPerformanceFrequency();
    constexpr unsigned int FPS = 60;
    constexpr double targetFrameTime = 1.0 / FPS;
    const auto targetTicks = static_cast<uint64_t>(targetFrameTime * static_cast<double>(perfFreq));
    uint64_t frameStart = SDL_GetPerformanceCounter();
    uint64_t frameCount{};

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const auto blue = static_cast<float>(frameCount % 256) / 255.0f;

        renderer.clear({0.1f, 0.2f, blue, 1.0f});

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        shaderProgram.bind();
        shaderProgram.setUniform("u_Color", glm::vec4{blue, .3f, .8f, 1.f});
        const auto model = glm::translate(glm::mat4{1.f}, translation);
        const auto mvp = projection * view * model;
        shaderProgram.setUniform("u_Mvp", mvp);
        renderer.draw(vertexArray, indexBuffer, shaderProgram);

        // Debug Window
        {
            ImGui::SliderFloat3("float", &translation[0], 0.0f, 960.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / io.Framerate, io.Framerate);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        renderer.swapWindow(window);

        uint64_t const frameEnd = SDL_GetPerformanceCounter();

        if (uint64_t const elapsedTicks = frameEnd - frameStart; elapsedTicks < targetTicks) {
            uint64_t const delayTicks = targetTicks - elapsedTicks;
            const auto delayMs = static_cast<uint32_t>((delayTicks * 1000) / perfFreq);
            SDL_Delay(delayMs);
        }

        frameCount++;
        frameStart = SDL_GetPerformanceCounter();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}
