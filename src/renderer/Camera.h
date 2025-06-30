#pragma once
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

class Camera {
public:
    static constexpr glm::vec3 s_up{0.f, 1.f, 0.f};

    Camera(const glm::mat4& view, const glm::vec3& position, const glm::vec3& direction,
           const float fov = 90.f, const glm::vec2 aspectRatio = glm::vec2{16.f, 9.f})
        : m_projection{glm::perspective(glm::radians(fov / 2), aspectRatio.x / aspectRatio.y, 0.1f, 100.0f)},
          m_view{view},
          m_position{position},
          m_direction{direction}, m_fov{fov / 2} {
    }

    [[nodiscard]] glm::vec3 basisRight() const {
        return glm::normalize(glm::cross(s_up, m_direction));
    }

    [[nodiscard]] glm::vec3 basisUp() const {
        return glm::normalize(glm::cross(m_direction, basisRight()));
    }

private:
    glm::mat4 m_projection{};
    glm::mat4 m_view{};
    glm::vec3 m_position{};
    glm::vec3 m_direction{};
    float m_fov{};
};
