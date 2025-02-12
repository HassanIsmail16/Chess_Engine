#include "GamePlayState.h"
#include "../events/EventDispatcher.h"
#include "StateForward.h"

GamePlayState::GamePlayState() {
	board = std::make_unique<Board>();
	history = std::make_unique<MoveHistory>();

	// TODO: clean up event subscriptions
	EventDispatcher::getInstance().subscribe(EventType::MoveEvent, [&](std::shared_ptr<Event> event) {
		auto move_event = std::dynamic_pointer_cast<MoveEvent>(event);
		auto move = move_event->getMove();
		board->makeMove(move);
		history->recordMove(move, board->computeHash());
		});
}

void GamePlayState::run(const float& dt, sf::RenderWindow& window, const InputManager& input_manager) {
	update(dt);
	handleInput(input_manager);
	render(window);
}

void GamePlayState::exit() {
	return;
}

std::string GamePlayState::getName() const {
	return "GamePlayState";
}

void GamePlayState::update(const float& dt) {
	Move* last_move = (history->isEmpty() ? nullptr : new Move(history->getLastMove()));
	board->update(dt, last_move);
	return;
}

void GamePlayState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::Green);
	board->render(window);
	window.display();
}

void GamePlayState::handleInput(const InputManager& input_manager) {
	// testing state transitions
	if (input_manager.isKeyJustPressed(sf::Keyboard::Key::Enter)) {
		EventDispatcher::getInstance().pushEvent(
			std::make_shared<ReplaceCurrentStateEvent>(
				std::make_unique<MainMenuState>()
			)
		);
	}

	if (input_manager.isMouseButtonJustPressed(sf::Mouse::Button::Left)) {
		auto mouse_position = input_manager.getMousePosition();

		if (board->getGeometry().isInsideBoardTiles(mouse_position)) {
			Position selected_position = board->getGeometry().getTileBoardPosition(input_manager.getMousePosition());

			auto selected_piece = board->getSelectedPiece();

			if (selected_piece && board->isValidMove(selected_position)) {
				if (board->isCastlingMove(selected_position)) {
					selected_position.type = (selected_position.col == 6 ? PositionType::KingSideCastle : PositionType::QueenSideCastle);
				}

				Move move(selected_piece->getPosition(), selected_position, board->getPieceAt(selected_position).get());
				
				EventDispatcher::getInstance().pushEvent(
					std::make_shared<MoveEvent>(move)
				);
			}

			board->selectPiece(selected_position);
		}
	}
}
