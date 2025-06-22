#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <array>
#include <string>
#include <glad/glad.h>

#include "Window.h"
#include "renderer/GlRenderer.h"
#include "renderer/buffer/Index.h"
#include "renderer/buffer/Vertex.h"
#include "renderer/shader/Parser.h"
#include "renderer/shader/Program.h"
#include "renderer/VertexArray.h"
#include "renderer/texture/Texture.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
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
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const auto blue = static_cast<float>(frameCount % 256) / 255.0f;

        renderer.clear({0.1f, 0.2f, blue, 1.0f});

        shaderProgram.bind();
        shaderProgram.setUniform("u_Color", glm::vec4{blue, .3f, .8f, 1.f});

        renderer.draw(vertexArray, indexBuffer, shaderProgram);

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
}
