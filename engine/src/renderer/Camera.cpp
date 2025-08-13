#include "Camera.h"

#include "core/InputMap.h"

void Engine::Renderer::Camera::debugMove(const double deltaTime, const float moveSpeed, const float mouseSensitivity) {
    auto& inputMap = InputMap::getInstance();

    static const auto actionForward = inputMap.createAction("DebugCamForward");
    static const auto actionBackward = inputMap.createAction("DebugCamBackward");
    static const auto actionRight = inputMap.createAction("DebugCamRight");
    static const auto actionLeft = inputMap.createAction("DebugCamLeft");
    static const auto actionUp = inputMap.createAction("DebugCamUp");
    static const auto actionDown = inputMap.createAction("DebugCamDown");
    static const auto actionShoot = inputMap.createAction("DebugCamShoot");

    const auto bind = [&inputMap] {
        inputMap.bindToKey(SDL_SCANCODE_W, actionForward);
        inputMap.bindToKey(SDL_SCANCODE_S, actionBackward);
        inputMap.bindToKey(SDL_SCANCODE_D, actionRight);
        inputMap.bindToKey(SDL_SCANCODE_A, actionLeft);
        inputMap.bindToKey(SDL_SCANCODE_SPACE, actionUp);
        inputMap.bindToKey(SDL_SCANCODE_LSHIFT, actionDown);
        inputMap.bindToMouseButton(SDL_BUTTON_LEFT, actionShoot);
        return 0;
    };

    [[maybe_unused]] static const auto callBind{bind()};


    if (inputMap.isActionJustPressed(actionShoot)) {
        LOG("Shoot!\n");
    }

    const glm::vec3 forward{glm::normalize(getDirection() * glm::vec3{1.f, 0.f, 1.f})};
    const glm::vec3 right{glm::normalize(glm::cross(forward, Math::Vec3::up))};
    auto velocity{Math::Vec3::zero};

    if (inputMap.isActionPressed(actionForward)) {
        velocity += forward;
    }

    if (inputMap.isActionPressed(actionBackward)) {
        velocity -= forward;
    }

    if (inputMap.isActionPressed(actionRight)) {
        velocity += right;
    }

    if (inputMap.isActionPressed(actionLeft)) {
        velocity -= right;
    }

    if (inputMap.isActionPressed(actionUp)) {
        velocity += Math::Vec3::up;
    }

    if (inputMap.isActionPressed(actionDown)) {
        velocity += Math::Vec3::down;
    }

    // Normalize so diagonal strafing isn't faster
    if (glm::length(velocity) > 0.0f) {
        velocity = glm::normalize(velocity);
    }

    // Move
    translate(velocity * moveSpeed * static_cast<float>(deltaTime));

    // Rotate
    const glm::vec2 mouseDelta = inputMap.getMouseVelocity();
    rotateFromMouseDelta(mouseDelta * static_cast<float>(deltaTime), mouseSensitivity);
}
