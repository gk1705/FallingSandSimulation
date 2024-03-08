#include "SmokeParticleSystem.h"

sf::Vector2i SmokeParticleSystem::gatherMove(sf::Vector2i gridPosition)
{
	Particle& particle = particleGrid.at(gridPosition);
	sf::Vector2i finalPosition = gridPosition;
	switch (particle.state)
	{
	case ParticleState::Active:
		{
			finalPosition = gatherActiveMove(gridPosition);
			break;
		}
	case ParticleState::Suspended:
		{
			finalPosition = gatherSuspendedMove(gridPosition);
			break;
		}
	}

	if (finalPosition == gridPosition)
	{
		if (particle.state == ParticleState::Active)
		{
			particle.state = ParticleState::Suspended;
			particleGrid.expandDirtyRect(finalPosition);
		}
	}

	return finalPosition;
}

sf::Vector2i SmokeParticleSystem::gatherActiveMove(sf::Vector2i gridPosition) const
{
	const Particle& particle = particleGrid.at(gridPosition);
	const sf::Vector2i predictedEndpoint = sf::Vector2i(gridPosition.x + particle.velocity.x, gridPosition.y + particle.velocity.y);
	const sf::Vector2i actualEndpoint = particleGrid.findPointBeforeCollision(gridPosition, predictedEndpoint);
	return actualEndpoint;
}

sf::Vector2i SmokeParticleSystem::gatherSuspendedMove(sf::Vector2i gridPosition) const
{
	const sf::Vector2i moveUp = sf::Vector2i(gridPosition.x, gridPosition.y - 1);
	const sf::Vector2i moveUpLeft = sf::Vector2i(gridPosition.x - 1, gridPosition.y - 1);
	const sf::Vector2i moveUpRight = sf::Vector2i(gridPosition.x + 1, gridPosition.y - 1);
	const sf::Vector2i moveLeft = sf::Vector2i(gridPosition.x - EngineConstants::SMOKESPREAD, gridPosition.y);
	const sf::Vector2i moveRight = sf::Vector2i(gridPosition.x + EngineConstants::SMOKESPREAD, gridPosition.y);
	sf::Vector2i finalPosition = gridPosition;

	if (particleGrid.isInBoundsAndEmpty(moveUp))
	{
		finalPosition = moveUp;
	}
	else if (particleGrid.isInBoundsAndEmpty(moveUpLeft) && particleGrid.isInBoundsAndEmpty(moveUpRight))
	{
		if (bool randomUpLeft = getPseudoRandomInteger() % 2 == 0)
		{
			finalPosition = moveUpLeft;
		}
		else
		{
			finalPosition = moveUpRight;
		}
	}
	else if (particleGrid.isInBoundsAndEmpty(moveUpLeft))
	{
		finalPosition = moveUpLeft;
	}
	else if (particleGrid.isInBoundsAndEmpty(moveUpRight))
	{
		finalPosition = moveUpRight;
	}
	else if (particleGrid.isValidMove(gridPosition, moveLeft) && particleGrid.isValidMove(gridPosition, moveRight))
	{
		if (bool randomLeft = getPseudoRandomInteger() % 2 == 0)
		{
			finalPosition = particleGrid.getValidMove(gridPosition, moveLeft);
			particleGrid.checkWayPointsForFreeUpPositionAndOccupy(gridPosition, finalPosition);
		}
		else
		{
			finalPosition = particleGrid.getValidMove(gridPosition, moveRight);
			particleGrid.checkWayPointsForFreeUpPositionAndOccupy(gridPosition, finalPosition);
		}
	}
	else if (particleGrid.isValidMove(gridPosition, moveLeft))
	{
		finalPosition = particleGrid.getValidMove(gridPosition, moveLeft);
		particleGrid.checkWayPointsForFreeUpPositionAndOccupy(gridPosition, finalPosition);
	}
	else if (particleGrid.isValidMove(gridPosition, moveRight))
	{
		finalPosition = particleGrid.getValidMove(gridPosition, moveRight);
		particleGrid.checkWayPointsForFreeUpPositionAndOccupy(gridPosition, finalPosition);
	}

	return finalPosition;
}

void SmokeParticleSystem::computeMove(sf::Vector2i source, sf::Vector2i destination)
{
	const Particle& particle = particleGrid.at(source);
	switch (particle.state)
	{
	case ParticleState::Active:
		{
			computeActiveMove(source, destination);
			break;
		}
	case ParticleState::Suspended:
		{
			computeSuspendedMove(source, destination);
			break;
		}
	}
}

void SmokeParticleSystem::computeActiveMove(sf::Vector2i source, sf::Vector2i destination) const
{
	Particle& particle = particleGrid.at(source);
	const sf::Vector2i predictedEndpoint = sf::Vector2i(source.x + particle.velocity.x, source.y + particle.velocity.y);

	if (predictedEndpoint != destination)
	{
		particleGrid.transferMomentum(source);
	}

	const sf::Vector2i aboveEndpoint = sf::Vector2i(destination.x, destination.y - 1);
	if (!particleGrid.isInBoundsAndEmpty(aboveEndpoint))
	{
		particle.velocity.y = 0;
		particleGrid.applyFriction(particle);
	}
	else
	{
		particle.velocity.y = -EngineConstants::AIRSUSPENSIONRATE;
	}

	particleGrid.performMove(source, destination, particle); //TODO perform move on another buffer
}

void SmokeParticleSystem::computeSuspendedMove(sf::Vector2i source, sf::Vector2i destination) const
{
	Particle& particle = particleGrid.at(source);
	const sf::Vector2i moveUp = sf::Vector2i(source.x, source.y - 1);

	if (destination == moveUp)
	{
		particle.velocity.y = -EngineConstants::AIRSUSPENSIONRATE;
		particle.state = ParticleState::Active;
	}
	particleGrid.performMove(source, destination, particle);
}
