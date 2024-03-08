#include "StampTool.h"

void StampTool::handleInput(const sf::Event& event)
{
	if (event.type == sf::Event::MouseWheelScrolled)
	{
		if (event.mouseWheelScroll.delta < 0)
		{
			shrinkStamp();
		}
		else
		{
			enlargeStamp();
		}
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		fill();
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		erase();
	}
}

void StampTool::render()
{
	if (!isMouseInsideGrid()) return;

	const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	const float radius = stampSize * particleGrid.particleSize + particleGrid.particleSize / 2.f;
	sf::CircleShape circle(radius);
	circle.setPosition(mousePos.x, mousePos.y);
	circle.setOrigin(radius, radius);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineThickness(1);
	circle.setOutlineColor(sf::Color::Red);

	window.draw(circle);
}

void StampTool::shrinkStamp()
{
	if (stampSize > 0)
	{
		stampSize--;
	}
}

void StampTool::enlargeStamp()
{
	if (stampSize < maxStampSize)
	{
		stampSize++;
	}
}

void StampTool::fill() const
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	mousePos.x = mousePos.x / particleGrid.particleSize;
	mousePos.y = mousePos.y / particleGrid.particleSize;
	particleGrid.drawFilledCircle(mousePos, stampSize, particleTypeIterator.currentType);
}

void StampTool::erase() const
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	mousePos.x = mousePos.x / particleGrid.particleSize;
	mousePos.y = mousePos.y / particleGrid.particleSize;
	particleGrid.eraseCircle(mousePos, stampSize);
}

bool StampTool::isMouseInsideGrid() const
{
	const int columnCount = window.getSize().x / particleGrid.particleSize;
	const int rowCount = window.getSize().y / particleGrid.particleSize;
	const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	return mousePos.x >= 0 && mousePos.x < columnCount * particleGrid.particleSize && mousePos.y >= 0 && mousePos.y < rowCount * particleGrid.particleSize;
}
