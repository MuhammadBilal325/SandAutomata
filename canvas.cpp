#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
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