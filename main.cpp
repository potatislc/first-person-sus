#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <utility>
#include <fstream>
#include <string>
#include "./src/Window.h"
#include "src/renderer/GlRenderer.h"
#include "src/renderer/buffer/Index.h"
#include "src/renderer/buffer/Vertex.h"
#include "src/renderer/shader/Parser.h"
#include "src/renderer/shader/Program.h"
#include "src/renderer/VertexArray.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
#ifdef __linux__
    setenv("ASAN_OPTIONS", "detect_leaks=1", 1);
#endif
    const Window window = Renderer::GlRenderer::createWindow("Hej", 800, 600);
    const Renderer::GlRenderer renderer = Renderer::GlRenderer::create(window);

    [[maybe_unused]] constexpr std::array<glm::vec2, 3> famousTriangle = {
        {
            {-.5f, -.5f},
            {0.f, .5f},
            {.5f, -.5f}
        }
    };

    constexpr std::array<glm::vec2, 4> famousSquare = {
        {
            {-.5f, .5f}, // Top-left
            {.5f, .5f}, // Top-right
            {.5f, -.5f}, // Bottom-right
            {-.5f, -.5f} // Bottom-left
        }
    };

    const std::array<unsigned int, 6> indices = {
        0, 1, 2,
        2, 3, 0
    };

    const Renderer::VertexArray vertexArray;

    const Renderer::Buffer::Vertex vertexBuffer{famousSquare.data(), sizeof(famousSquare)};
    Renderer::Buffer::Vertex::Layout vertexLayout{};
    vertexLayout.push<float>(glm::vec2::length());
    vertexArray.addBuffer(vertexBuffer, vertexLayout);

    const Renderer::Buffer::Index indexBuffer{indices.data(), sizeof(indices)};

    const Renderer::Shader::Program shaderProgram{Renderer::Shader::Parser{"../res/shader/Basic.glsl"}};

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

        float blue = static_cast<float>(frameCount % 256) / 255.0f;
        RENDERER_API_CALL(glClearColor(0.1f, 0.2f, blue, 1.0f));
        RENDERER_API_CALL(glClear(GL_COLOR_BUFFER_BIT));

        shaderProgram.bind();
        RENDERER_API_CALL(glUniform4f(shaderProgram.getUniformLocation("u_Color"), blue, .3f, .8f, 1.f));


        vertexArray.bind();
        indexBuffer.bind();

        RENDERER_API_CALL(
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));

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
