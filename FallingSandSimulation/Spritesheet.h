#pragma once

#include <SFML/Graphics.hpp>

class Spritesheet
{
public:
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::RectangleShape> grid;
    int gridCols;
    int gridRows;

    Spritesheet(const std::string& filepath, int cols, int rows)
        : gridCols(cols), gridRows(rows)
    {
        if (!texture.loadFromFile(filepath))
        {
            throw std::runtime_error("Failed to load spritesheet.");
        }
        sprite.setTexture(texture);

        const int rectWidth = texture.getSize().x / gridCols * 2;
        const int rectHeight = texture.getSize().y / gridRows * 2;

        for (int y = 0; y < gridRows; ++y)
        {
            for (int x = 0; x < gridCols; ++x)
            {
                sf::RectangleShape rect(sf::Vector2f(rectWidth, rectHeight));
                rect.setOutlineThickness(1);
                rect.setOutlineColor(sf::Color::Red);
                rect.setFillColor(sf::Color::Transparent);
                rect.setPosition(x * rectWidth + 1, y * rectHeight + 1);
                grid.push_back(rect);
            }
        }
    }
};