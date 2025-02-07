#pragma once

enum class EventType {
	GameStartEvent,
	ExitMainMenuEvent
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