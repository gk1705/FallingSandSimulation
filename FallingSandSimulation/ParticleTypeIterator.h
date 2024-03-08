#pragma once

#include <unordered_map>

#include "Particle.h"

class ParticleTypeIterator
{
public:
	ParticleType currentType = ParticleType::Empty;
	std::string particleTypeString = "Selected Type: None";
	std::vector<ParticleType> particleTypes;
	std::unordered_map<ParticleType, sf::Color> particleColors;
	std::unordered_map<ParticleType, std::string> particleNames;

	ParticleTypeIterator();

	void iterateElement(int direction);
	void iterateParticleTypeUp();
	void iterateParticleTypeDown();

private:
	std::vector<ParticleType>::iterator particleTypeIterator;
};
