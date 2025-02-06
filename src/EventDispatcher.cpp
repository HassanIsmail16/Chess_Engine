#include "EventDispatcher.h"

void EventDispatcher::subscribe(EventType type, EventHandler handler) {
	handlers[type].push_back(handler);
}

void EventDispatcher::processEvents() {
	while (!event_queue.empty()) {
		auto event = event_queue.front(); 
		event_queue.pop();

		EventType type = event->getType(); 
		if (handlers.find(type) != handlers.end()) { 
			for (auto& handler : handlers[type]) {
				if (event->isHandled()) {
					break;
				} // exit if event is already handled

				handler(event);
			}
		} 
	}
}

void EventDispatcher::pushEvent(std::shared_ptr<Event> event) {
	event_queue.push(event);
}
