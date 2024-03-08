#pragma once
#include <SFML/System/Vector2.hpp>

class IParticleSystem
{
public:
	virtual bool simulate(sf::Vector2i gridPosition) { return false; }
	virtual sf::Vector2i gatherMove(sf::Vector2i gridPosition) { return { gridPosition }; }
	virtual void computeMove(sf::Vector2i source, sf::Vector2i destination) {}
	virtual ~IParticleSystem() = default;
};