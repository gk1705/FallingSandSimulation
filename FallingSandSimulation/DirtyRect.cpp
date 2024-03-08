#include "DirtyRect.h"

sf::Vector2i DirtyRect::getDimensions() const
{
	lockRect();
	const sf::Vector2i dimensions = reset ? sf::Vector2i(0, 0) : sf::Vector2i(maxPosition.x - minPosition.x, maxPosition.y - minPosition.y);
	unlockRect();
	return dimensions;
}

void DirtyRect::expandRect(sf::Vector2i point)
{
	lockRect();
	if (reset)
	{
		minPosition = point;
		maxPosition = point + sf::Vector2i(1, 1);
		reset = false;
		unlockRect();
		return;
	}

	minPosition.x = std::min(minPosition.x, point.x);
	minPosition.y = std::min(minPosition.y, point.y);
	maxPosition.x = std::max(maxPosition.x, point.x + 1);
	maxPosition.y = std::max(maxPosition.y, point.y + 1);
	unlockRect();
}

void DirtyRect::resetRect() const
{
	lockRect();
	reset = true;
	unlockRect();
}

DirtyRect::DirtyRect(const DirtyRect& other)
{
	lockBoth(other);

	minPosition = other.minPosition;
	maxPosition = other.maxPosition;
	reset = other.reset;

	unlockRect();
	other.unlockRect();
}

DirtyRect& DirtyRect::operator=(const DirtyRect& other)
{
	if (this != &other)
	{
		// avoiding deadlock by locking in consistent order
		lockBoth(other);

		minPosition = other.minPosition;
		maxPosition = other.maxPosition;
		reset = other.reset;

		unlockRect();
		other.unlockRect();
	}
	return *this;
}

void DirtyRect::lockRect() const
{
	while (lock.test_and_set(std::memory_order_acquire))
	{
		// spin-wait without blocking
	}
}

void DirtyRect::unlockRect() const
{
	lock.clear(std::memory_order_release);
}

void DirtyRect::lockBoth(const DirtyRect& other) const
{
	if (this < &other)
	{
		lockRect();
		other.lockRect();
	}
	else
	{
		other.lockRect();
		lockRect();
	}
}