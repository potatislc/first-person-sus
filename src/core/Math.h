#include <glm/vec3.hpp>

namespace Math::Vec3 {
    inline constexpr glm::vec3 forward{0.f, 0.f, -1.f};
    inline constexpr glm::vec3 backward{0.f, 0.f, 1.f};
    inline constexpr glm::vec3 up{0.f, 1.f, 0.f};
    inline constexpr glm::vec3 down{0.f, -1.f, 0.f};
    inline constexpr glm::vec3 left{-1.f, 0.f, 0.f};
    inline constexpr glm::vec3 right{1.f, 0.f, 0.f};

    inline constexpr glm::vec3 one{1.f};
    inline constexpr glm::vec3 zero{0.f};
}
