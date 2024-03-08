#pragma once
#include <SFML/System/Vector2.hpp>

#include "DirtyRect.h"

class ParticleChunk
{
public:
	sf::Vector2i start;
	sf::Vector2i end;

	DirtyRect dirtyRect;
	DirtyRect nextDirtyRect;

	ParticleChunk(sf::Vector2i start, sf::Vector2i end)
		: start(start), end(end)
	{
	}

	ParticleChunk(const ParticleChunk& other);
	ParticleChunk& operator=(const ParticleChunk& other);

	void incrementCount();
	void decrementCount();
	void expandDirtyRect(sf::Vector2i point);
	void swapDirtyRects();
	int getCount() const;

private:
	std::atomic<int> count;
	mutable std::atomic_flag lock = ATOMIC_FLAG_INIT;

	void lockChunk() const;
	void unlockChunk() const;
	void lockBoth(const ParticleChunk& other) const;
};
