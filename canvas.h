#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
sf::Color HSBtoRGB(float hue);
float PingPong(float t, float min, float max);
enum BlockType : unsigned short {
    Invalid,
    Empty,
    Sand,
    Water,
    Stone
};

struct Block {
    float color;
    BlockType type;
    sf::Vector2f velocity;

    Block() : color(0), type(Empty){};
    Block(float color, BlockType type) : color(color), type(type){};
};
class Canvas {
private:
    int width, height, sandscale;
    int arrheight, arrwidth;
    int amount = 0;
    std::vector<std::vector<Block>> arr;
    std::vector<std::vector<Block>> newArr;
    sf::Uint8* pixels;
    sf::Uint8* newPixels;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Font font;
    sf::Text text;
    const float gravity = 9.8;
    const int blockColors[4][2] = {
        {0, 0},
        {10, 65},
        {170, 240},
        {304, 306},
    };
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
        font.loadFromFile("arial.ttf");
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(10, 10);
    }
    ~Canvas() {
        delete[] pixels;
        delete[] newPixels;
    }
    void AddParticles(sf::Vector2i& mousepos, int extent, BlockType type, float color) {
        int row = mousepos.y / sandscale;
        int col = mousepos.x / sandscale;
        int colmin = 0, colmax = 0;
        if (type - 1 > 0) {
            colmin = blockColors[type - 1][0];
            colmax = blockColors[type - 1][1];
        }
        for (int i = extent * -1; i < extent; i++) {
            for (int j = extent * -1; j < extent; j++) {
                if (row + i >= 0 && row + i < arrheight && col + j >= 0 && col + j < arrwidth) {
                    if (arr[row + i][col + j].type != Empty || (rand() % 4 == 0))
                        continue;
                    else {
                        arr[row + i][col + j].color = PingPong(color, colmin, colmax);
                        arr[row + i][col + j].type = type;
                        arr[row + i][col + j].velocity = sf::Vector2f(1, 1);
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
    void drawAmount(sf::RenderWindow& window) {
        text.setString("Amount: " + std::to_string(amount));
        window.draw(text);
    }
    void runAutomata() {
        for (int i = 0; i < arrheight; i++)
            for (int j = 0; j < arrwidth; j++)
                newArr[i][j].type = Empty;
        for (int i = arrheight - 1; i >= 0; i--) {
            for (int j = 0; j < arrwidth; j++) {
                if (arr[i][j].type != Empty) {
                    Block state = arr[i][j];
                    arr[i][j].type = Empty;
                    if (state.type == Sand) {
                        // For sand we first go downwards and check the farthest we can get
                        // within the bounds of the vertical velocity. If we find
                        // a spot within that column we push our sand to it
                        int y = 0;
                        BlockType below = Invalid;
                        bool brokefall = 0;  // if our fall was broken reset velocity
                        for (int k = 1; k <= state.velocity.y && k + i < arrheight; k++) {
                            if (newArr[i + k][j].type == Empty) {
                                below = Empty;
                                y = i + k;
                            } else {
                                // This will only run if something got in our way while we were falling
                                brokefall = 1;
                                break;
                            }
                        }
                        int dir = (rand() % 2) - 1;
                        // if (dir == 0)
                        dir = 1;
                        BlockType belowA = Invalid;
                        BlockType belowB = Invalid;
                        if (withinCols(j + dir) && withinRows(i + 1)) {
                            if (arr[i][j + dir].type == Empty)
                                belowA = newArr[i + 1][j + dir].type;
                        }
                        if (withinCols(j - dir) && withinRows(i + 1)) {
                            if (arr[i][j - dir].type == Empty)
                                belowB = newArr[i + 1][j - dir].type;
                        }

                        if (below == Empty) {
                            // If we were in freefall add more velocity
                            if (!brokefall)
                                state.velocity.y += 0.098;
                            newArr[y][j] = state;
                        } else if (belowA == Empty)
                            newArr[i + 1][j + dir] = state;
                        else if (belowB == Empty)
                            newArr[i + 1][j - dir] = state;
                        else
                            newArr[i][j] = state;

                    } else if (state.type == Water) {
                        // BlockType below, belowA, belowB, belowC, belowD;  // Below, Below left, Below Right, Left, Right
                        // below = belowA = belowB = belowC = belowD = Invalid;
                        // int dir = (rand() % 2) - 1;
                        // if (dir == 0)
                        //     dir = 1;
                        // if (withinRows(i + 1)) {
                        //     below = arr[i + 1][j].type;
                        //     if (below == Empty)
                        //         below = newArr[i + 1][j].type;

                        //     if (withinCols(j + dir)) {
                        //         belowA = arr[i + 1][j + dir].type;
                        //         if (belowA == Empty)
                        //             belowA = newArr[i + 1][j + dir].type;
                        //     }
                        //     if (withinCols(j - dir)) {
                        //         belowB = arr[i + 1][j - dir].type;
                        //         if (belowB == Empty)
                        //             belowB = newArr[i + 1][j - dir].type;
                        //     }
                        // }
                        // if (withinCols(j - 1)) {
                        //     belowC = arr[i][j - 1].type;
                        //     if (belowC == Empty)
                        //         belowC = newArr[i][j - 1].type;
                        // }
                        // if (withinCols(j + 1)) {
                        //     belowD = arr[i][j + 1].type;
                        //     if (belowD == Empty)
                        //         belowD = newArr[i][j + 1].type;
                        // }

                        // if (below == Empty)
                        //     newArr[i + 1][j] = state;
                        // else if (belowA == Empty)
                        //     newArr[i + 1][j + dir] = state;
                        // else if (belowB == Empty)
                        //     newArr[i + 1][j - dir] = state;
                        // else if (belowC == Empty)
                        //     newArr[i][j - 1] = state;
                        // else if (belowD == Empty)
                        //     newArr[i][j + 1] = state;
                        // else
                        //     newArr[i][j] = state;
                    }
                }
            }
        }
        amount = 0;
        for (int i = 0; i < arrheight; i++)
            for (int j = 0; j < arrwidth; j++)
                if (newArr[i][j].type != Empty)
                    amount++;
        arr.swap(newArr);
        setPixels();
        mapPixels();
    }
    void clear() {
        for (int i = 0; i < arrheight; i++)
            for (int j = 0; j < arrwidth; j++)
                arr[i][j].type = Empty;
        setPixels();
        mapPixels();
    }
};