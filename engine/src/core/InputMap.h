#pragma once
#include <array>
#include <limits>
#include <vector>
#include <glm/vec2.hpp>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>

#include "Assert.h"

namespace Engine {
    class InputMap {
    public:
        using InputCode = uint8_t;
        using ActionId = uint16_t;

        enum class ActionState : uint8_t {
            RELEASED,
            PRESSED,
            JUST_RELEASED,
            JUST_PRESSED
        };

        static constexpr ActionId s_noAction{0};
        static constexpr auto s_noActionName = "No Action";

#define ASSERT_ACTION_INIT() ASSERT_MSG(isActionInitialized(action), "Action of id: " << action << " is not initialized!\n")
#define ASSERT_VALID_MOUSE_CODE() ASSERT_MSG(code != 0 && code <= m_mouseMap.size(), "Input code is outside valid range for mouse map: " << code << " (Must be: [" << SDL_BUTTON_LEFT << ", " << SDL_BUTTON_X2 << "])\n");

        class Binding {
        public:
            bool bindAction(ActionId action);

            void unbindAction(ActionId action);

            [[nodiscard]] ActionState getActionState(const InputMap& inputMap) const;

            void setActionState(InputMap& inputMap, ActionState state) const;

        private:
            static constexpr uint8_t s_capacity{2};

            [[nodiscard]] auto findAction(ActionId action);

            [[nodiscard]] bool isActionBound(ActionId action);

            std::vector<ActionId> m_boundActions;
        };

        InputMap() {
            m_actionNames.front() = s_noActionName;
        }

        static InputMap& getInstance(const size_t player = 0) {
            static std::vector<InputMap> instances{1};
            if (player + 1 > instances.size()) {
                instances.resize(player);
            }

            return instances[0];
        }

        Binding& getKeyBinding(const InputCode code) {
            return m_keyMap[code]; // Check bounds at call site
        }

        Binding& getMouseBinding(const InputCode code) {
            ASSERT_VALID_MOUSE_CODE();
            return m_mouseMap[code - 1];
        }

        ActionId createAction(std::string name) {
            m_actions.emplace_back();
            m_actionNames.emplace_back(std::move(name));
            return m_actions.size() - 1;
        }

        [[nodiscard]] ActionState getActionState(const ActionId action) const {
            ASSERT_ACTION_INIT();

            return m_actions[action];
        }

        [[nodiscard]] size_t getActionCount() const {
            return m_actions.size();
        }

        bool bindToKey(const InputCode code, const ActionId action) {
            ASSERT_ACTION_INIT();
            return m_keyMap[code].bindAction(action);
        }

        bool bindToMouseButton(const InputCode code, const ActionId action) {
            ASSERT_ACTION_INIT();
            ASSERT_VALID_MOUSE_CODE();
            return m_mouseMap[code - 1].bindAction(action);
        }

        void updateState() {
            // Wow, very readable
            for (auto& action: m_actions) {
                if (action > ActionState::PRESSED) {
                    action = static_cast<ActionState>(static_cast<int>(action) - 2);
                }
            }

            m_mouseVelocity = glm::vec2{};
        }

        [[nodiscard]] bool isActionJustPressed(const ActionId action) const {
            ASSERT_ACTION_INIT();
            return m_actions[action] == ActionState::JUST_PRESSED;
        }

        [[nodiscard]] bool isActionPressed(const ActionId action) const {
            ASSERT_ACTION_INIT();
            return m_actions[action] == ActionState::PRESSED || m_actions[action] == ActionState::JUST_PRESSED;
        }

        [[nodiscard]] bool isActionJustReleased(const ActionId action) const {
            ASSERT_ACTION_INIT();
            return m_actions[action] == ActionState::JUST_RELEASED;
        }

        [[nodiscard]] bool isActionReleased(const ActionId action) const {
            ASSERT_ACTION_INIT();
            return m_actions[action] == ActionState::RELEASED || m_actions[action] == ActionState::JUST_RELEASED;
        }

        static const char* getMouseButtonName(const InputCode button) {
            switch (button) {
                case SDL_BUTTON_LEFT: return "Left";
                case SDL_BUTTON_RIGHT: return "Right";
                case SDL_BUTTON_MIDDLE: return "Middle";
                case SDL_BUTTON_X1: return "X1";
                case SDL_BUTTON_X2: return "X2";
                default: return "Unknown";
            }
        }

        void updateMouseVelocity(const glm::vec2 velocity) {
            m_mouseVelocity = velocity;
        }

        [[nodiscard]] glm::vec2 getMouseVelocity() const {
            return m_mouseVelocity;
        }

    private:
        [[nodiscard]] bool isActionInitialized(const ActionId action) const {
            return action < m_actions.size();
        }

        std::vector<ActionState> m_actions{1};
        std::vector<std::string> m_actionNames{1};
        std::array<Binding, std::numeric_limits<InputCode>::max()> m_keyMap{};
        std::array<Binding, SDL_BUTTON_X2> m_mouseMap{};
        glm::vec2 m_mouseVelocity{};
    };
}
