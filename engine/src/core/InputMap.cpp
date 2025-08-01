#include "InputMap.h"

#include <algorithm>

auto Engine::InputMap::Binding::findAction(const ActionId action) {
    return std::ranges::find(m_boundActions, action);
}

bool Engine::InputMap::Binding::isActionBound(const ActionId action) {
    return findAction(action) != m_boundActions.end();
}

bool Engine::InputMap::Binding::bindAction(const ActionId action) {
    if (isActionBound(action)) {
        return false;
    }

    m_boundActions.emplace_back(action);
    return true;
}

void Engine::InputMap::Binding::unbindAction(const ActionId action) {
    m_boundActions.erase(findAction(action));
}

Engine::InputMap::ActionState Engine::InputMap::Binding::getActionState(const InputMap& inputMap) const {
    if (m_boundActions.front() == s_noAction) {
        return ActionState::RELEASED;
    }

    return inputMap.m_actions[m_boundActions.front()];
}

void Engine::InputMap::Binding::setActionState(InputMap& inputMap, const ActionState state) const {
    for (const ActionId id: m_boundActions) {
        inputMap.m_actions[id] = state;
    }
}
