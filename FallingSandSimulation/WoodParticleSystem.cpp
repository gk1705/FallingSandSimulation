#include "WoodParticleSystem.h"

bool WoodParticleSystem::simulate(sf::Vector2i gridPosition)
{
	const int igniteMulChance = particleGrid.getCardinalDirectionsAreOfTypeCount(gridPosition, ParticleType::Fire);
	if (EngineConstants::WOODIGNITECHANCE * igniteMulChance > getPseudoRandomDouble())
	{
		particleGrid.setParticle(gridPosition, Particle(ParticleType::Fire, ParticleState::Active, ParticleLocomotionType::Static, EngineConstants::BURNINGWOODLIFETIME, 2, sf::Vector2i(0, 0), sf::Color(sf::Color::Red)));
	}
	return true;
}