#include "StateManager.h"

StateManager::StateManager() {
	// TODO: clean up event subscriptions
	EventDispatcher::getInstance().subscribe(EventType::ReplaceCurrentStateEvent, [&](std::shared_ptr<Event> event) {
		auto state_replacement_event = std::dynamic_pointer_cast<ReplaceCurrentStateEvent>(event);
		auto& new_state = state_replacement_event->new_state;
		this->replaceCurrentState(std::move(new_state));
		});

	EventDispatcher::getInstance().subscribe(EventType::PushNewStateEvent, [&](std::shared_ptr<Event> event) {
		auto state_push_event = std::dynamic_pointer_cast<PushNewStateEvent>(event);
		auto& new_state = state_push_event->new_state;
		this->pushNewState(std::move(new_state));
		});

	EventDispatcher::getInstance().subscribe(EventType::PopCurrentStateEvent, [&](std::shared_ptr<Event> event) {
		this->popCurrentState();
		});
}

void StateManager::pushNewState(std::unique_ptr<State> new_state) {
	if (!new_state) {
		LOG_WARNING("StateManager::pushNewState() was called with no new_state");
		return;
	}

	std::string state_name = new_state->getName();
	states.push(std::move(new_state));
	LOG_INFO("State (", state_name, ") was pushed");
}

void StateManager::popCurrentState() {
	if (states.empty()) {
		LOG_WARNING("StateManager::popCurrentState() was called on an empty state stack");
		return;
	}

	std::string state_name = this->getCurrentState()->getName();
	states.pop();
	LOG_INFO("State (", state_name, ") was popped");
}

void StateManager::replaceCurrentState(std::unique_ptr<State> new_state) {
	if (!new_state) {
		LOG_WARNING("StateManager::replaceCurrentState() was called with no new_state");
		return;
	}

	popCurrentState();
	pushNewState(std::move(new_state));
}

State* StateManager::getCurrentState() {
	if (states.empty()) {
		LOG_WARNING("StateManager::getCurrentState() was called on an empty state stack");
		return nullptr;
	}

	return states.top().get();
}