#include <SFML/Graphics.hpp>
#include <iostream>
#include "AssetManager.h"
#include "InputManager.h"
#include "EventDispatcher.h"
#include "StateManager.h"
#include "MainMenuState.h"

int main() {
    // Updated RenderWindow constructor
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML Window");
    window.setFramerateLimit(5); // set frame rate to 5 fps for debugging purposes

    InputManager input;

    StateManager manager;
    manager.pushNewState(std::make_unique<MainMenuState>());
    // Create a shape (no change here)
    sf::CircleShape shape(100.0f);
    shape.setFillColor(sf::Color::Green);

    // ------- Testing AssetManager
    // Load Assets
    AssetManager& assets = AssetManager::getInstance();
    assets.loadAssets("assets/images/");
    assets.loadAssets("assets/fonts/");
    assets.loadAssets("assets/sounds/");
    assets.loadAssets("assets/test/");

    // Retrieve Asset
    sf::Sprite test = assets.getSprite("king-w");

    // ------- End of Testing AssetManager

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            // Updated event handling
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            input.update(event, window);
        }
        
        manager.getCurrentState()->run(0, window, input);
        input.endFrame();
    }

    return 0;
}
