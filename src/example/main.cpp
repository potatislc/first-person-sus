#include "../core/Application.h"
#include "../scene/test/Test.h"
#include "../scene/test/Cube.h"

int main([[maybe_unused]] int /*unused*/, [[maybe_unused]] char** /*unused*/) {
#ifdef __linux__
    setenv("ASAN_OPTIONS", "detect_leaks=1", 1);
#endif
    Application& application = Application::initialize("Hej", 960, 540);
    application.setBaseScene(std::move(std::make_unique<Scene::Cube>()));
    application.run();
}
