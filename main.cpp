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

inline constexpr std::string windowName = "Game";

static void clearGlErrors() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

[[nodiscard]] static auto logGlErrors(const char* functionName = "", const char* fileName = "", const size_t line = 0) {
    auto emptyLog = true;
    while (const auto err = glGetError()) {
        std::cerr << "OpenGL error: (0x" << std::hex << err << std::dec << ")" << " from function: "
                << functionName << " inside file: " << fileName << "at line: " << line << '\n';
        emptyLog = false;
    }

    return emptyLog;
}

#ifdef NDEBUG

#define GL_CALL(x) (x)
#define GL_CALL_RET(x) (x)

#else

template<typename Func>
void callGl(Func&& func, const char* code, const char* file, const size_t line) {
    clearGlErrors();
    std::forward<Func>(func)();
    assert(logGlErrors(code, file, line));
}

template<typename Func>
auto callGlRet(Func&& func, const char* code, const char* file, const size_t line) -> decltype(func()) {
    clearGlErrors();
    auto result = std::forward<Func>(func)();
    assert(logGlErrors(code, file, line));
    return result;
}

#define GL_CALL(x) callGl([&] { x; }, #x, __FILE__, __LINE__)
#define GL_CALL_RET(x) callGlRet([&] { return (x); }, #x, __FILE__, __LINE__)

#endif

static SDL_Window* createGlWindow(const std::string& name, const unsigned int width, const unsigned int height) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int major{};
    int minor{};
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
    auto* context = SDL_GL_CreateContext(window);
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

    std::cout << "GL Version: " << GL_CALL_RET(glGetString(GL_VERSION)) << '\n';
    std::cout << "GLSL Version: " << GL_CALL_RET(glGetString(GL_SHADING_LANGUAGE_VERSION)) << '\n';

    return true;
}

class Shader {
public:
    Shader() = default;

    Shader(const unsigned int type, std::string source) : source{std::move(source)}, type{type} {
    }

    Shader(const Shader&) = delete;

    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept : source{std::move(other.source)}, type{other.type}, id{other.id} {
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

        GL_CALL(glDeleteShader(id));
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

        const auto shaderId = GL_CALL_RET(glCreateShader(type));
        const auto* const src = source.c_str();
        GL_CALL(glShaderSource(shaderId, 1, &src, nullptr));
        GL_CALL(glCompileShader(shaderId));

        int success{};
        GL_CALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success));
        if (success == 0) {
            std::array<char, 512> infoLog{};
            GL_CALL(glGetShaderInfoLog(shaderId, infoLog.size(), nullptr, infoLog.data()));
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

    Shader operator()() {
        auto fail = [this](const std::string& description) {
            std::cerr << "Failed to parse shader source: " << description << " At line "
                    << m_lineCount << ". " << "[LINE SOURCE]: " << m_line << '\n';
            return Shader{};
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

        return Shader{shaderType, ss.str()};
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

class ShaderProgram {
    const char* creationFailStr = "Failed to create shader program.";

    auto createProgram() {
        id = GL_CALL_RET(glCreateProgram());
        if (id == 0) {
            std::cerr << creationFailStr << '\n';
        }

        return static_cast<bool>(id);
    }

    [[nodiscard]] auto linkProgram() const {
        GL_CALL(glLinkProgram(id));

        int linkStatus = 0;
        GL_CALL(glGetProgramiv(id, GL_LINK_STATUS, &linkStatus));
        if (linkStatus == 0) {
            std::cerr << "Failed to link shader program." << '\n';
        }

        return static_cast<bool>(linkStatus);
    }

    void attachShader(const unsigned int shaderId) const {
        GL_CALL(glAttachShader(id, shaderId));
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

    explicit ShaderProgram(ShaderSourceParser sourceParser) {
        if (!createProgram()) {
            return;
        }

        while (auto shader{sourceParser.next()}) {
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

    ShaderProgram(ShaderProgram&& other) noexcept : id{other.id} {
        other.id = 0;
    }

    ShaderProgram& operator=(ShaderProgram&& other) noexcept {
        id = other.id;
        other.id = 0;
        return *this;
    }

    explicit ShaderProgram(unsigned int id) : id{id} {
    }

    void use() const {
        GL_CALL(glUseProgram(id));
    }

    explicit operator unsigned int() const {
        return id;
    }

    ~ShaderProgram() {
        if (id == 0) {
            return;
        }

        GL_CALL(glDeleteProgram(id));
    }

private:
    unsigned int id{};
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
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

    auto* const context = createGlContext(window);
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
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

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

    unsigned int buffer{};
    GL_CALL(glGenBuffers(1, &buffer));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(famousSquare), famousSquare.data(), GL_STATIC_DRAW));

    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr));

    unsigned int ibo{};
    GL_CALL(glGenBuffers(1, &ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW));

    /*auto parser = ShaderSourceParser("../res/shader/Basic.shader");
    auto shader1 = parser.next();
    auto shader2 = parser.next();
    std::cout << "Vertex " << shader1.getType() << ": " << shader1.getSource()
        << "Fragment "  << shader2.getType() << ": " << shader2.getSource() << '\n';*/

    const ShaderProgram shaderProgram{ShaderSourceParser{"../res/shader/Basic.glsl"}};
    shaderProgram.use();
    const int location = GL_CALL_RET(glGetUniformLocation(static_cast<unsigned int>(shaderProgram), "u_Color"));
    assert(location != -1);

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
        GL_CALL(glClearColor(0.1f, 0.2f, blue, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        // glDrawArrays(GL_TRIANGLES, 0, famousSquare.size());
        GL_CALL(glUniform4f(location, blue, .3f, .8f, 1.f));
        GL_CALL(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));

        SDL_GL_SwapWindow(window);

        uint64_t const frameEnd = SDL_GetPerformanceCounter();

        if (uint64_t const elapsedTicks = frameEnd - frameStart; elapsedTicks < targetTicks) {
            uint64_t const delayTicks = targetTicks - elapsedTicks;
            const auto delayMs = static_cast<uint32_t>((delayTicks * 1000) / perfFreq);
            SDL_Delay(delayMs);
        }

        frameCount++;
        frameStart = SDL_GetPerformanceCounter();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
