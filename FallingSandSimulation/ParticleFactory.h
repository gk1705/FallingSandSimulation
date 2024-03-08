#pragma once

#include <unordered_map>

#include "Particle.h"
#include "helpers.h"

class ParticleFactory
{
public:
	ParticleFactory();

	Particle getParticle(ParticleType type);

private:
	std::unordered_map<ParticleType, Particle> particles;
	std::vector<sf::Color> offsetColors;

	sf::Color getOffsetColor(sf::Color color) const;
	sf::Color resolveParticleColor(ParticleType type) const;
};