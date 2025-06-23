#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <array>
#include <string>
#include <glad/glad.h>
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
    const Window window = Renderer::GlRenderer::createWindow("Hej", 800, 600);
    const auto renderer = Renderer::GlRenderer{window};

    constexpr std::array<glm::vec2, 8> square{
        {
            {-.5f, -.5f}, {0.f, 0.f}, // Top-left
            {.5f, -.5f}, {1.f, 0.f}, // Top-right
            {.5f, .5f}, {1.f, 1.f}, // Bottom-right
            {-.5f, .5f}, {0.f, 1.f} // Bottom-left
        }
    };

    const std::array<uint32_t, 6> indices = {
        0, 1, 2,
        2, 3, 0
    };

    RENDERER_API_CALL(glEnable(GL_BLEND));
    RENDERER_API_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    const Renderer::VertexArray vertexArray;

    const Renderer::Buffer::Vertex vertexBuffer{square.data(), sizeof(square)};
    Renderer::Buffer::Vertex::Layout vertexLayout{};
    vertexLayout.push<float>(glm::vec2::length());
    vertexLayout.push<float>(glm::vec2::length());
    vertexArray.addBuffer(vertexBuffer, vertexLayout);

    const Renderer::Buffer::Index indexBuffer{indices.data(), indices.size()};

    const auto projection = glm::ortho(-2.f, 2.f, -1.5f, 1.5f, 1.f, -1.f);

    const Renderer::Shader::Program shaderProgram{Renderer::Shader::Parser{"../res/shader/Basic.glsl"}};

    const Renderer::Texture texture{"../res/texture/Melon.png"};
    texture.bind(0);
    shaderProgram.setUniform("u_Texture", 0);
    shaderProgram.setUniform("u_Mvp", projection);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window.get(), renderer.getContext());
    ImGui_ImplOpenGL3_Init("#version 330");
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

        renderer.draw(vertexArray, indexBuffer, shaderProgram);
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        } {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*) &clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))
                // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window) {
            ImGui::Begin("Another Window", &show_another_window);
            // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
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
