#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <utility>
#include <fstream>
#include <string>
#include <sstream>
#include "./src/Window.h"
#include "./src/Renderer.h"

class Shader {
public:
    Shader() = delete;

    explicit Shader(const Renderer& renderer) : renderer{&renderer} {
    }

    Shader(const Renderer& renderer, const unsigned int type, std::string source) : source{std::move(source)},
        renderer{&renderer}, type{type} {
    }

    Shader(const Shader&) = delete;

    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept : source{std::move(other.source)}, renderer{renderer}, type{other.type},
                                      id{other.id} {
    }

    Shader& operator=(Shader&& other) noexcept {
        source = std::move(other.source);
        type = other.type;
        id = other.id;
        return *this;
    }

    void destroy() {
        if (id == 0) {
            return;
        }

        RENDERER_API_CALL(*renderer, glDeleteShader(id));
        id = 0;
    }

    ~Shader() {
        destroy();
    }

    [[nodiscard]] bool hasValidSource() const {
        return !source.empty() && type != 0;
    }

    [[nodiscard]] bool isCompiled() const {
        return hasValidSource() && id != 0 && type != 0;
    }

    void compile() {
        if (!hasValidSource()) {
            std::cerr << "Shader compilation failed: " << "Invalid source\n";
            return;
        }

        const auto shaderId = RENDERER_API_CALL_RETURN(*renderer, glCreateShader(type));
        const auto* const src = source.c_str();
        RENDERER_API_CALL(*renderer, glShaderSource(shaderId, 1, &src, nullptr));
        RENDERER_API_CALL(*renderer, glCompileShader(shaderId));

        int success{};
        RENDERER_API_CALL(*renderer, glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success));
        if (success == 0) {
            std::array<char, 512> infoLog{};
            RENDERER_API_CALL(*renderer,
                              glGetShaderInfoLog(shaderId, infoLog.size(), nullptr, infoLog.data()));
            std::cerr << "Shader compilation failed:\n" << infoLog.data() << "\n";
            return;
        }

        id = shaderId;
    }

    [[nodiscard]] auto getId() const {
        return id;
    }

    [[nodiscard]] auto getType() const {
        return type;
    }

    [[nodiscard]] auto getSource() const {
        return source;
    }

    explicit operator bool() const {
        return hasValidSource();
    }

private:
    std::string source;
    const Renderer* renderer{};
    unsigned int type{};
    unsigned int id{};
};

class ShaderSourceParser {
public:
    ShaderSourceParser() = delete;

    ShaderSourceParser(const ShaderSourceParser&) = delete;

    ShaderSourceParser& operator=(const ShaderSourceParser&) = delete;

    ShaderSourceParser(ShaderSourceParser&&) = delete;

    ShaderSourceParser& operator=(ShaderSourceParser&&) = delete;

    ~ShaderSourceParser() = default;

    explicit ShaderSourceParser(const std::string& filePath) : m_stream{filePath} {
        if (m_stream.bad()) {
            std::cerr << "Invalid shader source path" << filePath << '\n';
        }

        assert(m_stream.good());
    }

    Shader operator()(const Renderer& renderer) {
        auto fail = [this, &renderer](const std::string& description) {
            std::cerr << "Failed to parse shader source: " << description << " At line "
                    << m_lineCount << ". " << "[LINE SOURCE]: " << m_line << '\n';
            return Shader{renderer};
        };

        std::stringstream ss;
        unsigned int shaderType = m_nextShaderType;

        while (std::getline(m_stream, m_line)) {
            ++m_lineCount;

            if (std::ranges::all_of(m_line,
                                    [](const unsigned char c) { return std::isspace(c); })) {
                continue;
            }

            if (m_line.contains("#shader")) {
                if (m_line.contains("vertex")) {
                    m_nextShaderType = GL_VERTEX_SHADER;
                } else if (m_line.contains("fragment")) {
                    m_nextShaderType = GL_FRAGMENT_SHADER;
                } else {
                    return fail("Could not deduce shader type.");
                }

                if (shaderType != 0) {
                    break;
                }

                shaderType = m_nextShaderType;

                continue;
            }

            ss << m_line << '\n';
        }

        if (shaderType == 0) {
            return fail("Shader type is not set.");
        }

        return Shader{renderer, shaderType, ss.str()};
    }

    Shader next(const Renderer& renderer) {
        return (*this)(renderer);
    }

private:
    std::ifstream m_stream;
    std::string m_line;
    size_t m_lineCount{};
    unsigned int m_nextShaderType{};
};

class ShaderProgram {
    const char* creationFailStr = "Failed to create shader program.";

    auto createProgram() {
        id = RENDERER_API_CALL_RETURN(*renderer, glCreateProgram());
        if (id == 0) {
            std::cerr << creationFailStr << '\n';
        }

        return static_cast<bool>(id);
    }

    [[nodiscard]] auto linkProgram() const {
        RENDERER_API_CALL(*renderer, glLinkProgram(id));

        int linkStatus = 0;
        RENDERER_API_CALL(*renderer, glGetProgramiv(id, GL_LINK_STATUS, &linkStatus));
        if (linkStatus == 0) {
            std::cerr << "Failed to link shader program." << '\n';
        }

        return static_cast<bool>(linkStatus);
    }

    void attachShader(const unsigned int shaderId) const {
        RENDERER_API_CALL(*renderer, glAttachShader(id, shaderId));
    }

public:
    template<typename... Args>
    explicit ShaderProgram(Args&... shaders) {
        if (!createProgram()) {
            return;
        }

        if (const bool success = ((shaders.compile(), shaders.isCompiled()) && ...); !success) {
            std::cerr << creationFailStr << '\n';
            return;
        }

        (attachShader(shaders.getId()), ...);

        if (!linkProgram()) {
            return;
        }
    }

    ShaderProgram(const Renderer& renderer, ShaderSourceParser sourceParser) : renderer(&renderer) {
        if (!createProgram()) {
            return;
        }

        while (auto shader{sourceParser.next(renderer)}) {
            shader.compile();
            if (!shader.isCompiled()) {
                std::cerr << creationFailStr << '\n';
                return;
            }

            attachShader(shader.getId());
        }

        if (!linkProgram()) {
            return;
        }
    }

    ShaderProgram(const ShaderProgram&) = delete;

    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept : renderer(other.renderer), id{other.id} {
        other.id = 0;
    }

    ShaderProgram& operator=(ShaderProgram&& other) noexcept {
        id = other.id;
        other.id = 0;
        return *this;
    }

    explicit ShaderProgram(const unsigned int id) : id{id} {
    }

    void use() const {
        RENDERER_API_CALL(*renderer, glUseProgram(id));
    }

    explicit operator unsigned int() const {
        return id;
    }

    [[nodiscard]] unsigned int getId() const {
        return id;
    }

    ~ShaderProgram() {
        if (id == 0) {
            return;
        }

        RENDERER_API_CALL(*renderer, glDeleteProgram(id));
    }

private:
    const Renderer* renderer{};
    unsigned int id{};
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
#ifdef __linux__
    setenv("ASAN_OPTIONS", "detect_leaks=1", 1);
#endif
    // Renderer::createGlRenderer(Window::createGlobalGlWindow("Hej", 800, 600));
    Window const window = Window::createGlWindow("Hej", 800, 600);
    GlRenderer const renderer = Renderer::createGlRenderer(window);

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

    const ShaderProgram shaderProgram{renderer, ShaderSourceParser{"../res/shader/Basic.glsl"}};
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

    return 0;
}
