#pragma once

#include "IParticleSystem.h"
#include "ParticleGrid.h"

class WaterParticleSystem : public IParticleSystem
{
public:
	WaterParticleSystem(ParticleGrid& particleGrid)
		: particleGrid(particleGrid)
	{
	}

	sf::Vector2i gatherMove(sf::Vector2i gridPosition) override;
	void computeMove(sf::Vector2i source, sf::Vector2i destination) override;

private:
	ParticleGrid& particleGrid;

	sf::Vector2i gatherActiveMove(sf::Vector2i gridPosition) const;
	sf::Vector2i gatherSuspendedMove(sf::Vector2i gridPosition) const;
	void computeActiveMove(sf::Vector2i source, sf::Vector2i destination) const;
	void computeSuspendedMove(sf::Vector2i source, sf::Vector2i destination) const;
};
