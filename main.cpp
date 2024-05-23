#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#define WIDTH 800
#define HEIGHT 600

#define sandscale 2  // How many pixels per grain of sand
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

void setPixels(sf::Uint8 *pixels, const std::vector<std::vector<float>> &arr, const std::vector<std::vector<float>> &newArr) {
    for (int i = 0; i < ARRHEIGHT; ++i) {
        for (int j = 0; j < ARRWIDTH; ++j) {
            int index = 4 * (i + j * ARRHEIGHT);
            sf::Color color = (arr[i][j] != 0) ? HSBtoRGB(arr[i][j]) : sf::Color::Black;
            pixels[index] = color.r;
            pixels[index + 1] = color.g;
            pixels[index + 2] = color.b;
            pixels[index + 3] = 255;
        }
    }
}
void mapPixels(sf::Uint8 *pixels, sf::Uint8 *newPixels) {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            int index = 4 * (i + j * HEIGHT);
            int arrIndex = 4 * (i / sandscale + (j / sandscale) * ARRHEIGHT);
            newPixels[index] = pixels[arrIndex];
            newPixels[index + 1] = pixels[arrIndex + 1];
            newPixels[index + 2] = pixels[arrIndex + 2];
            newPixels[index + 3] = pixels[arrIndex + 3];
        }
    }
}
void Automata(std::vector<std::vector<float>> &arr, std::vector<std::vector<float>> &newArr) {
    for (int i = 0; i < ARRHEIGHT; i++)
        for (int j = 0; j < ARRWIDTH; j++)
            newArr[i][j] = 0;
    for (int i = 0; i < ARRHEIGHT; i++) {
        for (int j = 0; j < ARRWIDTH; j++) {
            float state = arr[i][j];
            if (state > 0) {
                float below = (i + 1 < ARRHEIGHT) ? arr[i + 1][j] : -1;
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
    arr.swap(newArr);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sand!");
    window.setFramerateLimit(FPS);
    srand(time(NULL));
    sf::Texture texture;
    texture.create(HEIGHT, WIDTH);
    sf::Sprite sprite;
    sprite.setRotation(90);
    sprite.setScale(1, -1);
    sf::Uint8 *pixels = new sf::Uint8[ARRWIDTH * ARRHEIGHT * 4];
    sf::Uint8 *newPixels = new sf::Uint8[WIDTH * HEIGHT * 4];
    std::vector<std::vector<float>> arr(ARRHEIGHT, std::vector<float>(ARRWIDTH, 0));
    std::vector<std::vector<float>> newArr(ARRHEIGHT, std::vector<float>(ARRWIDTH, 0));
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
            int extent = 10;
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
        setPixels(pixels, arr, newArr);
        mapPixels(pixels, newPixels);
        texture.update(newPixels);
        sprite.setTexture(texture);
        window.draw(sprite);
        window.display();
    }
    delete[] pixels;
    delete[] newPixels;
    return 0;
}