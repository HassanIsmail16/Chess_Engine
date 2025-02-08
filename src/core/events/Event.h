#pragma once
#include <memory>
#include "../states/State.h"
#include "../states/MainMenuState.h"
#include "../states/GamePlayState.h"

enum class EventType {
	GameStartEvent,
	PushNewStateEvent,
	PopCurrentStateEvent,
	ReplaceCurrentStateEvent
};

class Event {
public:
	Event(EventType type) : type(type) {}
	virtual ~Event() = default;

	EventType getType() { return type; }

	bool isHandled() { return handled; }
	void setHandled() { handled = true; }

private:
	EventType type;
	bool handled = false;
};

class GameStartEvent : public Event {
public:
	GameStartEvent() : Event(EventType::GameStartEvent) {}
};

class PushNewStateEvent : public Event {
public:
	PushNewStateEvent() : Event(EventType::PushNewStateEvent), new_state(std::move(new_state)) {}
	std::unique_ptr<State> new_state;
};

class PopCurrentStateEvent : public Event {
public:
	PopCurrentStateEvent() : Event(EventType::PopCurrentStateEvent) {}
};

class ReplaceCurrentStateEvent : public Event {
public:
	ReplaceCurrentStateEvent(std::unique_ptr<State> new_state) : Event(EventType::ReplaceCurrentStateEvent), new_state(std::move(new_state)) {}
	std::unique_ptr<State> new_state;
};