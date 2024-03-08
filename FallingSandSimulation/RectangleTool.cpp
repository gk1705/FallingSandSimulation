#include "RectangleTool.h"

RectangleTool::RectangleTool(sf::RenderWindow& window, ParticleGrid& particleGrid,
	ParticleTypeIterator& particleTypeIterator): window(window), particleGrid(particleGrid), particleTypeIterator(particleTypeIterator)
{
	rectangle.setFillColor(sf::Color::Transparent);
	rectangle.setOutlineColor(sf::Color::Red);
	rectangle.setOutlineThickness(1);
	rectangle.setSize(sf::Vector2f(0.f, 0.f));
}

void RectangleTool::handleInput(const sf::Event& event)
{
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left)
	{
		startDrawing();
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		continueDrawing();
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
	{
		clearRectangle();
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
	{
		fillRectangle();
	}
	else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Middle)
	{
		startDraggingRectangle();
	}
	else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle)
	{
		stopDraggingRectangle();
	}
}

void RectangleTool::render()
{
	if (isDragging)
	{
		dragRectangle();
	}

	rectangle.setPosition(start.x, start.y);
	rectangle.setSize(sf::Vector2f(end.x - start.x, end.y - start.y));
	window.draw(rectangle);
}

void RectangleTool::startDrawing()
{
	start = sf::Mouse::getPosition(window);
	end = start;
}

void RectangleTool::continueDrawing()
{
	end = sf::Mouse::getPosition(window);
	clipRectangleToBorders();
}

void RectangleTool::fillRectangle() const
{
	if (isRectangleTooSmall() || IsRectangleOutsideOfWindow())
	{
		return;
	}

	particleGrid.fillRectangle(start / particleGrid.particleSize, end / particleGrid.particleSize, particleTypeIterator.currentType);
}

void RectangleTool::clearRectangle() const
{
	if (isRectangleTooSmall() || IsRectangleOutsideOfWindow())
	{
		return;
	}

	particleGrid.clearRectangle(start / particleGrid.particleSize, end / particleGrid.particleSize);
}

void RectangleTool::startDraggingRectangle()
{
	if (!checkIfMouseIsInsideRectangle())
	{
		return;
	}
	isDragging = true;
	previousMousePosition = sf::Mouse::getPosition(window);

	initialPoints = particleGrid.getMidpointsInRectangle(start, end);
	copiedParticles = particleGrid.getParticlesFromPoints(initialPoints);
}

void RectangleTool::stopDraggingRectangle()
{
	isDragging = false;
}

void RectangleTool::dragRectangle()
{
	const sf::Vector2i pos = sf::Mouse::getPosition(window);
	const sf::Vector2i delta = pos - previousMousePosition;
	previousMousePosition = pos;
	start += delta;
	end += delta;

	moveRectangle();
}

void RectangleTool::clipRectangleToBorders()
{
	const sf::Vector2i windowSize(window.getSize().x, window.getSize().y);
	start.x = std::max(1, std::min(start.x, windowSize.x - 1));
	start.y = std::max(1, std::min(start.y, windowSize.y - 1));
	end.x = std::max(1, std::min(end.x, windowSize.x - 1));
	end.y = std::max(1, std::min(end.y, windowSize.y - 1));
}

void RectangleTool::moveRectangle()
{
	if (isRectangleTooSmall())
	{
		return;
	}

	const std::vector<sf::Vector2i> newPoints = particleGrid.getMidpointsInRectangle(start, end);
	if (initialPoints.size() != newPoints.size() || initialPoints == newPoints)
	{
		return;
	}
	particleGrid.clearParticlesFromPoints(initialPoints);
	particleGrid.setParticlesFromPoints(newPoints, copiedParticles);
	initialPoints = newPoints;
}

bool RectangleTool::isRectangleTooSmall() const
{
	return isNearlyZero(rectangle.getSize().x) || isNearlyZero(rectangle.getSize().y);
}

bool RectangleTool::IsRectangleOutsideOfWindow() const
{
	sf::Vector2i pos = sf::Mouse::getPosition(window);
	int xStart = std::min(start.x, end.x);
	int xEnd = std::max(start.x, end.x);
	int yStart = std::min(start.y, end.y);
	int yEnd = std::max(start.y, end.y);
	const sf::Vector2i windowSize(window.getSize().x, window.getSize().y);

	const bool isOutsideBottomRight = start.x < 0 || start.y < 0 || end.x > windowSize.x || end.y > windowSize.y;
	const bool isOutsideTopLeft = end.x < 0 || end.y < 0 || start.x > windowSize.x || start.y > windowSize.y;

	return isOutsideBottomRight || isOutsideTopLeft;
}

bool RectangleTool::checkIfMouseIsInsideRectangle() const
{
	const sf::Vector2i pos = sf::Mouse::getPosition(window);
	const int xStart = std::min(start.x, end.x);
	const int xEnd = std::max(start.x, end.x);
	const int yStart = std::min(start.y, end.y);
	const int yEnd = std::max(start.y, end.y);

	return pos.x >= xStart && pos.x <= xEnd && pos.y >= yStart && pos.y <= yEnd;
}
