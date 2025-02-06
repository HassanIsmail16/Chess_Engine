#include <SFML/Graphics.hpp>
#include "AssetManager.h"

int main() {
    // Updated RenderWindow constructor
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML Window");

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
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear, draw, and display the window (no changes)
        window.clear();
        window.draw(test); // Part of testing the AssetManager
        window.display();
    }

    return 0;
}
