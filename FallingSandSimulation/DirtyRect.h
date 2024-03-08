#pragma once

#include <SFML/Graphics.hpp>

class DirtyRect
{
public:
	sf::Vector2i minPosition;
	sf::Vector2i maxPosition;

	DirtyRect() : minPosition(sf::Vector2i(-1, -1)), maxPosition(sf::Vector2i(-1, -1))
	{
	}

	DirtyRect(const DirtyRect& other);
	DirtyRect& operator=(const DirtyRect& other);

	sf::Vector2i getDimensions() const;
	void expandRect(sf::Vector2i point);
	void resetRect() const;

private:
	mutable std::atomic_flag lock = ATOMIC_FLAG_INIT; // mutable because lock is modified in a const function
	mutable bool reset = true;

	void lockRect() const;
	void unlockRect() const;
	void lockBoth(const DirtyRect& other) const;
};