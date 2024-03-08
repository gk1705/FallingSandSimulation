#include "ParticleTypeIterator.h"

ParticleTypeIterator::ParticleTypeIterator()
{
	particleTypes.push_back(ParticleType::Concrete);
	particleTypes.push_back(ParticleType::Sand);
	particleTypes.push_back(ParticleType::Water);
	particleTypes.push_back(ParticleType::Smoke);
	particleTypes.push_back(ParticleType::Wood);
	particleTypes.push_back(ParticleType::Fire);
	particleTypeIterator = particleTypes.begin();

	particleNames[ParticleType::Concrete] = "Concrete";
	particleNames[ParticleType::Sand] = "Sand";
	particleNames[ParticleType::Water] = "Water";
	particleNames[ParticleType::Smoke] = "Smoke";
	particleNames[ParticleType::Wood] = "Wood";
	particleNames[ParticleType::Fire] = "Fire";

	particleColors[ParticleType::Concrete] = sf::Color(0x808080ff);
	particleColors[ParticleType::Sand] = sf::Color(243, 224, 59);
	particleColors[ParticleType::Water] = sf::Color(0, 0, 200, 100);
	particleColors[ParticleType::Smoke] = sf::Color(105, 105, 105, 100);
	particleColors[ParticleType::Wood] = sf::Color(139, 69, 19);
	particleColors[ParticleType::Fire] = sf::Color(255, 69, 0);
}

void ParticleTypeIterator::iterateElement(int direction)
{
	particleTypeIterator += direction;
	if (particleTypeIterator == particleTypes.end())
	{
		particleTypeIterator = particleTypes.begin();
	}
	else if (particleTypeIterator < particleTypes.begin())
	{
		particleTypeIterator = particleTypes.end() - 1;
	}
	currentType = *particleTypeIterator;
	particleTypeString = "Selected Type: " + particleNames[currentType];
}

void ParticleTypeIterator::iterateParticleTypeUp()
{
	++particleTypeIterator;
	if (particleTypeIterator == particleTypes.end())
	{
		particleTypeIterator = particleTypes.begin();
	}
	currentType = *particleTypeIterator;
	particleTypeString = "Selected Type: " + particleNames[currentType];
}

void ParticleTypeIterator::iterateParticleTypeDown()
{
	if (particleTypeIterator == particleTypes.begin())
	{
		particleTypeIterator = particleTypes.end() - 1;
	}
	else
	{
		--particleTypeIterator;
	}
	currentType = *particleTypeIterator;
	particleTypeString = "Selected Type: " + particleNames[currentType];
}
