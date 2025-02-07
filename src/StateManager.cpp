#include "StateManager.h"

StateManager::StateManager() {
	// TODO: clean up event subscriptions
	EventDispatcher::getInstance().subscribe(EventType::ReplaceCurrentStateEvent, [&](std::shared_ptr<Event> event) {
		auto state_replacement_event = std::dynamic_pointer_cast<ReplaceCurrentStateEvent>(event);
		auto& new_state = state_replacement_event->new_state;
		this->transitionTo(std::move(new_state));
		});
}

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
}

State* StateManager::getCurrentState() {
	if (states.empty()) {
		return nullptr;
	}

	return states.top().get();
}