#pragma once
#include <array>
#include <limits>
#include <vector>
#include <SDL3/SDL_keyboard.h>

#include "Assert.h"

namespace Engine {
    class InputMap {
    public:
        using KeyCode = uint8_t;
        using ActionId = uint16_t;

        enum class ActionState : uint8_t {
            JUST_PRESSED,
            JUST_RELEASED,
            PRESSED,
            RELEASED
        };

        static constexpr ActionId s_noAction{0};
        static constexpr const char* s_noActionName = "No Action";

        class KeyBind {
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

        KeyBind& getKeyBind(const KeyCode code) {
            LOG("ASCII Key: " << SDL_GetKeyName(code) << '\n');
            return m_keyMap[code]; // Check bounds at call site
        }

        ActionId createAction(std::string name) {
            m_actions.emplace_back();
            m_actionNames.emplace_back(std::move(name));
            return m_actions.size() - 1;
        }

        [[nodiscard]] ActionState getActionState(const ActionId action) const {
            ASSERT_MSG(isActionInitialized(action), "Action of id: " << action << " is not initialized!\n");

            return m_actions[action];
        }

        [[nodiscard]] size_t getActionCount() const {
            return m_actions.size();
        }

        bool bind(const KeyCode code, const ActionId action) {
            ASSERT_MSG(isActionInitialized(action), "Action of id: " << action << " is not initialized!\n");
            return m_keyMap[code].bindAction(action);
        }

        void updateActionState() {
            // Wow, very readable
            for (auto& action: m_actions) {
                if (action < ActionState::PRESSED) {
                    action = static_cast<ActionState>(static_cast<int>(action) + 2);
                }
            }
        }

    private:
        [[nodiscard]] bool isActionInitialized(const ActionId action) const {
            return action < m_actions.size();
        }

        std::vector<ActionState> m_actions{1};
        std::vector<std::string> m_actionNames{1};
        std::array<KeyBind, std::numeric_limits<KeyCode>::max()> m_keyMap{};
    };
}
