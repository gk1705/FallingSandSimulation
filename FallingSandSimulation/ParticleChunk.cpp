#include "ParticleChunk.h"

void ParticleChunk::incrementCount()
{
	lockChunk();
	count.fetch_add(1, std::memory_order_relaxed);
	unlockChunk();
}

void ParticleChunk::decrementCount()
{
	lockChunk();
	count.fetch_sub(1, std::memory_order_relaxed);
	unlockChunk();
}

void ParticleChunk::expandDirtyRect(sf::Vector2i point)
{
	nextDirtyRect.expandRect(point);
}

void ParticleChunk::swapDirtyRects()
{
	dirtyRect = nextDirtyRect;
	nextDirtyRect.resetRect();
}

int ParticleChunk::getCount() const
{
	return count;
}

ParticleChunk::ParticleChunk(const ParticleChunk& other)
{
	lockBoth(other);

	start = other.start;
	end = other.end;
	dirtyRect = other.dirtyRect;
	nextDirtyRect = other.nextDirtyRect;

	unlockChunk();
	other.unlockChunk();
}

ParticleChunk& ParticleChunk::operator=(const ParticleChunk& other)
{
	if (this != &other)
	{
		lockBoth(other);
		count = other.count.load(std::memory_order_relaxed);
		unlockChunk();
		other.unlockChunk();
	}
	return *this;
}

void ParticleChunk::lockChunk() const
{
	while (lock.test_and_set(std::memory_order_acquire))
	{
		// spin-wait without blocking
	}
}

void ParticleChunk::unlockChunk() const
{
	lock.clear(std::memory_order_release);
}

void ParticleChunk::lockBoth(const ParticleChunk& other) const
{
	if (this < &other)
	{
		lockChunk();
		other.lockChunk();
	}
	else
	{
		other.lockChunk();
		lockChunk();
	}
}
