#include <SFML/Graphics.hpp>
#include <iostream>
#include "AssetManager.h"
#include "InputManager.h"
#include "EventDispatcher.h"

int main() {
    // Updated RenderWindow constructor
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML Window");
    window.setFramerateLimit(5); // set frame rate to 5 fps for debugging purposes

    InputManager input;
    EventDispatcher dispatcher;
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

        // ------- Testing InputManager
        if (input.isKeyJustPressed(sf::Keyboard::Key::A)) {
            std::cout << "A key was just pressed!" << std::endl;
        }

        if (input.isKeyHeld(sf::Keyboard::Key::A)) {
            std::cout << "A key is held down!" << std::endl;
        }

        if (input.wasKeyReleased(sf::Keyboard::Key::A)) {
            std::cout << "A was just released" << std::endl;
        }

        if (input.isMouseButtonJustPressed(sf::Mouse::Button::Left)) {
            std::cout << "Left was just pressed!" << std::endl;
        }

        if (input.isMouseButtonHeld(sf::Mouse::Button::Left)) {
            std::cout << "Left is held down!" << std::endl;
        }

        if (input.wasMouseButtonReleased(sf::Mouse::Button::Left)) {
            std::cout << "Left was just released" << std::endl;
        }

        if (input.hasMouseMoved()) {
            std::cout << "mouse moved" << std::endl;
        }
        
        auto mouse_position = input.getMousePosition();
        std::cout << "Mouse position: (x) " << mouse_position.x << " (y) " << mouse_position.y << std::endl;
        // ------- End of Testing InputManager

        input.endFrame();
        window.clear();
        window.draw(test); // Part of testing the AssetManager
        window.display();
    }

    return 0;
}
