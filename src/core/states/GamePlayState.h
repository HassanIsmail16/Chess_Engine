#pragma once

#include "State.h"
#include "../game/Board.h"
#include "../game/MoveHistory.h"
#include "../game/GameStatusManager.h"
#include "../game/PromotionPanel.h"
#include <memory>

class GamePlayState : public State {
public:
	GamePlayState();
	void run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) override;
	void exit() override;
	std::string getName() const override;
	~GamePlayState() = default;

private:
	void update(const float& dt) override;
	void render(sf::RenderWindow& window) override;
	void handleInput(const InputManager& input_manager) override;
	
	std::unique_ptr<PromotionPanel> promotion_panel;
	std::unique_ptr<Board> board;
	std::unique_ptr<MoveHistory> history;
	std::unique_ptr<GameStatusManager> status_manager;
};