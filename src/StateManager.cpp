#include "StateManager.h"

void StateManager::pushNewState(std::unique_ptr<State> new_state) {
	states.push(std::move(new_state));
}

void StateManager::removeCurrentState() {
	if (!states.empty()) {
		states.pop();
	}
}

void StateManager::transitionTo(std::unique_ptr<State> new_state) {
	if (!new_state) {
		return;
	}

	removeCurrentState();
	pushNewState(std::move(new_state));
	State* current_state = getCurrentState();

	if (current_state != nullptr) {
		current_state->run();
	}
}

State* StateManager::getCurrentState() {
	if (states.empty()) {
		return nullptr;
	}

	return states.top().get();
}