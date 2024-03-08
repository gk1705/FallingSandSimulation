#include "FireParticleSystem.h"

bool FireParticleSystem::simulate(sf::Vector2i gridPosition)
{
	Particle& particle = particleGrid.at(gridPosition);

	const int surroundedByWaterCount = particleGrid.getCardinalDirectionsAreOfTypeCount(gridPosition, ParticleType::Water);
	particle.currentLifeTime -= surroundedByWaterCount * 20;
	particle.currentLifeTime -= 1;
	if (particle.currentLifeTime <= 0)
	{
		if (particle.initialLifeTime != EngineConstants::BURNINGFIRELIFETIME)
		{
			particleGrid.setParticle(gridPosition, ParticleType::Smoke);
		}
		else
		{
			particleGrid.setEmptyParticle(gridPosition);
		}
		return false;
	}

	simulateSmokeSpawn(gridPosition);
	simulateFlameFlicker(particle);

	return true;
}

void FireParticleSystem::simulateFlameFlicker(Particle& particle) const
{
	constexpr int maxFlickerRate = 2;
	constexpr int minFlickerRate = 4;
	constexpr int flickerFalloff = 10;

	const int maxLifeTime = particle.initialLifeTime;
	int flickerRate = std::max(maxFlickerRate, (maxLifeTime - particle.currentLifeTime) / (maxLifeTime / flickerFalloff));
	flickerRate = std::min(flickerRate, minFlickerRate);

	if ((maxLifeTime - particle.currentLifeTime) % flickerRate == 0)
	{
		particle.color = ((maxLifeTime - particle.currentLifeTime) % (2 * flickerRate) == 0) ? sf::Color(255, 150, 0) : sf::Color(255, 200, 0); // TODO: more colors for flame
		particle.color.a = 255 * particle.currentLifeTime / static_cast<float>(maxLifeTime);
	}
}

void FireParticleSystem::simulateSmokeSpawn(sf::Vector2i gridPosition) const
{
	const Particle& particle = particleGrid.at(gridPosition);
	if (particle.initialLifeTime != EngineConstants::BURNINGFIRELIFETIME && particle.currentLifeTime % 5 == 0)
	{
		const sf::Vector2i abovePosition(gridPosition.x, gridPosition.y - 1);
		if (particleGrid.isInBoundsAndEmpty(abovePosition))
		{
			particleGrid.setParticle(abovePosition, ParticleType::Smoke);
		}
	}
}
