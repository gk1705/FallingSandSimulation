#pragma once
#include <SFML/Graphics.hpp>

#include "helpers.h"
#include "IInputTool.h"
#include "ParticleGrid.h"
#include "ParticleTypeIterator.h"

class RectangleTool : public IInputTool
{
public:
	RectangleTool(sf::RenderWindow& window, ParticleGrid& particleGrid, ParticleTypeIterator& particleTypeIterator);

	void handleInput(const sf::Event& event) override;
	void render() override;

private:
	sf::RenderWindow& window;
	ParticleGrid& particleGrid;
	ParticleTypeIterator& particleTypeIterator;

	sf::RectangleShape rectangle;
	sf::Vector2i start;
	sf::Vector2i end;

	bool isDragging = false;
	sf::Vector2i previousMousePosition;

	std::vector<Particle> copiedParticles;
	std::vector<sf::Vector2i> initialPoints;

	void startDrawing();
	void continueDrawing();
	void fillRectangle() const;
	void clearRectangle() const;
	void startDraggingRectangle();
	void stopDraggingRectangle();
	void dragRectangle();
	void clipRectangleToBorders();
	void moveRectangle();
	bool isRectangleTooSmall() const;
	bool IsRectangleOutsideOfWindow() const;
	bool checkIfMouseIsInsideRectangle() const;
};
