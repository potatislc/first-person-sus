#include "InputMap.h"

#include <algorithm>

auto Engine::InputMap::KeyBind::findAction(const ActionId action) {
    return std::ranges::find(m_boundActions, action);
}

bool Engine::InputMap::KeyBind::isActionBound(const ActionId action) {
    return findAction(action) != m_boundActions.end();
}

bool Engine::InputMap::KeyBind::bindAction(const ActionId action) {
    if (isActionBound(action)) {
        return false;
    }

    m_boundActions.emplace_back(action);
    return true;
}

void Engine::InputMap::KeyBind::unbindAction(const ActionId action) {
    m_boundActions.erase(findAction(action));
}

Engine::InputMap::ActionState Engine::InputMap::KeyBind::getActionState(const InputMap& inputMap) const {
    if (m_boundActions.front() == s_noAction) {
        return ActionState::RELEASED;
    }

    return inputMap.m_actions[m_boundActions.front()];
}

void Engine::InputMap::KeyBind::setActionState(InputMap& inputMap, const ActionState state) const {
    for (const ActionId id: m_boundActions) {
        inputMap.m_actions[id] = state;
    }
}
