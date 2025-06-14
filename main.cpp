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
#include "src/renderer/shader/Parser.h"
#include "src/renderer/shader/Program.h"

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

    unsigned int vao{};
    RENDERER_API_CALL(renderer, glGenVertexArrays(1, &vao));
    RENDERER_API_CALL(renderer, glBindVertexArray(vao));

    unsigned int buffer{};
    RENDERER_API_CALL(renderer, glGenBuffers(1, &buffer));
    RENDERER_API_CALL(renderer, glBindBuffer(GL_ARRAY_BUFFER, buffer));
    RENDERER_API_CALL(renderer,
                      glBufferData(GL_ARRAY_BUFFER, sizeof(famousSquare), famousSquare.data(), GL_STATIC_DRAW));

    RENDERER_API_CALL(renderer, glEnableVertexAttribArray(0));
    RENDERER_API_CALL(renderer,
                      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr));

    unsigned int ibo{};
    RENDERER_API_CALL(renderer, glGenBuffers(1, &ibo));
    RENDERER_API_CALL(renderer, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    RENDERER_API_CALL(renderer,
                      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW));

    const Renderer::Shader::Program shaderProgram{renderer, Renderer::Shader::Parser{"../res/shader/Basic.glsl"}};
    shaderProgram.use();
    const int location = RENDERER_API_CALL_RETURN(renderer,
                                                  glGetUniformLocation(shaderProgram.getId(), "u_Color"));
    assert(location != -1);
    RENDERER_API_CALL(renderer, glUseProgram(0));
    RENDERER_API_CALL(renderer, glBindBuffer(GL_ARRAY_BUFFER, 0));
    RENDERER_API_CALL(renderer, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

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
        RENDERER_API_CALL(renderer, glClearColor(0.1f, 0.2f, blue, 1.0f));
        RENDERER_API_CALL(renderer, glClear(GL_COLOR_BUFFER_BIT));

        RENDERER_API_CALL(renderer, glUseProgram(shaderProgram.getId()));
        RENDERER_API_CALL(renderer, glUniform4f(location, blue, .3f, .8f, 1.f));

        /*RENDERER_API_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
        RENDERER_API_CALL(glEnableVertexAttribArray(0));
        RENDERER_API_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr));*/
        RENDERER_API_CALL(renderer, glBindVertexArray(vao));
        RENDERER_API_CALL(renderer, glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        // glDrawArrays(GL_TRIANGLES, 0, famousSquare.size());
        RENDERER_API_CALL(renderer,
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
