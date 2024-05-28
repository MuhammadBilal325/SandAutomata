#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>

#include "canvas.h"
#define WIDTH 800
#define HEIGHT 600
#define FPS 60

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sand!");
    Canvas canvas(WIDTH, HEIGHT, 4);
    window.setFramerateLimit(FPS);
    srand(time(NULL));
    float hue = 0.f;
    bool buttonPressed = false;
    bool isPaused = false;
    BlockType type = Sand;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    buttonPressed = true;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    buttonPressed = false;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S)
                    type = Sand;
                else if (event.key.code == sf::Keyboard::W)
                    type = Water;
                else if (event.key.code == sf::Keyboard::C)
                    canvas.clear();
            }
        }
        hue += 0.5;
        if (hue > 360.f)
            hue = 0.f;
        if (buttonPressed) {
            sf::Vector2i mousepos = sf::Mouse::getPosition(window);
            canvas.AddParticles(mousepos, 2, type, hue);
        }
        window.clear();
        canvas.runAutomata();
        canvas.draw(window);
        canvas.drawAmount(window);
        window.display();
    }
    return 0;
}