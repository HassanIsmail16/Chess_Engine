#pragma once
#include <memory>
#include "../states/State.h"
#include "../states/MainMenuState.h"
#include "../states/GamePlayState.h"

enum class EventType {
	GameStartEvent,
	PushNewStateEvent,
	PopCurrentStateEvent,
	ReplaceCurrentStateEvent,
	MoveEvent,
	UndoMoveEvent,
	RedoMoveEvent,
	JumpToMoveEvent,
	PromotionEvent
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

class MoveEvent : public Event {
public:
	MoveEvent(const Move& move) : Event(EventType::MoveEvent), move(move) {}
	Move getMove() const { return move; }

private:
	Move move;
};

class UndoMoveEvent : public Event {
public:
	UndoMoveEvent() : Event(EventType::UndoMoveEvent) {}
};

class RedoMoveEvent : public Event {
public:
	RedoMoveEvent(): Event(EventType::RedoMoveEvent) {}
};

class JumpToMoveEvent : public Event {
public:
	JumpToMoveEvent(int index, std::string hash) : Event(EventType::JumpToMoveEvent), index(index), hash(hash) {}
	int getIndex() const { return index; }
	std::string getHash() const { return hash; }
private:
	int index;
	std::string hash;
};

class PromotionEvent : public Event {
public:
	PromotionEvent(const Move& move, const PieceType& promotion_type = PieceType::Pawn) : Event(EventType::PromotionEvent), move(move), promotion_type(promotion_type) {}
	Move getMove() { return move; }
	PieceType getPromotionType() { return promotion_type; }
	bool isResolved() { return promotion_type != PieceType::Pawn; }

private:
	Move move;
	PieceType promotion_type;
};