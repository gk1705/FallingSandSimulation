#pragma once

#include "IParticleSystem.h"
#include "SandParticleSystem.h"
#include "ThreadPool.h"
#include "ThreadSafeArray.h"

class ParticleSimulator
{
public:
	bool simulate = true;

	ParticleSimulator(ParticleGrid& particleGrid);
	~ParticleSimulator();

	void toggleSimulation();
	void simulateParticles();

private:
	ParticleGrid& particleGrid;
	ThreadPool threadPool;
	std::unordered_map<ParticleType, IParticleSystem*> particleSystems; // TODO: use a function pointer instead of a vtable
	// TODO: this hack of a stack allocation needs to be done dynamically or a larger array size will cause a stack overflow
	ThreadSafeArray<std::pair<sf::Vector2i, sf::Vector2i>, EngineConstants::SCREENWIDTH / EngineConstants::PARTICLESIZE * EngineConstants::SCREENHEIGHT / EngineConstants::PARTICLESIZE> possibleMoves;

	bool isValidParticle(sf::Vector2i gridPosition) const;
	static int getRandomNumber(int max);
	void gatherPossibleMoves();
	void sortPossibleMovesByDestination();
	std::vector<std::pair<int, int>> getArrayThreadRanges();
	void processMovesWithRandomContender(const std::vector<std::pair<int, int>>& ranges);
	// non threaded version for debugging
	void processMovesWithRandomContender();
	// single threaded because its very dependent on the order of execution
	void computeDensitySwap() const;
	void processParticles();
};