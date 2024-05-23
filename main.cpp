#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#define WIDTH 800
#define HEIGHT 600

#define sandscale 1  // How many pixels per grain of sand
#define ARRHEIGHT HEIGHT / sandscale
#define ARRWIDTH WIDTH / sandscale
#define FPS 60

float PingPong(float t, float min, float max) {
    float range = max - min;
    if (range == 0) {
        return min;
    }
    t = fmod(t - min, 2 * range);
    if (t < 0) {
        t += 2 * range;
    }
    return min + range - fabs(range - t);
}
int withinCols(int i) {
    return i >= 0 && i < ARRWIDTH;
}
int withinRows(int i) {
    return i >= 0 && i < ARRHEIGHT;
}

sf::Color HSBtoRGB(float hue) {
    float saturation = 1.f;
    float brightness = 1.f;
    hue = fmod(hue, 360);  // Cycle hue back to 0-359 if it's over 360
    hue /= 60;             // Convert hue to range 0-6

    int i = floor(hue);
    float f = hue - i;

    // Calculate intermediate values
    float p = brightness * (1 - saturation);
    float q = brightness * (1 - saturation * f);
    float t = brightness * (1 - saturation * (1 - f));

    switch (i) {
    case 0:
        return sf::Color(brightness * 255, t * 255, p * 255);
    case 1:
        return sf::Color(q * 255, brightness * 255, p * 255);
    case 2:
        return sf::Color(p * 255, brightness * 255, t * 255);
    case 3:
        return sf::Color(p * 255, q * 255, brightness * 255);
    case 4:
        return sf::Color(t * 255, p * 255, brightness * 255);
    default:
        return sf::Color(brightness * 255, p * 255, q * 255);
    }
}

void printArray(sf::VertexArray &vertices, std::vector<std::vector<float>> arr, sf::RenderWindow &window) {
    for (int i = 0; i < ARRHEIGHT; i++) {
        for (int j = 0; j < ARRWIDTH; j++) {
            int index = 4 * (i + j * ARRHEIGHT);
            vertices[index].position = sf::Vector2f(j * sandscale, i * sandscale);
            vertices[index + 1].position = sf::Vector2f((j + 1) * sandscale, i * sandscale);
            vertices[index + 2].position = sf::Vector2f((j + 1) * sandscale, (i + 1) * sandscale);
            vertices[index + 3].position = sf::Vector2f(j * sandscale, (i + 1) * sandscale);
            if (arr[i][j] != 0) {
                sf::Color color = HSBtoRGB(arr[i][j]);
                vertices[index].color = color;
                vertices[index + 1].color = color;
                vertices[index + 2].color = color;
                vertices[index + 3].color = color;
            } else {
                vertices[index].color = sf::Color::Black;
                vertices[index + 1].color = sf::Color::Black;
                vertices[index + 2].color = sf::Color::Black;
                vertices[index + 3].color = sf::Color::Black;
            }
        }
    }
    window.draw(vertices);
}
void Automata(std::vector<std::vector<float>> &arr, std::vector<std::vector<float>> &newArr) {
    for (int i = 0; i < ARRHEIGHT; i++)
        for (int j = 0; j < ARRWIDTH; j++)
            newArr[i][j] = 0;
    for (int i = 0; i < ARRHEIGHT; i++) {
        for (int j = 0; j < ARRWIDTH; j++) {
            float state = arr[i][j];
            if (state > 0) {
                float below = -1;
                if (withinRows(i + 1))
                    below = arr[i + 1][j];
                int dir = (rand() % 2) - 1;
                if (dir == 0)
                    dir = 1;

                float belowA = -1;
                float belowB = -1;
                if (withinCols(j + dir) && withinRows(i + 1)) {
                    belowA = arr[i + 1][j + dir];
                }
                if (withinCols(j - dir) && withinRows(i + 1)) {
                    belowB = arr[i + 1][j - dir];
                }
                if (below == 0)
                    newArr[i + 1][j] = state;
                else if (belowA == 0)
                    newArr[i + 1][j + dir] = state;
                else if (belowB == 0)
                    newArr[i + 1][j - dir] = state;
                else
                    newArr[i][j] = state;
            }
        }
    }
    arr = newArr;
}
int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sand!");

    sf::Color color(255, 255, 255);
    window.setFramerateLimit(FPS);
    srand(time(NULL));
    std::vector<std::vector<float>> arr(ARRHEIGHT, std::vector<float>(ARRWIDTH, 0));
    std::vector<std::vector<float>> newArr(ARRHEIGHT, std::vector<float>(ARRWIDTH, 0));
    sf::VertexArray vertices(sf::Quads, ARRHEIGHT * ARRWIDTH * 4);
    float hue = 0.f;
    bool buttonPressed = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    buttonPressed = true;
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    buttonPressed = false;
                }
            }
        }
        hue += 0.5;
        if (hue > 360.f)
            hue = 0.f;
        if (buttonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int row = mousePos.y / sandscale;
            int col = mousePos.x / sandscale;
            int extent = 5;
            for (int i = extent * -1; i < extent; i++) {
                for (int j = extent * -1; j < extent; j++) {
                    if (row + i >= 0 && row + i < ARRHEIGHT && col + j >= 0 && col + j < ARRWIDTH) {
                        if (arr[row + i][col + j] != 0)
                            continue;
                        else
                            arr[row + i][col + j] = PingPong(hue, 10, 65);
                    }
                }
            }
        }

        Automata(arr, newArr);
        window.clear();

        printArray(vertices, arr, window);
        window.display();
    }

    return 0;
}