#include "SandParticleSystem.h"

sf::Vector2i SandParticleSystem::gatherMove(sf::Vector2i gridPosition)
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
	case ParticleState::Dormant:
		{
			finalPosition = gatherDormantMove(gridPosition);
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
		else if (particle.state == ParticleState::Suspended)
			particle.state = ParticleState::Dormant;
	}

	return finalPosition;
}

sf::Vector2i SandParticleSystem::gatherActiveMove(sf::Vector2i gridPosition) const
{
	const Particle& particle = particleGrid.at(gridPosition);
	const sf::Vector2i predictedEndpoint = sf::Vector2i(gridPosition.x + particle.velocity.x, gridPosition.y + particle.velocity.y);
	const sf::Vector2i actualEndpoint = particleGrid.findPointBeforeCollision(gridPosition, predictedEndpoint);
	return actualEndpoint;
}

sf::Vector2i SandParticleSystem::gatherSuspendedMove(sf::Vector2i gridPosition) const
{
	const sf::Vector2i moveDown = sf::Vector2i(gridPosition.x, gridPosition.y + 1);
	const sf::Vector2i moveDownLeft = sf::Vector2i(gridPosition.x - 1, gridPosition.y + 1);
	const sf::Vector2i moveDownRight = sf::Vector2i(gridPosition.x + 1, gridPosition.y + 1);
	sf::Vector2i finalPosition = gridPosition;

	if (particleGrid.isInBoundsAndEmpty(moveDown))
	{
		finalPosition = moveDown;
	}
	else if (particleGrid.isInBoundsAndEmpty(moveDownLeft) && particleGrid.isInBoundsAndEmpty(moveDownRight))
	{
		if (bool randomDownLeft = getPseudoRandomInteger() % 2 == 0)
		{
			finalPosition = moveDownLeft;
		}
		else
		{
			finalPosition = moveDownRight;
		}
	}
	else if (particleGrid.isInBoundsAndEmpty(moveDownLeft))
	{
		finalPosition = moveDownLeft;
	}
	else if (particleGrid.isInBoundsAndEmpty(moveDownRight))
	{
		finalPosition = moveDownRight;
	}

	return finalPosition;
}

sf::Vector2i SandParticleSystem::gatherDormantMove(sf::Vector2i gridPosition) const
{
	const sf::Vector2i moveDown = sf::Vector2i(gridPosition.x, gridPosition.y + 1);
	if (particleGrid.isInBoundsAndEmpty(moveDown))
	{
		return moveDown;
	}

	return gridPosition;
}

void SandParticleSystem::computeMove(sf::Vector2i source, sf::Vector2i destination)
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
	case ParticleState::Dormant:
		{
			computeDormantMove(source, destination);
			break;
		}
	}
}

void SandParticleSystem::computeActiveMove(sf::Vector2i source, sf::Vector2i destination) const
{
	Particle& particle = particleGrid.at(source);
	const sf::Vector2i predictedEndpoint = sf::Vector2i(source.x + particle.velocity.x, source.y + particle.velocity.y);
	if (predictedEndpoint != destination)
	{
		particleGrid.transferMomentum(source);
	}

	const sf::Vector2i belowEndpoint = sf::Vector2i(destination.x, destination.y + 1);
	if (!particleGrid.isInBoundsAndEmpty(belowEndpoint))
	{
		particle.velocity.y = 0;
		particleGrid.applyFriction(particle);
	}
	else
	{
		particle.velocity.y = std::max(particle.velocity.y + EngineConstants::GRAVITYACCELERATION, EngineConstants::GRAVITY);
	}

	particleGrid.performMove(source, destination, particle);
}

void SandParticleSystem::computeSuspendedMove(sf::Vector2i source, sf::Vector2i destination) const
{
	Particle& particle = particleGrid.at(source);
	const sf::Vector2i moveDown = sf::Vector2i(source.x, source.y + 1);

	if (destination == moveDown)
	{
		particle.velocity.y = EngineConstants::GRAVITY;
		particle.state = ParticleState::Active;
	}
	particleGrid.performMove(source, destination, particle);
}

void SandParticleSystem::computeDormantMove(sf::Vector2i source, sf::Vector2i destination) const
{
	Particle& particle = particleGrid.at(source);
	particle.state = ParticleState::Suspended;
	particleGrid.performMove(source, destination, particle);
}
