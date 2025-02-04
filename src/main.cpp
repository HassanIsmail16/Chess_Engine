#include <SFML/Graphics.hpp>

int main() {
    // Updated RenderWindow constructor
    sf::RenderWindow window(sf::VideoMode(800, 600, 32), "SFML Window");

    // Create a shape (no change here)
    sf::CircleShape shape(100.0f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Updated event handling
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear, draw, and display the window (no changes)
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
