#pragma once
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "core/Math.h"

namespace Engine::Renderer {
    class Camera {
    public:
        explicit Camera(const glm::mat4& view = glm::mat4{1.f}, const glm::vec3& position = Math::Vec3::zero,
                        const glm::vec3& direction = Math::Vec3::forward,
                        const float fov = 90.f, const glm::vec2 aspectRatio = glm::vec2{16.f, 9.f})
            : m_projection{glm::perspective(glm::radians(fov / 2), aspectRatio.x / aspectRatio.y, 0.1f, 100.0f)},
              m_view{view},
              m_position{position}, m_halfFov{fov / 2} {
            setDirection(direction);
            m_yaw = glm::degrees(atan2(m_direction.z, m_direction.x));
            m_pitch = glm::degrees(asin(m_direction.y));
        }

        [[nodiscard]] glm::vec3 basisRight() const {
            return glm::normalize(glm::cross(Math::Vec3::up, m_direction));
        }

        [[nodiscard]] glm::vec3 basisUp() const {
            return glm::normalize(glm::cross(m_direction, basisRight()));
        }

        [[nodiscard]] glm::mat4 getView() const {
            return m_view;
        }

        [[nodiscard]] glm::mat4 getProjection() const {
            return m_projection;
        }

        [[nodiscard]] glm::vec3 getPosition() const {
            return m_position;
        }

        void updateViewMatrix() {
            m_view = glm::lookAt(m_position, m_position + m_direction, Math::Vec3::up);
        }

        void setPosition(const glm::vec3& position) {
            m_position = position;
            updateViewMatrix();
        }

        void translate(const glm::vec3& velocity) {
            m_position += velocity;
            updateViewMatrix();
        }

        void lookAt(const glm::vec3 target) {
            m_direction = glm::normalize(target - m_position);
            updateViewMatrix();
        }

        [[nodiscard]] glm::vec3 getDirection() const {
            return m_direction;
        }

        void setDirection(const glm::vec3 direction) {
            m_direction = glm::normalize(direction);
            updateViewMatrix();
        }

        void rotateFromMouseDelta(const glm::vec2& mouseVelocity, const float sensitivity = 0.1f) {
            m_yaw += mouseVelocity.x * sensitivity;
            m_pitch -= mouseVelocity.y * sensitivity;

            // Clamp pitch to prevent flipping
            m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

            // Convert to radians
            const float yawRad = glm::radians(m_yaw);
            const float pitchRad = glm::radians(m_pitch);

            glm::vec3 direction;
            direction.x = cos(pitchRad) * cos(yawRad);
            direction.y = sin(pitchRad);
            direction.z = cos(pitchRad) * sin(yawRad);

            setDirection(glm::normalize(direction));
        }

    private:
        glm::mat4 m_projection{};
        glm::mat4 m_view{};
        glm::vec3 m_position{};
        glm::vec3 m_direction{};
        float m_halfFov{};
        float m_pitch{};
        float m_yaw{};
    };
}
