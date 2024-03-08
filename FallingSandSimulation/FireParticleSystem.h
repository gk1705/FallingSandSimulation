#pragma once

#include "IParticleSystem.h"
#include "ParticleGrid.h"

class FireParticleSystem : public IParticleSystem
{
public:
	FireParticleSystem(ParticleGrid& particleGrid)
		: particleGrid(particleGrid)
	{

	}

	bool simulate(sf::Vector2i gridPosition) override;

private:
	ParticleGrid& particleGrid;

	void simulateFlameFlicker(Particle& particle) const;
	void simulateSmokeSpawn(sf::Vector2i gridPosition) const;
};