#include "GamePlayState.h"
#include "../events/EventDispatcher.h"
#include "StateForward.h"
#include "../log/Logger.hpp"

GamePlayState::GamePlayState() {
	//std::string hash = "r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/R1P1K3/q5b1";
	//board = std::make_unique<Board>(hash);
	board = std::make_unique<Board>();
	history = std::make_unique<MoveHistory>();
	status_manager = std::make_unique<GameStatusManager>();
	promotion_panel = nullptr;

	// TODO: clean up event subscriptions
	EventDispatcher::getInstance().subscribe(EventType::MoveEvent, [&](std::shared_ptr<Event> event) {
		auto move_event = std::dynamic_pointer_cast<MoveEvent>(event);
		auto move = move_event->getMove();
		board->makeMove(move);
		history->recordMove(move, board->computeHash(history->getTotalMoves()));
		status_manager->endTurn(*board);
		});

	EventDispatcher::getInstance().subscribe(EventType::PromotionEvent, [&](std::shared_ptr<Event> event) {
		auto promotion_event = std::dynamic_pointer_cast<PromotionEvent>(event);
		if (promotion_event->isResolved()) {
			board->promotePawn(promotion_event->getMove().to, promotion_event->getPromotionType());
			promotion_panel.release();
		} else {
			promotion_panel = std::make_unique<PromotionPanel>(promotion_event->getMove());
		}
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
	if (promotion_panel) {
		promotion_panel->update(dt);
		return;
	}

	if (board->isLastMovePromotion()) {
		EventDispatcher::getInstance().pushEvent(
			std::make_shared<PromotionEvent>(history->getLastMove())
		);
	}

	board->update(dt);
	return;
}

void GamePlayState::render(sf::RenderWindow& window) {
	window.clear(sf::Color::Green);


	board->render(window);
	history->render(window);

	if (promotion_panel) {
		promotion_panel->render(window);
	}

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

	if (promotion_panel) {
		promotion_panel->handleInput(input_manager);
		return;
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

				Piece* captured_piece = nullptr;
				Move move;

				if (board->isEnPassantMove(selected_position)) {
					selected_position.type = PositionType::EnPassant;
					int capture_direction = (selected_piece->getColor() == ChessColor::White ? -1 : 1);
					Position capture_position = {selected_position.row + capture_direction, selected_position.col};
					move = { selected_piece->getPosition(), selected_position, board->getPieceAt(capture_position).release() };
					LOG_WARNING("En passant done!");
				}
				else {
					move = { selected_piece->getPosition(), selected_position, board->getPieceAt(selected_position).get() };
				}
				
				
				EventDispatcher::getInstance().pushEvent(
					std::make_shared<MoveEvent>(move)
				);
			}
			
			board->selectPiece(selected_position, status_manager->getCurrentTurn());
		}
	}
}
