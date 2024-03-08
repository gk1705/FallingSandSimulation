#pragma once
#include "IParticleSystem.h"
#include "ParticleGrid.h"

class WoodParticleSystem : public IParticleSystem
{
public:
	ParticleGrid& particleGrid;

	WoodParticleSystem(ParticleGrid& particleGrid) : particleGrid(particleGrid)
	{
	}

	bool simulate(sf::Vector2i gridPosition) override;
};