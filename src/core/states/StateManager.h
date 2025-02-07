#pragma once
#include <SFML/Graphics.hpp>
#include <stack>
#include "State.h"
#include "../events/EventDispatcher.h"
#include "../log/Logger.hpp"

class StateManager {
public:
	StateManager();
	void pushNewState(std::unique_ptr<State> new_state);
	void removeCurrentState();
	void transitionTo(std::unique_ptr<State> new_state);
	State* getCurrentState();

private:
	std::stack<std::unique_ptr<State>> states;
};