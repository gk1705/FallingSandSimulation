#pragma once
#include <SFML/Graphics.hpp>

#include "IInputTool.h"
#include "ParticleGrid.h"
#include "ParticleTypeIterator.h"

class StampTool : public IInputTool
{
public:
    StampTool(sf::RenderWindow& window, ParticleGrid& particleGrid, ParticleTypeIterator& particleTypeIterator, int maxStampSize)
		: window(window), particleGrid(particleGrid), particleTypeIterator(particleTypeIterator), maxStampSize(maxStampSize)
    {

	}

    void handleInput(const sf::Event& event) override;
    void render() override;

private:
    sf::RenderWindow& window;
    ParticleGrid& particleGrid;
    ParticleTypeIterator& particleTypeIterator;
    int stampSize = 1;
    int maxStampSize;

    void shrinkStamp();
    void enlargeStamp();
    void fill() const;
    void erase() const;
    bool isMouseInsideGrid() const;
};
