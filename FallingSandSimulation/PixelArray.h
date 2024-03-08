#pragma once

#include <SFML/Graphics.hpp>
#include "ParticleGrid.h"

class PixelArray
{
public:
    sf::Uint8* pixels;

    PixelArray(ParticleGrid& particleGrid, WindowSettings windowSettings);
    ~PixelArray();

    void updatePixelArray() const;

private:
    ParticleGrid& particleGrid;
    int screenWidth;
    int screenHeight;
    int particleSize;
    int rowCount;
    int columnCount;
};