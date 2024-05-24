#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
sf::Color HSBtoRGB(float hue);
float PingPong(float t, float min, float max);
enum BlockType {
    Invalid,
    Empty,
    Sand,
    Water,
    Stone
};
struct Block {
    float color;
    BlockType type;

    Block() : color(0), type(Empty){};
    Block(float color, BlockType type) : color(color), type(type){};
};
class Canvas {
private:
    int width, height, sandscale;
    int arrheight, arrwidth;
    std::vector<std::vector<Block>> arr;
    std::vector<std::vector<Block>> newArr;
    sf::Uint8* pixels;
    sf::Uint8* newPixels;
    sf::Texture texture;
    sf::Sprite sprite;
    int withinCols(int i) {
        return i >= 0 && i < arrwidth;
    }

    int withinRows(int i) {
        return i >= 0 && i < arrheight;
    }

public:
    Canvas(int width, int height, int sandscale) : width(width), height(height), sandscale(sandscale) {
        arrheight = height / sandscale;
        arrwidth = width / sandscale;
        texture.create(height, width);
        sprite.setRotation(90);
        sprite.setScale(1, -1);
        arr = std::vector<std::vector<Block>>(arrheight, std::vector<Block>(arrwidth));
        newArr = std::vector<std::vector<Block>>(arrheight, std::vector<Block>(arrwidth));
        pixels = new sf::Uint8[width * height * 4];
        newPixels = new sf::Uint8[width * height * 4];
    }
    ~Canvas() {
        delete[] pixels;
        delete[] newPixels;
    }
    void AddParticles(sf::Vector2i& mousepos, int extent, BlockType type, float color) {
        int row = mousepos.y / sandscale;
        int col = mousepos.x / sandscale;
        for (int i = extent * -1; i < extent; i++) {
            for (int j = extent * -1; j < extent; j++) {
                if (row + i >= 0 && row + i < arrheight && col + j >= 0 && col + j < arrwidth) {
                    if (arr[row + i][col + j].type != Empty || (rand() % 4 == 0))
                        continue;
                    else {
                        arr[row + i][col + j].color = PingPong(color, 10, 65);
                        arr[row + i][col + j].type = type;
                    }
                }
            }
        }
    }
    void setPixels() {
        for (int i = 0; i < arrheight; i++) {
            for (int j = 0; j < arrwidth; j++) {
                int index = 4 * (i + j * arrheight);
                sf::Color color = (arr[i][j].type != Empty) ? HSBtoRGB(arr[i][j].color) : sf::Color::Black;
                pixels[index] = color.r;
                pixels[index + 1] = color.g;
                pixels[index + 2] = color.b;
                pixels[index + 3] = 255;
            }
        }
    }
    void mapPixels() {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int index = 4 * (i + j * height);
                int arrIndex = 4 * (i / sandscale + (j / sandscale) * arrheight);
                newPixels[index] = pixels[arrIndex];
                newPixels[index + 1] = pixels[arrIndex + 1];
                newPixels[index + 2] = pixels[arrIndex + 2];
                newPixels[index + 3] = pixels[arrIndex + 3];
            }
        }
    }
    void draw(sf::RenderWindow& window) {
        texture.update(newPixels);
        sprite.setTexture(texture);
        window.draw(sprite);
    }

    void runAutomata() {
        for (int i = 0; i < arrheight; i++)
            for (int j = 0; j < arrwidth; j++) {
                newArr[i][j].color = 0;
                newArr[i][j].type = Empty;
            }
        for (int i = 0; i < arrheight; i++) {
            for (int j = 0; j < arrwidth; j++) {
                Block state = arr[i][j];
                if (state.type != Empty) {
                    BlockType below = (i + 1 < arrheight) ? arr[i + 1][j].type : Invalid;
                    int dir = (rand() % 2) - 1;
                    if (dir == 0)
                        dir = 1;

                    BlockType belowA = Invalid;
                    BlockType belowB = Invalid;
                    if (withinCols(j + dir) && withinRows(i + 1)) {
                        belowA = arr[i + 1][j + dir].type;
                    }
                    if (withinCols(j - dir) && withinRows(i + 1)) {
                        belowB = arr[i + 1][j - dir].type;
                    }
                    if (below == Empty) {
                        newArr[i + 1][j] = state;

                    } else if (belowA == Empty) {
                        newArr[i + 1][j + dir] = state;

                    } else if (belowB == Empty) {
                        newArr[i + 1][j - dir] = state;

                    } else {
                        newArr[i][j] = state;
                    }
                }
            }
        }
        arr.swap(newArr);
        setPixels();
        mapPixels();
    }
};