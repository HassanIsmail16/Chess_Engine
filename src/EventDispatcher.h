#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <queue>
#include <vector>
#include "Event.h"

class EventDispatcher {
public:
	using EventHandler = std::function<void(std::shared_ptr<Event>)>;

	void subscribe(EventType type, EventHandler handler);
	void processEvents();
	void pushEvent(std::shared_ptr<Event> event);

private:
	std::queue<std::shared_ptr<Event>> event_queue;
	std::unordered_map<EventType, std::vector<EventHandler>> handlers;
};