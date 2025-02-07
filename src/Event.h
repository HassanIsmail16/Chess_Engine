#pragma once
#include <memory>
#include "State.h"
#include "MainMenuState.h"
#include "GamePlayState.h"

enum class EventType {
	GameStartEvent,
	EnterMainMenuEvent,
	ExitCurrentStateEvent,
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

class EnterMainMenuEvent : public Event {
public:
	EnterMainMenuEvent() : Event(EventType::EnterMainMenuEvent) {}
};

class ExitCurrentStateEvent : public Event {
public:
	ExitCurrentStateEvent() : Event(EventType::ExitCurrentStateEvent) {}
};

class ReplaceCurrentStateEvent : public Event {
public:
	ReplaceCurrentStateEvent(std::unique_ptr<State> new_state) : Event(EventType::ReplaceCurrentStateEvent), new_state(std::move(new_state)) {}
	std::unique_ptr<State> new_state;
};