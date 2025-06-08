#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <array>
#include <utility>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

inline constexpr std::string windowName = "Game";


static SDL_Window* createGlWindow(const std::string& name, unsigned int width, unsigned int height) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    std::cout << "Requested OpenGL version: " << major << "." << minor << '\n';

    SDL_Window* window = SDL_CreateWindow(name.c_str(), static_cast<int>(width), static_cast<int>(height),
                                          SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << '\n';
        return nullptr;
    }

    return window;
}

static SDL_GLContext createGlContext(SDL_Window* window) {
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << '\n';
        return nullptr;
    }

    return context;
}

static bool initializeGlLoader() {
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';

    return true;
}

struct Shader {
    Shader() = default;
    // Shader(unsigned int type, std::string source) : source(std::move(source)), type(type) {}
    bool hasValidSource() const {
        return !source.empty() && type != 0;
    }

    [[nodiscard]] bool isCompiled() const {
        return hasValidSource() && id != 0;
    }

    operator bool() {
        return hasValidSource();
    }

    std::string source;
    unsigned int type{};
    unsigned int id{};
};

class ShaderProgram {
public:
    ShaderProgram() = delete;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) : id(other.id) {
        other.id = 0;
    }
    ShaderProgram& operator=(ShaderProgram&& other) {
        id = other.id;
        other.id = 0;
        return *this;
    }
    explicit ShaderProgram(unsigned int id) : id(id) {}

    explicit operator unsigned int() const {
        return id;
    }

    ~ShaderProgram() {
        if (id != 0) {
            glDeleteProgram(id);
        }
    }

private:
    unsigned int id{};
};

class ShaderSourceParser {
public:
    explicit ShaderSourceParser(const std::string& filePath) : m_stream(filePath) {
        assert(m_stream.good());
    }

    Shader operator()() {
        auto fail = [this](const std::string& description) {
            std::cerr << "Failed to parse shader source: " << description << " At line "
            << m_lineCount << ". " << "[LINE SOURCE]: " << m_line << std::endl;
            return Shader{};
        };

        std::stringstream ss;
        Shader shader;
        shader.type = m_nextShaderType;

        while(std::getline(m_stream, m_line)) {
            ++m_lineCount;

            if (std::all_of(m_line.begin(), m_line.end(),
                            [](unsigned char c){ return std::isspace(c); })) {
                continue;
            }

            if (m_line.contains("#shader")) {
                if (m_line.contains("vertex")) {
                    m_nextShaderType = GL_VERTEX_SHADER;
                }
                else if (m_line.contains("fragment")) {
                    m_nextShaderType = GL_FRAGMENT_SHADER;
                }
                else {
                    return fail("Could not deduce shader type.");
                }

                if (shader.type != 0) {
                    break;
                }

                shader.type = m_nextShaderType;

                continue;
            }

            ss << m_line << '\n';
        }

        if (shader.type == 0) {
            return fail("Shader type is not set.");
        }

        shader.source = ss.str();

        return shader;
    }

    Shader next() {
        return (*this)();
    }

private:
    std::ifstream m_stream;
    std::string m_line;
    size_t m_lineCount{};
    unsigned int m_nextShaderType{};
};

static void compileShader(Shader& shader) {
    auto id = glCreateShader(shader.type);
    const auto *const src = shader.source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int success{};
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        std::array<char, 512> infoLog{};
        glGetShaderInfoLog(id, infoLog.size(), nullptr, infoLog.data());
        std::cerr << "Shader compilation failed:\n" << infoLog.data() << "\n";
        return;
    }

    shader.id = id;
}

template <typename ...Args>
static auto createShaderProgram(Args&... shaders) {
    static auto fail = []() {
        std::cerr << "Failed to create shader program." << '\n';
        return 0u;
    };

    unsigned int program = glCreateProgram();
    if (program == 0) {
        return ShaderProgram(fail());
    }

    bool success = ((compileShader(shaders), shaders.isCompiled()) && ...);
    if (!success) {
        return ShaderProgram(fail());
    }

    (glAttachShader(program, shaders.id), ...);

    glLinkProgram(program);

    int linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == 0) {
        return ShaderProgram(fail());
    }

    return ShaderProgram(program);
}

int main(int argc, char* argv[]) {
#ifdef __linux__
    setenv("ASAN_OPTIONS", "detect_leaks=1", 1);
#endif

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = createGlWindow(windowName, 800, 600);
    if (window == nullptr) {
        SDL_Quit();
        return 1;
    }

    SDL_GLContext context = createGlContext(window);
    if (context == nullptr) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!initializeGlLoader()) {
        SDL_DestroyWindow(window);
        SDL_GL_DestroyContext(context);
        SDL_Quit();
        return 1;
    }

    unsigned int vao{};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::array<glm::vec2, 3> famousTriangle = {{
                                                       {-.5f, -.5f},
                                                       {0.f, .5f},
                                                       {.5f, -.5f}
                                               }};

    unsigned int buffer{};
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(famousTriangle), famousTriangle.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);


    auto parser = ShaderSourceParser("../res/shader/Basic.shader");
    auto shader1 = parser.next();
    auto shader2 = parser.next();
    std::cout << "Vertex " << shader1.type << ": " << shader1.source
        << "Fragment "  << shader2.type << ": " << shader2.source << std::endl;

    auto shaderProgram = createShaderProgram(shader1, shader2);
    glUseProgram(static_cast<unsigned int>(shaderProgram));

    bool running = true;
    SDL_Event event;

    const uint64_t perfFreq = SDL_GetPerformanceFrequency();
    const unsigned int FPS = 60;
    const double targetFrameTime = 1.0 / FPS;
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
        glClearColor(0.1f, 0.2f, blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);

        uint64_t frameEnd = SDL_GetPerformanceCounter();
        uint64_t elapsedTicks = frameEnd - frameStart;

        if (elapsedTicks < targetTicks) {
            uint64_t delayTicks = targetTicks - elapsedTicks;
            auto delayMs = static_cast<uint32_t>((delayTicks * 1000) / perfFreq);
            SDL_Delay(delayMs);
        }

        frameCount++;
        frameStart = SDL_GetPerformanceCounter();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
