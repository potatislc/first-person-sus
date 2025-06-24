#include "../core/Application.h"

int main([[maybe_unused]] int /*unused*/, [[maybe_unused]] char** /*unused*/) {
#ifdef __linux__
    setenv("ASAN_OPTIONS", "detect_leaks=1", 1);
#endif
    Application application{"Hej", 960, 540};
    application.run();
}
