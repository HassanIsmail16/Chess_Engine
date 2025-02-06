#pragma once
#include <SFML/Graphics.hpp>
#include <stack>
#include "State.h"

class StateManager {
public:
	void pushNewState(std::unique_ptr<State> new_state);
	void removeCurrentState();
	void transitionTo(std::unique_ptr<State> new_state);
	State* getCurrentState();
private:
	std::stack<std::unique_ptr<State>> states;
};