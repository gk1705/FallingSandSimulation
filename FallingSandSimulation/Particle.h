#pragma once

#include <SFML/Graphics.hpp>

enum class ParticleState
{
	Active,
	Suspended,
	Dormant,
};

enum class ParticleType
{
	Empty,
	Concrete,
	Sand,
	Water,
	Smoke,
	Wood,
	Fire,
};

enum class ParticleLocomotionType
{
	Static,
	Dynamic,
};

class Particle
{
public:
	ParticleType type;
	ParticleState state;
	ParticleLocomotionType locomotionType;
	int initialLifeTime;
	int currentLifeTime;
	int density;
	sf::Vector2i velocity;
	sf::Color color;

	Particle()
	{
		type = ParticleType::Empty;
		state = ParticleState::Dormant;
		locomotionType = ParticleLocomotionType::Static;
		initialLifeTime = 0;
		currentLifeTime = 0;
		density = 0;
		velocity = sf::Vector2i(0, 0);
		color = sf::Color::Black;
	}

	Particle(ParticleType type, ParticleState state, ParticleLocomotionType locomotionType, int lifeTime, int density, sf::Vector2i velocity, sf::Color color)
		: type(type), state(state), locomotionType(locomotionType), initialLifeTime(lifeTime), currentLifeTime(lifeTime), density(density), velocity(velocity), color(color)
	{
	}
};